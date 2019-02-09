// Buffer cache.
//
// The buffer cache is a linked list of buf structures
// holding cached copies of disk block contents.
// Each buf has two state bits B_BUSY and B_VALID.
// If B_BUSY is set, it means that some code is currently
// editing buf, so other code is not allowed to look at it.
// To wait for a buffer that is B_BUSY, sleep on buf.
// (See buf_getblk() below.)
// 
// If B_VALID is set, it means that the memory contents
// have been initialized by reading them off the disk.
// (Conversely, if B_VALID is not set, the memory contents
// of buf must be initialized, often by calling bread,
// before being used.)
// 
// After making changes to a buf's memory, call bwrite to flush
// the changes out to disk, to keep the disk and memory copies
// in sync.
//
// When finished with a buffer, call brelse to release the buffer
// (i.e., clear B_BUSY), so that others can access it.
//
// Bufs that are not B_BUSY are fair game for reuse for other
// disk blocks.  It is not allowed to use a buf after calling brelse.

#include <rtos.h>
#include <stdlib.h>
#include <string.h>
#include <fs/buf.h>
#include <fs/param.h>
#include <fs/defs.h>
#include <fs/vnode.h>
#include <os/miscLib.h>

struct buf buf[NBUF];
// struct spinlock buf_table_lock;

/*
 * Linked list of all buffers.
 * bufHead->head is most recently used.
 * bufHead->tail is least recently used.
 */

LIST  bufHead;

/***************************************************************************
 *
 * binit - initialize the bufs
 *
 * RETURNS: N/A
 */

void binit (void)
{
    struct buf *b;

#if 0
    initlock (&buf_table_lock, "buf_table");
#endif

    /* Create linked list of buffers */
    listInit (&bufHead);
    for (b = buf; b < buf + NBUF; b++) {
        memset (b, 0, sizeof (struct buf));

        /*
         * For the present time, the number bytes to be read from/written to
         * a buffer will match the buffer size.  This could conceivably change
         * in the future.
         *
         * Furthermore, one or both of these fields may be necessary should
         * a file system require different buffer sizes.  (For example, the
         * DOS FAT file system could use this as the FAT is accessed by
         * sector, but the file and directory data are accessed by cluster.)
         */

        b->b_count  = 512;
        b->b_size   = 512;

        listAdd (&bufHead, &b->b_node);
    }
}

/***************************************************************************
 *
 * buf_getblk - get a buffer
 *
 * This routine attempts to get a buffer of size <nBytes> for a vnode/blkNum
 * pairing.  First, it checks for an existing match.  If not found, it will
 * then try to allocate a new pairing.  In either case, it is supposed to
 * return a locked buffer.
 *
 * RETURNS: buffer
 */

struct buf * buf_getblk (
    struct vnode *  pVnode,   /* pointer to vnode */
    lblkno_t        blkNum,   /* block # to assign to the buffer */
    unsigned        nBytes    /* desired buffer size (not yet used) */
    ) {
    struct buf *b;

#if 0
  acquire(&buf_table_lock);
#endif

    for (;;) {
        /* Search for buffer/blk# pair */
        for (b = (struct buf *) LIST_HEAD (&bufHead);
             b != NULL;
             b = (struct buf *) LIST_NEXT (&b->b_node)) {
            if ((b->b_flags & (B_BUSY | B_VALID)) &&
                (b->b_vp == pVnode) && (b->b_blkno == blkNum)) {
                break;
            }
        }

        if (b != NULL) {
            /* buffer/blk# pair found. */
            if (b->b_flags & B_BUSY) {
#if 0
                sleep(buf, &buf_table_lock);
#endif
            } else {
                b->b_flags |= B_BUSY;
                /*
                 * [b_vp] is already set to <pVnode>.
                 * [b_blkno] is already <blkNum>.
                 */
#if 0
                b->flags &= ~B_VALID; // Force reread from disk
                release(&buf_table_lock);
#endif
                return (b);
            }
        } else {
            /* Not found.  Get a non-busy buffer.  Start at LRU node. */
            for (b = (struct buf *) LIST_TAIL (&bufHead);
                 b != NULL;
                 b = (struct buf *) LIST_PREV (&b->b_node)) {
                if ((b->b_flags & B_BUSY) == 0) {
                    b->b_flags = B_BUSY;
                    b->b_blkno = blkNum;
                    b->b_vp    = pVnode;
#if 0
                    release (&buf_table_lock);
#endif
                    return (b);
                }
            }
            panic ("bget: no buffers");
        }
    }
}

/***************************************************************************
 *
 * bread - read data from backing media into a buffer
 *
 * RETURNS: OK on success, non-zero otherwise
 */

int  bread (
    struct vnode *  pVnode,    /* vnode to assign to buffer (not yet used) */
    lblkno_t        lbn,       /* logical block number */
    int             blkSize,   /* block size */
    struct ucred *  pCred,     /* ptr to user credentials */
    struct buf **   ppBuf      /* output: double ptr to buffer with data */
    ) {
    struct buf *  b;
    unsigned int  dev = pVnode->v_mount->mnt_dev;
    strategy_args_t  args;

    b = buf_getblk (pVnode, lbn, blkSize);
    if (b->b_flags & B_VALID) {
        *ppBuf = b;
        return (OK);
    }

    args.vp = pVnode;
    args.bp = b;
    VOP_STRATEGY (pVnode, args);
    b->b_flags |= B_VALID;

    *ppBuf = b;
    return (OK);
}

/***************************************************************************
 *
 * bwrite - write the buf's contents to backing media
 *
 * This routine writes the buf's contents to the backing media.  Note that the
 * buf must be locked before calling this routine.
 *
 * RETURNS: OK on success, non-zero otherwise
 */

int bwrite (
    struct buf *  b         /* ptr to buffer to write */
    ) {
    strategy_args_t  args;

    if ((b->b_flags & B_BUSY) == 0)
        panic ("bwrite");

    /* Eventually bwrite() will call the vnode's strategy routine. */
    args.vp = b->b_vp;
    args.bp = b;
    args.bp->b_flags &= ~B_READ;    /* Inform strategy to write */
    VOP_STRATEGY (b->b_vp, args);

    b->b_flags |= B_VALID;

    return (OK);
}

/***************************************************************************
 *
 * brelse - release the buffer
 *
 * This routine releases the buffer.  It is removed from its current position
 * on the list, and then added to the head of the list as it has become the
 * most recently used buffer.
 *
 * RETURNS: N/A
 */

void brelse (
    struct buf *  b
    ) {
    if ((b->b_flags & B_BUSY) == 0)
        panic ("brelse");

#if 0
    acquire(&buf_table_lock);
#endif
  
    listRemove (&bufHead, &b->b_node);        /* Remove from list */
    listInsert (&bufHead, NULL, &b->b_node);  /* Insert at list head */

    b->b_flags &= ~B_BUSY;
#if 0
    wakeup(buf);

    release(&buf_table_lock);
#endif
}

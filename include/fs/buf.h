#ifndef _buf_h
#define _buf_h

#include <fs/vnode.h>
#include <fs/bio.h>
#include <util/listLib.h>

/* defines */

/* The following macros are the buffer flags */

#define B_BUSY  0x0001  /* buffer is locked by some process */
#define B_VALID 0x0002  /* buffer contains the data of the sector */
#define B_READ  0x0004  /* buffer read operation if set, else write */

/* typedefs */

typedef struct buf {
    LIST_NODE        b_node;   /* linked list node */
    lblkno_t         b_blkno;  /* block number associated with buffer */
    int              b_flags;  /* buffer flags */
    int              b_count;  /* # of bytes to transfer */
    int              b_size;   /* size of the data area in buffer */
    struct vnode *   b_vp;     /* back ptr to vnode */
    bio_t *          b_bio;    /* ptr to block io */
    unsigned char *  b_data;   /* data area (matches [b_bio->bio_data]) */
} buf_t;

#endif /* _buf_h */


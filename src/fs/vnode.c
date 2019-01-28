#include <rtos.h>
#include <stdlib.h>
#include <fs/mount.h>
#include <fs/vnode.h>

/***************************************************************************
 *
 * vnodesCreate -
 *
 * RETURNS: ptr to array of vnodes (syncer vnode is the 1st), NULL on error
 */

struct vnode *  vnodesCreate (
    struct mount *  pMount,    /* ptr to mount */
    int             isize,     /* size of the inode structure */
    int             nvnode     /* # of file/logical vnodes to create (> 0) */
    ) {
    char *          ptr;
    struct vnode *  pVnode;
    int             i;
    int             j;

    j = sizeof (vnode_t) + ((sizeof (vnode_t) + isize) * nvnode);
    ptr = (char *) malloc (j);
    if (ptr == NULL) {
        return (NULL);
    }

    memset (ptr, 0, j);
    pVnode = (struct vnode *) ptr;
    ptr += (sizeof (vnode_t) * (nvnode + 1));

    i = 0;                  /* Do not give the syncer vnode (first in array) */
    goto skip;              /* a private data section.  */
                              
    while (i <= nvnode) {
        pVnode[i].v_data = ptr;
        ptr += isize;
skip:
        pVnode[i].v_mount = pMount;

        listInit (&pVnode[i].v_buflist);
        i++;
    }

    return (pVnode);
}

/***************************************************************************
 *
 * vnodesDelete -
 *
 * RETURNS: N/A
 */

void vnodesDelete (
    struct vnode *  pVnode,
    int             nvnode
    ) {
    free (pVnode);
}

/***************************************************************************
 *
 * vnodeLock - establish a lock by incrementing the use count
 *
 * RETURNS: OK
 */

STATUS vnodeLock (
    vnode_t *  pVnode
    ) {
    /* No semaphore is needed as the mount is single-threaded. */
    pVnode->v_count++;    /* overflow is unlikely */

    return (OK);
}

/***************************************************************************
 *
 * vnodeUnlock - release the lock by decrementing the use count
 *
 * RETURNS: OK on success, non-zero if VOP_INACTIVE failed
 */

STATUS vnodeUnlock (
    vnode_t *  pVnode
    ) {
    inactive_args_t  inactiveArgs;
    int              error = OK;

    /*
     * Note that access to the mount has already been single threaded.
     * Thus, there is no current risk of concurrent access, and no
     * semaphore is required for protection.
     */

    if (--pVnode->v_count == 0) {
        inactiveArgs.vp = pVnode;

        error = VOP_INACTIVE (pVnode, inactiveArgs);

        /* Not concerned with <error> result yet. */
    }

    return (error);
}

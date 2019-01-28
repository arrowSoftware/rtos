/* bio.h - Buffer IO header */

#ifndef __BIO_H
#define __BIO_H

/* includes */

#include <rtos.h>

/* defines */

#define BIO_READ   0x0001
#define BIO_WRITE  0x0002

/* typedefs */

/*
 * Notes: typically the [bio_done] function pointer will refer to a routine
 * that gives a semaphore ([bio_param]).  
 *
 * [bio_error] is the error code (if any) set by xbdStrategy().
 *
 * [bio_resid], indicates an IO error if set to non-zero by xbdStrategy().
 */

typedef struct bio {
    unsigned      bio_dev;                   /* device upon which FS runs */
    char *        bio_data;                  /* ptr to data area */
    unsigned      bio_flags;                 /* BIO_READ or BIO_WRITE */
    struct bio *  bio_chain;                 /* next bio in chain (not used) */
    int           bio_bcount;                /* # of bytes to read */
    lblkno_t      bio_blkno;                 /* logical block number */

    void        (*bio_done) (void *  param); /* rtn to call when done */
    void *        bio_param;                 /* parameter to bio_done */

    int           bio_error;                 /* error code (0 = OK) */
    int           bio_resid;                 /* bytes left (expect 0) */
} bio_t;

/* structs */

/* externs */

extern void *  bio_alloc (unsigned nbytes);
extern void    bio_free (void *ptr);

#endif

/* xbdPartition.c */

/*
DESCRIPTION
  For a good reference on partitions and how they are supposed to be set up,
please refer to the following webpages ....

http://www.nondot.org/sabre/os/files/Disk/CHSTranslation.txt
http://www.nondot.org/sabre/os/files/Partitions/PartitionTables.txt

These were the pages that were used as reference material when coding this
module.

  Example partitioning of Lower-Layer XBD Device:
  +----------------+----------------------------+-------------+
  |   Part #1      |         Part #2            |  Part #3    |
  +----------------+----------------------------+-------------+
  |              Lower-Layer XBD Device                       |
  +-----------------------------------------------------------+

Each XBD request comes from an upper layer, and get funnelled down to the
lower layer as necessary.  In the above diagram, there are three partitions;
each partition is an XBD device.  As each XBD partition shares the underlying
lower-layer XBD device, it is necessary to be able to identify which 
*/

/* includes */

#include <rtos.h>

/* defines */

/*
 * This semi-arbitrary limit for the number of XBD partitions comes from the
 * fact that there are 26 letters in the alphabet.  Unless extended partitions
 * are used, there will not be more than four (4) partitions per device.  When
 * extended partitions are eventually implemented, it is not expected that the
 * total number of partitions will exceed 26.
 */
#define XBD_MAX_PARTS    26     /* semi-arbitrary limit */

/* typedefs */

typedef struct partition_entry {
    u_char  bootIndicator;     /* Boot indicator: 0x80--used for booting */
    u_char  headStart;         /* Starting head for the partition */
    u_char  secStart;          /* Starting sector for the partition */
    u_char  cylStart;          /* Starting cylinder for the partition */

    u_char  systemIndicator;   /* System indicator (F/S type hint) */
    u_char  headEnd;           /* Ending head for the partition */
    u_char  secEnd;            /* Ending sector for the partition */
    u_char  cylEnd;            /* Ending cylinder for the partition */

    u_int   sector;            /* Starting sector of partition */
    u_int   nSectors;          /* # of sectors in the partition */
} PARTITION_ENTRY;

typedef struct xbd_partition  XBD_PARTITION;

typedef struct xbd_part {
    XBD              xbd;
    BOOL             isBootable;       /* TRUE if bootable */
    unsigned         head[2];          /* [0]: start value, [1]: end value */
    unsigned         sector[2];        /* [0]: start value, [1]: end value */
    unsigned         cylinder[2];      /* [0]: start value, [1]: end value */
    lblkno_t         startBlock;       /* starting block */
    lblkno_t         nBlocks;          /* number of blocks in the partition */
    XBD_PARTITION *  xbdPartitions;    /* back pointer */
} XBD_PART;

struct xbd_partition {
    device_t  subDevice;                /* ID's lower layer XBD device */
    int       nPartitions;              /* number of partitions */
    XBD_PART  xbdParts[XBD_MAX_PARTS];  /* list of XBD partitions */
};

LOCAL XBD_FUNCS  xbdPartitionFuncs = {
    &xbdPartitionIoctl,
    &xbdPartitionStrategy,
    &xbdPartitionDump,
};

/***************************************************************************
 *
 * xbdPartitionDump -
 *
 * RETURNS: OK on success, error otherwise
 */

LOCAL int xbdPartitionDump (
    XBD *     xbd,
    lblkno_t  lblkno,
    void *    data,
    size_t    nBytes
    ) {
    XBD_PART *  xbdPart = (XBD_PART *) xbd;
    int         rv;

    if ((lblkno < 0) || (lblkno > xbdPart->nBlocks)) {
        return (EINVAL);
    }

    lblkno += xbdPart->startBlock;

    rv = xbdDump (xbdPart->partitions->subDevice, lblkno, data, nBytes);

    return (rv);
}

/***************************************************************************
 *
 * xbdPartitionStrategy -
 *
 * RETURNS OK on success, error otherwise
 */

LOCAL int xbdPartitionStrategy (
    XBD *         xbd,
    struct bio *  pBio
    ) {
    XBD_PART *  xbdPartition = (XBD_PART *) xbd;
    int         rv;

    /*
     * TODO:
     * Massage the block number in the BIO.  The "massaging" will consist
     * of adding the partition's starting block number to the block number.
     */

    rv = xbdStrategy (xbdPartition->xbdPartitions->subDevice, pBio);

    return (rv);
}

/***************************************************************************
 *
 * xbdPartitionIoctl -
 *
 * RETURNS: OK on success, error otherwise
 */

LOCAL int  xbdPartitionIoctl (
    XBD *     xbd,
    unsigned  command,
    void *    data
    ) {
    XBD_PART *  xbdPartition = (XBD_PART *) xbd;
    int         rv = OK;

    switch (command) {
        XBD_GEOMETRY_GET:
            ...;
            break;

        default:
            /* Pass down to lower layer */
            rv = xbdIoctl (xbdPartition->xbdPartitions->subDevice,
                           command, data);
            break;
    }

    return (rv);
}

/***************************************************************************
 *
 * xbdSectorReadDone - 
 *
 * RETURNS: N/A
 */

LOCAL void xbdSectorReadDone (
    bio_t *  bio
    ) {
    semGive ((SEM_ID) bio->bio_param);
}

/***************************************************************************
 *
 * xbdSectorRead - read a sector from the underlying XBD device
 *
 * RETURNS: OK on success, error otherwise
 */

LOCAL int xbdSectorRead (
    device_t  device,       /* lower layer XBD device */
    lblkno_t  sector,       /* sector number to read */
    unsigned  sectorSize,   /* # of bytes in the sector */
    char *    data          /* pointer to data to read */
    ) {
    bio_t      bio;
    SEMAPHORE  sem;

    semBInit (&sem, SEM_Q_PRIORITY, SEM_EMPTY);

    bio.bio_dev    = device;               /* device to read */
    bio.bio_data   = data;                 /* data buffer */
    bio.bio_flags  = BIO_READ;             /* flag it as a read operation */
    bio.bio_chain  = NULL;                 /* no subsequent bios */
    bio.bio_bcount = sectorSize;           /* sector size (bytes to read) */
    bio.bio_blkno  = sector;               /* sector # to read */
    bio.bio_done   = xbdSectorReadDone;    /* rtn to call when done reading */
    bio.bio_param  = &sem;                 /* semaphore on which to wait */
    bio.bio_error  = OK;
    bio.bio_resid  = 0;                    

    xbdStrategy (device, &bio);
    semTake (&sem, WAIT_FOREVER);          /* wait for completion */

    return (bio.bio_error);
}

/***************************************************************************
 *
 * xbdPartitionsDetect - detect any pre-existing partitions
 *
 * This routine searches for any existing partitions on the device.  It should
 * only be called by the file system monitor.  For each found partition, it
 * raises a secondary insertion event to be handled by the file system monitor.
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS xbdPartitionsDetect (
    device_t  device
    ) {
    lblkno_t  nBlks;
    unsigned  blkSize;
    int       error;
    char *    data;
    PARTITION_ENTRY  partTable[4];
    XBD_PARTITION *  pXbdPartitions;
    XBD_PART *       pXbdPart;
    devname_t        devname;

    if (((error = xbdBlockSize (device, &blkSize)) != OK) ||
        ((error = xbdNBlocks (device, &nBlks)) != OK)) {
        errnoSet (error);
        return (ERROR);
    }

    if ((data = (char *) malloc (blkSize)) == NULL) {
        errnoSet (ENOMEM);
        return (ERROR);
    }

    pXbdPartitions = (XBD_PARTITION *) malloc (sizeof (XBD_PARTITION));
    if (pXbdPartitions == NULL) {
        errnoSet (ENOMEM);
        free (data);
        return (ERROR);
    }

    if ((error = xbdSectorRead (device, 0, blkSize, data)) != OK) {
        goto errorReturn;
    }

    /*
     * For now, it shall be assumed that block size of the XBD device is not
     * less than 512 bytes.  The partition table is supposed to be located at
     * byte offset 0x1be (446).  Should the block size be smaller than 512
     * bytes, a developer should be able to spot the potential problem.
     */

    memcpy (partTable, data + 0x1be, sizeof (partTable));
    memset (pXbdPartitions, 0, sizeof (XBD_PARTITION));

    /*
     * Initialize XBD partitions.
     * TODO: Add code to handle detection of extended partitions.
     */

    pXbdPart = &(pXbdPartitions->xbdParts[0]);

    strcpy (devname, xbdDevName (pXbdPartitions->xbdPartitions->subDevice));

    for (i = 0; i < 4; i++) {
        if (partTable[i].nSectors == 0) {
            continue;
        }

        pXbdPart->isBootable = ((partTable[i].bootIndicator & 0x80) != 0x00);
        pXbdPart->head[0] = partTable[i].headStart;
        pXbdPart->head[1] = partTable[i].headEnd;
        pXbdPart->sector[0] = partTable[i].secStart;
        pXbdPart->sector[1] = partTable[i].secEnd;
        pXbdPart->cylinder[0] = partTable[i].cylStart;
        pXbdPart->cylinder[1] = partTable[i].cylEnd;
        pXbdPart->startBlock = (lblkno_t) partTable[i].sector;
        pXbdPart->startBlock = (lblkno_t) partTable[i].nSectors;
        pXbdPart->xbdPartitions = pXbdPartitions;

        /*
         * TODO:
         * Add code to sanity check the settings.  Skipping this for now.
         */

        /* Attach the XBD (pseudo-ish code at the moment) */
        status = xbdAttach (&pXbdPart->xbd, &xbdPartitionFuncs,
                            underlying device name + ":" + pXbdPartitions->nPartitions + 1,
                            blockSize, nBlks, &partitionDevice);

        if (status != OK) {
            error = errnoGet();
            goto errorReturn;
        }

        /* Attachment successful.  Raise the secondary insertion event */
        if (erfEventRaise (xbdEventCategory, xbdSecondaryInsertionEvent, 
                           partitionDevice, NULL) != OK) {
            error = errnoGet ();
            goto errorReturn;
        }

        /* Successful raising of the event.  Accept the partition as valid. */
        pXbdPart++;
        pXbdPartitions->nPartitions++;

    }

    return (OK);

errorReturn:
    free (data);
    free (pXbdPartition);
    errnoSet (error);
    return (ERROR);
}

/***************************************************************************
 *
 * xbdPartition- partition an XBD device
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS xbdPartition (
    char *  pathName,     /* path to device to partition */
    int     nPartitions,  /* number of partitions to create */
    int     size1,        /* % of device for first partition */
    int     size2,        /* % of device for second partition */
    int     size3         /* % of device for third partition */
    ) {
    /*
     * Stub routine.   Eventually will need to ...
     * 
     * 1.  Eject/unmount any file system currently residing on each partition.
     * 2.  Eject/unmount the partitions.
     * 3.  This should leave an unpartitioned device that uses rawFS.
     *     Hmmm ... if not rawFS, then directly access underlying device
     *     with XBDs.
     * 4.  Create the new partitions using rawFS (or XBDs if required).
     */

    return (OK);
}

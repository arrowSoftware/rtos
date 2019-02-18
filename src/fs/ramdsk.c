// Ramdisk

#include <stdlib.h>
#include <string.h>

#include <fs/fs.h>
#include <fs/fsvar.h>

#define RAMDSK_MAX	1
#define RAMDSK_BLKSIZE	512

unsigned char *ramdsk_buf[RAMDSK_MAX];

#define debug printf("ramdsk.c:%d\n", __LINE__);
int ramdsk_init(int diskno, unsigned int nsecs)
{
    printf("ramdsk.c:%d ramdsk_init(%x, %d)\n", __LINE__, diskno, nsecs);
    struct superblock super;
debug
    ramdsk_buf[diskno] = (unsigned char *) malloc(nsecs * RAMDSK_BLKSIZE);
debug

    if (ramdsk_buf[diskno] == NULL)
    {
        return 1;
    }
debug

    memset(ramdsk_buf[diskno], 0x00, nsecs * RAMDSK_BLKSIZE);
debug

    super.size = nsecs;
    super.nblocks = nsecs;
    super.ninodes = 512;
    debug

    memcpy(ramdsk_buf[diskno], (unsigned char *) &super, sizeof(super));
debug

    return 0;
}

void ramdsk_deinit(int diskno)
{
  free(ramdsk_buf[diskno]);
}

// Run an entire disk operation.
void ramdsk_rw(int diskno, unsigned int secno, void *addr, unsigned int nsecs, int read)
{
  if (read)
    memcpy((unsigned char *) addr,
	ramdsk_buf[diskno] + secno * RAMDSK_BLKSIZE,
	nsecs * RAMDSK_BLKSIZE);
  else
    memcpy(ramdsk_buf[diskno] + secno * RAMDSK_BLKSIZE,
	(unsigned char *) addr,
	nsecs * RAMDSK_BLKSIZE);
}


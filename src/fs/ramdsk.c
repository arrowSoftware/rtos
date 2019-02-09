// Ramdisk

#include <stdlib.h>
#include <string.h>

#include <fs/fs.h>
#include <fs/fsvar.h>

#define RAMDSK_MAX	1
#define RAMDSK_BLKSIZE	512

unsigned char *ramdsk_buf[RAMDSK_MAX];

int ramdsk_init(int diskno, unsigned int nsecs)
{
  struct superblock super;

  if ( (ramdsk_buf[diskno] =
	(unsigned char *) malloc(nsecs * RAMDSK_BLKSIZE)) == NULL)
    return 1;

  memset(ramdsk_buf[diskno], 0x00, nsecs * RAMDSK_BLKSIZE);

  super.size = nsecs;
  super.nblocks = nsecs;
  super.ninodes = 512;

  memcpy(ramdsk_buf[diskno], (unsigned char *) &super, sizeof(super));

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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>

#include <fs/param.h>
#include <fs/fs.h>

#include <fs/common.h>

int nblocks = 1008;
int ninodes = 100;
int size = 1024;

int fsfd;
struct superblock sb;
char zeroes[512];
unsigned int freeblock;
unsigned int usedblocks;
unsigned int bitblocks;
unsigned int freeinode = 1;

void mkfs_balloc(int);
void wsect(unsigned int, void*);
void winode(unsigned int, struct dinode*);
void rinode(unsigned int inum, struct dinode *ip);
void rsect(unsigned int sec, void *buf);
static unsigned int mkfs_ialloc(unsigned short type);
void iappend(unsigned int inum, void *p, int n);
extern void ramdsk_rw(int diskno, unsigned int secno, void *addr, unsigned int nsecs, int read);

// convert to intel byte order
unsigned short
xshort(unsigned short x)
{
  unsigned short y;
  unsigned char *a = (unsigned char*) &y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

unsigned int
xint(unsigned int x)
{
  unsigned int y;
  unsigned char *a = (unsigned char*) &y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}

int mkfs(void)
{
  int i, cc, fd;
  unsigned int bn, rootino, inum, off;
  struct dirent de;
  char buf[512];
  struct dinode din;

/*
  if(argc < 2){
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }
*/

  assert((512 % sizeof(struct dinode)) == 0);
  assert((512 % sizeof(struct dirent)) == 0);

/*
  fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
  if(fsfd < 0){
    perror(argv[1]);
    exit(1);
  }
*/

  sb.size = xint(size);
  sb.nblocks = xint(nblocks); // so whole disk is size sectors
  sb.ninodes = xint(ninodes);

  bitblocks = size/(512*8) + 1;
  usedblocks = ninodes / IPB + 3 + bitblocks;
  freeblock = usedblocks;

  printf("used %d (bit %d ninode %d) free %d total %d\n", usedblocks,
         bitblocks, ninodes/IPB + 1, freeblock, nblocks+usedblocks);

  assert(nblocks + usedblocks == size);

  for(i = 0; i < nblocks + usedblocks; i++)
    wsect(i, zeroes);

  wsect(1, &sb);

  rootino = mkfs_ialloc(T_DIR);
  assert(rootino == 1);

  //bzero(&de, sizeof(de));
  memset(&de, 0, sizeof(de));
  de.d_ino = xshort(rootino);
  strcpy(de.d_name, ".");
  iappend(rootino, &de, sizeof(de));

  //bzero(&de, sizeof(de));
  memset(&de, 0, sizeof(de));
  de.d_ino = xshort(rootino);
  strcpy(de.d_name, "..");
  iappend(rootino, &de, sizeof(de));

/*
  for(i = 2; i < argc; i++){
    assert(index(argv[i], '/') == 0);

    if((fd = open(argv[i], 0)) < 0){
      perror(argv[i]);
      exit(1);
    }
    
    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    if(argv[i][0] == '_')
      ++argv[i];

    inum = ialloc(T_FILE);

    bzero(&de, sizeof(de));
    de.inum = xshort(inum);
    strncpy(de.name, argv[i], DIRSIZ);
    iappend(rootino, &de, sizeof(de));

    while((cc = read(fd, buf, sizeof(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }
*/

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.size);
  off = ((off/BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  winode(rootino, &din);

  mkfs_balloc(usedblocks);

  return 0;
}

void
wsect(unsigned int sec, void *buf)
{
/*
  if(lseek(fsfd, sec * 512L, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(write(fsfd, buf, 512) != 512){
    perror("write");
    exit(1);
  }
*/
  ramdsk_rw(pRootMount->mnt_dev, sec, buf, 1, 0);
}

unsigned int
i2b(unsigned int inum)
{
  return (inum / IPB) + 2;
}

void
winode(unsigned int inum, struct dinode *ip)
{
  char buf[512];
  unsigned int bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode*) buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

void
rinode(unsigned int inum, struct dinode *ip)
{
  char buf[512];
  unsigned int bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode*) buf) + (inum % IPB);
  *ip = *dip;
}

void
rsect(unsigned int sec, void *buf)
{
/*
  if(lseek(fsfd, sec * 512L, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(read(fsfd, buf, 512) != 512){
    perror("read");
    exit(1);
  }
*/
  ramdsk_rw(pRootMount->mnt_dev, sec, buf, 1, 1);
}

static unsigned int
mkfs_ialloc(unsigned short type)
{
  unsigned int inum = freeinode++;
  struct dinode din;

  //bzero(&din, sizeof(din));
  memset(&din, 0, sizeof(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  return inum;
}

void
mkfs_balloc(int used)
{
  unsigned char buf[512];
  int i;

  printf("balloc: first %d blocks have been allocated\n", used);
  assert(used < 512);
  //bzero(buf, 512);
  memset(buf, 0, 512);
  for(i = 0; i < used; i++) {
    buf[i/8] = buf[i/8] | (0x1 << (i%8));
  }
  printf("balloc: write bitmap block at sector %d\n", ninodes/IPB + 3);
  wsect(ninodes / IPB + 3, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void
iappend(unsigned int inum, void *xp, int n)
{
  char *p = (char*) xp;
  unsigned int fbn, off, n1;
  struct dinode din;
  char buf[512];
  unsigned int indirect[NINDIRECT];
  unsigned int x;

  rinode(inum, &din);

  off = xint(din.size);
  while(n > 0){
    fbn = off / 512;
    assert(fbn < MAXFILE);
    if(fbn < NDIRECT) {
      if(xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
        usedblocks++;
      }
      x = xint(din.addrs[fbn]);
    } else {
      if(xint(din.addrs[INDIRECT]) == 0) {
        // printf("allocate indirect block\n");
        din.addrs[INDIRECT] = xint(freeblock++);
        usedblocks++;
      }
      // printf("read indirect block\n");
      rsect(xint(din.addrs[INDIRECT]), (char*) indirect);
      if(indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        usedblocks++;
        wsect(xint(din.addrs[INDIRECT]), (char*) indirect);
      }
      x = xint(indirect[fbn-NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * 512 - off);
    rsect(x, buf);
    //bcopy(p, buf + off - (fbn * 512), n1);
    memcpy(buf + off - (fbn * 512), p, n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
  winode(inum, &din);
}

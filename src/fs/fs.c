#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include <fs/buf.h>
#include <fs/fs.h>
#include <fs/fsvar.h>
#include <fs/param.h>
#include <fs/defs.h>

#include <fs/common.h>

#include <fs/vnode.h>
#include <fs/mount.h>

#define debug printf("fs.c:%d\n", __LINE__);

// Inode table.  The inode table is an in-memory cache of the 
// on-disk inode structures.  If an inode in the table has a non-zero
// reference count, then some open files refer to it and it must stay
// in memory.  If an inode has a zero reference count, it is only in
// memory as a cache in hopes of being used again (avoiding a disk read).
// Any inode with reference count zero can be evicted from the table.
// 
// In addition to having a reference count, inodes can be marked busy
// (just like bufs), meaning that some code has logically locked the 
// inode, and others are not allowed to look at it. 
// This locking can last for a long
// time (for example, if the inode is busy during a disk access),
// so we don't use spin locks.  Instead, if a process wants to use
// a particular inode, it must sleep(ip) to wait for it to be not busy.
// See iget below.
struct inode inode[NINODE];
//struct spinlock inode_table_lock;

struct vnode * pRootSyncer;
struct mount * pRootMount;

void
iinit(void)
{
  //initlock(&inode_table_lock, "inode_table");
}

// Allocate a disk block.
static unsigned int
balloc(unsigned int dev)
{
  int b;
  struct buf *bp;
  struct superblock *sb;
  int bi = 0;
  int size;
  int ninodes;
  unsigned char m;

  bread (pRootSyncer, 1, 512, NULL, &bp);
  sb = (struct superblock*) bp->b_data;
  size = sb->size;
  ninodes = sb->ninodes;

  for(b = 0; b < size; b++) {
    if(b % BPB == 0) {
      brelse(bp);
      bread (pRootSyncer, BBLOCK(b, ninodes), 512, NULL, &bp);
    }
    bi = b % BPB;
    m = 0x1 << (bi % 8);
    if((bp->b_data[bi/8] & m) == 0) {  // is block free?
      break;
    }
  }
  if(b >= size)
    panic("balloc: out of blocks");

  bp->b_data[bi/8] |= 0x1 << (bi % 8);
  bwrite (bp);           /* mark it allocated on disk */
  brelse (bp);
  return b;
}

// Free a disk block.
static void
bfree(int dev, unsigned int b)
{
  struct buf *bp;
  struct superblock *sb;
  int bi;
  int ninodes;
  unsigned char m;

  bread (pRootSyncer, 1, 512, NULL, &bp);
  sb = (struct superblock*) bp->b_data;
  ninodes = sb->ninodes;
  brelse(bp);

  bread (pRootSyncer, b, 512, NULL, &bp);
  memset (bp->b_data, 0, BSIZE);
  bwrite (bp);
  brelse (bp);

  bread (pRootSyncer, BBLOCK(b, ninodes), 512, NULL, &bp);
  bi = b % BPB;
  m = ~(0x1 << (bi %8));
  bp->b_data[bi/8] &= m;
  bwrite (bp);  /* mark it free on disk */
  brelse (bp);
}

// Find the inode with number inum on device dev
// and return an in-memory copy.  Loads the inode
// from disk into the in-core table if necessary.
// The returned inode has busy set and has its ref count incremented.
// Caller must iput the return value when done with it.
struct inode *iget(unsigned int dev, unsigned int inum)
{
    printf("fs.c:%d iget(%d, %d)\n", __LINE__, dev, inum);
    struct inode *ip, *nip;
    struct dinode *dip;
    struct buf *bp;

    //acquire(&inode_table_lock);

    loop:
    nip = 0;
debug
    for(ip = &inode[0]; ip < &inode[NINODE]; ip++)
    {
debug
        if(ip->ref > 0 && ip->dev == dev && ip->inum == inum)
        {
debug
            if(ip->busy)
            {
debug
                //sleep(ip, &inode_table_lock);
                // Since we droped inode_table_lock, ip might have been reused
                // for some other inode entirely.  Must start the scan over,
                // and hopefully this time we will find the inode we want
                // and it will not be busy.
                goto loop;
            }
debug
            ip->ref++;
            ip->busy = 1;
            //release(&inode_table_lock);
            return ip;
        }
debug
        if(nip == 0 && ip->ref == 0)
        {
            nip = ip;
        }
debug
    }
debug
    if(nip == 0)
    {
        panic("out of inodes");
    }
debug
    nip->dev = dev;
    nip->inum = inum;
    nip->ref = 1;
    nip->busy = 1;
debug
    //release(&inode_table_lock);
debug
    bread(pRootSyncer, IBLOCK(inum), 512, NULL, &bp);
debug
    dip = &((struct dinode*)(bp->b_data))[inum % IPB];
    nip->type = dip->type;
    nip->major = dip->major;
    nip->minor = dip->minor;
    nip->nlink = dip->nlink;
    nip->size = dip->size;
    memmove(nip->addrs, dip->addrs, sizeof(nip->addrs));
debug
    brelse(bp);
debug
    return nip;
}

// Copy inode in memory, which has changed, to disk.
// Caller must have locked ip.
void
iupdate(struct inode *ip)
{
  struct buf *bp;
  struct dinode *dip;

#if 0
  bp = bread(ip->dev, IBLOCK(ip->inum));
#endif
  bread (pRootSyncer, IBLOCK (ip->inum), 512, NULL, &bp);
  dip = &((struct dinode*)(bp->b_data))[ip->inum % IPB];
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  memmove (dip->addrs, ip->addrs, sizeof(ip->addrs));
  bwrite (bp);   // mark it allocated on the disk
  brelse (bp);
}

// Allocate a new inode with the given type
// from the file system on device dev.
struct inode *ialloc(unsigned int dev, short type)
{
    struct inode *ip;
    struct dinode *dip = 0;
    struct superblock *sb;
    int ninodes;
    int inum;
    struct buf *bp;

  bread (pRootSyncer, 1, 512, NULL, &bp);
  sb = (struct superblock*) bp->b_data;
  ninodes = sb->ninodes;
  brelse(bp);

  for(inum = 1; inum < ninodes; inum++) {  // loop over inode blocks
    bread (pRootSyncer, IBLOCK(inum), 512, NULL, &bp);
    dip = &((struct dinode*)(bp->b_data))[inum % IPB];
    if(dip->type == 0) {  // a free inode
      break;
    }
    brelse(bp);
  }

  if(inum >= ninodes)
    panic("ialloc: no inodes left");

  memset(dip, 0, sizeof(*dip));
  dip->type = type;
  bwrite (bp);   /* mark it allocated on the disk */
  brelse (bp);
  ip = iget (dev, inum);
  return ip;
}

// Free the given inode from its file system.
static void
ifree(struct inode *ip)
{
  ip->type = 0;
  iupdate(ip);
}

// Lock the given inode (wait for it to be not busy,
// and then ip->busy).  
// Caller must already hold a reference to ip.
// Otherwise, if all the references to ip go away,
// it might be reused underfoot.
void
ilock(struct inode *ip)
{
  if(ip->ref < 1)
    panic("ilock");

  //acquire(&inode_table_lock);

  while(ip->busy)
    ;//sleep(ip, &inode_table_lock);
  ip->busy = 1;

  //release(&inode_table_lock);
}

// Caller holds reference to ip and has locked it.
// Caller no longer needs to examine / change it.
// Unlock it, but keep the reference.
void
iunlock(struct inode *ip)
{
  if(ip->busy != 1 || ip->ref < 1)
    panic("iunlock");

  //acquire(&inode_table_lock);

  ip->busy = 0;
  //wakeup(ip);

  //release(&inode_table_lock);
}

// Return the disk block address of the nth block in inode ip.
unsigned int
bmap(struct inode *ip, unsigned int bn)
{
  unsigned x;
  unsigned int *a;
  struct buf *inbp;

  if(bn >= MAXFILE)
    panic("bmap 1");
  if(bn < NDIRECT) {
    x = ip->addrs[bn];
    if(x == 0)
      panic("bmap 2");
  } else {
    if(ip->addrs[INDIRECT] == 0)
      panic("bmap 3");
#if 0
    inbp = bread(ip->dev, ip->addrs[INDIRECT]);
#endif
    bread (pRootSyncer, ip->addrs[INDIRECT], 512, NULL, &inbp);
    a = (unsigned int*) inbp->b_data;
    x = a[bn - NDIRECT];
    brelse(inbp);
    if(x == 0)
      panic("bmap 4");
  }
  return x;
}

// Truncate the inode ip, discarding all its data blocks.
void
itrunc(struct inode *ip)
{
  int i, j;
  struct buf *inbp;

  for(i = 0; i < NADDRS; i++) {
    if(ip->addrs[i] != 0) {
      if(i == INDIRECT) {
#if 0
        inbp = bread(ip->dev, ip->addrs[INDIRECT]);
#endif
        bread (pRootSyncer, ip->addrs[INDIRECT], 512, NULL, &inbp);
        unsigned int *a = (unsigned int*) inbp->b_data;
        for(j = 0; j < NINDIRECT; j++) {
          if(a[j] != 0) {
            bfree(ip->dev, a[j]);
            a[j] = 0;
          }
        }
        brelse(inbp);
      }
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }
  ip->size = 0;
  iupdate(ip);
}

// Caller holds reference to ip and has locked it,
// possibly editing it.
// Release lock and drop the reference.
void
iput(struct inode *ip)
{
  if(ip->ref < 1 || ip->busy != 1)
    panic("iput");

  if((ip->ref == 1) && (ip->nlink == 0)) {
    itrunc(ip);
    ifree(ip);
  }

  //acquire(&inode_table_lock);

  ip->ref -= 1;
  ip->busy = 0;
  //wakeup(ip);

  //release(&inode_table_lock);
}

// Caller holds reference to ip but not lock.
// Drop reference.
void
idecref(struct inode *ip)
{
  ilock(ip);
  iput(ip);
}

// Increment reference count for ip.
void
iincref(struct inode *ip)
{
  ilock(ip);
  ip->ref++;
  iunlock(ip);
}

// Copy stat information from inode.
void
stati(struct inode *ip, struct stat *st)
{
  st->st_dev = ip->dev;
  st->st_ino = ip->inum;
  st->st_mode = ip->type;
  st->st_nlink = ip->nlink;
  st->st_size = ip->size;
}

#define min(a, b) ((a) < (b) ? (a) : (b))

// Read data from inode.
int
readi(struct inode *ip, char *dst, unsigned int off, unsigned int n)
{
  unsigned int target = n, n1;
  struct buf *bp;

  while(n > 0 && off < ip->size){
#if 0
    bp = bread(ip->dev, bmap(ip, off / BSIZE));
#endif
    bread (pRootSyncer, bmap(ip, off / BSIZE), 512, NULL, &bp);
    n1 = min(n, ip->size - off);
    n1 = min(n1, BSIZE - (off % BSIZE));
    memmove(dst, bp->b_data + (off % BSIZE), n1);
    n -= n1;
    off += n1;
    dst += n1;
    brelse(bp);
  }

  return target - n;
}

// Allocate the nth block in inode ip if necessary.
static int
newblock(struct inode *ip, unsigned int lbn)
{
  struct buf *inbp;
  unsigned int *inaddrs;
  unsigned int b;

  if(lbn < NDIRECT) {
    if(ip->addrs[lbn] == 0) {
      b = balloc(ip->dev);
      if(b <= 0)
        return -1;
      ip->addrs[lbn] = b;
    }
  } else {
    if(ip->addrs[INDIRECT] == 0) {
      b = balloc(ip->dev);
      if(b <= 0)
        return -1;
      ip->addrs[INDIRECT] = b;
    }
#if 0
    inbp = bread(ip->dev, ip->addrs[INDIRECT]);
#endif
    bread (pRootSyncer, ip->addrs[INDIRECT], 512, NULL, &inbp);
    inaddrs = (unsigned int*) inbp->b_data;
    if(inaddrs[lbn - NDIRECT] == 0) {
      b = balloc(ip->dev);
      if(b <= 0)
        return -1;
      inaddrs[lbn - NDIRECT] = b;
      bwrite (inbp);
    }
    brelse (inbp);
  }
  return 0;
}

// Write data to inode.
int
writei(struct inode *ip, char *addr, unsigned int off, unsigned int n)
{
  if(ip->type == T_FILE || ip->type == T_DIR) {
    struct buf *bp;
    int r = 0;
    int m;
    int lbn;
    while(r < n) {
      lbn = off / BSIZE;
      if(lbn >= MAXFILE)
        return r;
      if(newblock(ip, lbn) < 0) {
        printf("newblock failed\n");
        return r;
      }
      m = min(BSIZE - off % BSIZE, n-r);
#if 0
      bp = bread(ip->dev, bmap(ip, lbn));
#endif
      bread (pRootSyncer, bmap(ip, lbn), 512, NULL, &bp);
      memmove (bp->b_data + off % BSIZE, addr, m);
      bwrite (bp);
      brelse (bp);
      r += m;
      off += m;
    }
    if(r > 0) {
      if(off > ip->size) {
        if(ip->type == T_DIR)
          ip->size = ((off / BSIZE) + 1) * BSIZE;
        else
          ip->size = off;
      }
      iupdate(ip);
    }
    return r;
  } else {
    panic("writei: unknown type");
    return 0;
  }
}

// look up a path name, in one of three modes.
// NAMEI_LOOKUP: return locked target inode.
// NAMEI_CREATE: return locked parent inode.
//   return 0 if name does exist.
//   *ret_last points to last path component (i.e. new file name).
//   *ret_ip points to the the name that did exist, if it did.
//   *ret_ip and *ret_last may be zero even if return value is zero.
// NAMEI_DELETE: return locked parent inode, offset of dirent in *ret_off.
//   return 0 if name doesn't exist.
struct inode *namei(char *path,
                    int mode,
                    unsigned int *ret_off,
                    char **ret_last,
                    struct inode **ret_ip)
{
    struct inode *dp;
    char *cp = path, *cp1;
    unsigned int off, dev;
    struct buf *bp;
    struct dirent *ep;
    int i, l, atend;
    unsigned int ninum;
debug
    if(ret_off)
    {
        *ret_off = 0xffffffff;
    }

    if(ret_last)
    {
        *ret_last = 0;
    }

    if(ret_ip)
    {
        *ret_ip = 0;
    }
debug
    if(*cp == '/')
    {
debug
        dp = iget(pRootMount->mnt_dev, 1);
debug
    }
    else
    {
        dp = cwd;
debug
        iincref(dp);
debug
        ilock(dp);
debug
    }
debug
    for(;;)
    {
        while(*cp == '/')
        {
            cp++;
        }
debug
    if(*cp == '\0')
    {
        if(mode == NAMEI_LOOKUP)
        {
            return dp;
        }

        if(mode == NAMEI_CREATE && ret_ip)
        {
            *ret_ip = dp;
            return 0;
        }

        iput(dp);
        return 0;
    }
debug
    if(dp->type != T_DIR)
    {
        iput(dp);
        return 0;
    }

    for(i = 0; cp[i] != 0 && cp[i] != '/'; i++);
    {
        l = i;
    }

    if(i > DIRSIZ)
    {
        l = DIRSIZ;
    }
debug
    for(off = 0; off < dp->size; off += BSIZE){
#if 0
      bp = bread(dp->dev, bmap(dp, off / BSIZE));
#endif
      bread (pRootSyncer, bmap (dp, off / BSIZE), 512, NULL, &bp);
      for(ep = (struct dirent*) bp->b_data;
          ep < (struct dirent*) (bp->b_data + BSIZE);
          ep++){
        if(ep->d_ino == 0)
          continue;
        if(memcmp(cp, ep->d_name, l) == 0 &&
           (l == DIRSIZ || ep->d_name[l]== 0)){
          // entry matches path element
          off += (unsigned char*)ep - bp->b_data;
          ninum = ep->d_ino;
          brelse(bp);
          cp += i;
          goto found;
        }
      }
      brelse(bp);
    }
    atend = 1;
    for(cp1 = cp; *cp1; cp1++)
      if(*cp1 == '/')
        atend = 0;
    if(mode == NAMEI_CREATE && atend){
      if(*cp == '\0'){
        iput(dp);
        return 0;
      }
      *ret_last = cp;
      return dp;
    }

    iput(dp);
    return 0;

  found:
    if(mode == NAMEI_DELETE && *cp == '\0'){
      *ret_off = off;
      return dp;
    }
    dev = dp->dev;
    iput(dp);
    dp = iget(dev, ninum);
    if(dp->type == 0 || dp->nlink < 1)
      panic("namei");
  }
}

// Write a new directory entry (name, ino) into the directory dp.
// Caller must have locked dp.
void
wdir(struct inode *dp, char *name, unsigned int ino)
{
  unsigned int off;
  struct dirent de;
  int i;

  for(off = 0; off < dp->size; off += sizeof(de)){
    if(readi(dp, (char*) &de, off, sizeof(de)) != sizeof(de))
      panic("wdir read");
    if(de.d_ino == 0)
      break;
  }

  de.d_ino = ino;
  for(i = 0; i < DIRSIZ && name[i]; i++)
    de.d_name[i] = name[i];
  for( ; i < DIRSIZ; i++)
    de.d_name[i] = '\0';

  if(writei(dp, (char*) &de, off, sizeof(de)) != sizeof(de))
    panic("wdir write");
}

// Create a new inode named name inside dp
// and return its locked inode structure.
// If name already exists, return 0.
struct inode*
mknod1(struct inode *dp, char *name, short type, short major, short minor)
{
  struct inode *ip;

  ip = ialloc(dp->dev, type);
  if(ip == 0)
    return 0;
  ip->major = major;
  ip->minor = minor;
  ip->size = 0;
  ip->nlink = 1;

  iupdate(ip);  // write new inode to disk

  wdir(dp, name, ip->inum);

  return ip;
}

// Create the path cp and return its locked inode structure.
// If cp already exists, return 0.
struct inode*
mknod(char *cp, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char *last;

  if((dp = namei(cp, NAMEI_CREATE, 0, &last, 0)) == 0)
    return 0;

  ip = mknod1(dp, last, type, major, minor);

  iput(dp);

  return ip;
}

// Unlink the inode named cp.
int
unlinki(char *cp)
{
  struct inode *ip, *dp;
  struct dirent de;
  unsigned int off, inum, dev;

  dp = namei(cp, NAMEI_DELETE, &off, 0, 0);
  if(dp == 0)
    return -1;

  dev = dp->dev;

  if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de) || de.d_ino == 0)
    panic("unlink no entry");

  inum = de.d_ino;

  memset(&de, 0, sizeof(de));
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink dir write");

  iupdate(dp);
  iput(dp);

  ip = iget(dev, inum);

  if(ip->nlink < 1)
    panic("unlink nlink < 1");

  ip->nlink--;

  iupdate(ip);
  iput(ip);

  return 0;
}

// Create the path new as a link to the same inode as old.
int
linki(char *name1, char *name2)
{
  struct inode *ip, *dp;
  char *last;

  if((ip = namei(name1, NAMEI_LOOKUP, 0, 0, 0)) == 0)
    return -1;
  if(ip->type == T_DIR){
    iput(ip);
    return -1;
  }

  iunlock(ip);

  if((dp = namei(name2, NAMEI_CREATE, 0, &last, 0)) == 0) {
    idecref(ip);
    return -1;
  }
  if(dp->dev != ip->dev){
    idecref(ip);
    iput(dp);
    return -1;
  }

  ilock(ip);
  ip->nlink++;
  iupdate(ip);

  wdir(dp, last, ip->inum);
  iput(dp);
  iput(ip);

  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <fs/buf.h>
#include <fs/param.h>
#include <fs/defs.h>
#include <fs/fs.h>
#include <fs/fsvar.h>
#include <fs/mount.h>

extern int sys_open(char *path, int omode);
extern int sys_read(int fd, char *cp, int n);
extern int sys_write(int fd, char *cp, int n);
extern int sys_close(int fd);
extern int ramdsk_init(int diskno, unsigned int nsecs);
extern int mkfs(void);
extern int sys_mkdir(char *path);
extern void ramdsk_deinit(int diskno);


/*
 * Most of the fields in the following structure are filled with
 * dummy values.  When the VFS is complete, NULL function pointers
 * will not be allowed.
 */

vfsops_t vfsops = {
    "ram",                 /* a string identifying type of device */
    128,                   /* arbitrary value at the moment */
    sizeof (struct inode), /* size of the in-memory inode */
    1,                     /* # of 32-bit words in cookie */
    NULL,                  /* ptr to FS specific mount routine */
    NULL,                  /* ptr to FS specific unmount routine */
    NULL,                  /* ptr to FS specific routine to get root vnode */
    NULL,                  /* ptr to FS specific statvfs routine */
    NULL,                  /* ptr to FS specific routine to get any vnode */
    NULL,                  /* ptr to FS specific initialization routine */
    NULL,                  /* ptr to FS specific re-initialization routine */
    NULL,                  /* ptr to FS specific done routine */
    NULL,                  /* ptr to FS specific start transaction routine */
    NULL                   /* ptr to FS specific end transaction routine */
};

unsigned char blkbuf[512];
struct buf *pbuf;
struct inode *pinode;

struct inode*
ialloc(unsigned int dev, short type);

void print_blk(struct buf *pbuf)
{
  int  i;

  for (i = 0; i < 512; i++) {
    if ((i % 16) == 0) printf("\n");
    printf("%c", pbuf->b_data[i]);
  }
  printf("\n---------------------------------------------------------------\n");
}

void test_blkdev(void)
{
  bread (pRootSyncer, 256, 512, NULL, &pbuf);
  if (pbuf == NULL) {
    fprintf (stderr, "Error unable to retive block\n");
    exit (1);
  }

  print_blk (pbuf);

  strcpy ((char *) &pbuf->b_data[64], "This is a test!");

  bwrite (pbuf);
  brelse (pbuf);

  bread (pRootSyncer, 256, 512, NULL, &pbuf);
  if (pbuf == NULL) {
    fprintf(stderr, "Error unable to retive block\n");
    exit(1);
  }
  print_blk(pbuf);

}

void print_inode(struct inode *pinode)
{
  int i;

  printf("dev: %d, "
	 "inum: %d, "
	 "ref: %d, "
	 "busy: %d, "
	 "type: %d\n"
	 "major: %d, "
	 "minor: %d, "
	 "nlink: %d, "
	 "size: %d",
	 pinode->dev,
	 pinode->inum,
	 pinode->ref,
	 pinode->busy,
	 pinode->type,
	 pinode->major,
	 pinode->minor,
	 pinode->nlink,
	 pinode->size);
  for (i = 0; i < NADDRS; i++) {
    if ( (i % 6) == 0) printf("\n");
    printf("addrs[%d]: %d, ", i, pinode->addrs[i]);
  }
  printf("\n");
}

void test_inode(void)
{
  int inum;

  if ( (pinode = ialloc(pRootMount->mnt_dev, 1)) == NULL) {
    fprintf(stderr, "Error unable to allocate inode\n");
    exit(1);
  }
  print_inode(pinode);
  inum = pinode->inum;
  //ilock(pinode);
  iupdate(pinode);
  iunlock(pinode);

  if ( (pinode = iget(pRootMount->mnt_dev, inum)) == NULL) {
    fprintf(stderr, "Error unable to find inode %d\n", inum);
    exit(1);
  }
  print_inode(pinode);
}

void test_filename(void)
{
  unsigned int offset;
  char **name;
  struct inode *dip;

  printf("Create /usr\n");
  if ( (pinode = namei("/usr", NAMEI_CREATE,
		       &offset, name, &dip) ) == NULL) {
    fprintf(stderr, "Error unable to resolve pathname\n");
    exit(1);
  }
  cwd = pinode;

  printf("Created file: %s\n", *name);
  if (dip)
    print_inode(dip);
  print_inode(pinode);

/*
  printf("Create /usr/test\n");
  if ( (pinode = namei("test", NAMEI_CREATE,
		       &offset, name, &dip) ) == NULL) {
    fprintf(stderr, "Error unable to resolve pathname\n");
    exit(1);
  }
*/
}

static int buf_size;
void fill_file(char *path)
{
  int fd, n;
  char buf[] = "Lexme";

  if ( (fd = sys_open(path, O_RDWR)) < 0) {
    fprintf(stderr, "Error unable to resolve pathname\n");
    exit(1);
  }

  buf_size = strlen(buf);
  if ( (n = sys_write(fd, buf, buf_size)) < 0) {
    fprintf(stderr, "Error unable to write: %d\n", n);
    exit(1);
  }

  printf("Wrote %s with %d bytes to file\n", buf, n);

  sys_close(fd);
}

void check_file(char *path)
{
  int fd, n;
  char buf[] = "...........................";

  if ( (fd = sys_open(path, 0)) < O_RDONLY) {
    fprintf(stderr, "Error unable to resolve pathname\n");
    exit(1);
  }

  printf("Found file (fd = %d): %s\n", fd, path);

  if ( (n = sys_read(fd, buf, buf_size)) < 0) {
    fprintf(stderr, "Error unable to read from file\n");
    exit(1);
  }

  printf("Read %d bytes from file: %s\n", n, buf);

  sys_close(fd);
}

#define debug printf("fs_test.c:%d\n", __LINE__);
int fs_test(void)
{
    int fd;
    int error;
debug
    error = mountCreate(&vfsops, 1, 10, "/devname", &pRootMount);
debug
    if (error != OK)
    {
        return error;
    }
debug
    pRootSyncer = pRootMount->mnt_syncer;
debug
    if (ramdsk_init(pRootMount->mnt_dev, 1024) != 0)
    {
        fprintf(stderr, "Error initializing ramdisk\n");
        return 1;
    }
debug
    mkfs();
debug
    binit();
debug
    iinit();
debug
    fileinit();
debug
    //test_blkdev();
    //test_inode();
    //test_inode();
    //test_filename();
debug
    sys_mkdir("/usr");
debug
    if ( (fd = sys_open("/usr/file", O_CREAT)) < 0)
    {
        fprintf(stderr, "Error unable to create file\n");
        return 1;
    }
debug
    sys_close(fd);
debug
    fill_file("/usr/file");
debug
    check_file("/usr/file");
debug
    ramdsk_deinit(pRootMount->mnt_dev);
debug
    return 0;
}


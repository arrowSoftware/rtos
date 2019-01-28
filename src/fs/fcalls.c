#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#include <fs/param.h>
#include <fs/defs.h>
#include <fs/buf.h>
#include <fs/fs.h>
#include <fs/fsvar.h>
#include <fs/file.h>

#include <fs/common.h>

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argfd(int fd, struct file **pf)
{
  struct file *f;

  if(fd < 0 || fd >= NOFILE || (f=ofile[fd]) == 0)
    return -1;
  if(pf)
    *pf = f;
  return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
  int fd;

  for(fd = 0; fd < NOFILE; fd++){
    if(ofile[fd] == 0){
      ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

int
sys_write(int fd, char *cp, int n)
{
  struct file *f;

  if(argfd(fd, &f) < 0)
    return -1;
  return filewrite(f, cp, n);
}

int
sys_read(int fd, char *cp, int n)
{
  struct file *f;

  if(argfd(fd, &f) < 0)
    return -1;
  return fileread(f, cp, n);
}

int
sys_close(int fd)
{
  struct file *f;
  
  if(argfd(fd, &f) < 0)
    return -1;
  ofile[fd] = 0;
  fileclose(f);
  return 0;
}

int
sys_open(char *path, int omode)
{
  struct inode *ip, *dp;
  int fd;
  struct file *f;
  char *last;

  if(omode & O_CREAT){
    dp = namei(path, NAMEI_CREATE, 0, &last, &ip);
    if(dp){
      ip = mknod1(dp, last, T_FILE, 0, 0);
      iput(dp);
      if(ip == 0)
        return -1;
    } else if(ip == 0){
      return -1;
    } else if(ip->type == T_DIR){
      iput(ip);
      return -1;
    }
  } else {
    ip = namei(path, NAMEI_LOOKUP, 0, 0, 0);
    if(ip == 0)
      return -1;
  }
  if(ip->type == T_DIR && ((omode & O_RDWR) || (omode & O_WRONLY)) ) {
    iput(ip);
    return -1;
  }

  if((f = filealloc()) == 0){
    iput(ip);
    return -1;
  }
  if((fd = fdalloc(f)) < 0){
    iput(ip);
    fileclose(f);
    return -1;
  }

  iunlock(ip);
  f->type = FD_FILE;
  if(omode & O_RDWR) {
    f->readable = 1;
    f->writable = 1;
  } else if(omode & O_WRONLY) {
    f->readable = 0;
    f->writable = 1;
  } else {
    f->readable = 1;
    f->writable = 0;
  }
  f->ip = ip;
  f->off = 0;

  return fd;
}

int
sys_mknod(char *path, int type, int major, int minor)
{
  struct inode *nip;
  int len;
  
  if(len >= DIRSIZ)
    return -1;

  if((nip = mknod(path, type, major, minor)) == 0)
    return -1;
  iput(nip);
  return 0;
}

int
sys_mkdir(char *path)
{
  struct inode *nip;
  struct inode *dp;
  struct dirent de;
  char *last;

  dp = namei(path, NAMEI_CREATE, 0, &last, 0);
  if(dp == 0)
    return -1;

  nip = mknod1(dp, last, T_DIR, 0, 0);
  if(nip == 0){
    iput(dp);
    return -1;
  }

  dp->nlink++;
  iupdate(dp);

  memset(de.d_name, '\0', DIRSIZ);
  de.d_name[0] = '.';
  de.d_ino = nip->inum;
  writei(nip, (char*) &de, 0, sizeof(de));

  de.d_ino = dp->inum;
  de.d_name[1] = '.';
  writei(nip, (char*) &de, sizeof(de), sizeof(de));

  iput(dp);
  iput(nip);

  return 0;
}

int
sys_chdir(char *path)
{
  struct inode *ip;

  if((ip = namei(path, NAMEI_LOOKUP, 0, 0, 0)) == 0)
    return -1;

  if(ip == cwd) {
    iput(ip);
    return 0;
  }

  if(ip->type != T_DIR) {
    iput(ip);
    return -1;
  }

  idecref(cwd);
  cwd = ip;
  iunlock(cwd);
  return 0;
}

int
sys_unlink(char *path)
{
  return unlinki(path);
}

int
sys_fstat(int fd, struct stat *st)
{
  struct file *f;
  
  if(argfd(fd, &f) < 0)
    return -1;
  return filestat(f, st);
}

int
sys_dup(void)
{
  struct file *f;
  int fd;
  
  if(argfd(fd, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  fileincref(f);
  return fd;
}

int
sys_link(char *old, char *new)
{
  return linki(old, new);
}


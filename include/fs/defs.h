#include <sys/stat.h>
#include <dirent.h>

#include <fs/param.h>
#include <fs/buf.h>
#include <fs/fs.h>
#include <fs/fsvar.h>
#include <fs/vnode.h>

// common.c
extern struct inode *cwd;

// spinlock.c
/*
struct spinlock;
void initlock(struct spinlock*, char*);
void acquire(struct spinlock*);
void release(struct spinlock*);
int holding(struct spinlock*);
void getcallerpcs(void*, unsigned int*);
*/

// bio.c
void binit (void);
struct buf* buf_getblk (struct vnode *pVnode, lblkno_t blkNum, unsigned nBytes);
int  bread (struct vnode *pVnode, lblkno_t lbn, int blkSize,
            struct ucred *pCred, struct buf **ppBuf);
int  bwrite (struct buf *pBuf);
void brelse (struct buf *pBuf);

// fs.c
extern unsigned int rootdev;
void iinit(void);
struct inode* iget(unsigned int, unsigned int);
void ilock(struct inode*);
void iunlock(struct inode*);
void itrunc(struct inode*);
void idecref(struct inode*);
void iincref(struct inode*);
void iput(struct inode*);
struct inode* namei(char*, int, unsigned int*, char**, struct inode**);
void stati(struct inode*, struct stat*);
int readi(struct inode*, char*, unsigned int, unsigned int);
int writei(struct inode*, char*, unsigned int, unsigned int);
struct inode* mknod(char*, short, short, short);
struct inode* mknod1(struct inode*, char*, short, short, short);
int unlinki(char*);
void iupdate(struct inode*);
int linki(char*, char*);

// file.c
struct stat;
void fileinit(void);
struct file* filealloc(void);
void fileclose(struct file*);
int fileread(struct file*, char*, int n);
int filewrite(struct file*, char*, int n);
int filestat(struct file*, struct stat*);
void fileincref(struct file*);


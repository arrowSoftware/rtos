/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2008 - 2009 Surplus Users Ham Society
*
*   Real rtos is free software: you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License as published by
*   the Free Software Foundation, either version 2.1 of the License, or
*   (at your option) any later version.
*
*   Real rtos is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with Real rtos.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

/* stdioLib.c - Base file for stdio */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <rtos.h>
#include <rtos/taskLib.h>
#include <rtos/taskHookLib.h>
#include <rtos/errnoLib.h>
#include <rtos/memPartLib.h>
#include <util/classLib.h>
#include <util/objLib.h>
#include <io/ioLib.h>
#include <stdio.h>

/* Imports */
IMPORT TCB_ID taskIdCurrent;

/* Locals */
LOCAL OBJ_CLASS	fpClass;
LOCAL BOOL	stdioLibInstalled = FALSE;
LOCAL BOOL	stdioFpCleanupHookDone = FALSE;

LOCAL void	stdioStdfpCleanup(TCB_ID tcbId);

/* Globals */
CLASS_ID	fpClassId = &fpClass;

/*******************************************************************************
 * stdioLibInit - Initialize stdio library
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS stdioLibInit(void)
{
  /* Check if already installed */
  if (stdioLibInstalled)
    return OK;

  /* Initialize class */
  if (classInit(fpClassId, sizeof(FILE),
		OFFSET(FILE, objCore),
		memSysPartId,
		(FUNCPTR) stdioFpCreate,
		(FUNCPTR) stdioFpInit,
		(FUNCPTR) stdioFpDestroy) != OK)
    return ERROR;

  /* Mark as installed */
  stdioLibInstalled = TRUE;

  return OK;
}

/*******************************************************************************
 * stdioFpCreate - Allocate file structure
 *
 * RETURNS: Pointer to FILE structure
 ******************************************************************************/

FILE* stdioFpCreate(void)
{
  FILE *fp;

  /* Check if library is installed */
  if (!stdioLibInstalled)
    return NULL;

  /* Allocate object */
  fp = objAlloc(fpClassId);
  if (fp == NULL)
    return NULL;

  /* Initialize object */
  if (stdioFpInit(fp) != OK) {
    objFree(fpClassId, fp);
    return NULL;
  }

  return fp;
}

/*******************************************************************************
 * stdioFpInit - Initialize file structure
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS stdioFpInit(FILE *fp)
{
  /* Check if installed library */
  if (!stdioLibInstalled)
    return ERROR;

  /* Initialize structure */
  fp->_p 		= NULL;			/* Current position */
  fp->_r		= 0;			/* Read space left */
  fp->_w		= 0;			/* Write space left */
  fp->_flags		= 1;			/* Flags */
  fp->_file		= -1;			/* Filedescriptor no. */
  fp->_bf._base		= NULL;			/* Start of buffer */
  fp->_bf._size		= 0;			/* Buffer size */
  fp->_lbfsize		= 0;			/* Line buffer size */
  fp->_ub._base		= NULL;			/* Ungetc buffer */
  fp->_ub._size		= 0;			/* Ungetc buffer size */
  fp->_lb._base		= NULL;			/* Line buffer */
  fp->_lb._size		= 0;			/* Line buffer size */
  fp->_blksize		= 0;			/* Block size */
  fp->_offset		= 0;			/* Offset */
  fp->taskId		= (int) taskIdCurrent;	/* Owner task */

  /* Initialize object class */
  objCoreInit(&fp->objCore, fpClassId);

  return OK;
}

/*******************************************************************************
 * stdioFpDestroy - Free file structure
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS stdioFpDestroy(FILE *fp)
{
  /* Check if installed library */
  if (!stdioLibInstalled)
    return ERROR;

  objCoreTerminate(&fp->objCore);

  return objFree(fpClassId, fp);
}

/*******************************************************************************
 * stdioStdfpCleanup - Reclaim standard file pointers
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void stdioStdfpCleanup(TCB_ID tcbId)
{
  int i;

  /* Close standard file pointers */
  for (i = 0; i < 3; i++)
    if (tcbId->taskStdFp[i] != NULL)
      fclose(tcbId->taskStdFp[i]);
}

/*******************************************************************************
 * stdioInitStd - Initalize startdard file pointers
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

LOCAL STATUS stdioInitStd(int stdFd)
{
  FILE *fp;

  /* Check if installed library */
  if (!stdioLibInstalled)
    return ERROR;

  /* Create object */
  fp = stdioFpCreate();
  if (fp == NULL)
    return ERROR;
  
  /* Select fileno. */
  switch(stdFd) {

    /* Set read only */
    case STDIN_FILENO:
	fp->_flags = __SRD;
	break;

    /* Set write only */
    case STDOUT_FILENO:
	fp->_flags = __SWR;
	break;

    /* Set write only without buffering */
    case STDERR_FILENO:
	fp->_flags = __SWRNBF;
	break;
  }

  /* Set fileno. in file struct */
  fp->_file = stdFd;

  /* File fp in task */
  taskIdCurrent->taskStdFp[stdFd] = fp;

  /* Task should deallocate std fps when deleted */
  if ( !stdioFpCleanupHookDone &&
       ((fp == stdout) || (fp == stdin) || (fp == stderr)) ) {

    /* Install task delete hook */
    taskDeleteHookAdd((FUNCPTR) stdioStdfpCleanup);
    stdioFpCleanupHookDone = TRUE;

  }

  return OK;
}

/*******************************************************************************
* stdioFp - Get file
*
* RETURNS: FILE pointer
*******************************************************************************/

FILE* stdioFp(int stdFd)
{
  /* Initialize if needed */
  if (taskIdCurrent->taskStdFp[stdFd] == NULL)
    stdioInitStd(stdFd);

  return taskIdCurrent->taskStdFp[stdFd];
}

/*******************************************************************************
* __stdin - Get stdin file
*
* RETURNS: stdin FILE pointer
*******************************************************************************/

FILE** __stdin(void)
{
  /* Initialize if needed */
  if (taskIdCurrent->taskStdFp[STDIN_FILENO] == NULL)
    stdioInitStd(STDIN_FILENO);

  return &taskIdCurrent->taskStdFp[STDIN_FILENO];
}

/*******************************************************************************
* __stdout - Get stdout file
*
* RETURNS: stdout FILE pointer
*******************************************************************************/

FILE** __stdout(void)
{
  /* Initialize if needed */
  if (taskIdCurrent->taskStdFp[STDOUT_FILENO] == NULL)
    stdioInitStd(STDOUT_FILENO);

  return &taskIdCurrent->taskStdFp[STDOUT_FILENO];
}

/*******************************************************************************
* __stderr - Get stderr file
*
* RETURNS: stderr FILE pointer
*******************************************************************************/

FILE** __stderr(void)
{
  /* Initialize if needed */
  if (taskIdCurrent->taskStdFp[STDERR_FILENO] == NULL)
    stdioInitStd(STDERR_FILENO);

  return &taskIdCurrent->taskStdFp[STDERR_FILENO];
}

/*******************************************************************************
 * __smakebuf - Allocate stdio buffer or turn buffering off
 *
 * RETURNS: N/A
 ******************************************************************************/

void __smakebuf(FILE *fp)
{
  size_t size;
  BOOL isTty;
  void *p;
  struct stat st;

  /* If unbuffered */
  if (fp->_flags & __SNBF) {

    fp->_bf._base = fp->_nbuf;			/* Use safety buffer */
    fp->_p = fp->_nbuf;				/* Set start pointer */
    fp->_bf._size = 1;				/* Just one byte */
    return;

  }

  /* If no file assosiated */
  if (fp->_file < 0) {

    isTty = FALSE;				/* Can't be a tty */
    size = BUFSIZ;				/* Standard size */
    fp->_flags |= __SNPT;			/* No optimization */

  } /* End if no file assosiated */

  /* Else if get block size from driver */
  else if ( ioctl (fp->_file, FIOSTATGET, (int) &st) < 0 ) {

    isTty = TRUE;				/* Could be a tty */
    size = BUFSIZ;				/* Standard size */
    fp->_flags |= __SNPT;			/* No optimization */

  } /* End else if get block size from driver */

  /* Else */
  else {

    isTty = ( (st.st_mode & S_IFMT) == S_IFCHR);
    if (st.st_blksize <= 0)
      size = BUFSIZ;				/* Standard size */
    else
      size = st.st_blksize;			/* Use block size */

    /* Optimize fseek */
    if ( (st.st_mode & S_IFMT) == S_IFREG) {

      fp->_flags |= __SOPT;
      fp->_blksize = st.st_blksize;

    }
    else
      fp->_flags |= __SNPT;

  } /* End else */

  /* Allocate buffer */
  p = malloc(size);

  /* If malloc failed */
  if (p == NULL) {

    /* Try unbuffered */
    fp->_flags |= __SNBF;
    fp->_bf._base = fp->_nbuf;
    fp->_p = fp->_bf._base;
    fp->_bf._size = 1;

  } /* End if malloc failed */

  /* Else malloc succeded */
  else {

    fp->_flags |= __SMBF;
    fp->_bf._base = p;
    fp->_p = fp->_bf._base;
    fp->_bf._size = size;

    /* If tty, turn line buffer on */
    if ( isTty && isatty(fp->_file) )
      fp->_flags |= __SLBF;

  } /* End else malloc succeded */
}

/*******************************************************************************
 * __swsetup - Check or fix for write access
 *
 * RETURNS: N/A
 ******************************************************************************/

int __swsetup(FILE *fp)
{
  /* If not writable */
  if ( (fp->_flags & __SWR) == 0) {

    /* If not read and writable */
    if ( (fp->_flags & __SRW) == 0)
      return EOF;

    /* If readable */
    if (fp->_flags & __SRD) {

      /* Check ungetc buffer */
      if ( HASUB(fp) )
        FREEUB(fp);

      fp->_flags &= ~(__SRD | __SEOF);
      fp->_r = 0;
      fp->_p = fp->_bf._base;

    } /* End if readable */

    /* Make writable */
    fp->_flags |= __SWR;

  } /* End if not writable */

  /* Create buffer if nessesary */
  if (fp->_bf._base == NULL)
    __smakebuf(fp);

  /* If line buffered */
  if (fp->_flags & __SLBF) {

    fp->_w = 0;
    fp->_lbfsize = -fp->_bf._size;

  } /* End if line buffered */

  /* Else not line buffered */
  else {

    if (fp->_flags & __SNBF)
      fp->_w = 0;
    else
      fp->_w = fp->_bf._size;

  } /* End else not line  buffered */

  return 0;
}

/*******************************************************************************
* __sflags - Internal flags get for a given mode
*
* RETURNS: 0 or flags
*******************************************************************************/

int __sflags(const char *mode, int *optr)
{
  int o, m, ret;

  /* Select mode */
  switch(*mode++) {

    /* Open for reading */
    case 'r':
	ret = __SRD;
	m = O_RDONLY;
	o = 0;
	break;

    /* Open for writing */
    case 'w':
	ret = __SWR;
	m = O_WRONLY;
	o = O_CREAT | O_TRUNC;
	break;

    /* Open for append */
    case 'a':
	ret = __SWR;
	m = O_WRONLY;
	o = O_CREAT | O_APPEND;
	break;

    /* Invalid mode */
    default:
	errnoSet(EINVAL);
	return 0;
  }

  /* If both read and write */
  if ( (*mode == '+') || (*mode == 'b' && mode[1] == '+') ) {
    ret = __SRW;
    m = O_RDWR;
  }

  /* Store file open mode */
  *optr = m | o;

  /* Check for rubbish in 2nd char */
  if ( (*mode != '+') && (*mode != 'b') && (*mode != '\0') )
    return 0;

  /* If no 3rd char */
  if (*mode++ == '\0')
    return ret;

  /* Check for rubbish in 3rd char */
  if ( (*mode != '+') && (*mode != 'b') && (*mode != '\0') )
    return 0;

  /* If no 4th char */
  if (*mode++ == '\0')
    return ret;

  /* Check for rubbish in 4th char */
  if (*mode != '\0')
    return 0;

  return ret;
}

/*******************************************************************************
* __sread - Internal read for stdio
*
* RETURNS: Bytes read
*******************************************************************************/

int __sread(FILE *fp, char *buf, int n)
{
  int rv;

  /* System call read */
  rv = read(fp->_file, buf, n);

  if (rv >= 0)
    fp->_offset += rv;
  else
    fp->_flags &= ~__SOFF;

  return rv;
}

/*******************************************************************************
* __swrite - Internal write for stdio
*
* RETURNS: Bytes written
*******************************************************************************/
extern void write_serial_str(char *buffer, int size);
/* TYLER */
int __swrite(FILE *fp, char *buf, int n)
{
  if (fp->_flags & __SAPP)
    lseek(fp->_file, (off_t) 0, SEEK_END);

  fp->_flags &= ~__SOFF;

  write_serial_str(buf, n);
  return write(fp->_file, buf, n);
}

/*******************************************************************************
* __sflush - Flush stream fp
*
* RETURNS: 0 or EOF
*******************************************************************************/

int __sflush(FILE *fp)
{
  unsigned char *p;
  int i, n, flags;

  /* Store flags */
  flags = fp->_flags;

  /* If no write */
  if ( (flags & __SWR) == 0)
    return 0;

  /* Get base pointer */
  p = fp->_bf._base;
  if (p == NULL)
    return 0;

  /* Calculate write size */
  n = fp->_p - p;

  /* Setup buffer position */
  fp->_p = p;

  /* If line buffer of no buffer */
  if (flags & (__SLBF | __SNBF) )
    fp->_w = 0;
  else
    fp->_w = fp->_bf._size;

  /* Write all bytes */
  for (; n > 0; n -= i, p += i) {
    i = __swrite(fp, (char *) p, n);
    if (i <= 0) {
      fp->_flags |= __SERR;
      return EOF;
    }
  }

  return 0;
}

/*******************************************************************************
 * __sseek - Internal seek in file
 *
 * RETURNS: Seek position or EOF
 ******************************************************************************/

fpos_t __sseek(FILE *fp, fpos_t offset, int whence)
{
  off_t roffs;

  /* Call lseek */
  roffs = lseek(fp->_file, (off_t) offset, whence);

  if (roffs == -1L) {

    fp->_flags &= ~__SOFF;

  }
  else {

    fp->_flags |= __SOFF;
    fp->_offset = roffs;
  }

  return roffs;
}

/*******************************************************************************
 * __close - Internal close
 *
 * RETURNS: 0 or EOF
 ******************************************************************************/

int __sclose(FILE *fp)
{
  int fd, ret;

  /* Dont close if it is a std fd */
  if ( (fd >= 0) && (fd < 3) )
    ret = 0;
  else if ( close(fd) < 0 )
    ret = EOF;

  return ret;
}

/*******************************************************************************
 * __swbuf - Write to stdio buffer
 *
 * RETURNS: Last char written
 ******************************************************************************/

int __swbuf(int c, FILE *fp)
{
  unsigned char uc;
  int n;

  fp->_w = fp->_lbfsize;

  if (CANTWRITE(fp))
       return EOF;

  uc = (unsigned char) c;

  /* Calculate size to write */
  n = fp->_p - fp->_bf._base;

  /* Flush if needed */
  if (n >= fp->_bf._size) {

    if ( fflush(fp) )
      return EOF;

    /* Back at zero bytes left */
    n = 0;
  }

  /* Store last char and decrease write space left */
  fp->_w--;
  *fp->_p++ = c;

  /* Flush if full buffer on newline when line buffered */
  if ( (++n == fp->_bf._size) || (fp->_flags & __SLBF && c== '\n') )
    if ( fflush(fp) )
      return EOF;

  return c;
}

/*******************************************************************************
 * lflush - Stdio internal
 *
 * RETURNS: 0 or EOF
 ******************************************************************************/

LOCAL int lflush(FILE *fp)
{
  if ( (fp->_flags & (__SLBF | __SWR)) == (__SLBF | __SWR))
    return __sflush(fp);

  return 0;
}

/*******************************************************************************
 * __srefill - Refill stdio buffer
 *
 * RETURNS: 0 or EOF
 ******************************************************************************/

int __srefill(FILE *fp)
{
  /* Reset read space left */
  fp->_r = 0;

  /* Check if EOF */
  if (fp->_flags & __SEOF)
    return EOF;

  /* If not reading */
  if ( (fp->_flags & __SRD) == 0) {

    /* If not read and writable */
    if ( (fp->_flags & __SRW) == 0) {
      errnoSet(EBADF);
      return EOF;
    }

    /* If writable */
    if (fp->_flags & __SWR) {

      if ( __sflush(fp) )
        return EOF;

      /* Switch to reading */
      fp->_flags &= ~__SWR;
      fp->_w = 0;
      fp->_lbfsize = 0;

    }

    fp->_flags |= __SRD;

  } /* End if not reading */

  /* Else must be reading */
  else {

    /* If ungetc buffer */
    if (HASUB(fp)) {

      FREEUB(fp);

      fp->_r = fp->_ur;
      if (fp->_ur != 0) {

        fp->_p = fp->_up;
        return 0;

      }

    }

  } /* End else must be reading */

  /* Try to make read buffer */
  if (fp->_bf._base == NULL)
    __smakebuf(fp);

  /* If neede to flush first */
  if ( fp->_flags & (__SLBF | __SNBF) )
    lflush(fp);

  /* Start at beginnig of buffer */
  fp->_p = fp->_bf._base;
  fp->_r = __sread(fp, (char *) fp->_p, fp->_bf._size);
  fp->_flags &= ~__SMOD;

  /* If no read space left */
  if (fp->_r <= 0) {

    if (fp->_r == 0)
      fp->_flags |= __SEOF;

    else {

      fp->_r = 0;
      fp->_flags |= __SERR;

    }

    return EOF;

  } /* End if no read space left */

  return 0;
}

/*******************************************************************************
 * __srget - Get data to stdio buffer
 *
 * RETURNS: 0 or EOF
 ******************************************************************************/

int __srget(FILE *fp)
{
  if (__srefill(fp) == 0) {

    fp->_r--;
    return *fp->_p++;

  }

  return EOF;
}

/*******************************************************************************
 * __sfvwrite - Internal write function
 *
 * RETURNS: 0 or EOF
 ******************************************************************************/

int __sfvwrite(FILE *fp, struct __suio *uio)
{
  char *p, *nl;
  int nlDone, nlDist, w, s;
  size_t len;
  struct __siov *iov;

  /* If resid is zero */
  if (uio->uio_resid == 0)
    return 0;

  /* Setup for writing */
  if ( CANTWRITE(fp) )
    return EOF;

  /* Get pointer to __siov struct */
  iov = uio->uio_iov;

  /* Get pointer, length and advance */
  p = iov->iov_base;
  len = iov->iov_len;
  iov++;

  /* If unbuffered */
  if (fp->_flags & __SNBF) {

    /* Do while resid not zero */
    do {

      /* While length is zero */
      while (len == 0) {

        /* Get pointer, length and advance */
        p = iov->iov_base;
        len = iov->iov_len;
	iov++;

      } /* End while length is zero */

      /* Write buffer */
      w = __swrite(fp, p, min(len, BUFSIZ));
      if (w <= 0) {
	fp->_flags |= __SERR;
	return EOF;
      }

      /* Advance buffer, and decrease length */
      p += w;
      len -= w;

      /* Advance resid */
      uio->uio_resid -= w;

    } while (uio->uio_resid != 0);

  } /* End unbuffered */

  /* Else if not line buffered */
  else if ( (fp->_flags & __SLBF) == 0 ) {

    /* Do while resid not zero */
    do {

      /* While length is zero */
      while (len == 0) {

        /* Get pointer, length and advance */
        p = iov->iov_base;
        len = iov->iov_len;
	iov++;

      } /* End while length is zero */

      /* Get write position */
      w = fp->_w;

      /* If this is a sprintf string */
      if (fp->_flags & __SSTR) {

	/* Fix write position */
        if (len < w)
          w = len;

	/* Copy */
	memcpy(fp->_p, p, (size_t) w);

	/* Advance in FILE struct */
	fp->_w -= w;
	fp->_p += w;
	w = len;

      } /* End if this is a sprintf string */

      /* Else if ahead in buffer */
      else if (fp->_p > fp->_bf._base && len > w) {

	/* Copy */
	memcpy(fp->_p, p, (size_t) w);

	/* Advance in FILE struct */
	fp->_p += w;

	/* Flush */
	if ( fflush(fp) ) {
	  fp->_flags |= __SERR;
	  return EOF;
        }

      } /* End else if ahead in buffer */

      /* Else if more to write */
      else if ( len >= (w = fp->_bf._size) ) {

	/* Write data */
	w = __swrite (fp, p ,w);
	if (w <= 0) {
	  fp->_flags |= __SERR;
	  return EOF;
	}

      } /* End else if more to write */

      /* Else normal copy */
      else {

	/* Get length to write */
	w = len;

	/* Copy */
	memcpy(fp->_p, p, (size_t) w);

	/* Advance in FILE struct */
	fp->_w -= w;
	fp->_p += w;

      } /* End else normal copy */

      /* Advance locals */
      p += w;
      len -= w;

      /* Advance resid */
      uio->uio_resid -= w;

    } while (uio->uio_resid != 0);

  } /* End else if not line buffered */

  /* Else line buffered */
  else {

    nlDist = 0;

    /* Do while resid not zero */
    do {

      /* Reset linecount done */
      nlDone = 0;

      /* While length is zero */
      while (len == 0) {

        /* Get pointer, length and advance */
        p = iov->iov_base;
        len = iov->iov_len;
	iov++;

      } /* End while length is zero */

      /* If lines hasn't been counted */
      if (!nlDone) {

        nl = memchr(p, '\n', len);
	if (nl != NULL)
	  nlDist = nl + 1 - p;
	else
	  nlDist = len + 1;

	/* Mark that lines has been counted */
	nlDone = 1;

      } /* End if lines hasn't been counted */

      /* Calculate minimum between length and number of newlines */
      s = min(len, nlDist);

      /* Get write size */
      w = fp->_w + fp->_bf._size;

      /* If position ahead of base */
      if (fp->_p > fp->_bf._base && s > w) {

	/* Copy */
	memcpy(fp->_p, p, (size_t) w);

	/* Advance in FILE struct */
	fp->_p += w;

	/* Flush */
	if ( fflush(fp) ) {
	  fp->_flags |= __SERR;
	  return EOF;
        }

      } /* End if position ahead of base */

      /* Else if minimum newlines and length gt. size */
      else if ( s >= (w = fp->_bf._size) ) {

	/* Write */
	w = __swrite(fp, p, w);
	if (w <= 0) {
	  fp->_flags |= __SERR;
	  return EOF;
        }

      } /* End else if minimum newlines and length gt. size */

      /* Else normal copy */
      else {

	/* Write min bt. newlines and len */
	w = s;

	/* Copy */
	memcpy(fp->_p, p, (size_t) w);

	/* Advance in FILE struct */
	fp->_w -= w;
	fp->_p += w;

      } /* End else normal copy */

      /* Decrease by bytes written */
      nlDist -= w;

      /* If zero reached */
      if (nlDist == 0) {

	/* Flush */
	if ( fflush(fp) ) {
	  fp->_flags |= __SERR;
	  return EOF;
        }

	/* Need to recalculate newlines */
	nlDone = 0;

      } /* End if zero reached */

      /* Advance locals */
      p += w;
      len -= w;

      /* Advance resid */
      uio->uio_resid -= w;

    } while (uio->uio_resid != 0);

  } /* End else line buffered */

  return 0;
}

/*******************************************************************************
 * __submore - Expand ungetc buffer
 *
 * RETURNS: 0 or EOF
 ******************************************************************************/

int __submore(FILE *fp)
{
  int i;
  unsigned char *p;

  /* If create a new buffer */
  if (fp->_ub._base == fp->_ubuf) {

    /* Allocate a new buffer */
    p = (unsigned char *) malloc(BUFSIZ);
    if (p == NULL)
      return EOF;

    /* Setup ungetc buffer */
    fp->_ub._base = p;
    fp->_ub._size = BUFSIZ;

    /* Advance in buffer beyond existing space */
    p += BUFSIZ - sizeof(fp->_ubuf);

    /* Fill buffer width data */
    for (i = sizeof(fp->_ubuf); --i >= 0;)
      p[i] = fp->_ubuf[i];

    /* Advance current position */
    fp->_p = p;

    return 0;

  } /* End if create a new buffer */

  /* Get length */
  i = fp->_ub._size;

  /* Resize buffer */
  p = (unsigned char *) realloc(fp->_ub._base, i << 1);
  if (p == NULL)
    return EOF;

  /* Copy to end of buffer */
  memcpy(p + i, p, (size_t) i);

  /* Setup FILE struct */
  fp->_p = p + i;
  fp->_ub._base = p;
  fp->_ub._size = i << 1;

  return 0;
}


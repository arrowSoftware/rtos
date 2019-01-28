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

/* ttyLib.c - tty support library */

#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <rtos/errnoLib.h>
#include <util/ringLib.h>
#include <io/ioLib.h>
#include <io/ttyLib.h>

/* Imports */
IMPORT FUNCPTR _func_excJobAdd;
IMPORT FUNCPTR _func_selWakeupListInit;
IMPORT FUNCPTR _func_selWakeupAll;

IMPORT void sysReboot(void);

/* Locals */
LOCAL int ttyMutexOptions = SEM_Q_FIFO | SEM_DELETE_SAFE;
LOCAL FUNCPTR	ttyAbortFunc		= NULL;
LOCAL char	ttyAbortChar		= 0x03;
LOCAL char	ttyBackSpaceChar 	= 0x08;
LOCAL char	ttyDeleteLineChar	= 0x15;
LOCAL char	ttyEofChar		= 0x04;
LOCAL char	ttyMonitorTrapChar	= 0x18;
LOCAL int	ttyXoffTreshold		= 80;
LOCAL int 	ttyXonTreshold		= 100;
LOCAL int	ttyWriteTreshold	= 20;
LOCAL int	ttyXoffChars		= 0;
LOCAL int	ttyXoffMaxChars		= 0;

LOCAL void ttyFlush(TTY_DEV_ID ttyId);
LOCAL void ttyFlushRead(TTY_DEV_ID ttyId);
LOCAL void ttyFlushWrite(TTY_DEV_ID ttyId);
LOCAL void ttyReadXoff(TTY_DEV_ID ttyId, BOOL xoff);
LOCAL void ttyWriteXoff(TTY_DEV_ID ttyId, BOOL xoff);
LOCAL void ttyTxStartup(TTY_DEV_ID ttyId);

LOCAL void ttySelAdd(TTY_DEV_ID ttyId, int arg);
LOCAL void ttySelDelete(TTY_DEV_ID ttyId, int arg);

/*******************************************************************************
* ttyDevInit - Intialize a tty device
*
* RETURNS: OR or ERROR
*******************************************************************************/

STATUS ttyDevInit(TTY_DEV_ID ttyId,
		  int readBufferSize,
		  int writeBufferSize,
		  FUNCPTR txStartup)
{
  /* Clear struct */
  memset(ttyId, 0, sizeof(TTY_DEV));

  /* Allocate ring buffers */
  ttyId->readBuffer = ringCreate(readBufferSize);
  if (ttyId->readBuffer == NULL)
    return(ERROR);

  ttyId->writeBuffer = ringCreate(writeBufferSize);
  if (ttyId->writeBuffer == NULL) {
    ringDelete(ttyId->readBuffer);
    return(ERROR);
  }

  /* Initialize struct */
  ttyId->txStartup = txStartup;

  /* Initialize semaphores */
  semBInit(&ttyId->readSync, SEM_Q_PRIORITY, SEM_EMPTY);
  semBInit(&ttyId->writeSync, SEM_Q_PRIORITY, SEM_EMPTY);
  semMInit(&ttyId->mutex, ttyMutexOptions);

  /* Initialize select list if installed */
  if (_func_selWakeupListInit != NULL)
    ( *_func_selWakeupListInit) (&ttyId->selWakeupList);

  ttyFlush(ttyId);

  return(OK);
}

/*******************************************************************************
* ttyDevRemove - Remove a tty device
*
* RETURNS: OR or ERROR
*******************************************************************************/

STATUS ttyDevRemove(TTY_DEV_ID ttyId)
{
  /* Remove buffers */
  semTake(&ttyId->mutex, WAIT_FOREVER);

  if (ttyId->readBuffer != NULL) {
    ttyId->readState.flushingReadBuffer = TRUE;
    ringDelete(ttyId->readBuffer);
  }
  else {
    semGive(&ttyId->mutex);
    return(ERROR);
  }

  if (ttyId->writeBuffer != NULL) {
    ringDelete(ttyId->writeBuffer);
    ttyId->writeState.flushingWriteBuffer = TRUE;
  }
  else {
    semGive(&ttyId->mutex);
    return(ERROR);
  }

  semGive(&ttyId->mutex);

  return(OK);
}

/*******************************************************************************
* ttyAbortFuncSet - Set abort function
*
* RETURNS: N/A
*******************************************************************************/

void ttyAbortFuncSet(FUNCPTR func)
{
  ttyAbortFunc = func;
}

/*******************************************************************************
* ttyAbortSet - Set abort char
*
* RETURNS: N/A
*******************************************************************************/

void ttyAbortSet(char c)
{
  ttyAbortChar = c;
}

/*******************************************************************************
* ttyBackSpaceSet - Set backspace char
*
* RETURNS: N/A
*******************************************************************************/

void ttyBackSpaceSet(char c)
{
  ttyBackSpaceChar = c;
}

/*******************************************************************************
* ttyDeleteLineSet - Set line delete char
*
* RETURNS: N/A
*******************************************************************************/

void ttyDeleteLineSet(char c)
{
  ttyDeleteLineChar = c;
}

/*******************************************************************************
* ttyEofSet - Set end of file char
*
* RETURNS: N/A
*******************************************************************************/

void ttyEOFSet(char c)
{
  ttyEofChar = c;
}

/*******************************************************************************
* ttyMonitorTrapSet - Set monitor trap char
*
* RETURNS: N/A
*******************************************************************************/

void ttyMonitorTrapSet(char c)
{
  ttyMonitorTrapChar = c;
}

/*******************************************************************************
* ttyIoctl - Control request
*
* RETURNS: OK or ERROR
*******************************************************************************/

int ttyIoctl(TTY_DEV_ID ttyId, int req, int arg)
{
  STATUS status;
  int old_opts;

  /* Init vars */
  status = OK;

  /* Handle request */
  switch(req) {
    case FIONREAD: *((int *) arg) = ringNBytes(ttyId->readBuffer); break;
    case FIONWRITE: *((int *) arg) = ringNBytes(ttyId->writeBuffer); break;
    case FIOFLUSH: ttyFlush(ttyId); break;
    case FIOCANCEL:
      semTake(&ttyId->mutex, WAIT_FOREVER);

      /* Cancel read transaction */
      ttyId->readState.canceled = TRUE;
      semGive(&ttyId->readSync);

      /* Cancel write transaction */
      ttyId->writeState.canceled = TRUE;
      semGive(&ttyId->writeSync);

      semGive(&ttyId->mutex);
      break;
    case FIORFLUSH: ttyFlushRead(ttyId); break;
    case FIOWFLUSH: ttyFlushWrite(ttyId); break;
    case FIOGETOPTIONS: return (ttyId->options);
    case FIOSETOPTIONS:
      /* Store old */
      old_opts = arg;

      /* Set new */
      ttyId->options = arg;

      /* Check if read flushing is needed */
      if ( (old_opts & OPT_LINE) != (ttyId->options & OPT_LINE) )
        ttyFlushRead(ttyId);

      /* Check xoff options */
      if ( (old_opts & OPT_TANDEM) && !(ttyId->options & OPT_TANDEM) ) {
        ttyReadXoff(ttyId, FALSE);
        ttyWriteXoff(ttyId, FALSE);
      }
      break;
    case FIOISATTY: return(TRUE);
    case FIOPROTOHOOK: ttyId->protoHook = (FUNCPTR) arg; break;
    case FIOPROTOARG: ttyId->protoArg = (ARG) arg; break;
    case FIORBUFSET:
      semTake(&ttyId->mutex, WAIT_FOREVER);
      ttyId->readState.flushingReadBuffer = TRUE;

      /* Delete old read buffer */
      if (ttyId->readBuffer != NULL) ringDelete(ttyId->readBuffer);

      /* Create new read buffer with arumented size */
      ttyId->readBuffer = ringCreate(arg);
      if (ttyId->readBuffer == NULL)
        status = ERROR;

      ttyId->readState.flushingReadBuffer = FALSE;
      semGive(&ttyId->mutex);
    break;
    case FIOWBUFSET:
      semTake(&ttyId->mutex, WAIT_FOREVER);
      ttyId->writeState.flushingWriteBuffer = TRUE;

      /* Delete old write buffer */
      if (ttyId->writeBuffer != NULL) ringDelete(ttyId->writeBuffer);

      /* Create new write buffer with arumented size */
      ttyId->writeBuffer = ringCreate(arg);
      if (ttyId->writeBuffer == NULL)
        status = ERROR;

      ttyId->writeState.flushingWriteBuffer = FALSE;
      semGive(&ttyId->mutex);
    break;

    case FIOSELECT:
      ttySelAdd(ttyId, arg);
    break;

    case FIOUNSELECT:
      ttySelDelete(ttyId, arg);
    break;

    default:
      status = ERROR;
    break;

  }

  return(status);
}

/*******************************************************************************
* ttyWrite - Write to tty
*
* RETURNS: Number of bytes written
*******************************************************************************/

int ttyWrite(TTY_DEV_ID ttyId,
	     char *buffer,
	     int nBytes)
{
  int bwrote;
  int nStart;

  nStart = nBytes;
  ttyId->writeState.canceled = FALSE;

  while (nBytes > 0) {
    semTake(&ttyId->writeSync, WAIT_FOREVER);
    semTake(&ttyId->mutex, WAIT_FOREVER);

    /* Check if write was canceled */
    if (ttyId->writeState.canceled) {
      semGive(&ttyId->mutex);
      return (nStart - nBytes);
    }

    ttyId->writeState.writeBufferBusy = TRUE;
    bwrote = ringBufferPut(ttyId->writeBuffer, buffer, nBytes);
    ttyId->writeState.writeBufferBusy = FALSE;

    ttyTxStartup(ttyId);

    nBytes -= bwrote;
    buffer += bwrote;

    /* Check if more room is avilable */
    if (ringFreeBytes(ttyId->writeBuffer) > 0)
      semGive(&ttyId->writeSync);

    semGive(&ttyId->mutex);
  }

  return(nStart);
}

/*******************************************************************************
* ttyRead - Read from tty
*
* RETURNS: Number of bytes read
*******************************************************************************/

int ttyRead(TTY_DEV_ID ttyId,
	    char *buffer,
	    int nBytes)
{
  int n, nn, freeBytes;
  RING_ID ringId;

  ttyId->readState.canceled = FALSE;

  semTake(&ttyId->readSync, WAIT_FOREVER);

  /* Loop until read ring is not empty */
  while (1) {

    /* Don't know why sleep is needed here */
    taskDelay(1);

    semTake(&ttyId->mutex, WAIT_FOREVER);

    /* Check if write was canceled */
    if (ttyId->readState.canceled) {
      semGive(&ttyId->mutex);
      return (0);
    }

    ringId = ttyId->readBuffer;
    if (!ringIsEmpty(ringId)) {
      break;
    }

    semGive(&ttyId->mutex);
  }

  /* Get characters from ring buffer */
  if (ttyId->options & OPT_LINE) {
    if (ttyId->lnBytesLeft == 0)
      RING_ELEMENT_GET(ringId, &ttyId->lnBytesLeft, nn);

    n = min((int) ttyId->lnBytesLeft, nBytes);
    ringBufferGet(ringId, buffer, n);
    ttyId->lnBytesLeft -= n;

  } else n = ringBufferGet(ringId, buffer, nBytes);

  /* Check xon */
  if ( (ttyId->options & OPT_TANDEM) && ttyId->readState.xoff ) {
    freeBytes = ringFreeBytes(ringId);
    if (ttyId->options & OPT_LINE)
      freeBytes -= ttyId->lnNBytes + 1;

    if (freeBytes > ttyXonTreshold)
      ttyReadXoff(ttyId, FALSE);
  }

  /* Check if there is more to read */
  if (!ringIsEmpty(ringId)) {
    semGive(&ttyId->readSync);
  }

  semGive(&ttyId->mutex);

  return(n);
}

/*******************************************************************************
* ttyTx - tty transmit from interrupt
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS ttyIntTx(TTY_DEV_ID ttyId, char *pc)
{
  RING_ID ringId;
  int nn;

  ringId = ttyId->writeBuffer;

  /* Check xon/xoff */
  if (ttyId->readState.pending) {
    ttyId->readState.pending = FALSE;
    *pc = ttyId->readState.xoff ? XOFF : XON;

    if (ttyId->readState.xoff) {
      if (ttyXoffChars > ttyXoffMaxChars)
        ttyXoffMaxChars = ttyXoffChars;
      ttyXoffChars = 0;
    }
  }

  else if (ttyId->writeState.xoff || ttyId->writeState.flushingWriteBuffer)
    ttyId->writeState.busy = FALSE;

  else if (ttyId->writeState.cr) {
    *pc = '\n';
    ttyId->writeState.cr = FALSE;
  }

  else if (RING_ELEMENT_GET(ringId, pc, nn) == FALSE)
    ttyId->writeState.busy = FALSE;

  else {
    ttyId->writeState.busy = TRUE;
    if ( (ttyId->options & OPT_CRMOD) && (*pc == '\n') ) {
      *pc = '\r';
      ttyId->writeState.cr = TRUE;
    }

    if (ringFreeBytes(ringId) == ttyWriteTreshold) {
      semGive(&ttyId->writeSync);
      if (_func_selWakeupAll != NULL)
        ( *_func_selWakeupAll) (&ttyId->selWakeupList, SELWRITE);
    }
  }

  if (!ttyId->writeState.busy)
    return(ERROR);

  return(OK);
}

/*******************************************************************************
* ttyIntRd - tty read from interrupt
*
* RETURNS: OK or ERROR
*******************************************************************************/

STATUS ttyIntRd(TTY_DEV_ID ttyId, char c)
{
  RING_ID ringId;
  int nn, freeBytes, options;
  BOOL releaseTaskLevel, echoed;
  STATUS status;

  options = ttyId->options;
  echoed = FALSE;
  status = OK;

  if (ttyId->readState.flushingReadBuffer)
    return(ERROR);

  if (ttyId->protoHook != NULL) {
    if ( (*ttyId->protoHook)(ttyId->protoArg, c) == TRUE)
      return(status);
  }

  /* Check 7 bit */
  if (options & OPT_7_BIT)
    c &= 0x7f;

  /* Check for abort */
  if ( (c == ttyAbortChar) && (options & OPT_ABORT) && ttyAbortFunc != NULL)
    (*ttyAbortFunc)();

  /* Check monitor trap */
  else if ( (c == ttyMonitorTrapChar) && (options & OPT_MON_TRAP) ) {
    if (_func_excJobAdd != NULL)
      ( *_func_excJobAdd)(sysReboot,
			  (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0, (ARG) 0);
    else
      sysReboot();
  }

  /* Check xon/xoff */
  else if ( ((c == XOFF) || (c == XOFF)) && (options & OPT_TANDEM) )
    ttyWriteXoff (ttyId, (c == XOFF));

  /* Else do read */
  else {
    /* Count number of chars while in xoff */
    if (ttyId->readState.xoff)
      ttyXoffChars++;

    /* Check carriage return */
    if ( (options & OPT_CRMOD) && (c == '\r') )
      c = '\n';

    /* Check for echo on */
    if ( (options & OPT_ECHO) &&
	 (!ttyId->writeState.writeBufferBusy &&
	  !ttyId->writeState.flushingWriteBuffer) ) {

      ringId = ttyId->writeBuffer;

     /* Check for line options */
     if (options & OPT_LINE) {

       if (c == ttyDeleteLineChar) {
         RING_ELEMENT_PUT(ringId, '\n', nn);
         echoed = TRUE;
       }

       else if (c == ttyBackSpaceChar) {
         if (ttyId->lnNBytes != 0) {
	   /* echo backspace-space-backspace */
           ringBufferPut(ringId, "\b \b", 3);
           echoed = TRUE;
         }
       }

       else if ( (c < 0x20) && (c != '\n') ) {
         /* echo ^-c */
         RING_ELEMENT_PUT(ringId, '^', nn);
         RING_ELEMENT_PUT(ringId, c + '@', nn);
         echoed = TRUE;
       }

       /* Else no special char */
       else {
         RING_ELEMENT_PUT(ringId, c, nn);
         echoed = TRUE;

       } /* End else no special char */

     } /* End if line options */

     /* Else no line options */
     else {
       RING_ELEMENT_PUT(ringId, c, nn);
       echoed = TRUE;

     } /* End else no line options */

     /* If echo start tx */
     if (echoed)
       ttyTxStartup(ttyId);

    } /* End check for echo on */

    /* Put char in read buffer */
    ringId = ttyId->readBuffer;
    releaseTaskLevel = FALSE;

    /* Check for non-line options */
    if (! (options & OPT_LINE) ) {
      if(RING_ELEMENT_PUT(ringId, c, nn) == FALSE)
        status = ERROR;

      if (ringNBytes(ringId) == 1)
        releaseTaskLevel = TRUE;

    } /* End check for non-line options */

    /* Else line options */
    else {
      freeBytes = ringFreeBytes(ringId);

      if (c == ttyBackSpaceChar) {
        if (ttyId->lnNBytes != 0)
          ttyId->lnNBytes--;
      }

      else if (c == ttyDeleteLineChar)
        ttyId->lnNBytes = 0;

      else if (c == ttyEofChar) {
        if (freeBytes > 0)
          releaseTaskLevel = TRUE;
      }

      /* Else no special char */
      else {

        /* Check for freeBytes >= 2 */
        if (freeBytes >= 2) {

          if (freeBytes >= (ttyId->lnNBytes + 2) )
            ttyId->lnNBytes++;
          else
            status = ERROR;

          ringPutAhead(ringId, c, (int) ttyId->lnNBytes);

          if ( (c == '\n') || (ttyId->lnNBytes == 255) )
            releaseTaskLevel = TRUE;

        } /* End check for freeBytes >= 2 */
        else status = ERROR;

      } /* End else no special char */

      if (releaseTaskLevel) {
        ringPutAhead(ringId, (char) ttyId->lnNBytes, 0);
        ringMoveAhead(ringId, (int) ttyId->lnNBytes + 1);
        ttyId->lnNBytes = 0;
      }

    } /* End else line options */

    /* Check for xon/xoff */
    if (options & OPT_TANDEM) {
      freeBytes = ringFreeBytes(ringId);
      if (ttyId->options & OPT_LINE)
        freeBytes -= ttyId->lnNBytes + 1;

      if (!ttyId->readState.xoff) {
        if (freeBytes < ttyXoffTreshold)
          ttyReadXoff(ttyId, TRUE);
      } else {
        if (freeBytes > ttyXonTreshold)
          ttyReadXoff(ttyId, FALSE);
      }
    } /* End check for xon/xoff */

    if (releaseTaskLevel) {
      semGive(&ttyId->readSync);
      if (_func_selWakeupAll != NULL)
        ( *_func_selWakeupAll) (&ttyId->selWakeupList, SELREAD);
    }

  } /* End else do read */

  return(status);
}

/*******************************************************************************
* ttyFlush - Flush a tty device
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void ttyFlush(TTY_DEV_ID ttyId)
{
  ttyFlushRead(ttyId);
  ttyFlushWrite(ttyId);
}

/*******************************************************************************
* ttyFlushRead - Flush a tty devices read buffer
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void ttyFlushRead(TTY_DEV_ID ttyId)
{
  semTake(&ttyId->mutex, WAIT_FOREVER);
  ttyId->readState.flushingReadBuffer = TRUE;
  ringFlush(ttyId->readBuffer);

  ttyId->lnNBytes = 0;
  ttyId->lnBytesLeft = 0;
  ttyReadXoff(ttyId, FALSE);
  ttyId->readState.flushingReadBuffer = FALSE;
  semGive(&ttyId->mutex);
}

/*******************************************************************************
* ttyFlushWrite - Flush a tty devices write buffer
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void ttyFlushWrite(TTY_DEV_ID ttyId)
{
  semTake(&ttyId->mutex, WAIT_FOREVER);
  ttyId->writeState.flushingWriteBuffer = TRUE;
  ringFlush(ttyId->writeBuffer);
  semGive(&ttyId->writeSync);

  ttyId->writeState.flushingWriteBuffer = FALSE;
  semGive(&ttyId->mutex);

  /* Wakeup select if installed */
  if (_func_selWakeupAll != NULL)
    ( *_func_selWakeupAll) (&ttyId->selWakeupList, SELWRITE);
}

/*******************************************************************************
* ttyReadXoff - Set read xon/xoff
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void ttyReadXoff(TTY_DEV_ID ttyId, BOOL xoff)
{
  int level;

  INT_LOCK(level);

  if (ttyId->readState.xoff != xoff) {
    ttyId->readState.xoff = xoff;
    ttyId->readState.pending = TRUE;

    if (!ttyId->writeState.busy) {
      ttyId->writeState.busy = TRUE;
      INT_UNLOCK(level);
      (*ttyId->txStartup)(ttyId);
      return;
    }
  }

  INT_UNLOCK(level);
}

/*******************************************************************************
* ttyWriteXoff - Set write xon/xoff
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void ttyWriteXoff(TTY_DEV_ID ttyId, BOOL xoff)
{
  int level;

  INT_LOCK(level);

  if (ttyId->writeState.xoff != xoff) {
    ttyId->writeState.xoff = xoff;

    if (!xoff) {
      if (!ttyId->writeState.busy) {
        ttyId->writeState.busy = TRUE;
        INT_UNLOCK(level);
        (*ttyId->txStartup)(ttyId);
        return;
      }
    }
  }

  INT_UNLOCK(level);
}

/*******************************************************************************
* ttyTxStartup - Start transmitter if nessasary
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void ttyTxStartup(TTY_DEV_ID ttyId)
{
  int level;

  if (!ttyId->writeState.busy) {
    INT_LOCK(level);

    if (!ttyId->writeState.busy) {
      ttyId->writeState.busy = TRUE;
      INT_UNLOCK(level);
      (*ttyId->txStartup)(ttyId);
      return;
    }

    INT_UNLOCK(level);
  }
}

/******************************************************************************
* ttySelAdd - Ioctl add select on file descriptor
*
* RETURNS: N/A
******************************************************************************/

LOCAL void ttySelAdd(TTY_DEV_ID ttyId, int arg)
{

  /* Add select node to tty wakeup list */
  selNodeAdd(&ttyId->selWakeupList, (SEL_WAKEUP_NODE *) arg);

  /* If select on read */
  if ( (selWakeupType( (SEL_WAKEUP_NODE *) arg) == SELREAD) &&
       (ringNBytes(ttyId->readBuffer) > 0) )
    selWakeup( (SEL_WAKEUP_NODE *) arg);

  /* If select on write */
  if ( (selWakeupType( (SEL_WAKEUP_NODE *) arg) == SELWRITE) &&
       (ringFreeBytes(ttyId->writeBuffer) > 0) )
    selWakeup( (SEL_WAKEUP_NODE *) arg);
}

/******************************************************************************
* ttySelDelete - Ioctl delete select on filedescriptor
*
* RETURNS: N/A
******************************************************************************/

LOCAL void ttySelDelete(TTY_DEV_ID ttyId, int arg)
{
  selNodeDelete(&ttyId->selWakeupList, (SEL_WAKEUP_NODE *) arg);
}


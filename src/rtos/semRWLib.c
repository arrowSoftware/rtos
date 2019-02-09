/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2009 Surplus Users Ham Society
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

/* semRWLib.c - multiple reader single writer library */

/* includes */

#include <rtos.h>
#include <arch/intArchLib.h>
#include <stdlib.h>
#include <string.h>
#include <rtos/errnoLib.h>
#include <rtos/semLib.h>
#include <arch/intArchLib.h>
#include <util/classLib.h>
#include <rtos/taskLib.h>
#include <rtos/rtosLib.h>
#include <rtos/eventLib.h>
#include <os/sigLib.h>

/* defines */

#define SEM_RW_UNPEND_WRITER   0x01
#define SEM_RW_UNPEND_READER   0x02
#define SEM_RW_SEND_EVENTS     0x04
#define SEM_RW_SAFE_Q_FLUSH    0x08

/* Imports */
IMPORT BOOL kernelState;
IMPORT STATUS kernExit(void);
IMPORT TCB_ID taskIdCurrent;
IMPORT CLASS_ID semClassId;
IMPORT int errno;

/* forward declarations */

LOCAL STATUS semRWCommonGive (RW_SEMAPHORE *, int);
LOCAL STATUS semWriterGive (RW_SEMAPHORE * pSem, int level);
LOCAL STATUS semReaderGive (RW_SEMAPHORE * pSem, int level, int index);
LOCAL STATUS semRWGive (SEM_ID  semId);
LOCAL void semRWTimeoutHandler (void *  pVoid, int src);
LOCAL STATUS semRWUpgradeInternal (SEM_ID semId, unsigned timeout, BOOL force);

/* Locals */
LOCAL BOOL semRWLibInstalled = FALSE;

/***************************************************************************
 *
 * semRWLibInit - initialize multiple reader single writer semaphore library
 *
 * This routine initializes the multiple reader single writer semaphore
 * library.  It is expected to only be called once (during startup).
 *
 * RETURNS: OK or ERROR
 */

STATUS semRWLibInit (void) {
    /* Instatiate class */
    if (semRWLibInstalled) {
        return(OK);
    }

    /* Only install give call method as takes do not map well. */
    semGiveTable[SEM_TYPE_RW] = (FUNCPTR) semRWGive;

    /* Mark library as installed */
    semRWLibInstalled = TRUE;

    return(OK);
}

/***************************************************************************
 *
 * semRWCreate - allocate and initialize a reader-writer semaphore
 *
 * This routine allocates memory and then initializes it for a multiple
 * reader single writer semaphore.
 *
 * RETURNS: SEM_ID on success, NULL otherwise
 */

SEM_ID semRWCreate (
    int     options,
    int     maxReaders
    ) {
    SEM_ID  semId;
    void *  pBasic;
    int     size;

    /* Check if lib is installed */
    if (!semRWLibInstalled) {
        errnoSet (S_semLib_NOT_INSTALLED);
        return (NULL);
    }

    /* Allocate memory */
    size = sizeof (SEM_RW_BASIC);
    if (maxReaders > 0) {
        size += (maxReaders * sizeof (SEM_RW_EXTRA));
    }

    semId = (SEM_ID) objAllocPad (semClassId, size, &pBasic);
    if (semId == NULL) {

        /* errno set by objAllocPad */
        return (NULL);
    }

    /* Initialze structure */
    if (semRWInit (semId, options, maxReaders) != OK) {

        /* errno set by semRWInit() */
        objFree (semClassId, semId);
        return (NULL);
    }

    return (semId);
}
  
/***************************************************************************
 *
 * semRWInit - initialize the read-write semaphore
 *
 * This routine initializes the read-write semaphore.
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS semRWInit (
    SEM_ID  semId,
    int     options,
    int     maxReaders
    ) {
    RW_SEM_ID  rwSemId;
    int        i;

    /* Check if lib is installed */
    if (!semRWLibInstalled) {
        errnoSet (S_semLib_NOT_INSTALLED);
        return(ERROR);
    }

    if (maxReaders < 0) {
        errnoSet (S_semLib_INVALID_MAX_READERS);
        return (ERROR);
    }

#if 0    /* Inversion safe read-write semaphores not yet supported */
    if (((options & SEM_Q_MASK) != SEM_Q_PRIORITY) &&
        (options & SEM_INVERSION_SAFE)) {
        return (ERROR);
    }

    if ((maxReaders == 0) && (options & SEM_INVERSION_SAFE)) {
        return (ERROR);
    }
#else
    if (options & SEM_INVERSION_SAFE) {
        /* This option is not yet supported */
        errnoSet (S_semLib_INVALID_OPTION);
        return (ERROR);
    }
#endif

    rwSemId = (RW_SEM_ID) semId;
    /* Initialize writer and reader semaphore queues */
    if (semQInit (&rwSemId->semaphore.qHead, options) != OK) {
        return (ERROR);
    }

    if (semQInit (&rwSemId->basic.qHead, options) != OK) {
        /* errno set by semQInit() */
        return (ERROR);
    }

    rwSemId->semaphore.semOwner = NULL;
    rwSemId->semaphore.recurse = 0;
    rwSemId->semaphore.options = options;
    rwSemId->semaphore.semType = SEM_TYPE_RW;
    rwSemId->basic.maxReaders  = maxReaders;
    rwSemId->basic.nReaders    = 0;

    for (i = 0; i < maxReaders; i++) {
        rwSemId->extra[i].recurse = 0;
        rwSemId->extra[i].flags   = 0;
        rwSemId->extra[i].pad     = 0;
        rwSemId->extra[i].owner   = NULL;
    }

    /* Initialize events */
    eventResourceInit (&rwSemId->semaphore.events);

    /* Initialize object core */
    objCoreInit (&rwSemId->semaphore.objCore, semClassId);

    return (OK);
}

/***************************************************************************
 *
 * semRWGive - common entry point for reader and writer gives
 *
 * This routine automatically detects whether to try and give a reader,
 * or a writer.  Detection is based upon whether the current task is found
 * to have a reader-lock, or a writer lock.  In the event that no maximum
 * has been given for reader locks, then if the current task does not have
 * a writer-lock, it will be assumed to have a reader-lock.
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS semRWGive (
    SEM_ID  semId
    ) {
    RW_SEMAPHORE *  pRWsem;
    int  level;
    int  i;

    if (INT_RESTRICT () != OK) {
        errnoSet (S_intLib_NOT_ISR_CALLABLE);
        return (ERROR);
    }

    INT_LOCK (level);

    /* Verify object */
    if (OBJ_VERIFY(semId, semClassId) != OK) {

        INT_UNLOCK(level);
        /* errno set by OBJ_VERIFY() */
        return (ERROR);
    }

    pRWsem = (RW_SEMAPHORE *) semId;

    if (semId->semOwner == taskIdCurrent) {
        /* Current task has this read-write semaphore writer-locked */
        return (semWriterGive (pRWsem, level));
    }

    i = 0;
    if (pRWsem->basic.maxReaders != 0) {
        for (i = 0; i < pRWsem->basic.nReaders; i++) {
            if (pRWsem->extra[i].owner == taskIdCurrent) {
                break;
            }
        }
    }

    if (i == pRWsem->basic.nReaders) {
        /* Current task does not have it reader-locked. */
        INT_UNLOCK (level);
        errnoSet (S_semLib_NOT_OWNER);
        return (ERROR);
    }

    /* Current task has it reader-locked. */
    return (semReaderGive (pRWsem, level, i));
}

/***************************************************************************
 *
 * semWriterGive - give a writer semaphore
 *
 * Note that interrupts are locked upon entering this routine.
 *
 * RETURNS: OK on success, ERROR otherwise
 */

LOCAL STATUS semWriterGive (
    RW_SEMAPHORE *  pRWsem,   /* pointer to read-write semaphore */
    int             level     /* previous interrupt-lock level */
    ) {
    int     i;
    int     flags;
    SEM_RW_EXTRA *  pExtra;      /* ptr to r/w semaphore's extra section */
    SEM_RW_BASIC *  pBasic;      /* ptr to r/w semaphore's basic section */
    SEMAPHORE *     pSem;
    STATUS          status;

    pSem = &pRWsem->semaphore;

    if (pSem->recurse > 0) {    /* Check recursive case */
        pSem->recurse--;
        INT_UNLOCK (level);
        return (OK);
    }

    pBasic = &pRWsem->basic;
    pExtra = pRWsem->extra;

    /*
     * There is a writer-lock available to give.  If there are any pended
     * writers, unpend one.  Otherwise, attempt to unpend as many readers as
     * possible.
     *
     * Since two queues are used (one for readers and one for writers), FIFO
     * pending order can not be enforced across queues, although priority
     * pending order can.  Again, for simplicity, if there is a writer
     * pending, it shall be assumed that its importance is intrinsically
     * greater than ANY pended reader.
     */

    flags = 0;

    /* Determine if reader, writer or nothing is to be removed from pend Q */
    if (Q_FIRST (&pSem->qHead) != NULL) {
        flags |= SEM_RW_UNPEND_WRITER;
    } else if (Q_FIRST (&pBasic->qHead) != NULL) {
        pSem->semOwner = NULL;
        flags |= SEM_RW_UNPEND_READER;
    }

    if ((flags == 0) && (pSem->events.taskId != NULL)) {
        flags |= SEM_RW_SEND_EVENTS;
    }

    if ((pSem->options & SEM_DELETE_SAFE) &&
        (--taskIdCurrent->safeCount == 0) &&
        (Q_FIRST (&taskIdCurrent->safetyQ) != NULL)) {
        flags |= SEM_RW_SAFE_Q_FLUSH;
    }

    if (flags == 0) {        /* If nothing else needs to be done  */
        INT_UNLOCK (level);  /* then unlock interrupts and return */
        return(OK);
    }

    kernelState = TRUE;      /* More work needs to be done. */
    INT_UNLOCK (level);      /* Enter kernel state, and unlock ints. */

    status = semRWCommonGive (pRWsem, flags);    /* Common give work */
    status |= kernExit ();
    return (status);
}

/***************************************************************************
 *
 * semReaderGive - give a reader semaphore
 *
 * RETURNS: OK on success, ERROR otherwise
 */

LOCAL STATUS semReaderGive (
    RW_SEMAPHORE *  pRWsem,     /* read-write semaphore ID */
    int             level,       /* old interrrupt level */
    int             index
    ) {
    int     i;
    int     flags;
    SEM_RW_EXTRA *  pExtra;      /* ptr to r/w semaphore's extra section */
    SEM_RW_BASIC *  pBasic;      /* ptr to r/w semaphore's basic section */
    SEMAPHORE *     pSem;
    STATUS          status;

    pSem   = &pRWsem->semaphore;
    pBasic = &pRWsem->basic;
    pExtra = pRWsem->extra;

    if (pBasic->maxReaders != 0) {
        if (pExtra[index].recurse > 0) {
            pExtra[index].recurse--;
            INT_UNLOCK (level);
            return (OK);
        }

        pBasic->nReaders--;
        pExtra[i] = pExtra[pBasic->nReaders];
        memset (&pExtra[pBasic->nReaders], 0, sizeof (SEM_RW_EXTRA));
    } else {
        /* [nReaders] is known to be greater than zero */
        pBasic->nReaders--;
    }

    /*
     * There is a reader-lock available to give.  Only unpend a reader (if
     * there is a pended one) IF there are no pended writers.  This makes for
     * the simplest behaviour, but it might not be the best.  This strategy
     * should be reviewed at some point in the future.  This means that the
     * priority of a writer is intrinsically greater than that of a reader
     * regardless of the priority level of the task.
     */

    flags = 0;

    /* Determine if reader, writer or nothing is to be removed from pend Q */
    if (Q_FIRST (&pSem->qHead) != NULL) {
        if (pBasic->nReaders == 0) {           /* No more readers. */
            flags |= SEM_RW_UNPEND_WRITER;
        } 
    } else if (Q_FIRST (&pBasic->qHead) != NULL) {
        pSem->semOwner = NULL;     /* This line probably not needed. */
        flags |= SEM_RW_UNPEND_READER;
    }

    if ((flags == 0) && (pSem->events.taskId != NULL)) {
        flags |= SEM_RW_SEND_EVENTS;
    }

    if ((pSem->options & SEM_DELETE_SAFE) &&
        (--taskIdCurrent->safeCount == 0) &&
        (Q_FIRST (&taskIdCurrent->safetyQ) != NULL)) {
        flags |= SEM_RW_SAFE_Q_FLUSH;
    }

    if (flags == 0) {       /* If nothing else needs to be done  */
        INT_UNLOCK (level);  /* then unlock interrupts and return */
        return(OK);
    }

    kernelState = TRUE;      /* More work needs to be done. */
    INT_UNLOCK (level);      /* Enter kernel state, and unlock ints. */

    status = semRWCommonGive (pRWsem, flags);    /* Common give work */
    status |= kernExit ();
    return (status);
}

/***************************************************************************
 *
 * semWriterTake - take a writer-lock on the read-write semaphore
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS semWriterTake (
    SEM_ID    semId,
    unsigned  timeout
    ) {
    int level;                /* previous interrupt level */
    int pend;
    int i;
    RW_SEMAPHORE *  pRWsem;
    SEM_RW_BASIC *  pBasic;
    SEM_RW_EXTRA *  pExtra;
    STATUS status = OK;

    if (INT_RESTRICT () != OK) {
        errnoSet (S_intLib_NOT_ISR_CALLABLE);
        return (ERROR);
    }

tryAgain:
    INT_LOCK (level);

    /* Verify object */
    if (OBJ_VERIFY (semId, semClassId) != OK) {

        INT_UNLOCK(level);
        /* errno set by OBJ_VERIFY() */
        return (ERROR);
    }

    if (semId->semOwner == taskIdCurrent) {
        semId->recurse++;
        if (semId->recurse == 0) {
            semId->recurse--;
            INT_UNLOCK (level);
            errnoSet (S_semLib_RECURSION_LIMIT);
            return (ERROR);
        }
        INT_UNLOCK (level);
        return (OK);
    }

    pRWsem = (RW_SEMAPHORE *) semId;
    pBasic = &pRWsem->basic;
    pExtra = pRWsem->extra;

    /* Ensure writer is not a known reader */
    if (pBasic->maxReaders != 0) {
        for (i = 0; i < pBasic->nReaders; i++) {
            if (pExtra[i].owner == taskIdCurrent) {
                INT_UNLOCK (level);
                errnoSet (S_semLib_INVALID_OPERATION);
                return (ERROR);
            }
        }
    }

    /* If not locked by anyone, take a write-lock */
    if ((semId->semOwner == NULL) && (pBasic->nReaders == 0)) {
        semId->semOwner = taskIdCurrent;

        if (semId->options & SEM_DELETE_SAFE) {
            taskIdCurrent->safeCount++;
        }

        INT_UNLOCK (level);
        return (OK);
    }

    if (timeout == WAIT_NONE) {
        INT_UNLOCK (level);
        errnoSet (S_objLib_UNAVAILABLE);
        return (ERROR);
    }

    /* The current task must pend. */
    kernelState = TRUE;
    INT_UNLOCK (level);

    rtosPendQPut (&semId->qHead, timeout);

    status = kernExit ();
    if (status == SIG_RESTART) {
        goto tryAgain;
    }

    return (status);
}

/***************************************************************************
 *
 * semReaderTake - take a reader-lock on the read-write semaphore
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS semReaderTake (
    SEM_ID    semId,
    unsigned  timeout
    ) {
    int level;
    int pend;
    int i;
    RW_SEMAPHORE *  pRWsem;
    SEM_RW_BASIC *  pBasic;
    SEM_RW_EXTRA *  pExtra;
    STATUS status = OK;

    if (INT_RESTRICT () != OK) {
        errnoSet (S_intLib_NOT_ISR_CALLABLE);
        return (ERROR);
    }

tryAgain:
    INT_LOCK (level);

    /* Verify object */
    if (OBJ_VERIFY (semId, semClassId) != OK) {

        /* errno set by OBJ_VERIFY() */
        INT_UNLOCK (level);
        return (ERROR);
    }

    if (semId->semOwner == taskIdCurrent) {
        INT_UNLOCK (level);
        errnoSet (S_semLib_INVALID_OPERATION);
        return (ERROR);
    }

    pRWsem = (RW_SEMAPHORE *) semId;
    pBasic = &pRWsem->basic;
    pExtra = pRWsem->extra;

    /* Cover recursive reader take case. */
    if (pBasic->maxReaders != 0) {
        for (i = 0; i < pBasic->nReaders; i++) {
            if (pExtra[i].owner == taskIdCurrent) {
                pExtra[i].recurse++;
                if (pExtra[i].recurse == 0) {
                    pExtra[i].recurse--;
                    INT_UNLOCK (level);
                    errnoSet (S_semLib_RECURSION_LIMIT);
                    return (ERROR);
                }
                INT_UNLOCK (level);
                return (OK);
            }
        }
    }

    pend = 1;
    if ((semId->semOwner == NULL) && (Q_FIRST (&semId->qHead) == NULL)) {
        /*
         * The r/w semaphore is not write-locked, and is not waiting to
         * become so either.  It is assumed that if this is due to a timeout,
         * as many pended readers as possible will have already become
         * unpended.
         */

        if (pBasic->maxReaders != 0) {
            if (pBasic->nReaders < pBasic->maxReaders) {
                /* Add the reader to the reader list. */
                pExtra[pBasic->nReaders].recurse = 0;
                pExtra[pBasic->nReaders].flags = 0;
                pExtra[pBasic->nReaders].pad = 0;
                pExtra[pBasic->nReaders].owner = taskIdCurrent;
                pBasic->nReaders++;
                pend = 0;
            } 
        } else {
            pBasic->nReaders++;
            pend = 0;
        }
    }

    if (pend == 0) {
        if (semId->options & SEM_DELETE_SAFE) {
            taskIdCurrent->safeCount++;
        }

        INT_UNLOCK (level);
        return (OK);
    }

    if (timeout == WAIT_NONE) {
        INT_UNLOCK (level);
        errnoSet (S_objLib_UNAVAILABLE);
        return (ERROR);
    }

    /* The current task must pend. */
    kernelState = TRUE;
    INT_UNLOCK (level);

    rtosPendQWithHandlerPut (&semId->qHead, timeout,
                            semRWTimeoutHandler, semId, 0);

    status = kernExit ();
    if (status == SIG_RESTART) {
        goto tryAgain;
    }

    return (status);
}

/***************************************************************************
 *
 * semRWCommonGive - common "give" code for both readers and writers
 *
 * RETURNS: OK on success, ERROR otherwise
 */

LOCAL STATUS semRWCommonGive (
    RW_SEMAPHORE *  pRWsem,
    int             flags
    ) {
    SEMAPHORE *     pSem;
    SEM_RW_BASIC *  pBasic;
    SEM_RW_EXTRA *  pExtra;
    TCB_ID  pWriterTask;
    TCB_ID  pReaderTask;
    STATUS eventStatus;
    STATUS returnStatus = OK;
    int oldErrno = errno;

    pSem   = &pRWsem->semaphore;
    pBasic = &pRWsem->basic;
    pExtra = pRWsem->extra;

    /* Check which kernel work should be done */

    if (flags & SEM_RW_UNPEND_WRITER) {   /* Unpend a single writer */
        pWriterTask = (TCB_ID) Q_FIRST (&pSem->qHead);

        rtosPendQGet(&pSem->qHead);

        /* Get owner */
        pSem->semOwner = pWriterTask;

        if (pSem->options & SEM_DELETE_SAFE) {
            pWriterTask->safeCount++;
        }
    } else if (flags & SEM_RW_UNPEND_READER) {
        /*
         * No pended writers.  Attempt to unpend as many readers as possible.
         * Note that if a pended writer timed-out, then we may have multiple
         * readers to unpend.  This is because, at least at this time, the
         * timeout does not automatically unpend more readers.
         */

        while (((pBasic->nReaders < pBasic->maxReaders) ||
                (pBasic->maxReaders == 0)) &&
                ((pReaderTask = (TCB_ID) Q_FIRST (&pBasic->qHead)) != NULL)) {
            rtosPendQGet (&pBasic->qHead);

            pExtra[pBasic->nReaders].recurse = 0;
            pExtra[pBasic->nReaders].flags   = 0;
            pExtra[pBasic->nReaders].pad     = 0;
            pExtra[pBasic->nReaders].owner   = pReaderTask;
            pBasic->nReaders++;

            if (pSem->options & SEM_DELETE_SAFE) {
                pReaderTask->safeCount++;
            }
        }
    }

    /* Should safe queue be flushed */
    if (flags & SEM_RW_SAFE_Q_FLUSH) {
        rtosPendQFlush (&taskIdCurrent->safetyQ);
    }

    /* Should events be sent */
    if (flags & SEM_RW_SEND_EVENTS) {
        eventStatus = eventResourceSend (pSem->events.taskId,
				         pSem->events.registered);

        if (eventStatus != OK) {
            if ((pSem->options & SEM_EVENTSEND_ERROR_NOTIFY) != 0) {
                errnoSet (S_eventLib_SEND_ERROR);
                returnStatus = ERROR;
            }

            pSem->events.taskId = NULL;
        } else if ((pSem->events.options & EVENTS_SEND_ONCE) != 0)
            pSem->events.taskId = NULL;
    }

    if (returnStatus == OK) {
        errnoSet (oldErrno);
    }

    return (returnStatus);
}

/***************************************************************************
 *
 * semRWUpgradeForce - force a reader lock to upgrade
 *
 * This routine forces an upgrade to occur even if this semaphore has a 
 * a priority ordered queue, or an unlimited number of simultaneous readers,
 * or any recursive takes.
 *
 * If it is a priority ordered queue, note that there is no present way to
 * determine if the race condition described in semRWUpgrade() was hit.  This
 * is very RISKY and NOT RECOMMENDED.
 *
 * If it can have an unlimited number of readers, then the verification that
 * the current task is a reader can not be done.  This routine will now
 * assume (and that of course is where the danger lies) that it does have a
 * reader lock (if the reader count is non-zero), and act accordingly.
 *
 * Finally, as always with upgrades, check the return value and error code.
 * If the error code is S_semLib_LOST_LOCK, then YOU DO NOT HAVE ANY LOCKS.
 *
 * RETURNS: OK on success, ERROR on failure
 */

STATUS semRWUpgradeForce (
    SEM_ID    semId,
    unsigned  timeout
    ) {
    return (semRWUpgradeInternal (semId, timeout, TRUE));
}

/***************************************************************************
 *
 * semRWUpgrade - upgrade a reader lock to a writer lock
 *
 * This routine attempts to upgrade a reader-lock to a writer-lock.  If the
 * current task does not have the reader-lock, it will automatically fail.
 * If there are pended writers, then it will fail.  It should also be noted
 * that ONLY read-write semaphores with both a non-zero maximum number of
 * readers AND a FIFO ordered queue can be upgraded.
 *
 * The FIFO ordered queue limitation comes from the fact that if there are
 * no pended writers, and we pend the current task then, it is guaranteed
 * to be first in the list.  If it is a priority ordered queue, then a higher
 * task may come along and usurp its place in the queue.  When this other task
 * gets the writer-lock, the pending upgrade lock will be violated and thus
 * invalidated.  In other words, the pended upgrade would not have a lock.
 *
 * If the semaphore has been recursively taken, then all recursive takes
 * assigned to this task are immediately lost!
 *
 * If the upgrade fails, check the errno.  If it is S_semLib_LOST_LOCK, then
 * the lock has been completely lost!  Otherwise, it still retains the
 * reader-lock.
 *
 * RETURNS: OK on success, ERROR on failure
 */

STATUS semRWUpgrade (
    SEM_ID     semId,
    unsigned   timeout
    ) {
    return (semRWUpgradeInternal (semId, timeout, FALSE));
}

/***************************************************************************
 *
 * semRWUpgradeInternal - common code to forced and regular upgrades
 *
 * RETURNS: OK on success, ERROR on failure
 */

STATUS semRWUpgradeInternal (
    SEM_ID    semId,
    unsigned  timeout,
    BOOL      force
    ) {
    int level;
    int pend;
    int i;
    int recurse;
    SEM_RW_BASIC *  pBasic;
    SEM_RW_EXTRA *  pExtra;
    RW_SEMAPHORE *  rwSemId;
    STATUS status = OK;

    if (INT_RESTRICT () != OK) {
        errnoSet (S_intLib_NOT_ISR_CALLABLE);
        return (ERROR);
    }

    rwSemId = (RW_SEMAPHORE *) semId;

    INT_LOCK (level);
    
    /* Verify object */
    if (OBJ_VERIFY(&rwSemId->semaphore, semClassId) != OK) {

        INT_UNLOCK(level);
        /* errno set by OBJ_VERIFY() */
        return (ERROR);
    }

    if (((semId->options & SEM_Q_MASK) != SEM_Q_FIFO) && !force) {
        errnoSet (S_semLib_INVALID_Q_TYPE);
        return (ERROR);
    }

    pBasic = &rwSemId->basic;
    pExtra = rwSemId->extra;

    if (Q_FIRST (&rwSemId->semaphore.qHead != NULL)) {
        /* Can not upgrade if there is a writer pended.  Period. */
        INT_UNLOCK (level);
        errnoSet (S_semLib_INVALID_OPERATION);
        return (ERROR);
    }

    if (pBasic->maxReaders == 0) {
        if (!force || (pBasic->nReaders == 0)) {
            INT_UNLOCK (level);
            errnoSet (S_semLib_INVALID_OPERATION);
            return (ERROR);
        }
    } else {
        /* Determine if the current task has a reader-lock */
        for (i = 0; i < pBasic->nReaders; i++) {
            if (pExtra[i].owner == taskIdCurrent) {
                break;
            }
        }

        if (i == pBasic->nReaders) {
            INT_UNLOCK (level);
            errnoSet (S_semLib_INVALID_OPERATION);
            return (ERROR);
        }

        recurse = pExtra[i].recurse;
        pExtra[i].recurse = 0;
    }

    if (pBasic->nReaders == 1) {
        /* Can upgrade immediately */
        pBasic->nReaders = 0;
        rwSemId->semaphore.recurse  = 0;
        rwSemId->semaphore.semOwner = taskIdCurrent;
        if (pBasic->maxReaders != 0) {
            memset (pExtra, 0, sizeof (SEM_RW_EXTRA));
        }
        INT_UNLOCK (level);
        return (OK);
    }

    /* Do not attempt to pend if <timeout> is WAIT_NONE */

    if (timeout == WAIT_NONE) {
        if (pBasic->maxReaders != 0) {
            pExtra[i].recurse = recurse;    /* Restore recurse count */
        }
        INT_UNLOCK (level);
        errnoSet (S_objLib_UNAVAILABLE);
        return (ERROR);
    }

    /*
     * The current task must pend for the upgrade.  Remove
     * it from the list of readers.
     */

    pBasic->nReaders--;
    if (pBasic->maxReaders != 0) {
        pExtra[i] = pExtra[pBasic->nReaders];
        memset (&pExtra[pBasic->nReaders], 0, sizeof (SEM_RW_EXTRA));
    }

    kernelState = TRUE;
    INT_UNLOCK (level);

    rtosPendQPut (&rwSemId->semaphore.qHead, timeout);

    status = kernExit ();     /* As kernExit() may return a code indicating */
    if (status != OK) {       /* signal interruption, this must be treated  */
        status = ERROR;       /* as failure and loss of lock.               */
        errnoSet (S_semLib_LOST_LOCK);
    }

    return (status);
} 

/***************************************************************************
 *
 * semRWDowngrade - downgrade a writer lock to a reader lock
 *
 * This routine attempts to downgrade a writer-lock to a reader-lock.  If the
 * current task does not have the writer-lock, it will automatically fail.
 * This routine is only useful if there pended readers AND no pended writers.
 * Downgrading will allow other readers potential access to the locked resource
 * without sacrificing the current task's lock.
 *
 * Unlike the upgrade, there is no queue-type limitation.
 *
 * Like the upgrade, it can not be done if there are recursive locks.
 *
 * RETURNS: OK on success, ERROR on failure
 */

STATUS semRWDowngrade (
    SEM_ID  semId
    ) {
    int level;
    int unpend;
    int i;
    SEM_RW_BASIC *  pBasic;
    SEM_RW_EXTRA *  pExtra;
    RW_SEMAPHORE *  rwSemId;
    STATUS status = OK;

    if (INT_RESTRICT () != OK) {
        errnoSet (S_intLib_NOT_ISR_CALLABLE);
        return (ERROR);
    }

    rwSemId = (RW_SEMAPHORE *) semId;
    INT_LOCK (level);
    
    /* Verify object */
    if (OBJ_VERIFY(&rwSemId->semaphore, semClassId) != OK) {

        INT_UNLOCK(level);
        /* errno set by OBJ_VERIFY() */
        return (ERROR);
    }

    if (rwSemId->semaphore.semOwner != taskIdCurrent) {
        INT_UNLOCK (level);
        errnoSet (S_semLib_NOT_OWNER);
        return (ERROR);
    }

    pBasic = &rwSemId->basic;
    pExtra = rwSemId->extra;

    /*
     * There should be no reader-locks present at this point.  See comments
     * in upgrade section about recursively taken locks.
     */

    if (pExtra[i].recurse != 0) {
        INT_UNLOCK (level);
        errnoSet (S_semLib_INVALID_OPERATION);
        return (ERROR);
    }

    /*
     * We currently have a writer-lock.  We can downgrade immediately.  The
     * existence of any pended writers or pended readers has no effect on
     * this action.  If you think about it, we are only restricting the
     * privileges this single lock has.  If there are writers pending, then
     * no other readers can unpend.  If not, but there are readers pending,
     * then more readers can unpend.
     *
     * One day, it may be worthwhile to track that this reader was downgraded.
     * This could be useful in upgrading it back to a writer.
     */

    pBasic->nReaders++;
    if (pBasic->maxReaders != 0) {
        rwSemId->semaphore.semOwner = NULL;
        pExtra->recurse = 0;
        pExtra->flags = 0;
        pExtra->pad = 0;
        pExtra->owner = taskIdCurrent;
    }

    unpend = 0;   /* Default */
    if (Q_FIRST (&rwSemId->semaphore.qHead) == NULL) {
        unpend = (Q_FIRST (&pBasic->qHead) != NULL);
    }

    if (unpend == 0) {
        INT_UNLOCK (level);
        return (OK);
    }

    kernelState = TRUE;    /* Enter kernel */
    INT_UNLOCK (level);

    status = semRWCommonGive (rwSemId, SEM_RW_UNPEND_READER);

    status |= kernExit ();
    return (status);
} 

/***************************************************************************
 *
 * semRWTimeoutHandler - handler called in event task times out on rw-semaphore
 *
 * This routine is the handler that is called if a task pended on a read-write
 * semaphore times-out.  If there are no writers pending, then this routine
 * will attempt to unpend as many readers as possible.
 *
 * Please note that <kernelState> is TRUE.
 *
 * Future note: When priority inheritance is added, this routine will need to
 * be updated.
 *
 * RETURNS: N/A
 */

LOCAL void semRWTimeoutHandler (
    void *  pVoid,    /* task that timed out */
    int     src       /* reason handler is called (rtos_TIMEOUT) */
    ) {
    RW_SEMAPHORE *  pSem;
    TCB *  pTask = (TCB *) pVoid;    /* task that timed out */

    if ((src != rtos_TIMEOUT) ||
	!kernelState ||
	(taskIdVerify ((int) pTask) != OK))
        return;

    pSem = pTask->pObj;
    if (OBJ_VERIFY(&pSem->semaphore, semClassId) != OK)
        return;

    if (pSem->semaphore.semType != SEM_TYPE_RW)
        return;

    /*
     * The following sanity checks have passed.
     * 1.  Called due to a timeout.
     * 2.  <kernelState> is TRUE.
     * 3.  Timed-out task exists.
     * 4.  Object is an existing read-write semaphore.
     */  

    if ((pSem->semaphore.semOwner == NULL) &&
        (Q_FIRST (&pSem->semaphore.qHead) != NULL)) {
        return;
    }

    /* Not writer-locked, and no pended writers--unpend readers. */

    semRWCommonGive (pSem, SEM_RW_UNPEND_READER);

    /* As <kernelState> is TRUE, do not call kernExit() */
    return;
}

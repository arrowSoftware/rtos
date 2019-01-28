/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2010 Surplus Users Ham Society
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

/* fsEventUtilLib.c - file systems event utility library */

/* includes */

#include <rtos.h>

#include <stdlib.h>

#include <fs/erfLib.h>
#include <fs/fsEventUtilLib.h>

/* defines */

/* structs */

/* typedefs */

/* globals */

/* locals */

LOCAL BOOL fsEventUtilLibInitialized = FALSE;
LOCAL int  fsEventCategory;
LOCAL int  fsEventPathAdded;

/***************************************************************************
 *
 * fsEventUtilInit - initialize the file system event utility library
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS fsEventUtilInit (void) {
    if (fsEventUtilLibInitialized) {
        return (OK);
    }

    if (erfCategoryAllocate (&fsEventCategory) != OK) {
        /* errno set by erfCategoryAllocate() */
        return (ERROR);
    }

    if (erfTypeAllocate (fsEventCategory, &fsEventPathAdded) != OK) {
        /* errno set by erfTypeAllocate() */
        return (ERROR);
    }

    fsEventUtilLibInitialized = TRUE;

    return (OK);
}

/***************************************************************************
 *
 * fsEventHandler -
 *
 * RETURNS: N/A
 */

LOCAL void fsEventHandler (
    int     category,
    int     type,
    void *  pEventData,
    void *  pUserData
    ) {
    FS_PATH_WAIT_STRUCT *  pWaitData = pUserData;
    char *                 path      = pEventData;

    if ((category != fsEventCategory) && (type != fsEventPathAdded) ||
        (strcmp (path, pWaitData->path) != 0)) {
        return;
    }

    semGive (pWaitData->semId);

    erfHandlerUnregister (fsEventCategory, fsEventPathAdded, fsEventHandler,
                          pWaitData);
}

/***************************************************************************
 *
 * fsPathAddedEventSetup - prepare to wait for a path
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS fsPathAddedEventSetup (
    FS_PATH_WAIT_STRUCT *  pWaitData,
    char *                 path
    ) {
    if ((pWaitData == NULL) || (path == NULL)) {
        errnoSet (S_fsEventUtil_INVALID_PARAMETER);
        return (ERROR);
    }

    pWaitData->semId = semBCreate (SEM_Q_PRIORITY, SEM_EMPTY);
    if (pWaitData->semId == NULL) {
        /* errno set by semBCreate() */
        return (ERROR);
    }
    pWaitData->path = path;

    if (erfHandlerRegister (fsEventCategory, fsEventPathAdded,
                            fsEventHandler, pWaitData, 0) != OK) {
        semDelete (pWaitData->semId);
        return (ERROR);
    }

    return (OK);
}

/***************************************************************************
 *
 * fsPathAddedEventRaise - raise a path added event
 *
 * RETURNS: N/A
 */

void fsPathAddedEventRaise (
    char *  path
    ) {
    STATUS  status;

    /*
     * XXX
     * Note: It is still to be determined if raising the event
     * should remain asynchonous, or if it should be synchronous,
     * or if it does not matter at all.
     * XXX
     */

    erfEventRaise (fsEventCategory, fsEventPathAdded,
                   ERF_ASYNC_PROCESS, path, NULL);

    return;
}

/***************************************************************************
 *
 * fsWaitForPath - wait for the path added event
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS  fsWaitForPath (
    FS_PATH_WAIT_STRUCT *  pWaitData
    ) {
    STATUS  status;

    if ((pWaitData == NULL) ||
        (pWaitData->semId == NULL) || (pWaitData->path == NULL)) {
        errnoSet (S_fsEventUtil_INVALID_PARAMETER);
        return (ERROR);
    }

    status = semTake (pWaitData->semId, WAIT_FOREVER);

    /*
     * Under successful operations, it would not be necessary to unregister
     * the waiting path as it would have already been done.  However, as
     * semTake() may fail, unregister it to be safe.  Note that if the success
     * path was taken, this unregister has no effect.
     */

    erfHandlerUnregister (fsEventCategory, fsEventPathAdded, fsEventHandler,
                          pWaitData);

    semDelete (pWaitData->semId);

    return (status);
}

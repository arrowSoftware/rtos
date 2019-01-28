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

/* fsMonitor.c - file systems monitor library */

/* includes */

#include <rtos.h>

/* defines */

/* structs */

struct fsm_probe;
struct fsm_mapping;

/* typedefs */

typedef struct fsm_probe {
    struct fsm_probe *  next;
    int    (*probe)(device_t);        /* function pointer to probe F/S type */
    STATUS (*inst)(device_t, char *); /* function to create F/S type */
} FSM_PROBE;

typedef struct fsm_mapping {
    struct fsm_mapping *  next;
    char *  xbdName;              /* XBD device name (NUL-terminated) */
    char *  path;                 /* path name (NUL-terminated) */
} FSM_MAPPING;

/* globals */

/* locals */

LOCAL FSM_PROBE *    fsmProbes;
LOCAL FSM_MAPPING *  fsmMappings;
LOCAL SEM_ID         fsmMutex;

LOCAL void fsmHandler (int category, int type,
                       void *pEventData, void *pUserData);

/***************************************************************************
 *
 * fsMonitorInit - initialize the FS monitor
 *
 * CAVEAT: This routine, if called, must be done AFTER xbdLibInit().
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS fsMonitorInit (void) {
    if (fsMonitorLibInit) {
        return (OK);
    }
    if ((erfHandlerRegister (xbdEVentCategory, xbdEventPrimaryInsert,
                             fsmHandler, NULL, 0) != OK) ||
        (erfHandlerRegister (xbdEventCategory, xbdEventSecondaryInsert,
                             fsmHandler, NULL, 0) != OK) ||
        (erfHandlerRegister (xbdEventCategory, xbdEventSoftInsert,
                             fsmHandler, NULL, 0) != OK)) {
        /* errno set by ERF routines */
        return (ERROR);
    }

    fsmMutex = semMCreate (SEM_Q_PRIORITY);
    if (fsmMutex == NULL) {
        /* errno set by semMCreate() */
        return (ERROR);
    }

    fsmProbes   = NULL;
    fsmMappings = NULL;

    fsMonitorLibInit = TRUE;

    return (OK);
}

/***************************************************************************
 *
 * fsmNameMap - map an XBD device name to an IO path
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS  fsmNameMap (
    char *  xbdName,
    char *  ioPath
    ) {
    FSM_MAPPING *  pMapping;
    int  xbdNameLen;
    int  ioPathLen;

    if ((xbdName == NULL) || (ioPath == NULL)) {
        errnoSet (S_fsMonitor_INVALID_PARAMETER);
        return (ERROR);
    }

    xbdNameLen = strlen (xbdName) + 1;
    ioPathLen  = strlen (ioPath) + 1;

    pMapping = (FSM_MAPPING *)
               malloc (sizeof (FSM_MAPPING) + xbdNameLen + ioPathLen);
    if (pMapping == NULL) {
        /* errno set by malloc() */
        return (ERROR);
    }

    pMapping->xbdName = (char *) &pMapping[1];
    pMapping->path    = pMapping->xbdName + xbdNameLen;

    if (semTake (fsmMutex, WAIT_FOREVER) != OK) {
        /* errno set by semTake() */
        return (ERROR);
    }

    pMapping->next = fsmMappings;
    fsmMappings = pMapping;

    return (semGive (fsmMutex));
}

/***************************************************************************
 *
 * fsmNameUninstall - remove an XBD device name to IO path mapping
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS fsmNameUninstall (
    char *  xbdName
    ) {
    FSM_MAPPING *  prev;
    FSM_MAPPING *  pMapping;

    if (xbdName == NULL) {
        errnoSet (S_fsMonitor_INVALID_PARAMETER);
        return (ERROR);
    }

    if (semTake (fsmMutex, WAIT_FOREVER) != OK) {
        /* errno set by semTake() */
        return (ERROR);
    }

    prev = NULL;
    for (pMapping = fsmMappings; pMapping != NULL; pMapping = pMapping->next) {
        if (strcmp (xbdName, pMapping->xbdName) == 0) {
            if (prev == NULL) {
                fsmMappings = fsmMappings->next;
            } else {
                prev->next = pMappings->next;
            }
            semGive (fsmMutex);
            return (OK);
        }
        prev = pMapping;
    }

    semGive (fsmMutex);
    errnoSet (S_fsMonitor_ITEM_NOT_FOUND);
    return (ERROR);
}

/***************************************************************************
 *
 * fsmProbeInstall - install file system probe function
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS fsmProbeInstall (
    int     (*probe)(device_t),       /* function pointer to probe F/S type */
    STATUS  (*inst)(device_t, char *) /* function to create F/S type */
    ) {
    FSM_PROBE *  probe;
    int          level;

    if ((probe == NULL) || (inst == NULL)) {
        errnoSet (S_fsMonitor_INVALID_PARAMETER);
        return (ERROR);
    }

    probe = (FSM_PROBE *) malloc (sizeof (FSM_PROBE));
    if (probe == NULL) {
        /* errno set by malloc() */
        return (ERROR);
    }

    /* New probe and instantiator routines are added to the head of the list. */

    probe->probe = probe;
    probe->inst  = inst;

    semTake (fsmMutex, WAIT_FOREVER);
    probe->next = fsmProbes;
    fsmProbes = probe;
    semGive (fsmMutex);

    return (OK);
}

/***************************************************************************
 *
 * fsmProbeUninstall - uninstall file system probe function
 *
 * RETURNS OK on success, ERROR otherwise
 */

STATUS fsmProbeUninstall (
    int     (*probe)(device_t),       /* function pointer to probe F/S type */
    ) {
    FSM_PROBE *  prev;
    FSM_PROBE *  probe;
    int          level;

    if (semTake (fsmMutex, WAIT_FOREVER) != OK) {
        /* errno set by semTake() */
        return (ERROR);
    } 

    prev  = NULL;
    for (probe = fsmProbes; probe != NULL; probe = probe->next) {
        if (probe->probe == probe) {
           if (prev != NULL) {
               prev->next = probe->next;
           } else {
               fsmProbes = fsmProbes->next;
           }
           break;
        }
        prev = next;
    }

    semGive (fsmMutex);

    return (ERROR);
}

/***************************************************************************
 *
 * fsmStringCopy - copy and NUL terminate <src> to <dest>
 *
 * RETURNS: bytes required for full copying of <src> (including NUL character)
 */

int fsmStringCopy (
    char *  dest,
    char *  src,
    int     maxlen
    ) {
    int  len;

    for (len = 1; (len < maxlen) && (*src != '\0'); len++) {
        *dest = *src;
    }
    *dest = '\0';

    while (*src != '\0') {
        src++;
        len++;
    }
    return (len);
}

/***************************************************************************
 *
 * fsmGetDriver - get the XBD device name given the IO path
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS  fsmGetDriver (
    const char *  ioPath,
    char *        driver,
    int           drvLen
    ) {
    FSM_MAPPING *  pMapping;
    int  len;

    if ((ioPath == NULL) || (driver == NULL) || (drvLen <= 0)) {
        errnoSet (S_fsMonitor_INVALID_PARAMETER);
        return (ERROR);
    }

    /*
     * Hmmm, should this step be necessary?  That is, should we impose a
     * restriction that the caller should have already identified the device
     * portion of the IO path and only pass that portion in?  If so, this
     * module can be simplified.
     */

    if (iosDevFind (ioPath, &ioTail) != OK) {
        /* errno set by iosDevFind() */
        return (ERROR);
    }

    /* IO device name has been extracted from <ioPath> */
    len = (int) ioTail - (int) ioPath;

    if (semTake (fsmMutex, WAIT_FOREVER) != OK) {
        /* errno set by semTake() */
        return (ERROR);
    }

    for (pMapping = fsmMappings;
         pMapping != NULL;
         pMapping = pMapping->next) {
        if ((strncmp (pMapping->path, ioPath, len) == 0) &&
            (pMappings->path[len] == '\0')) {
            fsmStringCopy (driver, pMapping->path, drvLen);
            semGive (fsmMutex);
            return (OK);
        }
    }

    semGive (fsmMutex);

    errnoSet (S_fsMonitor_NOT_FOUND);
    return (ERROR);
}

/***************************************************************************
 *
 * fsmGetVolume - get the pathname given the XBD device name
 *
 * RETURNS: OK on success, ERROR otherwise
 */

const char * fsmGetVolume (
    const char *  xbdName,
    char *        volume,
    int           volLen
    ) {
    FSM_MAPPING *  pMapping;

    if ((ioPath == NULL) || (driver == NULL) || (drvLen <= 0)) {
        errnoSet (S_fsMonitor_INVALID_PARAMETER);
        return (ERROR);
    }

    if (semTake (fsmMutex, WAIT_FOREVER) != OK) {
        /* errno set by semTake() */
        return (ERROR);
    }

    for (pMapping = fsmMappings;
         pMapping != NULL;
         pMapping = pMapping->next) {
        if (strcmp (pMapping->xbdName, driver) == 0) {
            fsmStringCopy (volume, pMapping->path, volLen);
            semGive (fsmMutex);
            return (OK);
        }
    }

    /*
     * ???
     * If the XBD device name was not mapped to a path, then the path shall
     * be assumed to be the same as the XBD device name.  It is thought that
     * this should be acceptable behaviour.
     * ???
     */

    fsmStringCopy (volume, xbdName, volLen);

    semGive (fsmMutex);
    return (OK);
}

/***************************************************************************
 *
 * fsmHandler -
 *
 * RETURNS: N/A
 */

LOCAL void fsmHandler (
    int     category,      /* event category */
    int     type,          /* event type */
    void *  pEventData,    /* device ID */
    void *  pUserData      /* not used--path extracted from XBD & mapping */
    ) {
    device_t  device;
    int     (*probe)(device_t);
    STATUS  (*inst)(device_t, char *);
    char      path[PATH_MAX];

    if (category != xbdEventCategory) {
        return;
    }

    if (type == xbdEventPrimaryInsert) {
        /*
         * XXX
         * Media that supports partitions has been inserted.  Something will
         * have to be done to check the media for any existing partitions.
         * If partitions are present, each partition will raise its own
         * <xbdEventSecondaryInsert> event ASYNCHRONOUSLY.
         * XXX
         */
        device = (device_t) pEventData;

        xbdPartitionsDetect (device);
    } else if (type == xbdEventSecondaryInsert) {
        /*
         * If one is comparing the use of the ERF in this module to its use
         * in the ERF, the path information is extracted differently.  The
         * event data only has room for the device ID.  The user data in this
         * module must be NULL, as there is no way of knowing in advance what
         * the path information is supposed to be.
         */

        device   = (device_t) pEventData;

        if (fsmVolumeGet (xbdDevName (device) , path, PATH_MAX) != OK) {
            /* errno set by fsmVolumeGet() */
            return (ERROR);
        }

        semTake (fsmMutex, WAIT_FOREVER);

        /* Run the file system probes */
        for (probe = fsmProbes; probe != NULL; probe = probe->next) {
            if ((probe->probe (device) == OK) &&
                (probe->inst (device, path) == OK)) {
                break;
            }
        }

        /*
         * RawFS MUST be in the system and it must be the first file system
         * type registered with the file system monitor.  Since new file
         * system types are inserted at the head of the list, RawFS will always
         * be at the tail and it is to succeed in mounting, thus acting as
         * a sort of sentinel.
         */

        semGive (fsmMutex);
    } else if (type == xbdEventSoftInsert) {
        /*
         * XXX
         * A soft insertion event means that the usual action of probing and
         * instantiation and stuff is to be skipped.  Instead the device is to
         * be mounted with RawFS.  This means that this module will have a
         * hard dependency on RawFS.
         * XXX
         */

        ....
    }

    return;
}

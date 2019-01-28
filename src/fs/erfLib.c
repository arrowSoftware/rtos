/* erfLib.c - event reporting framework */

/*
 * The event reporting framework will be used with the file system framework.
 * It may also be used without file systems for other purposes.
 */

/* includes */

#include <rtos.h>

#include <stdlib.h>

#include <rtos/msgQLib.h>
#include <rtos/semLib.h>
#include <rtos/taskLib.h>

#include <fs/erfLib.h>

/* defines */

#define ERF_TASK_PRIORITY  50    /* currently an arbitrary priority level */

/* structs */

struct erfType;

/* typedefs */

typedef struct erfType {
    struct erfType *  next;
    void    (*func)(int, int, void *, void*);
    void *  pUserData;
} ERF_TYPE;

typedef struct erfCategory {
    int          typesUsed;
    ERF_TYPE **  type;
} ERF_CATEGORY;

typedef struct erfMsg {
    int     category;
    int     type;
    void    (*pFreeFunc)(void *);
    void *  pEventData;
} ERF_MSG;

/* forward declarations */

LOCAL STATUS erfTask (void);

/* locals */

LOCAL ERF_CATEGORY *  erfTable;
LOCAL int          erfCategories;    /* maximum # of user categories */
LOCAL int          erfTypes;         /* maximum # of user types */
LOCAL int          erfCatUsed;       /* # of user categories in use */
LOCAL int          erfTid;
LOCAL MSG_Q_ID     erfMsgQ;
LOCAL SEM_ID       erfMutex;

/***************************************************************************
 * 
 * erfLibInit - 
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS erfLibInit (
    int  maxCategories,
    int  maxTypes
    ) {
    char *  data;
    int     size;
    int     i;

    if (erfTable != NULL) {
        return (OK);
    }

    size  = maxCategories * sizeof (ERF_CATEGORY);
    size += maxCategories * maxTypes * sizeof (ERF_TYPE **);

    data = (char *) malloc (size);
    if (data == NULL) {
        /* errno handled by malloc() */
        return (ERROR);
    }

    memset (data, 0, size);

    erfTable = (ERF_CATEGORY *) data;
    for (i = 0; i < maxCategories; i++) {
        erfTable[i].type = (ERF_TYPE **) data;
        data += (maxTypes * sizeof (ERF_TYPE **));
    }

    erfMutex = semMCreate (SEM_Q_PRIORITY);
    if (erfMutex == NULL) {
        /* errno handled by semMCreate() */
        free (erfTable);
        erfTable = NULL;
        return (ERROR);
    }

    erfMsgQ = msgQCreate (32, sizeof (ERF_MSG), MSG_Q_PRIORITY);
    if (erfMsgQ == NULL) {
        /* errno handled by msgQCreate() */
        free (erfTable);
        semDelete (erfMutex);
        erfTable = NULL;
        return (ERROR);
    }

    erfTid = taskSpawn ("erfTask", ERF_TASK_PRIORITY, 0, 8192, erfTask,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (erfTid == ERROR) {
        /* errno handled by taskSpawn() */
        free (erfTable);
        semDelete (erfMutex);
        msgQDelete (erfMsgQ);
        erfTable = NULL;
        return (ERROR);
    }

    return (OK);
}

/***************************************************************************
 *
 * erfHandlerRegister -
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS erfHandlerRegister (
    int     eventCategory,
    int     eventType,
    void    (*func)(int, int, void *, void*),
    void *  pUserData,
    int     flags
    ) {
    ERF_TYPE *     erfType;   
    int  i = 0;
    int  j = 0;

    if ((func == NULL) || (eventCategory >= erfCatUsed) ||
        (eventType >= erfTable[eventCategory].typesUsed)) {
        errnoSet (S_erfLib_INVALID_PARAMETER);
        return (ERROR);
    }

    erfType = (ERF_TYPE *) malloc (sizeof (ERF_TYPE));
    if (erfType == NULL) {
        /* errno set by malloc() */
        return (ERROR);
    }

    erfType->func = func;
    erfType->pUserData = pUserData;

    semTake (erfMutex, WAIT_FOREVER);

    erfType->next = erfTable[eventCategory].type[eventType];
    erfTable[eventCategory].type[eventType] = erfType;

    semGive (erfMutex);

    return (OK);
}

/***************************************************************************
 *
 * erfHandlerUnRegister -
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS erfHandlerUnregister (
    int     eventCategory,
    int     eventType,
    void    (*func)(int, int, void *, void*),
    void *  pUserData
    ) {
    ERF_TYPE *  erfType;
    ERF_TYPE *  prevErfType;
    STATUS      status;

    if ((eventCategory >= erfCatUsed) ||
        (eventType >= erfTable[eventCategory].typesUsed)) {
        errnoSet (S_erfLib_INVALID_PARAMETER);
        return (ERROR);
    }

    semTake (erfMutex, WAIT_FOREVER);

    erfType = erfTable[eventCategory].type[eventType];

    prevErfType = NULL;
    while (erfType != NULL) {
        /* Check if handler and user data match. */
        if ((erfType->func == func) && (erfType->pUserData == pUserData)) {
            /* Update pointers */
            if (prevErfType != NULL) {
                prevErfType->next = erfType->next;
            } else {
                erfTable[eventCategory].type[eventType] = erfType->next;
            }

            free (erfType);
            semGive (erfMutex);
            return (OK);
        }

        prevErfType = erfType;
        erfType = erfType->next;
    }

    semGive (erfMutex);

    errnoSet (S_erfLib_HANDLER_NOT_FOUND);
    return (ERROR);
}

/***************************************************************************
 *
 * erfTask - 
 *
 * RETURNS: Not expected to return
 */

LOCAL STATUS erfTask (void) {
    ERF_MSG     erfMsg;
    ERF_TYPE *  erfType;
    int  eventCategory;
    int  eventType;

    while (msgQReceive (erfMsgQ, &erfMsg, sizeof (ERF_MSG),
                        WAIT_FOREVER) == OK) {
        eventCategory = erfMsg.category;
        eventType     = erfMsg.type;

        semTake (erfMutex, WAIT_FOREVER);

        erfType = erfTable[eventCategory].type[eventType];
        while (erfType != NULL) {
            erfType->func (eventCategory, eventType,
                           erfMsg.pEventData, erfType->pUserData);
            erfType = erfType->next;
        }

        semGive (erfMutex);

        if (erfMsg.pFreeFunc != NULL) {
            erfMsg.pFreeFunc (erfMsg.pEventData);
        }
    }

    /* XXX Do we want to restart this task? XXX */

    return (ERROR);
}

/***************************************************************************
 *
 * erfEventRaise - raise an event
 * 
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS erfEventRaise (
    int     eventCategory,
    int     eventType,
    int     processType,
    void *  pEventData,
    void    (*freeFunc)(void *)
    ) {
    ERF_TYPE *  erfType;
    ERF_MSG     erfMsg;
    STATUS      status;

    if ((eventCategory >= erfCatUsed) ||
        (eventType >= erfTable[eventCategory].typesUsed)) {
        errnoSet (S_erfLib_INVALID_PARAMETER);
        return (ERROR);
    }

    if (processType == ERF_ASYNC_PROCESS) {
        erfMsg.category   = eventCategory;
        erfMsg.type       = eventType;
        erfMsg.pFreeFunc  = freeFunc;
        erfMsg.pEventData = pEventData;

        status = msgQSend (erfMsgQ, &erfMsg, sizeof (ERF_MSG),
                           WAIT_NONE, MSG_PRI_NORMAL);
        return (status);
    }

    semTake (erfMutex, WAIT_FOREVER);

    erfType = erfTable[eventCategory].type[eventType];
    while (erfType != NULL) {
        erfType->func (eventCategory, eventType,
                       pEventData, erfType->pUserData);
        erfType = erfType->next;
    }
    semGive (erfMutex);

    if (freeFunc != NULL) {
        freeFunc (pEventData);
    }

    return (OK);
}

/***************************************************************************
 *
 * erfCategoryAllocate -
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS erfCategoryAllocate (
    int *  pCategory
    ) {
    int     i;

    if (pCategory == NULL) {
        errnoSet (S_erfLib_INVALID_PARAMETER);
        return (ERROR);
    }

    semTake (erfMutex, WAIT_FOREVER);

    if (erfCatUsed == erfCategories) {
        semGive (erfMutex);
        errnoSet (S_erfLib_TOO_MANY_USER_CATS);
        return (ERROR);
    }

    /*
     * Presently, neither categories nor types can be deleted.  Thus, only the
     * number of allocated categories needs to be incremented.  Also, there is
     * no need to initialize [typesUsed] as that was done when the table was
     * initialized.
     */

    *pCategory = erfCatUsed;
    erfCatUsed++;

    semGive (erfMutex);

    return (OK);
}

/***************************************************************************
 *
 * erfTypeAllocate -
 *
 * RETURNS: OK on success, ERROR otherwise
 */

STATUS erfTypeAllocate (
    int    category,
    int *  pType
    ) {
    ERF_TYPE *  pErfType;
    int  i;

    if ((category >= erfCategories) || (pType == NULL)) {
        errnoSet (S_erfLib_INVALID_PARAMETER);
        return (ERROR);
    }

    semTake (erfMutex, WAIT_FOREVER);

    if (erfTable[category].typesUsed == erfTypes) {
        semGive (erfMutex);
        errnoSet (S_erfLib_TOO_MANY_USER_TYPES);
        return (ERROR);
    }

    /*
     * Presently, neither categories, nor types can be deleted.
     * The ERF_TYPE pointer is already NULL from table initialization.
     * Thus, only the number of allocated types needs to be incremented.
     */

    *pType = erfTable[category].typesUsed;
    erfTable[category].typesUsed++;

    semGive (erfMutex);

    return (OK);
}

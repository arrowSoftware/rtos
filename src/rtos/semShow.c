#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <arch/intArchLib.h>
#include <util/qLib.h>
#include <util/classLib.h>
#include <rtos/taskLib.h>
#include <rtos/eventLib.h>
#include <rtos/eventShow.h>
#include <rtos/semLib.h>
#include <rtos/semShow.h>
#include <rtos/taskLib.h>

IMPORT Q_HEAD kernTickQ;

LOCAL char *semTypeName[MAX_SEM_TYPE] = {
  "BINARY", "MUTEX", "COUNTING", "RW"
};

/*******************************************************************************
 * semShowInit - Initialize semaphore info facilities
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS semShowInit(void)
{
  return classShowConnect(semClassId, (FUNCPTR) semShow);
}

/*******************************************************************************
 * semInfo - Get list of task blocking on a semaphore
 *
 * RETURNS: Number of tasks
 ******************************************************************************/

int semInfo(SEM_ID semId, int idList[], int max)
{
  int n, level;

  if (INT_RESTRICT() != OK)
    return ERROR;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Verify object class */
  if (OBJ_VERIFY(semId, semClassId) != OK) {

    INT_UNLOCK(level);
    return ERROR;

  }

  /* Get number of tasks blocking on semaphore */
  n = Q_INFO(&semId->qHead, idList, max);

  /* Unlock interrupts */
  INT_UNLOCK(level);

  return n;
}

/*******************************************************************************
 * semShow - Show semaphore info
 *
 * RETURNS: OK or ERROR
 ******************************************************************************/

STATUS semShow(SEM_ID semId, int mode)
{
  TCB_ID tcbId;
  int i, nTask;
  int level;
  int taskIdList[20], taskDList[20];
  char tmpString[128];
  EVENTS_RESOURCE evRes;

  /* Lock interrupts */
  INT_LOCK(level);

  /* Get number of tasks blocking on semaphore */
  nTask = semInfo(semId, taskIdList, 20);
  if (nTask == ERROR) {

    INT_UNLOCK(level);
    printf("Invalid semaphore id: %#x\n", (int) semId);
    return ERROR;

  }

  /* If any tasks in list */
  if (nTask > 0) {

    /* For all tasks */
    for (i = 0;
         i < min( nTask, NELEMENTS(taskIdList) );
         i++) {

      tcbId = (TCB_ID) taskIdList[i];
      if (tcbId->status & TASK_DELAY)
        taskDList[i] = Q_KEY(&kernTickQ, &tcbId->tickNode, 1);
      else
        taskDList[i] = 0;

    } /* End for all tasks */

  } /* End if any tasks in list */

  /* Get owner task and events resource */
  tcbId = SEM_OWNER_GET(semId);
  evRes = semId->events;

  /* Unlock interrupts */
  INT_UNLOCK(level);

  /* Get queue type */
  if ( (semId->options & SEM_Q_MASK) == SEM_Q_FIFO )
    strcpy(tmpString, "FIFO");
  else
    strcpy(tmpString, "PRIORITY");

  /* Print info */
  printf("\n");
  printf("Semaphore Id        : 0x%-10x\n", (int) semId);
  printf("Semaphore Type      : %-10s\n", semTypeName[semId->semType]);
  printf("Task Queuing        : %-10s\n", tmpString);
  printf("Pended Tasks        : %-10d\n", nTask);

  /* Select semaphore type */
  switch(semId->semType) {

    case SEM_TYPE_BINARY:

      if (tcbId != NULL)
        printf("State               : %-10s\n", "EMPTY");
      else
        printf("State               : %-10s\n", "FULL");

    break;

    case SEM_TYPE_MUTEX:

      /* If non null tcb */
      if (tcbId != NULL) {

        printf("Owner               : 0x%-10x", (int) tcbId);

        if ( taskIdVerify((int) tcbId) != OK )
          printf(" Deleted!\n");
        else if (tcbId->name != NULL)
          printf(" (%s)\n", tcbId->name);
        printf("\n");

      } /* End if non null tcb */

      /* Else null tcb */
      else {

        printf("Owner               : %-10s\n", "NONE");

      } /* End else null tcb */

    break;

    case SEM_TYPE_COUNTING:

      printf("Count:              : %-10d\n", SEM_COUNT(semId));

    break;

    default:

      printf("State:             : 0x%-10x\n", (int) tcbId);

    break;

  } /* End select semaphore type */

  /* Get options string */
  if ( (semId->options & SEM_Q_MASK) == SEM_Q_FIFO )
    strcpy(tmpString, "SEM_Q_FIFO");
  else
    strcpy(tmpString, "SEM_Q_PRIORITY");

  if ( (semId->options & SEM_EVENTSEND_ERROR_NOTIFY) != 0)
    strcat(tmpString, "\t\t\t\tSEM_EVENTSEND_ERROR_NOTIFY\n");

  /* Show options */
  printf("Options             : 0x%x\t%s\n", semId->options, tmpString);

  /* Show events resource */
  eventResourceShow(&evRes);

  /* If detailed info requested */
  if (mode >= 1) {

    /* If tasks pending */
    if (nTask) {

      printf("\nPended Tasks\n"
	     "------------\n");

      printf("   NAME      TID    PRI TIMEOUT\n");
      printf("---------- -------- --- -------\n");

      /* For all tasks */
      for (i = 0; i < min( nTask, NELEMENTS(taskIdList) ); i++)
        printf("%-11.11s%8x %3d %7u\n",
	       taskName(taskIdList[i]),
	       taskIdList[i],
	       ( (TCB_ID) taskIdList[i])->priority,
	       taskDList[i]);

    } /* End if tasks pending */

  } /* End if detailed info */

  printf("\n");

  return OK;
}


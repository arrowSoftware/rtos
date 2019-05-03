#include "config.h"
#include <stdlib.h>
#include <rtos.h>
#include <arch/sysArchLib.h>
#include <arch/mmuArchLib.h>
#include <os/vmLib.h>

/* Defines */

/* Imports */
IMPORT FUNCPTR sysMmyLibInit;
IMPORT PHYS_MEM_DESC sysPhysMemDesc[];
IMPORT int sysPhysMemDescNumEntries;
extern STATUS mmuLibInit(int pageSize);
extern void printExc(char *fmt, ARG arg0, ARG arg1, ARG arg2, ARG arg3, ARG arg4);

/* Locals */

/* Globals */

/* Functions */

/*******************************************************************************
 * usrMmuInit - Initialize virtual memory
 *
 * RETURNS: N/A
 ******************************************************************************/

void usrMmuInit(void)
{
  /* If initialization of mmu unit fails */
  if ( mmuLibInit(VM_PAGE_SIZE) != OK ) {

    printExc("usrRoot: MMU configuration failed, errno = %#x", (char*)errno,
		(ARG) 0, (ARG) 0, (ARG) 0,  (ARG) 0);
    sysReboot();

  } /* End if initialization of mmu unit fails */

  /* In initialization of virual memory */
  if ( (vmLibInit(VM_PAGE_SIZE) != OK) ||
       (vmGlobalMapInit(&sysPhysMemDesc[0],
			sysPhysMemDescNumEntries,
			TRUE) == NULL) ) {

    printExc("usrRoot: MMU configuration failed, errno = %#x", (char*)errno,
		(ARG) 0, (ARG) 0, (ARG) 0,  (ARG) 0);
    sysReboot();

  } /* End if initialization of virtual memory fails */
}


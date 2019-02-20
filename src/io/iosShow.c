#include <stdio.h>
#include <stdlib.h>
#include <rtos.h>
#include <util/listLib.h>
#include <io/iosLib.h>
#include <io/iosShow.h>
#include <unistd.h>

extern int iosMaxDrv;
extern int iosMaxFd;
extern DRV_ENTRY *iosDrvTable;
extern FD_ENTRY *iosFdTable;
extern LIST iosDevList;

/*******************************************************************************
 * iosShowInit - Initialize I/O show
 *
 * RETURNS: N/A
 ******************************************************************************/
void iosShowInit(void)
{
}

/*******************************************************************************
 * iosDevShow - Show I/O devices
 *
 * RETURNS: N/A
 ******************************************************************************/
void iosFdShow(void)
{
    char *stin;
    char *stout;
    char *sterr;
    FD_ENTRY *pFdEntry;
    int fd;
    int xfd;

    printf ("%3s %-20s %3s\n", "fd", "name", "drv");

    for (fd = 0; fd < iosMaxFd; fd++)
    {
    pFdEntry = &iosFdTable [fd];
    if (pFdEntry->used)
        {
        xfd = STD_FIX(fd);

        stin  = (xfd == ioGlobalStdGet(STDIN_FILENO))  ? "in"  : "";
        stout = (xfd == ioGlobalStdGet(STDOUT_FILENO)) ? "out" : "";
        sterr = (xfd == ioGlobalStdGet(STDERR_FILENO)) ? "err" : "";

        printf ("%3d %-20s %3d %s %s %s\n",
            xfd,
            (pFdEntry->name == NULL) ? "(unknown)" : pFdEntry->name,
            pFdEntry->pDevHeader->drvNumber, stin, stout, sterr);
        }
    }
}

void iosDevShow(void)
{
  DEV_HEADER *pDevHeader;

  /* Print header */
  printf("%3s %-20s\n", "drv", "name");

  for (pDevHeader = (DEV_HEADER *) LIST_HEAD(&iosDevList);
       pDevHeader != NULL;
       pDevHeader = (DEV_HEADER *) LIST_NEXT(&pDevHeader->node))
    printf("%3d %-20s\n", pDevHeader->drvNumber, pDevHeader->name);
}

void iosDrvShow(void)
{
    register int i;

    printf ("%3s %9s  %9s  %9s  %9s  %9s  %9s  %9s\n",
    "drv", "create", "delete", "open", "close", "read", "write", "ioctl");

    for (i = 1; i < iosMaxDrv; i++)
    {
        if (iosDrvTable[i].dev_used)
        {
            printf ("%3d %9x  %9x  %9x  %9x  %9x  %9x  %9x\n", i,
                    iosDrvTable[i].dev_create, iosDrvTable[i].dev_delete,
                    iosDrvTable[i].dev_open, iosDrvTable[i].dev_close,
                    iosDrvTable[i].dev_read, iosDrvTable[i].dev_write,
                    iosDrvTable[i].dev_ioctl);
        }
    }
}

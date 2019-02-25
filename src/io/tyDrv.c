#include "io/ttyLib.h"
#include <rtos/sioLib.h>
#include <string.h>
#include <stdlib.h>

#define NULL ((void*)0)

typedef struct
{
    TTY_DEV  tyDev;
    SIO_CHAN *sioChan;
} TYCO_DEV;

/* driver number assigned to this driver */
static int ttyDrvNum;

static int tyOpen();
static int tyClose(TYCO_DEV *tyCoDev);
static STATUS tyIoctl();
static void tyStartup();

STATUS ttyDrv(void)
{
    /* check if driver already installed */
    if (ttyDrvNum > 0)
    {
        return (OK);
    }

    ttyDrvNum = iosDrvInstall(tyOpen, (FUNCPTR) NULL, tyOpen,
                              tyClose, ttyRead, ttyWrite, tyIoctl);

    return (ttyDrvNum == ERROR ? ERROR : OK);
}

STATUS ttyDevCreate(char *name,         /* name to use for this device      */
                    SIO_CHAN *sioChan, /* pointer to core driver structure */
                    int rdBufSize,      /* read buffer size, in bytes       */
                    int wrtBufSize)     /* write buffer size, in bytes      */
{
    TYCO_DEV *tyCoDev;

    if (ttyDrvNum <= 0)
    {
        /*errnoSet(S_ioLib_NO_DRIVER);*/
        return ERROR;
    }

    if (sioChan == (SIO_CHAN *)ERROR)
    {
        return ERROR;
    }

    /* allocate memory for the device */
    if ((tyCoDev = (TYCO_DEV *) malloc (sizeof (TYCO_DEV))) == NULL)
    {
        return ERROR;
    }

    /* initialize the ty descriptor */
    if (ttyDevInit(&tyCoDev->tyDev, rdBufSize, wrtBufSize,
                  (FUNCPTR) tyStartup) != OK)
    {
        free (tyCoDev);
        return ERROR;
    }

    /* initialize the SIO_CHAN structure */
    tyCoDev->sioChan = sioChan;
    sioCallbackInstall(sioChan, SIO_CALLBACK_GET_TX_CHAR, ttyIntTx, (void *)tyCoDev);
    sioCallbackInstall(sioChan, SIO_CALLBACK_PUT_RCV_CHAR, ttyIntRd, (void *)tyCoDev);

    /* start the device cranking */
    sioIoctl(sioChan, SIO_MODE_SET, (void *)SIO_MODE_INT);

    /* add the device to the I/O system */
    return (iosDevAdd (&tyCoDev->tyDev.devHeader, name, ttyDrvNum));
}

static int tyOpen(TYCO_DEV *tyCoDev, /* device to control */
                   char *name,         /* device name */
                   int flags,          /* flags */
                   int mode)           /* mode selected */
{
    /* increment number of open paths */
    tyCoDev->tyDev.numOpen++;
    sioIoctl(tyCoDev->sioChan, SIO_OPEN, NULL);
    return (int)tyCoDev;
}

static int tyClose(TYCO_DEV *tyCoDev)
{
    if (!(--tyCoDev->tyDev.numOpen))
    {
        sioIoctl(tyCoDev->sioChan, SIO_HUP, NULL);
    }

    return (int)tyCoDev;
}

static int tyIoctl(TYCO_DEV *tyCoDev, /* device to control */
                    int request,       /* request code */
                    void *arg)         /* some argument */
{
    int status;

    if (request == FIOBAUDRATE)
    {
        return (sioIoctl(tyCoDev->sioChan, SIO_BAUD_SET, arg) == OK ? OK : ERROR);
    }

    status = sioIoctl(tyCoDev->sioChan, request, arg);

    if (status == ENOSYS)
    {
        return (ttyIoctl(&tyCoDev->tyDev, request, (int)arg));
    }

    return status;
}

static void tyStartup(TYCO_DEV *tyCoDev)
{
    sioTxStartup(tyCoDev->sioChan);
}


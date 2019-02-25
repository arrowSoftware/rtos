#ifndef INCLUDE_RTOS_SIOLIB_H_
#define INCLUDE_RTOS_SIOLIB_H_

/* SIO controls */
#define SIO_BAUD_GET 0x1003
#define SIO_BAUD_SET 0x1004
#define SIO_HW_OPT_GET 0x1005
#define SIO_HW_OPT_SET 0x1006
#define SIO_MODE_GET 0x1007
#define SIO_MODE_SET 0x1008
#define SIO_AVAIL_MODES_GET 0x1009
#define SIO_OPEN 0x100A
#define SIO_HUP 0x100B

/* Callback types */
#define SIO_CALLBACK_GET_TX_CHAR 1
#define SIO_CALLBACK_PUT_RCV_CHAR 2

/* SIO_MODE_SET options */
#define SIO_MODE_POLL 1
#define SIO_MODE_INT  2

typedef struct sio_drv_funcs SIO_DRV_FUNCS;

typedef struct sio_chan
{
    SIO_DRV_FUNCS *drvFuncs;
} SIO_CHAN;

struct sio_drv_funcs
{
    int (*ioctl)(SIO_CHAN *sioChan, int cmd, void *arg);
    int (*txStartup)(SIO_CHAN *sioChan);
    int (*callbackInstall)(SIO_CHAN *sioChan, int callbackType, STATUS (*callback)(), void *callbackArg);
    int (*pollInput)(SIO_CHAN *sioChan, char *inChar);
    int (*pollOutput)(SIO_CHAN *sioChan, char outChar);
};

#define sioIoctl(sioChan, cmd, arg) ((sioChan)->drvFuncs->ioctl(sioChan, cmd, arg))
#define sioTxStartup(sioChan)       ((sioChan)->drvFuncs->txStartup(sioChan))
#define sioCallbackInstall(sioChan,callBackType, callback,arg) \
        ((sioChan)->drvFuncs->callbackInstall(sioChan, callBackType, callback, arg))
#define sioPollInput(sioChan, inChar) ((sioChan)->drvFuncs->pollInput(sioChan, inChar)
#define sioPollOutput(sioChan, outChar) ((sioChan)->drvFuncs->pollOutput(sioChan, outChar)

#endif /* INCLUDE_RTOS_SIOLIB_H_ */

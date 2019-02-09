#include <stdlib.h>
#include <arch/sysArchLib.h>
#include <string.h>
#include <drv/video/vgahw.h>
#include <ugl/ugl.h>
#include <ugl/driver/graphics/generic/udgen.h>
#include <ugl/driver/graphics/generic/udgen8.h>
#include <ugl/driver/graphics/vga/udvga.h>
#include <ugl/driver/graphics/vga/udvgax.h>

/* Locals */
UGL_LOCAL UGL_MODE modes[] = {
  {"320x200x8 @ 60Hz",
   320,200,8,60,
   UGL_MODE_CRT,
   UGL_MODE_INDEXED_COLOR,
   0,				// Clock
   640,664,760,800,0,
   480,491,493,525},
  {"320x240x8 @ 60Hz",
   320,240,8,60,
   UGL_MODE_CRT,
   UGL_MODE_INDEXED_COLOR,
   0,				// Clock
   640,664,760,800,0,
   480,491,493,525},
  {"320x240x8 @ 60Hz",
   320,240,8,60,
   UGL_MODE_CRT,
   UGL_MODE_INDEXED_COLOR|UGL_MODE_INTERLEACE_SIMULATE,
   0,				// Clock
   640,664,760,800,0,
   480,491,493,525},
  {"640x480x4 @ 60Hz",		// h: 31.5 kHz v: 60Hz
   640,480,4,60,
   UGL_MODE_CRT,
   UGL_MODE_INDEXED_COLOR,
   0,				// Clock
   640,664,760,800,0,
   480,491,493,525},
  {"768x576x4 @ 54Hz",		// h: 32.5 kHz v: 54Hz
   768,576,4,54,
   UGL_MODE_CRT,
   UGL_MODE_INDEXED_COLOR,
   1,				// Clock
   768,795,805,872,0,
   576,577,579,600},
  {"800x600x4 @ 52Hz",		// h: 31.5 kHz v: 52Hz
   800,600,4,52,
   1,				// Clock
   UGL_MODE_CRT,
   UGL_MODE_INDEXED_COLOR,
   800,826,838,900,0,		// 900
   600,601,603,617}		// 617
};

UGL_LOCAL UGL_MODE *pModes = modes;

LOCAL UGL_STATUS uglVgaModeAvailGet(UGL_DEVICE_ID devId,
			            UGL_UINT32 *pNumModes,
			            const UGL_MODE **pModeArray);
LOCAL UGL_STATUS uglVgaModeSet(UGL_DEVICE_ID devId, UGL_MODE *pMode);

/*******************************************************************************
 * uglVgaDevCreate - Create vga graphics driver
 *
 * RETURNS: Pointer to driver structure (UGL_UGI_DRIVER *) or UGL_NULL
 ******************************************************************************/

UGL_UGI_DRIVER *uglVgaDevCreate(UGL_UINT32 arg0,
				UGL_UINT32 arg1,
				UGL_UINT32 arg2)
{
  UGL_VGA_DRIVER *pDrv;
  UGL_DEVICE_ID devId = UGL_NULL;

  /* Allocate memory for driver data structure */
  pDrv = (UGL_VGA_DRIVER *) malloc( sizeof(UGL_VGA_DRIVER) );
  if (pDrv == NULL)
    return UGL_NULL;

  /* Store at begining of device struct, since it must be the first field */
  devId = (UGL_DEVICE_ID) pDrv;

  /* Initialize device driver */
  uglUgiDevInit(devId);

  /* Set frame buffer address */
  pDrv->generic.fbAddress	=	(UGL_UINT8 *) 0xa0000L;

  /* Setup device support methods */
  devId->destroy		=	uglVgaDevDestroy;

  /* Setup info method */
  devId->info			=	uglVgaInfo;

  /* Setup mode support methods */
  devId->modeAvailGet		=	uglVgaModeAvailGet;
  devId->modeSet		=	uglVgaModeSet;

  /* Setup graphics context support methods */
  devId->gcCreate		= 	uglGenericGcCreate;
  devId->gcCopy			=	uglGenericGcCopy;
  devId->gcDestroy		=	uglGenericGcDestroy;
  devId->gcSet			=	uglVgaGcSet;

  /* Set color support methods */
  devId->colorAlloc		=	uglGenericColorAllocIndexed;
  devId->colorFree		=	uglGenericColorFreeIndexed;
  devId->clutGet		=	uglVgaClutGet;
  devId->clutSet		=	uglVgaClutSet;
  devId->colorConvert		=	uglVga4BitColorConvert;

  return devId;
}

/*******************************************************************************
 * uglVgaModeAvailGet - Get available graphics modes
 *
 * RETURNS: UGL_STATUS_OK
 ******************************************************************************/

UGL_STATUS uglVgaModeAvailGet(UGL_DEVICE_ID devId,
			      UGL_UINT32 *pNumModes,
			      const UGL_MODE **pModeArray)
{
  /* Store pointer to list */
  *pModeArray = pModes;

  /* Calculate number of elements */
  *pNumModes = sizeof(modes) / sizeof(modes[0]);

  return UGL_STATUS_OK;
}

/*******************************************************************************
 * uglVgaModeSet - Set graphics mode
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 ******************************************************************************/

UGL_STATUS uglVgaModeSet(UGL_DEVICE_ID devId, UGL_MODE *pMode)
{
  UGL_VGA_DRIVER *pDrv;
  UGL_INT32 modeIndex;
  UGL_UINT8 byteValue;
  struct vgaHWRec regs;

  /* Get driver from device struct, since it it the first entry */
  pDrv = (UGL_VGA_DRIVER *) devId;

  /* Get requested mode from list */
  modeIndex = uglGenericModeFind(modes, pMode,
				 sizeof(modes) / sizeof(modes[0]));
  if (modeIndex == UGL_STATUS_ERROR)
    return UGL_STATUS_ERROR;

  /* Store current mode */
  devId->pMode = &modes[modeIndex];

  /* Set graphics mode */
  vgaInitMode(&modes[modeIndex], &regs);
  vgaLoadPalette(&regs, NULL);
  vgaRestore(&regs, 0);

  switch(modeIndex) {

    case 0:

    /* Modify registers */
    /* Change sync polarity */
    UGL_OUT_BYTE(0x3c0, 0x63);

    /* Modify sequencer registers */
    UGL_OUT_WORD(0x3c4, 0x0e04);		/* Enable Chain-4 */

    /* Modify graphics controller registers */
    UGL_OUT_WORD(0x3ce, 0x4005);		/* Set mode register */

    /* Modify CRT controller registers */
    UGL_OUT_WORD(0x3d4, 0x2c11);		/* Disable write protection */
    UGL_OUT_WORD(0x3d4, 0xbf06);		/* Vertical total */
    UGL_OUT_WORD(0x3d4, 0x1f07);		/* Overflow register */
    UGL_OUT_WORD(0x3d4, 0x4109);		/* Set duplicate scanlines */
    UGL_OUT_WORD(0x3d4, 0x9c10);		/* Vertical retrace start */
    UGL_OUT_WORD(0x3d4, 0x8e11);		/* Vertical retrace end */
    UGL_OUT_WORD(0x3d4, 0x8f12);		/* Vertical display end */
    UGL_OUT_WORD(0x3d4, 0x2813);		/* Set logical width */
    UGL_OUT_WORD(0x3d4, 0x4014);		/* Enable double word mode */
    UGL_OUT_WORD(0x3d4, 0x9615);		/* Vertical blanking start */
    UGL_OUT_WORD(0x3d4, 0xb916);		/* Vertical blanking end */
    UGL_OUT_WORD(0x3d4, 0xa317);		/* Enable word mode */

    /* Set attribute registers */
    byteValue = UGL_IN_BYTE(0x3da);	/* Start attribute set */
    UGL_OUT_BYTE(0x3c0, 0x10);		/* Set mode control register */
    UGL_OUT_BYTE(0x3c0, 0x41);		/* for lores 256 color */
    UGL_OUT_BYTE(0x3c0, 0x20);		/* End attribute set */

    /* Set releated info */
    pDrv->bytesPerLine	=	devId->pMode->Width;
    pDrv->colorPlanes	=	devId->pMode->Depth;

    /* Set graphics primitives support methods */
    devId->pixelSet		=	uglGeneric8BitPixelSet;

    /* Setup bitmap support methods */
    devId->bitmapCreate		=	uglGeneric8BitBitmapCreate;
    devId->bitmapDestroy	=	uglGeneric8BitBitmapDestroy;
    devId->bitmapBlt		=	uglGeneric8BitBitmapBlt;
    devId->bitmapWrite		=	uglGeneric8BitBitmapWrite;

    /* Create palette for 256 colors */
    if (uglGenericClutCreate((UGL_GENERIC_DRIVER *) devId, 256) != UGL_STATUS_OK)
      return UGL_STATUS_ERROR;

    /* Clear screen */
    memset(pDrv->generic.fbAddress, 0,
	   devId->pMode->Height * pDrv->bytesPerLine);

    break;

    case 1:

    /* Modify registers */
    /* Change sync polarity */
    UGL_OUT_BYTE(0x3c0, 0x63);

    /* Modify sequencer registers */
    UGL_OUT_WORD(0x3c4, 0x0604);		/* Disable Chain-4 */

    /* Modify graphics controller registers */
    UGL_OUT_WORD(0x3ce, 0x4005);		/* Set mode register */

    /* Modify CRT controller registers */
    UGL_OUT_WORD(0x3d4, 0x2c11);		/* Disable write protection */
    UGL_OUT_WORD(0x3d4, 0x0d06);		/* Vertical total */
    UGL_OUT_WORD(0x3d4, 0x4109);		/* Set duplicate scanlines */
    UGL_OUT_WORD(0x3d4, 0xac11);		/* Vertical retrace end */
    UGL_OUT_WORD(0x3d4, 0x2813);		/* Set logical width */
    UGL_OUT_WORD(0x3d4, 0x0014);		/* Disable double word mode */
    UGL_OUT_WORD(0x3d4, 0x0616);		/* Vertical blanking end */
    UGL_OUT_WORD(0x3d4, 0xe317);		/* Disable word mode */

    /* Set attribute registers */
    byteValue = UGL_IN_BYTE(0x3da);	/* Start attribute set */
    UGL_OUT_BYTE(0x3c0, 0x10);		/* Set mode control register */
    UGL_OUT_BYTE(0x3c0, 0x41);		/* for lores 256 color */
    UGL_OUT_BYTE(0x3c0, 0x20);		/* End attribute set */

    /* Set releated info */
    pDrv->bytesPerLine	=	devId->pMode->Width / 4;
    pDrv->colorPlanes	=	devId->pMode->Depth;

    /* Set graphics primitives support methods */
    devId->pixelSet		=	uglVgaXPixelSet;

    /* Setup bitmap support methods */
    devId->bitmapCreate		=	uglVgaXBitmapCreate;
    devId->bitmapDestroy	=	uglVgaXBitmapDestroy;
    devId->bitmapBlt		=	uglVgaXBitmapBlt;
    devId->bitmapWrite		=	uglVgaXBitmapWrite;

    /* Create palette for 256 colors */
    if (uglGenericClutCreate((UGL_GENERIC_DRIVER *) devId, 256) != UGL_STATUS_OK)
      return UGL_STATUS_ERROR;

    /* Clear screen */
    UGL_OUT_WORD(0x3c4, 0x0f02);
    memset(pDrv->generic.fbAddress, 0,
	   2 * devId->pMode->Height * pDrv->bytesPerLine);

    break;

    case 2:

    /* Modify registers */
    /* Change sync polarity */
    UGL_OUT_BYTE(0x3c0, 0x63);

    /* Modify sequencer registers */
    UGL_OUT_WORD(0x3c4, 0x0604);		/* Disable Chain-4 */

    /* Modify graphics controller registers */
    UGL_OUT_WORD(0x3ce, 0x4005);		/* Set mode register */
    UGL_OUT_WORD(0x3ce, 0x0306);		/* Disable memory chaining */

    /* Modify CRT controller registers */
    UGL_OUT_WORD(0x3d4, 0x2c11);		/* Disable write protection */
    UGL_OUT_WORD(0x3d4, 0x0d06);		/* Vertical total */
    UGL_OUT_WORD(0x3d4, 0xac11);		/* Vertical retrace end */
    UGL_OUT_WORD(0x3d4, 0x2813);		/* Set logical width */
    UGL_OUT_WORD(0x3d4, 0x0014);		/* Disable double word mode */
    UGL_OUT_WORD(0x3d4, 0x0616);		/* Vertical blanking end */
    UGL_OUT_WORD(0x3d4, 0xe317);		/* Disable word mode */

    /* Set attribute registers */
    byteValue = UGL_IN_BYTE(0x3da);	/* Start attribute set */
    UGL_OUT_BYTE(0x3c0, 0x10);		/* Set mode control register */
    UGL_OUT_BYTE(0x3c0, 0x41);		/* for lores 256 color */
    UGL_OUT_BYTE(0x3c0, 0x20);		/* End attribute set */

    /* Set releated info */
    pDrv->bytesPerLine	=	devId->pMode->Width / 4;
    pDrv->colorPlanes	=	devId->pMode->Depth;

    /* Create palette for 256 colors */
    if (uglGenericClutCreate((UGL_GENERIC_DRIVER *) devId, 256) != UGL_STATUS_OK)
      return UGL_STATUS_ERROR;

    /* Clear both screens */
    UGL_OUT_WORD(0x3c4, 0x0f02);
    memset(pDrv->generic.fbAddress, 0,
	   2 * devId->pMode->Height * pDrv->bytesPerLine);
    break;

    default:

    /* Set releated info */
    pDrv->bytesPerLine	=	devId->pMode->Width / 8;
    pDrv->colorPlanes	=	devId->pMode->Depth;

    /* Set graphics primitives support methods */
    devId->pixelSet		=	uglVgaPixelSet;

    /* Setup bitmap support methods */
    devId->bitmapCreate		=	uglVgaBitmapCreate;
    devId->bitmapDestroy	=	uglVgaBitmapDestroy;
    devId->bitmapBlt		=	uglVgaBitmapBlt;
    devId->bitmapWrite		=	uglVgaBitmapWrite;

    /* Create palette for 16 colors */
    if (uglGenericClutCreate((UGL_GENERIC_DRIVER *) devId, 16) != UGL_STATUS_OK)
      return UGL_STATUS_ERROR;

    /* Set write mode 3 */
    UGL_OUT_BYTE(0x3ce, 0x05);
    byteValue = UGL_IN_BYTE(0x3cf);
    UGL_OUT_BYTE(0x3cf, byteValue | 0x03);

    /* Set set/reset register */
    UGL_OUT_WORD(0x3ce, 0x0f01);

    /* Clear screen */
    UGL_OUT_WORD(0x3ce, 0);
    memset(pDrv->generic.fbAddress, 0xff,
	   devId->pMode->Height * pDrv->bytesPerLine);

    /* Enable video output */
    UGL_OUT_BYTE(0x3c4, 0x01);
    UGL_OUT_BYTE(0x3c5, 0x01);

    break;

  }

  return UGL_STATUS_OK;
}


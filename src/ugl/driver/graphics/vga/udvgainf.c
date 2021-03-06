#include <ugl/ugl.h>
#include <ugl/driver/graphics/generic/udgen.h>
#include <ugl/driver/graphics/vga/udvga.h>

/*******************************************************************************
 * uglVgaInfo - Get information about video mode
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 ******************************************************************************/

UGL_STATUS uglVgaInfo(UGL_DEVICE_ID devId,
		      UGL_INFO_REQ infoReq,
		      void *info)
{
  UGL_GENERIC_DRIVER *pDrv = NULL;
  UGL_FB_INFO *fbInfo = NULL;
  UGL_COLOR_INFO *colorInfo = NULL;
  UGL_MODE_INFO *modeInfo = NULL;

  /* Get driver first in device struct */
  pDrv = (UGL_GENERIC_DRIVER *) devId;

  /* Check if mode is initialized */
  if (devId->pMode == UGL_NULL)
    return UGL_STATUS_ERROR;

  switch(infoReq) {
    case UGL_FB_INFO_REQ:
      fbInfo = (UGL_FB_INFO *) info;
      fbInfo->width = devId->pMode->Width;
      fbInfo->height = devId->pMode->Height;
      fbInfo->fbAddress = pDrv->fbAddress;
      fbInfo->displayMemAvail = 0;
      fbInfo->flags = 0;
    break;

    case UGL_COLOR_INFO_REQ:
      colorInfo = (UGL_COLOR_INFO *) info;
      colorInfo->cmodel = UGL_CMODEL_INDEXED;
      colorInfo->cspace = UGL_CSPACE_RGB;
      colorInfo->clutSize = 1 << devId->pMode->Depth;
      colorInfo->depth = devId->pMode->Depth;
      colorInfo->flags = UGL_CLUT_WRITE;
    break;

    case UGL_MODE_INFO_REQ:
      modeInfo = (UGL_MODE_INFO *) info;
      modeInfo->width = devId->pMode->Width;
      modeInfo->height = devId->pMode->Height;
      modeInfo->colorDepth = devId->pMode->Depth;
      modeInfo->clutSize = 1 << devId->pMode->Depth;
      if (modeInfo->colorDepth == 8) {
        modeInfo->colorModel = UGL_INDEXED_8;
        modeInfo->colorFormat = 0;
      }
      else if (modeInfo->colorDepth == 4) {
        modeInfo->colorModel = UGL_INDEXED_4;
        modeInfo->colorFormat = 0;
      }
      else if (modeInfo->colorDepth == 1) {
        modeInfo->colorModel = UGL_INDEXED_1;
        modeInfo->colorFormat = 0;
      }
      modeInfo->fbAddress = pDrv->fbAddress;
      modeInfo->displayMemAvail = 0;
      fbInfo->flags = 0;
    break;

    default: return UGL_STATUS_ERROR;
  }

  return UGL_STATUS_OK;
}


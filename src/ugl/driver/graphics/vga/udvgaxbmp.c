#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <arch/sysArchLib.h>
#include <rtos/memPartLib.h>
#include <ugl/ugl.h>
#include <ugl/driver/graphics/generic/udgen.h>
#include <ugl/driver/graphics/vga/udvga.h>

/*******************************************************************************
 * uglVgaXBitmapCreate - Create vga bitmap
 *
 * RETURNS: Pointer to bitmap
 ******************************************************************************/

UGL_DDB_ID uglVgaXBitmapCreate(UGL_DEVICE_ID devId,
			       UGL_DIB *pDib,
			       UGL_DIB_CREATE_MODE createMode,
			       UGL_UINT32 initValue,
			       UGL_MEM_POOL_ID poolId)
{
  UGL_VGA_DDB *pVgaBmp;
  UGL_UINT32 i, planeSize;
  UGL_SIZE width, height;
  UGL_UINT8 *pPlane;
  UGL_RECT srcRect;
  UGL_POINT destPoint;
  UGL_STATUS status;

  /* Get bitmap info, from screen if NULL DIB */
  if (pDib == UGL_NULL) {
    width = devId->pMode->Width;
    height = devId->pMode->Height;
  }

  /* Else get info from device independet bitmap */
  else {
    width = pDib->width;
    height = pDib->height;
  }

  /* Calculate plane size including 1 shift byte for each scanline */
  planeSize = width * height / 4;

  /* Allocate storage for header and color planes */
  pVgaBmp = (UGL_VGA_DDB *) UGL_PART_MALLOC(poolId,
	sizeof(UGL_VGA_DDB) +				/* Header */
	4 * sizeof(UGL_UINT8 *) +			/* Plane array */
       	4 * planeSize					/* Planes */
	);
  if (pVgaBmp == NULL)
    return UGL_NULL;

  /* Setup color planes */
  /* After the plane pointer array are the storage are of the bitplanes */
  /* Setup all pointers in the array to point to these areas */
  pVgaBmp->pPlaneArray = (UGL_UINT8 **) (((UGL_UINT8 *) pVgaBmp) +
	sizeof(UGL_VGA_DDB));
  pPlane = (UGL_UINT8 *) &pVgaBmp->pPlaneArray[4];
  for (i = 0; i < 4; i++) {
    pVgaBmp->pPlaneArray[i] = pPlane;
    pPlane += planeSize;
  }

  /* Initialize contents of color planes */
  switch(createMode) {

    /* Fill all planes with initial value */
    case UGL_DIB_INIT_VALUE:

      pVgaBmp->colorDepth = devId->pMode->Depth;
      pVgaBmp->header.width = width;
      pVgaBmp->header.height = height;
      pVgaBmp->stride = width;
      pVgaBmp->shiftValue = 0;

      for (i = 0; i < 4; i++)
        memset(pVgaBmp->pPlaneArray[i], initValue, planeSize);

    break;

    /* Init from general bitmap */
    case UGL_DIB_INIT_DATA:

      pVgaBmp->colorDepth = devId->pMode->Depth;
      pVgaBmp->header.width = width;
      pVgaBmp->header.height = height;
      pVgaBmp->stride = width;
      pVgaBmp->shiftValue = 0;

      /* Clear destination */
      for (i = 0; i < 4; i++)
        memset(pVgaBmp->pPlaneArray[i], initValue, planeSize);

      /* Read from source */
      srcRect.left = 0;
      srcRect.top = 0;
      srcRect.right = width - 1;
      srcRect.bottom = height - 1;
      destPoint.x = 0;
      destPoint.y = 0;
      status = (*devId->bitmapWrite)(devId, pDib, &srcRect,
				     (UGL_DDB_ID) pVgaBmp, &destPoint);

      if (status != UGL_STATUS_OK) {
        (*devId->bitmapDestroy)(devId, (UGL_DDB_ID) pVgaBmp, poolId);
	return UGL_NULL;
      }

    break;

    /* None */
    default:
    break;
  }

  return (UGL_DDB_ID) pVgaBmp;
}

/*******************************************************************************
 * uglVgaXBitmapDestroy - Free vga bitmap
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 ******************************************************************************/

UGL_STATUS uglVgaXBitmapDestroy(UGL_DEVICE_ID devId,
			        UGL_DDB_ID ddbId,
			        UGL_MEM_POOL_ID poolId)
{
  /* Free memory */
  UGL_PART_FREE(poolId, ddbId);

  return UGL_STATUS_OK;
}

/*******************************************************************************
 * uglVgaXBltPlane - Blit to one bitplane
 *
 * RETURNS: N/A
 ******************************************************************************/

UGL_LOCAL void uglVgaXBltPlane(UGL_DEVICE_ID devId,
			       UGL_UINT8 *pSrc,
			       UGL_RECT *pSrcRect,
			       UGL_SIZE srcStride,
			       UGL_UINT8 *pDest,
			       UGL_RECT *pDestRect,
			       UGL_SIZE destStride,
			       UGL_RASTER_OP rasterOp)
{
  int i, y, width, height;
  UGL_UINT8 *src, *dest;
  UGL_UINT8 startMask, endMask;

  /* Calculate vars */
  width = (pDestRect->right >> 3) - (pDestRect->left >> 3) + 1;
  height = UGL_RECT_HEIGHT(*pDestRect) - 1;
  src = pSrc + pSrcRect->top * srcStride + (pSrcRect->left >> 3);
  dest = pDest + pDestRect->top * destStride + (pDestRect->left >> 3);

  /* Generate masks */
  startMask = 0xff >> (pDestRect->left & 0x07);
  endMask = 0xff << (7 - (pDestRect->right & 0x07));

  /* if pSrc == 0 => Fill entire plane with zeros */
  if (pSrc == (UGL_UINT8 *) 0) {

    /* Select rasterOp */
    switch(rasterOp) {

      /* Case raster operation copy or and */
      case UGL_RASTER_OP_COPY:
      case UGL_RASTER_OP_AND:

      /* Over height */
      for (y = 0; y < height; y++) {

	/* Just one pixel */
        if (width == 1) {

	  /* Fill start and end */
	  dest[0] &= ~(startMask & endMask);

	} /* End if just one pixel */

	/* More pixels */
	else {
	
	  /* Fill start mask */
	  dest[0] &= ~startMask;

	  /* Fill middle */
	  if (width > 2)
	    memset(&dest[1], 0, width - 2);

	  /* Fill end mask */
	  dest[width - 1] &= ~endMask;

	} /* End if more pixels than one */

	/* Advance to next line */
	dest += destStride;

      } /* End over height */

      break; /* End case raster operation copy or and */

    } /* End select raster op */

    /* Done */
    return;

  } /* End if pSrc == 0 => fill with zeros */

  /* if pSrc == -1 => Fill entire plane with ones */
  if (pSrc == (UGL_UINT8 *) -1) {

    /* Select rasterOp */
    switch(rasterOp) {

      /* Case raster operation copy or or */
      case UGL_RASTER_OP_COPY:
      case UGL_RASTER_OP_OR:

      /* Over height */
      for (y = 0; y < height; y++) {

	/* Just one pixel */
        if (width == 1) {

	  /* Fill start and end */
	  dest[0] |= startMask & endMask;

	} /* End if just one pixel */

	/* More pixels */
	else {
	
	  /* Fill start mask */
	  dest[0] |= startMask;

	  /* Fill middle */
	  if (width > 2)
	    memset(&dest[1], 0xff, width - 2);

	  /* Fill end mask */
	  dest[width - 1] |= endMask;

	} /* End if more pixels than one */

	/* Advance to next line */
	dest += destStride;

      } /* End over height */

      break; /* End case raster operation copy or or */

      /* Case raster operation xor */
      case UGL_RASTER_OP_XOR:

      /* Over height */
      for (y = 0; y < height; y++) {

	/* Just one pixel */
        if (width == 1) {

	  /* Fill start and end */
	  dest[0] ^= startMask & endMask;

	} /* End if just one pixel */

	/* More pixels */
	else {
	
	  /* Fill start mask */
	  dest[0] ^= startMask;

	  /* Fill middle */
	  if (width > 2)
	    for (i = 1; i < width - 1; i++)
	      dest[i] ^= 0xff;

	  /* Fill end mask */
	  dest[i] ^= endMask;

	} /* End if more pixels than one */

	/* Advance to next line */
	dest += destStride;

      } /* End over height */

      break; /* End case raster operation xor */

    } /* End select raster op */

    /* Done */
    return;

  } /* End if pSrc == -1 => fill with ones */

  /* If here this is a plane copy */
  /* Select raster op */
  switch(rasterOp) {

    /* Case raster operation copy */
    case UGL_RASTER_OP_COPY:

    /* Over height */
    for (y = 0; y < height; y++) {

      /* Check if just one pixel */
      if (width == 1) {

	/* Blit start and end mask */
        dest[0] |= src[0] & (startMask & endMask);
	dest[0] &= src[0] | ~(startMask & endMask);

      } /* End if just one pixel */

      /* Else more than one pixel */
      else {

	/* Blit start mask */
        dest[0] |= src[0] & startMask;
	dest[0] &= src[0] | ~startMask;

	/* Blit middle */
	if (width > 2)
	  memcpy(&dest[1], &src[1], width - 2);

	/* Blit end mask */
	dest[width - 1] |= src[width - 1] & endMask;
	dest[width - 1] &= src[width - 1] | ~endMask;

      } /* End else more than one pixel */

      /* Advance one line */
      src += srcStride;
      dest += destStride;

    } /* End over height */

    break; /* End case raster operation copy */

    /* Case raster operation and */
    case UGL_RASTER_OP_AND:

    /* Over height */
    for (y = 0; y < height; y++) {

      /* Check if just one pixel */
      if (width == 1) {

	/* Blit start and end mask */
	dest[0] &= src[0] | ~(startMask & endMask);

      } /* End if just one pixel */

      /* Else more than one pixel */
      else {

	/* Blit start mask */
        dest[0] &= src[0] | ~startMask;

	/* Blit middle */
	if (width > 2)
          for (i = 1; i < width - 1; i++)
	    dest[i] &= src[i];

	/* Blit end mask */
	dest[i] &= src[i] | ~endMask;

      } /* End else more than one pixel */

      /* Advance one line */
      src += srcStride;
      dest += destStride;

    } /* End over height */

    break; /* End case raster operation and */

    /* Case raster operation or */
    case UGL_RASTER_OP_OR:

    /* Over height */
    for (y = 0; y < height; y++) {

      /* Check if just one pixel */
      if (width == 1) {

	/* Blit start and end mask */
	dest[0] |= src[0] & (startMask & endMask);

      } /* End if just one pixel */

      /* Else more than one pixel */
      else {

	/* Blit start mask */
        dest[0] |= src[0] & startMask;

	/* Blit middle */
	if (width > 2)
          for (i = 1; i < width - 1; i++)
	    dest[i] |= src[i];

	/* Blit end mask */
	dest[i] |= src[i] & endMask;

      } /* End else more than one pixel */

      /* Advance one line */
      src += srcStride;
      dest += destStride;

    } /* End over height */

    break; /* End case raster operation or */

    /* Case raster operation xor */
    case UGL_RASTER_OP_XOR:

    /* Over height */
    for (y = 0; y < height; y++) {

      /* Check if just one pixel */
      if (width == 1) {

	/* Blit start and end mask */
	dest[0] ^= src[0] & (startMask & endMask);

      } /* End if just one pixel */

      /* Else more than one pixel */
      else {

	/* Blit start mask */
        dest[0] ^= src[0] & startMask;

	/* Blit middle */
	if (width > 2)
          for (i = 1; i < width - 1; i++)
	    dest[i] ^= src[i];

	/* Blit end mask */
	dest[i] ^= src[i] & endMask;

      } /* End else more than one pixel */

      /* Advance one line */
      src += srcStride;
      dest += destStride;

    } /* End over height */

    break; /* End case raster operation xor */

  } /* End select raster op */

}

/*******************************************************************************
 * uglVgaXBltColorToColor - Blit from memory to memory bitmap
 *
 * RETURNS: N/A
 ******************************************************************************/

UGL_LOCAL void uglVgaXBltColorToColor(UGL_DEVICE_ID devId,
				      UGL_VGA_DDB *pSrcBmp,
				      UGL_RECT *pSrcRect,
				      UGL_VGA_DDB *pDestBmp,
				      UGL_RECT *pDestRect)
{
  UGL_GENERIC_DRIVER *pDrv;
  int planeIndex, numPlanes;
  UGL_SIZE srcStride, destStride;
  UGL_UINT8 *src, *dest;

  /* Get driver first in device struct */
  pDrv = (UGL_GENERIC_DRIVER *) devId;

  /* Calculate vars */
  numPlanes = pDestBmp->colorDepth;
  srcStride = (pSrcBmp->header.width + 7) / 8 + 1;
  destStride = (pDestBmp->header.width + 7) / 8 + 1;

  /* Align source bitmap to dest */
  /*
  uglVgaBltAlign(devId, (UGL_BMAP_HEADER *) pSrcBmp, pSrcRect,
	         pDestBmp, pDestRect);
  */

  /* Over all blit planes */
  for (planeIndex = 0; planeIndex < numPlanes; planeIndex++) {

    src = pSrcBmp->pPlaneArray[planeIndex];
    dest = pDestBmp->pPlaneArray[planeIndex];

    uglVgaBltPlane(devId,
		   src, pSrcRect, srcStride,
		   dest, pDestRect, destStride,
		   pDrv->gc->rasterOp);

  } /* End over all blit planes */

}

/*******************************************************************************
 * uglVgaXBltColorToFrameBuffer - Blit from memory bitmap to frame buffer
 *
 * RETURNS: N/A
 ******************************************************************************/

UGL_LOCAL void uglVgaXBltColorToFrameBuffer(UGL_DEVICE_ID devId,
				            UGL_VGA_DDB *pBmp,
				            UGL_RECT *pSrcRect,
				            UGL_RECT *pDestRect)
{
  UGL_GENERIC_DRIVER *pDrv;
  UGL_VGA_DRIVER *pVgaDrv;
  UGL_RASTER_OP rasterOp;
  UGL_INT32 i, y, width, height, planeIndex;
  UGL_INT32 destBytesPerLine, srcBytesPerLine, srcOffset;
  UGL_UINT8 *destStart, regValue;
  UGL_UINT8 *src, *dest;

  /* Get driver first in device struct */
  pDrv = (UGL_GENERIC_DRIVER *) devId;
  pVgaDrv = (UGL_VGA_DRIVER *) devId;

  /* Cache raster op */
  rasterOp = pDrv->gc->rasterOp;

  /* Setup variables for blit */
  width = UGL_RECT_WIDTH(*pDestRect) / 4;
  height = UGL_RECT_HEIGHT(*pDestRect);
  destBytesPerLine = pVgaDrv->bytesPerLine;
  srcBytesPerLine = pBmp->header.width / 4; 
  destStart = (UGL_UINT8 *) pDrv->fbAddress +
	pDestRect->top * destBytesPerLine + (pDestRect->left >> 2);
  srcOffset = pSrcRect->top * srcBytesPerLine +
	(pSrcRect->left >> 2);

  /* Setup registers */
  /* Select write plane */
  UGL_OUT_BYTE(0x3c4, 0x02);

  /* Blit */
  /* Over height */
  for (y = 0; y < height; y++) {

    /* For all planes */
    for (planeIndex = 0; planeIndex < 4; planeIndex++) {

      /* Calulcate line vars */
      src = pBmp->pPlaneArray[planeIndex] + srcOffset;
      dest = destStart;
      i = 0;

      /* Select plane */
      regValue = 0x01 << planeIndex;
      UGL_OUT_BYTE(0x3c5, regValue);

      /* Select raster op */
      switch(rasterOp) {

        case UGL_RASTER_OP_COPY:
        memcpy(dest, src, width);
        break;

        case UGL_RASTER_OP_AND:
        for (i = 0; i < width; i++)
          dest[i] &= src[i];
        break;

        case UGL_RASTER_OP_OR:
        for (i = 0; i < width; i++)
          dest[i] |= src[i];
        break;

        case UGL_RASTER_OP_XOR:
        for (i = 0; i < width; i++)
          dest[i] ^= src[i];
        break;

      } /* End select raster op */

    } /* End for all planes */

    /* Advance line */
    srcOffset += srcBytesPerLine;
    destStart += destBytesPerLine;

  } /* End for height */
}

/*******************************************************************************
 * uglVgaXBltFrameBuffetToColor - Blit from frame buffer to memry bitmap
 *
 * RETURNS: N/A
 ******************************************************************************/

UGL_LOCAL void uglVgaXBltFrameBufferToColor(UGL_DEVICE_ID devId,
				            UGL_RECT *pSrcRect,
				            UGL_VGA_DDB *pBmp,
				            UGL_RECT *pDestRect)
{
  UGL_GENERIC_DRIVER *pDrv;
  UGL_VGA_DRIVER *pVgaDrv;
  UGL_RASTER_OP rasterOp;
  UGL_INT32 i, y, height, planeIndex, numPlanes, width;
  UGL_INT32 destBytesPerLine, srcBytesPerLine, destOffset;
  UGL_UINT8 startMask, endMask, byteValue;
  UGL_UINT8 *src, *srcStart, *dest;

  /* Get driver first in device struct */
  pDrv = (UGL_GENERIC_DRIVER *) devId;
  pVgaDrv = (UGL_VGA_DRIVER *) devId;

  /* Cache raster op */
  rasterOp = pDrv->gc->rasterOp;

  /* Align source bitmap to dest */
  /*
  uglVgaBltAlign(devId, (UGL_BMAP_HEADER *) pBmp, pDestRect,
	         UGL_NULL, pSrcRect);
  */

  /* Setup variables for blit */
  width = (pSrcRect->right >> 3) - (pSrcRect->left >> 3) + 1;
  height = UGL_RECT_HEIGHT(*pDestRect) - 1;
  numPlanes = pBmp->colorDepth;
  srcBytesPerLine = pVgaDrv->bytesPerLine;
  destBytesPerLine = (pBmp->header.width + 7) / 8 + 1;
  srcStart = (UGL_UINT8 *) pDrv->fbAddress +
	pSrcRect->top * srcBytesPerLine + (pSrcRect->left >> 3);
  destOffset = pDestRect->top * destBytesPerLine +
	(pDestRect->left >> 3);

  /* Generate masks */
  startMask = 0xff >> (pDestRect->left & 0x07);
  endMask = 0xff << (7 - (pDestRect->right & 0x07));
  if (width == 1)
    startMask &= endMask;

  /* Select read plane */
  UGL_OUT_BYTE(0x3ce, 0x04);

  /* Blit */
  /* For all planes */
  for (planeIndex = 0; planeIndex < numPlanes; planeIndex++) {

    /* Calulcate plane vars */
    src = srcStart;
    dest = pBmp->pPlaneArray[planeIndex] + destOffset;

    /* Select plane */
    UGL_OUT_BYTE(0x3cf, planeIndex);

    /* Select raster op */
    switch(rasterOp) {

      /* Case raster operation copy */
      case UGL_RASTER_OP_COPY:

      /* Over height */
      for (y = 0; y < height; y++) {
        i = 0;

        /* Blit start */
        byteValue = src[i];
        dest[i] |= byteValue & startMask;
        dest[i] &= byteValue | ~startMask;
        i++;

        /* Check if anything to more blit */
        if (width > 1) {

          /* Check if bigger blit */
          if (width > 2) {

	    /* Blit */
            memcpy(&dest[i], &src[i], width - 2);
	    i += width - 2;

          } /* End if bigger blit */

          /* Blit end */
          byteValue = src[i];
          dest[i] |= byteValue & endMask;
          dest[i] &= byteValue | ~endMask;

        } /* End if anything to more blit */

        /* Advance to next line */
        src += srcBytesPerLine;
        dest += destBytesPerLine;

      } /* End over height */

      break; /* End case raster operation copy */

      /* Case raster operation and */
      case UGL_RASTER_OP_AND:

      /* Over height */
      for (y = 0; y < height; y++) {
        i = 0;

        /* Blit start */
	dest[i] &= src[i] | ~startMask;
        i++;

        /* Check if anything to more blit */
        if (width > 1) {

          /* Check if bigger blit */
          if (width > 2) {

	    /* Blit */
	    for (i = 1; i < width - 1; i++)
	      dest[i] &= src[i];

          } /* End if bigger blit */

          /* Blit end */
	  dest[i] &= src[i] | ~endMask;

        } /* End if anything to more blit */

        /* Advance to next line */
        src += srcBytesPerLine;
        dest += destBytesPerLine;

      } /* End over height */

      break; /* End case raster operation and */

      /* Case raster operation or */
      case UGL_RASTER_OP_OR:

      /* Over height */
      for (y = 0; y < height; y++) {
        i = 0;

        /* Blit start */
	dest[i] |= src[i] & startMask;
        i++;

        /* Check if anything to more blit */
        if (width > 1) {

          /* Check if bigger blit */
          if (width > 2) {

	    /* Blit */
	    for (i = 1; i < width - 1; i++)
	      dest[i] |= src[i];

          } /* End if bigger blit */

          /* Blit end */
	  dest[i] |= src[i] & endMask;

        } /* End if anything to more blit */

        /* Advance to next line */
        src += srcBytesPerLine;
        dest += destBytesPerLine;

      } /* End over height */

      break; /* End case raster operation or */

      /* Case raster operation xor */
      case UGL_RASTER_OP_XOR:

      /* Over height */
      for (y = 0; y < height; y++) {
        i = 0;

        /* Blit start */
	dest[i] ^= src[i] & startMask;
        i++;

        /* Check if anything to more blit */
        if (width > 1) {

          /* Check if bigger blit */
          if (width > 2) {

	    /* Blit */
	    for (i = 1; i < width - 1; i++)
	      dest[i] ^= src[i];

          } /* End if bigger blit */

          /* Blit end */
	  dest[i] ^= src[i] & endMask;

        } /* End if anything to more blit */

        /* Advance to next line */
        src += srcBytesPerLine;
        dest += destBytesPerLine;

      } /* End over height */

      break; /* End case raster operation xor */

      /* Default case */
      default: break;

    } /* End select raster op */

  } /* End for all planes */

}

/*******************************************************************************
 * uglVgaXBitmapBlt - Blit from one bitmap memory area to another
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 ******************************************************************************/

UGL_STATUS uglVgaXBitmapBlt(UGL_DEVICE_ID devId,
			    UGL_DDB_ID srcBmpId,
			    UGL_RECT *pSrcRect,
			    UGL_DDB_ID destBmpId,
			    UGL_POINT *pDestPoint)
{
  UGL_VGA_DDB *pSrcBmp, *pDestBmp;
  UGL_RECT srcRect, destRect, clipRect;
  UGL_POINT destPoint;

  /* Store source and dest */
  pSrcBmp = (UGL_VGA_DDB *) srcBmpId;
  pDestBmp = (UGL_VGA_DDB *) destBmpId;

  /* Store starting point */
  destPoint.x = pDestPoint->x;
  destPoint.y = pDestPoint->y;

  /* Store source rectangle */
  srcRect.top = pSrcRect->top;
  srcRect.left = pSrcRect->left;
  srcRect.right = pSrcRect->right;
  srcRect.bottom = pSrcRect->bottom;

  /* Store destination rectangle */
  destRect.top = pDestPoint->y;
  destRect.left = pDestPoint->x;
  destRect.right = pDestPoint->x;
  destRect.bottom = pDestPoint->y;

  /* Store clip rectangle */
  if (destBmpId == UGL_DEFAULT_ID) {
    clipRect.top = 0;
    clipRect.bottom = devId->pMode->Height;
    clipRect.left = 0;
    clipRect.right = devId->pMode->Width;
  }

  /* Clip */
  if (uglGenericClipDdb(devId, &clipRect,
	(UGL_BMAP_ID *) &pSrcBmp, &srcRect,
	(UGL_BMAP_ID *) &pDestBmp, &destPoint) != UGL_TRUE)
    return UGL_STATUS_ERROR;

  /* Calculate destination */
  UGL_RECT_MOVE_TO_POINT(destRect, destPoint);
  UGL_RECT_SIZE_TO(destRect, UGL_RECT_WIDTH(srcRect),
		UGL_RECT_HEIGHT(srcRect));

  /* Blit */
  if (srcBmpId != UGL_DISPLAY_ID && destBmpId == UGL_DISPLAY_ID)
    uglVgaXBltColorToFrameBuffer(devId, pSrcBmp, &srcRect, &destRect);
  else if (srcBmpId == UGL_DISPLAY_ID && destBmpId != UGL_DISPLAY_ID)
    uglVgaXBltFrameBufferToColor(devId, &srcRect, pDestBmp, &destRect);
  else
    uglVgaXBltColorToColor(devId, pSrcBmp, &srcRect, pDestBmp, &destRect);

  return UGL_STATUS_OK;
}

/*******************************************************************************
 * uglVgaXBitmapWrite - Write a device independet bitmap to vga bitmap
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 ******************************************************************************/

UGL_STATUS uglVgaXBitmapWrite(UGL_DEVICE_ID devId,
			      UGL_DIB *pDib,
			      UGL_RECT *pSrcRect,
			      UGL_DDB_ID ddbId,
			      UGL_POINT *pDestPoint)
{
  UGL_VGA_DDB *pVgaBmp;
  UGL_RECT srcRect, destRect;
  UGL_POINT destPoint;
  UGL_INT32 x, y, left, numPlanes, planeIndex,
	destBytesPerLine, startIndex, srcOffset;
  UGL_UINT8 startMask;
  UGL_UINT8 **pPlaneArray;
  UGL_UINT8 *pSrc, *pClut;
  UGL_INT32 bpp, ppb, nPixels, shift, srcMask, destIndex, destMask;
  UGL_COLOR pixel, planeMask;

  /* Get device dependent bitmap */
  pVgaBmp = (UGL_VGA_DDB *) ddbId;

  /* Get dimensions */
  srcRect.top = pSrcRect->top;
  srcRect.bottom = pSrcRect->bottom;
  srcRect.left = pSrcRect->left;
  srcRect.right = pSrcRect->right;
  destPoint.x = pDestPoint->x;
  destPoint.y = pDestPoint->y;

  /* Calulcate destination dimensions */
  destRect.left = destPoint.x;
  destRect.top = destPoint.y;
  destRect.right = destPoint.x;
  destRect.bottom = destPoint.y;
  UGL_RECT_SIZE_TO(destRect, UGL_RECT_WIDTH(srcRect),
			     UGL_RECT_HEIGHT(srcRect));

  /* Precaculate variables */
  numPlanes = devId->pMode->Depth;
  srcOffset = (srcRect.top * pDib->stride) + srcRect.left;

  /* Handle case if the display is not the destination */
  if (ddbId != UGL_DISPLAY_ID) {

    /* Precaculate variables */
    left = destRect.left + pVgaBmp->shiftValue;
    destBytesPerLine = (pVgaBmp->header.width + 7) / 8 + 1;
    pPlaneArray = pVgaBmp->pPlaneArray;
    startIndex = destRect.top * destBytesPerLine + (left >> 3);
    startMask = 0x80 >> (left & 0x07);

    /* Handle case when source has a color lookup table */
    if (pDib->imageFormat != UGL_DIRECT) {

      /* Check if temporary clut should be generated */
      if (pDib->colorFormat != UGL_DEVICE_COLOR_32) {

        /* Generate */
	pClut = malloc(pDib->clutSize * sizeof(UGL_COLOR));
	if (pClut == UGL_NULL)
	  return UGL_STATUS_ERROR;

	/* Convert to 32-bit color */
	(*devId->colorConvert)(devId, pDib->pClut, pDib->colorFormat,
			       pClut, UGL_DEVICE_COLOR_32, pDib->clutSize);

      } /* End if temporary clut should be generated */

      /* Else keep current clut */
      else {

        pClut = pDib->pClut;

      } /* End else keep current clut */

      /* Select color mode */
      switch(pDib->imageFormat) {

	/* Byte aligned image */
	case UGL_INDEXED_8:

	  /* Calculate source pointer */
	  pSrc = (UGL_UINT8 *) pDib->pData + srcRect.top * pDib->stride;

	  /* For source height */
	  for (y = srcRect.top; y <= srcRect.bottom; y++) {

	    /* Variable recalculate for each line */
	    destIndex = startIndex;
	    destMask = startMask;

	    /* For source width */
	    for (x = srcRect.left; x <= srcRect.right; x++) {

	      /* Initialize pixel */
	      //pixel = ((UGL_COLOR *) pClut) [pSrc[x]];
	      pixel = pSrc[x];
	      planeMask = 0x01;

	      /* For each plane */
	      for (planeIndex = 0; planeIndex < numPlanes; planeIndex++) {

	        if ((pixel & planeMask) != 0)
		  pPlaneArray[planeIndex][destIndex] |= destMask;
		else
		  pPlaneArray[planeIndex][destIndex] &= ~destMask;

	        /* Advance plane mask */
	        planeMask <<= 1;

	      } /* End for each plane */

	      /* Advance to next pixel */
	      destMask >>= 1;

	      /* Check if a new byte was reached */
	      if (destMask == 0) {
	        destIndex++;
		destMask = 0x80;
	      }

	    } /* End for source width */

	    /* Advance to next line */
	    pSrc += pDib->stride;
	    startIndex += destBytesPerLine;

	  } /* End for source height */

	break;

	/* Less than one byte used for each pixel */
        case UGL_INDEXED_4:
	case UGL_INDEXED_2:
	case UGL_INDEXED_1:

          /* Precalulate pixel vars */
	  bpp = pDib->imageFormat & UGL_INDEX_MASK;
	  ppb = bpp / 8;

	  /* For source height */
	  for (y = srcRect.top; y <= srcRect.bottom; y++) {

	    /* Variable recalculate for each line */
	    pSrc = (UGL_UINT8 *) pDib->pData + srcOffset / ppb;
	    nPixels = srcOffset & ppb;
	    shift = (ppb - nPixels - 1) * bpp;
	    srcMask = (0xff >> (8 - bpp)) << shift;
	    destIndex = startIndex;
	    destMask = startMask;

	    /* For source width */
	    for (x = srcRect.left; x <= srcRect.right; x++) {

	      /* Initialize pixel */
	      pixel = (*pSrc & srcMask) >> shift;
	      planeMask = 0x01;

	      /* For each plane */
	      for (planeIndex = 0; planeIndex < numPlanes; planeIndex++) {

	        if ((pixel & planeMask) != 0)
		  pPlaneArray[planeIndex][destIndex] |= destMask;
		else
		  pPlaneArray[planeIndex][destIndex] &= ~destMask;

	        /* Advance plane mask */
	        planeMask <<= 1;

	      } /* End for each plane */

	      /* Advance to next pixel */
	      srcMask >>= bpp;
	      destMask >>= 1;
	      shift -= bpp;

	      /* Check if a new byte was reached */
	      /* For source */
	      if (srcMask == 0) {
	        pSrc++;
		shift = (bpp - 1) * bpp;
		srcMask = (0xff >> (8 - bpp)) << shift;
	      }

	      /* Check if a new byte was reached */
	      /* For destination */
	      if (destMask == 0) {
	        destIndex++;
		destMask = 0x80;
	      }

	    } /* End for source width */

	    /* Advance to next line */
	    srcOffset += pDib->stride;
	    startIndex += destBytesPerLine;

	  } /* End for source height */

	break;

	default:
	break;

      } /* End select color mode */

    } /* End not direct color */

  } /* End not display */

  return UGL_STATUS_OK;
}


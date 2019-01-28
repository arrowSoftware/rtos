/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of Real rtos.
*   Copyright (C) 2008 - 2009 Surplus Users Ham Society
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

/* uglugi.h - Universal graphics library output driver */

#ifndef _uglugi_h
#define _uglugi_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ugl_gc {
  struct ugl_ugi_driver	*pDriver;		/* Current driver */
  UGL_DDB		*pDefaultBitmap;	/* Rendering output */
  UGL_RECT		boundRect;		/* Bounding rectagle */
  UGL_RECT		viewPort;		/* View port */
  UGL_RECT		clipRect;		/* Clipping rectangle */
  UGL_RASTER_OP		rasterOp;		/* Raster operation */
  UGL_COLOR		backgroundColor;	/* Background color */
  UGL_COLOR		foregroundColor;	/* Foreground color */
  UGL_LOCK_ID		lockId;			/* Mutex */
} UGL_GC;

typedef struct ugl_gc *UGL_GC_ID;

typedef struct ugl_ugi_driver {
  UGL_MODE 	*pMode;				/* Current graphics mode */
  UGL_GC_ID	defaultGc;			/* Default graphics context*/
  UGL_LOCK_ID	lockId;				/* Mutex */

  /* Device support methods */
  UGL_STATUS	(*destroy)(struct ugl_ugi_driver *pDrv);

  /* Info support method */
  UGL_STATUS	(*info)(struct ugl_ugi_driver *pDrv,
			UGL_INFO_REQ infoReq,
			void *info);

  /* Mode support methods */
  UGL_STATUS	(*modeAvailGet)(struct ugl_ugi_driver *pDrv,
				UGL_UINT32 *pNumModes,
				const UGL_MODE **pModeArray);
  UGL_STATUS	(*modeSet)(struct ugl_ugi_driver *pDrv,
			   UGL_MODE *pMode);

  /* Graphics context support */
  UGL_GC*	(*gcCreate)(struct ugl_ugi_driver *pDrv);
  UGL_STATUS	(*gcCopy)(struct ugl_ugi_driver *pDrv,
			  UGL_GC *pSrcGc,UGL_GC *pDestGc);
  UGL_STATUS	(*gcDestroy)(struct ugl_ugi_driver *pDrv, UGL_GC *pGc);
  UGL_STATUS	(*gcSet)(struct ugl_ugi_driver *pDrv, UGL_GC *pGc);

  /* Graphics primitive support */
  UGL_STATUS	(*pixelSet)(struct ugl_ugi_driver *pDrv,
			    UGL_POINT *p, UGL_COLOR c);

  /* Color support */
  UGL_STATUS	(*colorAlloc)(struct ugl_ugi_driver *pDrv,
			      UGL_ARGB *pReqColors,
			      UGL_ORD *pIndex,
			      UGL_ARGB *pActualColors,
			      UGL_COLOR *pUglColors,
			      UGL_SIZE numColors);
  UGL_STATUS	(*colorFree)(struct ugl_ugi_driver *pDrv,
			     UGL_COLOR *pColors,
			     UGL_SIZE numColors);
  UGL_STATUS	(*clutSet)(struct ugl_ugi_driver *pDrv,
			   UGL_ORD offset,
			   UGL_ARGB *pColors,
			   UGL_SIZE numColors);
  UGL_STATUS	(*clutGet)(struct ugl_ugi_driver *pDrv,
			   UGL_ORD offset,
			   UGL_ARGB *pColors,
			   UGL_SIZE numColors);
  UGL_STATUS	(*colorConvert) (struct ugl_ugi_driver *pDrv,
		  		 void *srcArray,
				 UGL_COLOR_FORMAT srcFormat,
				 void *destArray,
				 UGL_COLOR_FORMAT destFormat,
				 UGL_SIZE arraySize);
  /* Bitmap support */
  UGL_DDB*	(*bitmapCreate)(struct ugl_ugi_driver *pDrv,
				UGL_DIB *pDib, UGL_DIB_CREATE_MODE createMode,
				UGL_COLOR intiValue, UGL_MEM_POOL_ID poolId);
  UGL_STATUS	(*bitmapDestroy)(struct ugl_ugi_driver *pDrv,
				 UGL_DDB *pDdb, UGL_MEM_POOL_ID poolId);
  UGL_STATUS	(*bitmapBlt)(struct ugl_ugi_driver *pDrv,
			     UGL_DDB *pSrcBmp, UGL_RECT *pSrcRect,
			     UGL_DDB *pDestBmp, UGL_POINT *pDestPoint);
  UGL_STATUS	(*bitmapWrite)(struct ugl_ugi_driver *pDrv,
			       UGL_DIB *pDib, UGL_RECT *pSrcRect,
			       UGL_DDB * pDdb, UGL_POINT *pDestPoint);
} UGL_UGI_DRIVER;

typedef struct ugl_ugi_driver *UGL_DEVICE_ID;

extern UGL_STATUS uglUgiDevInit(UGL_DEVICE_ID devId);
extern UGL_STATUS uglUgiDevDeinit(UGL_DEVICE_ID devId);
extern UGL_STATUS uglGraphicsDevDestroy(UGL_DEVICE_ID devId);

/*******************************************************************************
 * UGL_CLIP_LOOP_START/UGL_CLIP_LOOP_END - Start/end clipping loop
 *
 * RETURNS: N/A
 ******************************************************************************/

#define UGL_CLIP_LOOP_START(gc, pClipRect)				       \
{									       \
  UGL_STATUS _status;							       \
  do {									       \
    (pClipRect) = &(gc)->clipRect;					       \
    UGL_RECT_MOVE(*pClipRect, (gc)->viewPort.left, (gc)->viewPort.top);	       \
    _status = UGL_STATUS_FINISHED;
#define UGL_CLIP_LOOP_END						       \
  } while (_status == UGL_STATUS_OK);					       \
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _uglugi_h */


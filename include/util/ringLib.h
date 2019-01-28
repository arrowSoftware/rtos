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

/* ringLib.h - Ring buffer library */

#ifndef _ringLib_h
#define _ringLib_h

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct  {
  int offsetToBuffer;
  int offsetFromBuffer;
  int bufferSize;
  char *buffer;
} RING;

typedef RING *RING_ID;

/*******************************************************************************
* RING_ELEMENT_GET - Get character from ring buffer
*
* RETURNS: TRUE if there is character to return, FALSE if not
*******************************************************************************/

#define RING_ELEMENT_GET(ringId, pc, fp)				\
(									\
  fp = (ringId)->offsetFromBuffer,					\
  ((ringId)->offsetToBuffer == fp) ?					\
	FALSE								\
  :									\
	(								\
	*pc = (ringId)->buffer[fp],					\
	(ringId)->offsetFromBuffer =					\
		((++fp == (ringId)->bufferSize) ? 0 : fp),		\
	TRUE							\
	)								\
)

/*******************************************************************************
* RING_ELEMENT_PUT - Put character on ring buffer
*
* RETURNS: TRUE if there was room left, FALSE if not
*******************************************************************************/

#define RING_ELEMENT_PUT(ringId, c, tp)					\
(									\
  tp = (ringId)->offsetToBuffer,					\
  (tp == (ringId)->offsetFromBuffer - 1) ?				\
	FALSE								\
  :									\
	(								\
	(tp == (ringId)->bufferSize - 1) ?				\
	  (								\
	  ((ringId)->offsetFromBuffer == 0) ?				\
		FALSE							\
	:								\
		(							\
		(ringId)->buffer[tp] = c,				\
		(ringId)->offsetToBuffer = 0,				\
		TRUE							\
		)							\
	  )								\
  :									\
	(								\
	(ringId)->buffer[tp] = c,					\
	(ringId)->offsetToBuffer++,					\
	TRUE								\
	)								\
  )									\
)

extern RING_ID ringCreate(int size);
extern void ringDelete(RING_ID ringId);
extern void ringFlush(RING_ID ringId);
extern int ringBufferGet(RING_ID ringId,
		         char *buffer,
		         int nBytes);
extern int ringBufferPut(RING_ID ringId,
		         char *buffer,
		         int nBytes);
extern BOOL ringIsEmpty(RING_ID ringId);
extern BOOL ringIsFull(RING_ID ringId);
extern int ringFreeBytes(RING_ID ringId);
extern int ringNBytes(RING_ID ringId);
extern void ringPutAhead(RING_ID ringId, char c, int offset);
extern void ringMoveAhead(RING_ID ringId, int offset);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _ringLib_h */


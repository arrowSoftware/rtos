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

/* ringLib.c - Ring buffer library */

#include <stdlib.h>
#include <string.h>
#include <rtos.h>
#include <util/ringLib.h>

/*******************************************************************************
* ringCreate - Create a ring buffer
*
* RETURNS: Pointer to ring buffer
*******************************************************************************/

RING_ID ringCreate(int size)
{
  char *buffer;
  RING_ID ringId;

  ringId = (RING_ID) malloc( sizeof(RING) );
  if (ringId == NULL)
    return(NULL);

  buffer = (char *) malloc(++size);
  if (buffer == NULL) {
    free(ringId);
    return(NULL);
  }

  ringId->bufferSize = size;
  ringId->buffer = buffer;

  ringFlush(ringId);

  return(ringId);
}

/*******************************************************************************
* ringDelete - Delete a ring buffer
*
* RETURNS: N/A
*******************************************************************************/

void ringDelete(RING_ID ringId)
{
  free(ringId->buffer);
  free(ringId);
}

/*******************************************************************************
* ringFlush - Flush a ring buffer
*
* RETURNS: N/A
*******************************************************************************/

void ringFlush(RING_ID ringId)
{
  ringId->offsetToBuffer = 0;
  ringId->offsetFromBuffer = 0;
}

/*******************************************************************************
* ringBufferGet - Get from ring buffer
*
* RETURNS: Number of bytes read
*******************************************************************************/

int ringBufferGet(RING_ID ringId,
		  char *buffer,
		  int nBytes)
{
  int bread, bytes, offsetTmp;
  int offsetToBuffer;

  bread = 0;
  offsetTmp = 0;
  offsetToBuffer = ringId->offsetToBuffer;

  if (offsetToBuffer >= ringId->offsetFromBuffer) {
    /* To offset has not wrapped around */
    bread = min(nBytes, offsetToBuffer - ringId->offsetFromBuffer);
    memcpy(buffer, &ringId->buffer[ringId->offsetFromBuffer], bread);
    ringId->offsetFromBuffer += bread;
  } else {
    /* To offset has wrapped around */
    bread = min(nBytes, ringId->bufferSize - ringId->offsetFromBuffer);
    memcpy(buffer, &ringId->buffer[ringId->offsetFromBuffer], bread);
    offsetTmp = ringId->offsetFromBuffer + bread;

    /* Check wrapping */
    if (offsetTmp == ringId->bufferSize) {
      bytes = min(nBytes - bread, offsetToBuffer);
      memcpy(buffer + bread, ringId->buffer, bytes);
      ringId->offsetFromBuffer = bytes;
      bread += bytes;
    } else ringId->offsetFromBuffer = offsetTmp;
  }

  return(bread);
}

/*******************************************************************************
* ringBufferPut - Put to ring buffer
*
* RETURNS: Number of bytes written
*******************************************************************************/

int ringBufferPut(RING_ID ringId,
		  char *buffer,
		  int nBytes)
{
  int bwrote, bytes, offsetTmp;
  int offsetFromBuffer;

  bwrote = 0;
  offsetTmp = 0;
  offsetFromBuffer = ringId->offsetFromBuffer;

  if (offsetFromBuffer > ringId->offsetToBuffer) {
    /* offsetFrom is ahead of offsetTo */
    bwrote = min(nBytes, offsetFromBuffer - ringId->offsetToBuffer - 1);
    memcpy(&ringId->buffer[ringId->offsetToBuffer], buffer, bwrote);
    ringId->offsetToBuffer += bwrote;
  } else if (offsetFromBuffer == 0) {
    /* offsetFrom is at beginning */
    bwrote = min(nBytes, ringId->bufferSize - ringId->offsetToBuffer - 1);
    memcpy(&ringId->buffer[ringId->offsetToBuffer], buffer, bwrote);
    ringId->offsetToBuffer += bwrote;
  } else {
    /* From offset has wrapped around */
    bwrote = min(nBytes, ringId->bufferSize - ringId->offsetToBuffer);
    memcpy(&ringId->buffer[ringId->offsetToBuffer], buffer, bwrote);
    offsetTmp = ringId->offsetToBuffer + bwrote;

    /* Check wraping */
    if (offsetTmp == ringId->bufferSize) {
      bytes = min(nBytes - bwrote, offsetFromBuffer - 1);
      memcpy(ringId->buffer, buffer + bwrote, bytes);
      ringId->offsetToBuffer = bytes;
      bwrote += bytes;
    } else ringId->offsetToBuffer = offsetTmp;
  }

  return(bwrote);
}

/*******************************************************************************
* ringIsEmpty - Check if buffer is empty
*
* RETURNS: TRUE or FALSE
*******************************************************************************/

BOOL ringIsEmpty(RING_ID ringId)
{
  return(ringId->offsetToBuffer == ringId->offsetFromBuffer);
}

/*******************************************************************************
* ringIsFull - Check if buffer is full
*
* RETURNS: TRUE or FALSE
*******************************************************************************/

BOOL ringIsFull(RING_ID ringId)
{
  int n = ringId->offsetToBuffer - ringId->offsetFromBuffer + 1;

  return ((n == 0) || (n == ringId->bufferSize));
}

/*******************************************************************************
* ringFreeBytes - Check space left in ring buffer
*
* RETURNS: Bytes free
*******************************************************************************/

int ringFreeBytes(RING_ID ringId)
{
  int n = ringId->offsetFromBuffer - ringId->offsetToBuffer - 1;

  if (n < 0)
    n += ringId->bufferSize;

  return(n);
}

/*******************************************************************************
* ringNBytes - Get number of bytes in ring buffer
*
* RETURNS: Bytes in buffer
*******************************************************************************/

int ringNBytes(RING_ID ringId)
{
  int n = ringId->offsetToBuffer - ringId->offsetFromBuffer;

  if (n < 0)
    n += ringId->bufferSize;

  return(n);
}

/*******************************************************************************
* ringPutAhead - Put a char ahead of ringbuffer with no offset change
*
* RETURNS: N/A
*******************************************************************************/

void ringPutAhead(RING_ID ringId, char c, int offset)
{
  int n = ringId->offsetToBuffer + offset;

  if (n >= ringId->bufferSize)
    n -= ringId->bufferSize;

  *(ringId->buffer + n) = c;
}

/*******************************************************************************
* ringMoveAhead - Advance offset n bytes
*
* RETURNS: N/A
*******************************************************************************/

void ringMoveAhead(RING_ID ringId, int offset)
{
  offset += ringId->offsetToBuffer;

  if (offset >= ringId->bufferSize)
    offset -= ringId->bufferSize;

  ringId->offsetToBuffer = offset;
}


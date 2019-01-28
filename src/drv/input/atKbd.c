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

/* atKbd.c - PC keyboard driver */

#include <stdlib.h>
#include <string.h>
#include <arch/sysArchLib.h>
#include <arch/regs.h>
#include <arch/esf.h>
#include <arch/iv.h>
#include <arch/intArchLib.h>

#include <drv/input/atKbd.h>

int kbdon[128];
int keys = 0;

void atKbdHandler(void)
{
  unsigned char scancode, status;

  /* Read scancode from keyboard I/O port */
  scancode = sysInByte(KEY_BUFFER);
  status = sysInByte(KEY_CONTROL);

  /* Restore */
  status |= 0x82;
  sysOutByte(KEY_CONTROL, status);
  status &= 0x7f;
  sysOutByte(KEY_CONTROL, status);

  /* Check key up/down */
  if (scancode < 128) {

    /* Key was pressed */
    if (!kbdon[scancode])
    {
      keys++;
      kbdon[scancode] = 1;
    }

  } else {

    /* Key was released */
    if (kbdon[scancode - 128])
    {
      keys--;
      kbdon[scancode - 128] = 0;
    }

  }
}

void atKbdInit(void)
{
  int i;

  /* Clear all keys */
  for (i = 0; i < 128; i++)
    kbdon[i] = 0;

  /* Install interrupt handler */
  intConnectFunction(0x21, atKbdHandler, (void *) 0);
}


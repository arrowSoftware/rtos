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

/* i8048Kbd.c - Intel 8048 keyboard driver */

#include <rtos.h>
#include <drv/input/i8048Kbd.h>
#include <drv/serial/pcConsole.h>

/* IMPORTS */
IMPORT PC_CON_DEV pcConDev[];

/* Locals */
LOCAL KBD_CON_DEV kbdConDev;

LOCAL void kbdStatInit(void);
LOCAL void kbdConvChar(u_int8_t scancode);
LOCAL void kbdNormal(u_int8_t scancode);
LOCAL void kbdShift(u_int8_t scancode);
LOCAL void kbdCtrl(u_int8_t scancode);
LOCAL void kbdNum(u_int8_t scancode);
LOCAL void kbdCaps(u_int8_t scancode);
LOCAL void kbdStp(u_int8_t scancode);
LOCAL void kbdExt(u_int8_t scancode);
LOCAL void kbdEsc(u_int8_t scancode);
LOCAL void kbdHook(int op);

LOCAL void (*kbdHandler[])(u_int8_t scancode) =
{
  kbdNormal,
  kbdShift,
  kbdCtrl,
  kbdNum,
  kbdCaps,
  kbdStp,
  kbdExt,
  kbdEsc
};

/*******************************************************************************
* kbdHrdInit - Initialize keyboard hardware
*
* RETURNS: N/A
*******************************************************************************/

void kbdHrdInit(void)
{
  u_int8_t status;

  /* get status */
  status = sysInByte(STATUS_8048);

  /* Enable keyboard */
  sysOutByte(COMMAND_8048, (status | 0x40));

  /* Init state */
  kbdStatInit();
}

/*******************************************************************************
* kbdStatInit - Initialize keyboard state
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdStatInit(void)
{
  int i;

  /* Init virual consoles */
  for (i = 0; i < N_VIRTUAL_CONSOLES; i++)
    pcConDev[i].ks = &kbdConDev;

  kbdConDev.cursorMode = TRUE;

  /* Init default mode */
  kbdConDev.flags = KBD_NORMAL|KBD_NUM;
  kbdConDev.state = 0;
  kbdConDev.consoleHook = (FUNCPTR) kbdHook;
  kbdConDev.currConsole = PC_CONSOLE;
}

/*******************************************************************************
* kbdIntr - Keyboard interrupt callback
*
* RETURNS: N/A
*******************************************************************************/

void kbdIntr(void)
{
  u_int8_t scancode, status;

  /* Get scan code */
  scancode = sysInByte(DATA_8048);

  /* Get status */
  status = sysInByte(STATUS_8048);

  /* Send acknowledegement */
  sysOutByte(COMMAND_8048, (status | 0x80));

  /* Store status */
  sysOutByte(STATUS_8048, status);

  if (scancode == 0xfa) return;

  kbdConvChar(scancode);
}

/*******************************************************************************
* kbdConvChar - Convert scancode to character code
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdConvChar(u_int8_t scancode)
{
  int i;

  /* Extended flag */
  if (scancode == 0xe0) {
    kbdConDev.flags |= KBD_EXT;
    return;
  }

  /* High bit -> set break flag */
  if ( ((scancode & 0x80) << 2) == KBD_BRK)
    kbdConDev.flags |= KBD_BRK;
  else
    kbdConDev.flags &= ~KBD_BRK;

  if ( (scancode == 0xe1) || (kbdConDev.flags & KBD_E1) ) {
    if (scancode == 0xe1) {
      kbdConDev.flags ^= KBD_BRK;
      kbdConDev.flags ^=KBD_E1;
    }
    return;
  }

  /* Convert scancode */
  scancode &= 0x7f;

  if ( (kbdConDev.flags & KBD_EXT) == KBD_EXT) {
    for (i = 0; i < KBD_EXT_SIZE; i++) {
      if (scancode == kbdEnhanced[i]) {
        scancode = KBD_E0_BASE + i;
        i = -1;
        break;
      }
    }
    kbdConDev.flags ^=KBD_EXT;
    if (i != -1) {
      return;
    }
  }

  /* Invoke handler */
  (*kbdHandler [kbdAction[scancode]])(scancode);
}

/*******************************************************************************
* kbdNormal - Normal key handeling
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdNormal(u_int8_t scancode)
{
  u_int8_t c;
  int i;

  /* Get current console */
  i = kbdConDev.currConsole;

  if ( (kbdConDev.flags & KBD_BRK) == KBD_NORMAL ) {

    /* Get character */
    c = kbdMap[kbdConDev.state][scancode];

    if ( (c == 0xff) ||  (c == 0x00) ) return;

    /* If caps lock convert case */
    if ( ((kbdConDev.flags & KBD_CAPS) == KBD_CAPS) &&
         (c >= 'a' && c <= 'z')) c -= 'a' - 'A';

    /* Put on buffer */
    ttyIntRd(&pcConDev[i].ttyDev, c);
  }
}

/*******************************************************************************
* kbdShift - Shift key handeling
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdShift(u_int8_t scancode)
{
  if ( (kbdConDev.flags & KBD_BRK) == KBD_BRK ) {
    kbdConDev.state = KBA_NOR;
    kbdConDev.flags &= ~KBD_SHIFT;
  }
  else {
    kbdConDev.state = KBA_SHI;
    kbdConDev.flags |= KBD_SHIFT;
  }
}

/*******************************************************************************
* kbdCtrl - Control key handeling
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdCtrl(u_int8_t scancode)
{
  if ( (kbdConDev.flags & KBD_BRK) == KBD_BRK ) {
    kbdConDev.state = KBA_NOR;
    kbdConDev.flags &= ~KBD_CTRL;
  }
  else {
    kbdConDev.state = KBA_CTL;
    kbdConDev.flags |= KBD_CTRL;
  }
}

/*******************************************************************************
* kbdNum - Num lock key handeling
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdNum(u_int8_t scancode)
{
  if ( (kbdConDev.flags & KBD_BRK) == KBD_NORMAL ) {
    kbdConDev.flags ^= KBD_NUM;
    kbdConDev.state = (kbdConDev.flags & KBD_NUM) ? KBA_NOR : KBA_NUM;
  }
}

/*******************************************************************************
* kbdCaps - Caps lock key handeling
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdCaps(u_int8_t scancode)
{
  if ( (kbdConDev.flags & KBD_BRK) == KBD_NORMAL )
    kbdConDev.flags ^= KBD_CAPS;
}

/*******************************************************************************
* kbdStp - Stop key handeling
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdStp(u_int8_t scancode)
{
  int i;

  i = kbdConDev.currConsole;

  if ( (kbdConDev.flags & KBD_BRK) == KBD_NORMAL ) {
    kbdConDev.flags ^= KBD_STP;
    if (kbdConDev.flags & KBD_STP)
      ttyIntRd (&pcConDev[i].ttyDev, 0x13);
    else
      ttyIntRd (&pcConDev[i].ttyDev, 0x11);
  }
}

/*******************************************************************************
* kbdExt - Extended key handeling
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdExt(u_int8_t scancode)
{
  u_int8_t c;
  int i;

  i = kbdConDev.currConsole;

  if ( (kbdConDev.flags & KBD_BRK) == KBD_NORMAL ) {

    /* Get char */
    c = kbdMap[kbdConDev.state][scancode];

    /* Send escape char */
    ttyIntRd (&pcConDev[i].ttyDev, 0x1b);

    ttyIntRd (&pcConDev[i].ttyDev, '[');

    /* Send char */
    ttyIntRd (&pcConDev[i].ttyDev, c);
  }
}

/*******************************************************************************
* kbdEsc - Escape key handeling
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdEsc(u_int8_t scancode)
{
  u_int8_t c;
  int i;

  i = kbdConDev.currConsole;

  if ( (kbdConDev.flags & KBD_BRK) == KBD_NORMAL ) {

    /* Get char */
    c = kbdMap[kbdConDev.state][scancode];

    if ( (kbdConDev.flags & KBD_BRK) == KBD_NORMAL) {

      /* Send escape char */
      ttyIntRd (&pcConDev[i].ttyDev, 0x1b);

      ttyIntRd (&pcConDev[i].ttyDev, 'O');
    }

    /* Send char */
    ttyIntRd (&pcConDev[i].ttyDev, c);
  }
}

/*******************************************************************************
* kbdHook - Keyboard hook
*
* RETURNS: N/A
*******************************************************************************/

LOCAL void kbdHook(int op)
{
}


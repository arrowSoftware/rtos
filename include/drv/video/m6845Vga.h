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

/* m6845Vga.h - Vga terminal header */

#ifndef _m6845Vga_h
#define _m6845Vga_h

#define N_PARAM			16

#define ATRB_FG_BLACK		0x00
#define ATRB_FG_BLUE		0x01
#define ATRB_FG_GREEN		0x02
#define ATRB_FG_CYAN		0x03
#define ATRB_FG_RED		0x04
#define ATRB_FG_MAGENTA		0x05
#define ATRB_FG_BROWN		0x06
#define ATRB_FG_WHITE		0x07
#define ATRB_FG_BRIGHT		0x08

#define ATRB_FG_GRAY		(ATRB_FG_BLACK | ATRB_FG_BRIGHT)
#define ATRB_FG_LIGHTBLUE	(ATRB_FG_BLUE | ATRB_FG_BRIGHT)
#define ATRB_FG_LIGHTGREEN	(ATRB_FG_GREEN | ATRB_FG_BRIGHT)
#define ATRB_FG_LIGHTCYAN	(ATRB_FG_CYAN | ATRB_FG_BRIGHT)
#define ATRB_FG_LIGHTRED	(ATRB_FG_RED | ATRB_FG_BRIGHT)
#define ATRB_FG_LIGHTMAGENTA	(ATRB_FG_MAGENTA | ATRB_FG_BRIGHT)
#define ATRB_FG_YELLOW		(ATRB_FG_BROWN | ATRB_FG_BRIGHT)
#define ATRB_FG_BRIGHTWHITE	(ATRB_FG_WHITE | ATRB_FG_BRIGHT)

#define ATRB_BG_BLACK		0x00
#define ATRB_BG_BLUE		0x10
#define ATRB_BG_GREEN		0x20
#define ATRB_BG_CYAN		0x30
#define ATRB_BG_RED		0x40
#define ATRB_BG_MAGENTA		0x50
#define ATRB_BG_BROWN		0x60
#define ATRB_BG_WHITE		0x70
#define ATRB_BG_BLINK		0x80

#define TEXT_MODE		0
#define FG_ATTR_MASK		0x07
#define BG_ATTR_MASK		0x70
#define INT_BLINK_MASK		0x88

#define TEXT_SET		0
#define VT100_SET		1

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  u_int8_t *memBase;
  u_int8_t *selReg;
  u_int8_t *valReg;
  int row, col;
  u_int8_t *currCharPos;
  u_int8_t currAttrib;
  u_int8_t defaultAttrib;
  int nRow, nCol;
  int scrollStart, scrollEnd;
  BOOL reverse;
  BOOL autoWrap;
  BOOL savedReverse;
  int savedRow, savedCol;
  u_int8_t savedCurrAttrib;
  BOOL scrollCheck;
  u_int8_t * charSet;
  int vgaMode;
  BOOL colorMode;
  BOOL insertMode;
  char tabStop[80];
  u_int16_t escFlags;
  int escParamCount;
  BOOL escQuestion;
  int escParam[N_PARAM];
  char escResponse[10];
  FUNCPTR vgaHook;
} VGA_CON_DEV;

extern void vgaHrdInit(void);
extern void vgaLoadFont(unsigned char *fontArray, int height);
//extern int vgaWriteString(PC_CON_DEV *pc);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _m6845Vga_h */


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

/* vga.c - VGA terminal */

#include <string.h>
#include <drv/video/vga.h>

/* Textpointer, color and cursor position */
unsigned short *textmemptr = (unsigned short *) 0xb8000;
unsigned short attrib = 0x1f;
int csr_x = 0, csr_y = 0;

/* Initialize frame buffer */
void vgaInit(void)
{
  vgaClear2();
}

/* Clear screen */
void vgaClear2(void)
{
  unsigned short *ptr;
  unsigned short blank;
  int i;

  ptr = textmemptr;
  blank = 0x20 | (attrib << 8);

  /* Loop for all lines */
  for (i = 0; i < 25; i++, ptr+=80)
    memsetw(ptr, blank, 80);

  /* Reset position */
  csr_x = 0;
  csr_y = 0;
  vgaMoveCsr();
}

/* Update the hardware cursor, blink, blink */
void vgaMoveCsr(void)
{
  int temp;

  /* Equation to find index in linear chunk buffer */
  temp = csr_y * 80 + csr_x;

  sysOutByte(0x3d4, 14);
  sysOutByte(0x3d5, temp >> 8);
  sysOutByte(0x3d4, 15);
  sysOutByte(0x3d5, temp);
}

/* Scroll screen */
void vgaScroll2(void)
{
  unsigned short blank;
  int temp;

  /* Blank is defined as space */
  blank = 0x20 | (attrib << 8);

  /* Row 25 is end, scroll up */
  if (csr_y >= 25)
  {
    /* Move text chunk back */
    temp = csr_y - 25 + 1;
    memcpy(textmemptr, textmemptr + temp * 80, (25 - temp) * 80 * 2);

    /* Finally make last line blank */
    memsetw(textmemptr + (25 - temp) * 80, blank, 80);
    csr_y = 25 - 1;
  }
}

/* Put a single character on screen */
void vgaPutch(char c)
{
  u_int16_t *where;
  u_int16_t att = attrib << 8;

  /* Handle a backspace, by moving cursor back one space */
  if (c == 0x08)
  {
    if (csr_x != 0) csr_x--;
  }

  /* Handle a tab by incrementing the cursors x a multiple of 8 */
  else if (c == 0x09)
  {
    csr_x  = (csr_x + 8) & ~(8 - 1);
  }

  /* Handle a carrige return which brings back cursor to start of line */
  else if (c == '\r')
  {
    csr_x = 0;
  }

  /* Handle newlines like CR/LF was sent */
  else if (c == '\n')
  {
    csr_x = 0;
    csr_y++;
  }

  /* Any character greater than space is pritable */
  else if (c >= ' ')
  {
    where = (u_int16_t *) (textmemptr + (csr_y * 80 + csr_x));
    *where = c | att;	/* Char and color */
    csr_x++;
  }

  /* If end on line reached, advance to next line */
  if (csr_x >= 80)
  {
    csr_x = 0;
    csr_y++;
  }

  /* Scroll and move cursor if needed */
  vgaScroll2();
  vgaMoveCsr();
}

/* Print a line on screen */
void vgaPuts(const char *str)
{
  int i;
  for (i = 0; i < strlen(str); i++) vgaPutch(str[i]);
}

/* Set bg/fg color */
void vgaSetColor(u_int8_t fg, u_int8_t bg)
{
  /* Top 4 bytes are the background, bottom 4 foreground */
  attrib = (bg << 4) | (fg &0x0f);
}


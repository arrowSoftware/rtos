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

/* vga.c - Dreamcast terminal */

#include <string.h>
#include <drv/video/pvr.h>
#include <drv/bios/dreamcast/biosfont.h>
#include <drv/output/dreamcast/dcScrText.h>

/* Textpointer, color and cursor position */
static int csr_x = 0, csr_y = 0;
static int con_startx = 48, con_starty = 48;
static int con_endx   = DC_SCREEN_WIDTH - 48;
static int con_endy   = DC_SCREEN_HEIGHT - 48;
static int con_nrows, con_ncols;

/* Initialize frame buffer */
void dcScrTextInit(void)
{
  con_ncols = (con_endx - con_startx) / DC_FONT_WIDTH;
  con_nrows = (con_endy - con_starty) / DC_FONT_HEIGHT;
  vid_init(vid_check_cable(), DM_640x480, PM_RGB565);
  vid_clear(0,0,255);
  bfont_border_color(255,255,255);
  bfont_font_color(0,0,0);
  dcScrTextClear();
}

/* Clear screen */
void dcScrTextClear(void)
{
  int i, rowlen;
  u_int16_t *ptr;

  /* Clear console */
  ptr = vram_s + con_startx + con_starty * DC_SCREEN_WIDTH;
  rowlen = con_endx - con_startx;
  for (i = con_starty; i < con_endy; i++) {
    memsetw(ptr, 0xffff, rowlen);
    ptr += DC_SCREEN_WIDTH;
  }

  /* Reset position */
  csr_x = 0;
  csr_y = 0;
  dcScrTextMoveCsr();
}

/* Update the hardware cursor, blink, blink */
void dcScrTextMoveCsr(void)
{
}

/* Scroll screen */
void dcScrTextScroll(void)
{
  u_int16_t *src, *dst;
  size_t siz;
  int i;

  if (csr_y >= con_nrows)
  {
    src = vram_s + (con_starty + DC_FONT_HEIGHT) * DC_SCREEN_WIDTH;
    dst = vram_s + con_starty * DC_SCREEN_WIDTH;
    siz = (con_endy - con_starty - DC_FONT_HEIGHT) * DC_SCREEN_WIDTH;
    memcpyw(dst, src, siz);
    csr_y = con_nrows - 1;
  }
}

/* Put a single character on screen */
void dcScrTextPutch(char c)
{
  char str[2];

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
    str[0] = c;
    str[1] = '\0';
    bfont_draw_str(vram_s + csr_x * DC_FONT_WIDTH + con_startx +
		        (csr_y * DC_FONT_HEIGHT + con_starty) * DC_SCREEN_WIDTH,
			 DC_SCREEN_WIDTH, 1, str);
    csr_x++;
  }

  /* If end on line reached, advance to next line */
  if (csr_x >= con_ncols)
  {
    csr_x = 0;
    csr_y++;
  }

  /* Scroll and move cursor if needed */
  dcScrTextScroll();
  dcScrTextMoveCsr();
}

/* Print a line on screen */
void dcScrTextPuts(const char *str)
{
  int i;
  for (i = 0; i < strlen(str); i++) dcScrTextPutch(str[i]);
}


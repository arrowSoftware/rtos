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

/* dcScrText.h - Draw text on dreamcast screen */

#ifndef _dcScrText_h
#define _dcScrText_h

#define DC_SCREEN_WIDTH		640
#define DC_SCREEN_HEIGHT	480
#define	DC_FONT_WIDTH		12
#define DC_FONT_HEIGHT		24

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

extern void dcScrTextInit(void);
extern void dcScrTextClear(void);
extern void dcScrTextMoveCsr(void);
extern void dcScrTextScroll(void);
extern void dcSrcTextPutch(char c);
extern void dcScrTextPuts(const char *str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _dcScrText_h */


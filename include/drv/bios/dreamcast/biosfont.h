/* KallistiOS 1.2.0

   dc/biosfont.h
   (c)2000-2001 Dan Potter
   Japanese Functions (c)2002 Kazuaki Matsumoto

   $Id: biosfont.h,v 1.2 2008/05/10 15:02:52 alexuspol Exp $

*/


#ifndef __DC_BIOSFONT_H
#define __DC_BIOSFONT_H

#include <sys/types.h>

/* Constants for the function below */
#define BFONT_CODE_ISO8859_1	0	/* DEFAULT */
#define BFONT_CODE_EUC		1
#define BFONT_CODE_SJIS		2

/* Set color */
void bfont_border_color(int r, int g, int b);
void bfont_font_color(int r, int g, int b);

/* Select an encoding for Japanese (or disable) */
void bfont_set_encoding(int enc);

u_int8_t *bfont_find_char(int ch);
u_int8_t *bfont_find_char_jp(int ch);
u_int8_t *bfont_find_char_jp_half(int ch);

void bfont_draw(u_int16_t *buffer, int bufwidth, int opaque, int c);
void bfont_draw_thin(u_int16_t *buffer, int bufwidth, int opaque, int c, int iskana);
void bfont_draw_wide(u_int16_t *buffer, int bufwidth, int opaque, int c);
void bfont_draw_str(u_int16_t *buffer, int width, int opaque, char *str);

#endif  /* __DC_BIOSFONT_H */

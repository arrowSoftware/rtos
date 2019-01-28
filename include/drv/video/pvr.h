/* This file is part of the libdream Dreamcast function library.
 * Please see libdream.c for further details.
 *
 * (c)2000 Dan Potter
 */

#ifndef _pvr_h
#define _pvr_h

#define CT_VGA          0
#define CT_RGB          2
#define CT_COMPOSITE    3
#define PM_RGB555       0
#define PM_RGB565       1
#define PM_RGB888       3
#define DM_640x480      0
#define DM_320x240      1
#define DM_800x608      2

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

/* Globals */
extern unsigned long *vram_l;
extern unsigned short *vram_s;
extern int vram_config;
extern int vram_size;
extern int vid_cable_type;

/* Cable init */
extern int vid_check_cable(void);

/* Video init */
extern void vid_init(int cable_type, int disp_mode, int pixel_mode);

/* Other functions */
extern void vid_set_start(u_int32_t start);
extern void vid_border_color(int r, int g, int b);
extern void vid_clear(int r, int g, int b);
extern void vid_empty(void);
extern void vid_waitvbl(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _pvr_h */


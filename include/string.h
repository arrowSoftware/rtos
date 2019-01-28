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

/*  string.h - String functions */

#ifndef _string_h_
#define _string_h_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include <sys/types.h>

extern void *memchr(const void *str, int c, size_t len);
extern int memcmp(const void *s1, const void *s2, size_t len);
extern void *memcpy(void *dest, const void *src, size_t size);
extern void *memcpyw(void *dest, const void *src, size_t size);
extern void *memmove(void *dst, const void *src, size_t len);
extern void *memset(void *dest, char data, size_t size);
extern void *memsetw(short *dest, short data, size_t size);
extern size_t strlen(const char *s);
extern char *strpbrk(const char *str, const char *set);
extern char *strcpy(char *s1, const char *s2);
extern char *strncpy(char *s1, const char *s2, size_t size);
extern char *strcat(char *s1, const char *s2);
extern char *strchr(const char *s, int c);
extern char *strrchr(register const  char *s, int c);
extern size_t strspn(const char *s, const char *accept);
extern char *strstr(const char *s1, const char *s2);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern char *strtok_r(char *s, const char *delim, char **save_ptr);
extern char *strtok(char *s, const char *delim);
extern char *strdup(const char *s1);
extern void *memccpy(void *s1, const void *s2, int c, size_t n);
extern size_t strcspn(const char *s1, const char *s2);
extern char *strncat(char *s1, const char *s2, size_t n);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


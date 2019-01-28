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

/* stdlib.h - Standard library */

#ifndef _stdlib_h
#define _stdlib_h

#include <sys/types.h>
#include <rtos.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define NULL */
#if     !defined(NULL)
#if defined __GNUG__
#define NULL (__null__)
#else
#if !defined __cplusplus && 0
#define ((void *)0)
#else
#define NULL (0)
#endif
#endif
#endif

typedef struct {
  int quot;
  int rem;
} div_t;

typedef struct {
  long quot;
  long rem;
} ldiv_t;

#define PATH_MAX 1024

#define RAND_MAX	32767

#define min(a,b)	((a) < (b) ? (a) : (b))
#define max(a,b)	((a) > (b) ? (a) : (b))

extern void* malloc(size_t size);
extern void* realloc(void *buf, size_t size);
extern void* memalign(size_t alignement, size_t size);
extern void* valloc(size_t size);
extern void* calloc(size_t num, size_t size);
extern void free(void *buf);
extern int cfree(void *buf);

extern void exit(int code);

extern int abs(int i);
extern char *itoa(int value, char *string, int radix);
extern char *itox(int val, char *str);
extern long strtol(const char *str, char **endptr, int base);
extern void* srand(unsigned int seed);
extern int rand(void);
extern div_t div(int numer, int denom);
extern void div_r(int numer, int denom, div_t *result);
extern ldiv_t ldiv(long numer, long denom);
extern void ldiv_r(long numer, long denom, ldiv_t *result);

extern int putenv(char *string);
extern char* getenv(const char *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _stdlib_h */


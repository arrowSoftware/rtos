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

/* shellLibP.h - Shell library private header */

#ifndef _shellLibP_h
#define _shellLibP_h

#include <stdlib.h>
#include <rtos.h>

/* Defines */
#define LEX_EMPTY		-2

#define MAX_SHELL_LINE		128
#define MAX_SYMBOL_SIZE		8
#define MAX_SHELL_ARGS		30
#define MAX_FUNC_ARGS		12

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Defines */
#define YYSTYPE			VALUE
#define NULLVAL			(VALUE *) NULL

/* Types */
typedef enum {
  FIRST_LEXME,
  NORMAL,
  PR_OPEN,
  PR_OPEN_DONE,
  PR_CLOSE,
  PR_CLOSE_DONE
} AUTO_STATE;

typedef enum {
  T_UNKNOWN,
  T_BYTE,
  T_WORD,

#ifdef INCLUDE_FLOAT

  T_INT,
  T_FLOAT,
  T_DOUBLE

#else /* !INCLUDE_FLOAT */

  T_INT

#endif /* INCLUDE_FLOAT */

} TYPE;

typedef enum {
  LHS,
  RHS,
  FHS
} SIDE;

typedef struct {

  SIDE side;
  TYPE type;
  union {
    int *lv;

    char byte;
    short word;
    int rv;
    char *str;

#ifdef INCLUDE_FLOAT

    float fp;
    double dp;

#endif /* INCLUDE_FLOAT */

  } value;

} VALUE;

/* Macros */
#define SET_ERROR		semError = TRUE
#define CHECK			if (semError) YYERROR

/* Functions */
IMPORT void lexNewLine(char *line);
IMPORT int yylex(void);
IMPORT void yystart(char *line);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _shellLibP_h */


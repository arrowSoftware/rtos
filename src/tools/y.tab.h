/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NL = 0,
     T_SYMBOL = 258,
     D_SYMBOL = 259,
     U_SYMBOL = 260,
     NUMBER = 261,
     CHAR = 262,
     STRING = 263,
     FLOAT = 264,
     OR = 265,
     AND = 266,
     EQ = 267,
     NE = 268,
     GE = 269,
     LE = 270,
     INCR = 271,
     DECR = 272,
     ROT_LEFT = 273,
     ROT_RIGHT = 274,
     UMINUS = 275,
     PTR = 276,
     TYPECAST = 277,
     ENDFILE = 278,
     LEX_ERROR = 279,
     XORA = 280,
     ORA = 281,
     ANDA = 282,
     SHRA = 283,
     SHLA = 284,
     SUBA = 285,
     ADDA = 286,
     MODA = 287,
     DIVA = 288,
     MULA = 289,
     UNARY = 290
   };
#endif
/* Tokens.  */
#define NL 0
#define T_SYMBOL 258
#define D_SYMBOL 259
#define U_SYMBOL 260
#define NUMBER 261
#define CHAR 262
#define STRING 263
#define FLOAT 264
#define OR 265
#define AND 266
#define EQ 267
#define NE 268
#define GE 269
#define LE 270
#define INCR 271
#define DECR 272
#define ROT_LEFT 273
#define ROT_RIGHT 274
#define UMINUS 275
#define PTR 276
#define TYPECAST 277
#define ENDFILE 278
#define LEX_ERROR 279
#define XORA 280
#define ORA 281
#define ANDA 282
#define SHRA 283
#define SHLA 284
#define SUBA 285
#define ADDA 286
#define MODA 287
#define DIVA 288
#define MULA 289
#define UNARY 290




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;


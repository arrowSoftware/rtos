/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 23 "shell.yacc"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <a.out.h>
#include <rtos.h>
#include <os/symbol.h>
#include <os/symLib.h>
#include <io/ioLib.h>

#include <tools/private/shellLibP.h>

/* Imports */
IMPORT SYMTAB_ID sysSymTable;
IMPORT int redirInFd;
IMPORT int redirOutFd;

/* Locals */
LOCAL BOOL semError;
LOCAL BOOL usymUnprocessed;
LOCAL VALUE usymValue;
LOCAL int nArgs;
LOCAL ARG argStack[MAX_SHELL_ARGS];
LOCAL BOOL spawnFlag;

LOCAL void yyerror(char *str);
LOCAL void printSym(int val, char *prefix, char *suffix);
LOCAL BOOL checkLHSValue(VALUE *pValue);
LOCAL BOOL checkRHSValue(VALUE *pValue);
LOCAL int* getLHSValue(VALUE *pValue);
LOCAL VALUE* getRHSValue(VALUE *pValue, VALUE *pResult);
LOCAL void printLHSValue(VALUE *pValue);
LOCAL void printRHSValue(VALUE *pValue);
LOCAL void printValue(VALUE *pValue);
LOCAL int newString(char *str);
LOCAL VALUE newSym(char *name, TYPE type);
LOCAL void convUnknown(VALUE *pValue);
LOCAL void convByte(VALUE *pValue);
LOCAL void convWord(VALUE *pValue);
LOCAL void convInt(VALUE *pValue);

#ifdef INCLUDE_FLOAT

LOCAL void convFloat(VALUE *pValue);
LOCAL void convFloat(VALUE *pValue);

#endif /* INCLUDE_FLOAT */

LOCAL void typeConvert(VALUE *pValue, TYPE type, SIDE side);
LOCAL void assign(VALUE *pLHSValue, VALUE *pRHSValue);

LOCAL VALUE evalExp(VALUE *pValue1, int op, VALUE *pValue2);
LOCAL void setLHSValue(VALUE *pValue1, VALUE *pValue2);
LOCAL void setRHSValue(VALUE *pValue1, VALUE *pValue2);

LOCAL void binOp(VALUE *pResult, VALUE *pValue1, int op, VALUE *pValue2);

LOCAL VALUE evalUnknown(VALUE *pValue1, int op, VALUE *pValue2);
LOCAL VALUE evalByte(VALUE *pValue1, int op, VALUE *pValue2);
LOCAL VALUE evalWord(VALUE *pValue1, int op, VALUE *pValue2);
LOCAL VALUE evalInt(VALUE *pValue1, int op, VALUE *pValue2);

#ifdef INCLUDE_FLOAT

LOCAL VALUE evalFloat(VALUE *pValue1, int op, VALUE *pValue2);
LOCAL VALUE evalDouble(VALUE *pValue1, int op, VALUE *pValue2);

#endif /* INCLUDE_FLOAT */

LOCAL VALUE newArgList(void);
LOCAL void addArg(VALUE *pArgList, VALUE *pNewArg);
LOCAL VALUE funcCall(VALUE *pValue, VALUE *pArgList);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 256 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ || defined __cplusplus || defined _MSC_VER)
void *malloc (size_t); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  30
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   546

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  8
/* YYNRULES -- Number of rules.  */
#define YYNRULES  63
/* YYNRULES -- Number of states.  */
#define YYNSTATES  117

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   290

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    53,     2,     2,     2,    47,    40,     2,
      50,    52,    45,    43,    56,    44,     2,    46,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,    51,
      42,    25,    41,    36,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    49,     2,    55,    39,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    38,     2,    54,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      48
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     9,    10,    12,    14,    16,    18,
      20,    22,    24,    28,    33,    36,    39,    42,    45,    48,
      51,    57,    62,    66,    70,    74,    78,    82,    86,    90,
      94,    98,   102,   106,   110,   114,   118,   122,   126,   130,
     134,   138,   141,   144,   147,   150,   154,   158,   162,   166,
     170,   174,   178,   182,   186,   190,   194,   195,   200,   201,
     203,   205,   209,   213
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      58,     0,    -1,    59,    -1,    59,    51,    58,    -1,    -1,
      60,    -1,     4,    -1,     3,    -1,     8,    -1,     7,    -1,
       6,    -1,     9,    -1,    50,    60,    52,    -1,    60,    50,
      62,    52,    -1,    64,    60,    -1,    45,    60,    -1,    40,
      60,    -1,    44,    60,    -1,    53,    60,    -1,    54,    60,
      -1,    60,    36,    60,    37,    60,    -1,    60,    49,    60,
      55,    -1,    60,    21,    60,    -1,    60,    43,    60,    -1,
      60,    44,    60,    -1,    60,    45,    60,    -1,    60,    46,
      60,    -1,    60,    47,    60,    -1,    60,    18,    60,    -1,
      60,    19,    60,    -1,    60,    40,    60,    -1,    60,    39,
      60,    -1,    60,    38,    60,    -1,    60,    11,    60,    -1,
      60,    10,    60,    -1,    60,    12,    60,    -1,    60,    13,
      60,    -1,    60,    14,    60,    -1,    60,    15,    60,    -1,
      60,    41,    60,    -1,    60,    42,    60,    -1,    16,    60,
      -1,    17,    60,    -1,    60,    16,    -1,    60,    17,    -1,
      60,    32,    60,    -1,    60,    31,    60,    -1,    60,    28,
      60,    -1,    60,    27,    60,    -1,    60,    33,    60,    -1,
      60,    26,    60,    -1,    60,    35,    60,    -1,    60,    34,
      60,    -1,    60,    30,    60,    -1,    60,    29,    60,    -1,
      60,    25,    60,    -1,    -1,     5,    61,    25,    60,    -1,
      -1,    63,    -1,    60,    -1,    63,    56,    60,    -1,    50,
      22,    52,    -1,    50,    22,    50,    52,    52,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   127,   127,   128,   131,   132,   136,   137,   138,   141,
     142,   143,   144,   145,   147,   153,   157,   159,   160,   161,
     162,   165,   168,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   192,   195,   201,   207,   210,   213,   216,   219,
     222,   225,   228,   231,   234,   237,   239,   238,   256,   257,
     260,   262,   266,   267
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "NL", "error", "$undefined", "T_SYMBOL", "D_SYMBOL", "U_SYMBOL",
  "NUMBER", "CHAR", "STRING", "FLOAT", "OR", "AND", "EQ", "NE", "GE", "LE",
  "INCR", "DECR", "ROT_LEFT", "ROT_RIGHT", "UMINUS", "PTR", "TYPECAST",
  "ENDFILE", "LEX_ERROR", "'='", "XORA", "ORA", "ANDA", "SHRA", "SHLA",
  "SUBA", "ADDA", "MODA", "DIVA", "MULA", "'?'", "':'", "'|'", "'^'",
  "'&'", "'>'", "'<'", "'+'", "'-'", "'*'", "'/'", "'%'", "UNARY", "'['",
  "'('", "';'", "')'", "'!'", "'~'", "']'", "','", "$accept", "line",
  "stmt", "expr", "@1", "arglist", "nArgList", "typecast", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,    61,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   289,    63,    58,   124,    94,
      38,    62,    60,    43,    45,    42,    47,    37,   290,    91,
      40,    59,    41,    33,   126,    93,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    57,    58,    58,    59,    59,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    61,    60,    62,    62,
      63,    63,    64,    64
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     0,     1,     1,     1,     1,     1,
       1,     1,     3,     4,     2,     2,     2,     2,     2,     2,
       5,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     0,     4,     0,     1,
       1,     3,     3,     5
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     7,     6,    56,    10,     9,     8,    11,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     2,     5,     0,
       0,    41,    42,    16,    17,    15,     0,     0,    18,    19,
       1,     4,     0,     0,     0,     0,     0,     0,    43,    44,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    58,    14,     0,     0,
      62,    12,     3,    34,    33,    35,    36,    37,    38,    28,
      29,    22,    55,    50,    48,    47,    54,    53,    46,    45,
      49,    52,    51,     0,    32,    31,    30,    39,    40,    23,
      24,    25,    26,    27,     0,    60,     0,    59,    57,     0,
       0,    21,    13,     0,    63,    20,    61
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    16,    17,    18,    20,   106,   107,    19
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -43
static const yytype_int16 yypact[] =
{
      79,   -43,   -43,   -43,   -43,   -43,   -43,   -43,    79,    79,
      79,    79,    79,    59,    79,    79,     9,   -39,   254,    79,
     -12,    40,    40,    40,    40,    40,   -42,   170,    40,    40,
     -43,    79,    79,    79,    79,    79,    79,    79,   -43,   -43,
      79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,   -43,    79,   -38,
     -43,   -43,   -43,   308,   347,   445,   445,   453,   453,   461,
     461,   -43,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   213,   359,   398,   408,   453,   453,   496,
     496,    57,    57,    57,   124,   254,   -37,   -40,   295,   -35,
      79,   -43,   -43,    79,   -43,   295,   254
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -43,   -13,   -43,    -8,   -43,   -43,   -43,   -43
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      21,    22,    23,    24,    25,    27,    28,    29,    69,    30,
      70,    67,    31,    68,   109,   112,   113,   114,    72,     0,
       0,     0,     0,     0,    73,    74,    75,    76,    77,    78,
       0,     0,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,     0,
     108,    42,     1,     2,     3,     4,     5,     6,     7,     0,
       0,     0,     0,    38,    39,     8,     9,     0,    42,     0,
       0,    26,     1,     2,     3,     4,     5,     6,     7,    65,
      66,     0,     0,     0,     0,     8,     9,     0,     0,    10,
       0,     0,   115,    11,    12,   116,    65,    66,     0,    13,
       0,     0,    14,    15,     0,     0,     0,     0,     0,    10,
       0,     0,     0,    11,    12,     0,     0,     0,     0,    13,
       0,     0,    14,    15,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     0,    42,     0,     0,     0,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,     0,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,     0,    65,    66,     0,     0,     0,     0,   111,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
       0,    42,     0,     0,     0,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,     0,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,     0,    65,
      66,     0,    71,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,     0,    42,     0,     0,     0,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
     110,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,     0,    65,    66,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     0,    42,     0,     0,     0,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,     0,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,     0,    65,    66,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,     0,    42,     0,     0,    33,
      34,    35,    36,    37,    38,    39,    40,    41,     0,    42,
       0,    54,     0,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,     0,    65,    66,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,     0,    65,    66,    34,
      35,    36,    37,    38,    39,    40,    41,     0,    42,     0,
       0,    34,    35,    36,    37,    38,    39,    40,    41,     0,
      42,     0,     0,     0,     0,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,     0,    65,    66,    56,    57,
      58,    59,    60,    61,    62,    63,    64,     0,    65,    66,
      34,    35,    36,    37,    38,    39,    40,    41,     0,    42,
      34,    35,    36,    37,    38,    39,    40,    41,     0,    42,
       0,     0,     0,     0,     0,     0,     0,     0,    57,    58,
      59,    60,    61,    62,    63,    64,     0,    65,    66,    58,
      59,    60,    61,    62,    63,    64,     0,    65,    66,    36,
      37,    38,    39,    40,    41,     0,    42,     0,     0,    38,
      39,    40,    41,     0,    42,     0,     0,    38,    39,     0,
       0,     0,    42,     0,     0,     0,    58,    59,    60,    61,
      62,    63,    64,     0,    65,    66,    60,    61,    62,    63,
      64,     0,    65,    66,    60,    61,    62,    63,    64,     0,
      65,    66,    38,    39,     0,     0,     0,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    62,    63,    64,     0,    65,    66
};

static const yytype_int8 yycheck[] =
{
       8,     9,    10,    11,    12,    13,    14,    15,    50,     0,
      52,    19,    51,    25,    52,    52,    56,    52,    31,    -1,
      -1,    -1,    -1,    -1,    32,    33,    34,    35,    36,    37,
      -1,    -1,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    -1,
      68,    21,     3,     4,     5,     6,     7,     8,     9,    -1,
      -1,    -1,    -1,    16,    17,    16,    17,    -1,    21,    -1,
      -1,    22,     3,     4,     5,     6,     7,     8,     9,    49,
      50,    -1,    -1,    -1,    -1,    16,    17,    -1,    -1,    40,
      -1,    -1,   110,    44,    45,   113,    49,    50,    -1,    50,
      -1,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,    40,
      -1,    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    50,
      -1,    -1,    53,    54,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    -1,    21,    -1,    -1,    -1,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    -1,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    -1,    49,    50,    -1,    -1,    -1,    -1,    55,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      -1,    21,    -1,    -1,    -1,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    -1,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    -1,    49,
      50,    -1,    52,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    -1,    21,    -1,    -1,    -1,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    -1,    49,    50,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    -1,    21,    -1,    -1,    -1,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    -1,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    -1,    49,    50,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    -1,    21,    -1,    -1,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    -1,    21,
      -1,    36,    -1,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    -1,    49,    50,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    -1,    49,    50,    12,
      13,    14,    15,    16,    17,    18,    19,    -1,    21,    -1,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    -1,
      21,    -1,    -1,    -1,    -1,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    -1,    49,    50,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    -1,    49,    50,
      12,    13,    14,    15,    16,    17,    18,    19,    -1,    21,
      12,    13,    14,    15,    16,    17,    18,    19,    -1,    21,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,
      42,    43,    44,    45,    46,    47,    -1,    49,    50,    41,
      42,    43,    44,    45,    46,    47,    -1,    49,    50,    14,
      15,    16,    17,    18,    19,    -1,    21,    -1,    -1,    16,
      17,    18,    19,    -1,    21,    -1,    -1,    16,    17,    -1,
      -1,    -1,    21,    -1,    -1,    -1,    41,    42,    43,    44,
      45,    46,    47,    -1,    49,    50,    43,    44,    45,    46,
      47,    -1,    49,    50,    43,    44,    45,    46,    47,    -1,
      49,    50,    16,    17,    -1,    -1,    -1,    21,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    46,    47,    -1,    49,    50
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    16,    17,
      40,    44,    45,    50,    53,    54,    58,    59,    60,    64,
      61,    60,    60,    60,    60,    60,    22,    60,    60,    60,
       0,    51,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    21,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    49,    50,    60,    25,    50,
      52,    52,    58,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    62,    63,    60,    52,
      37,    55,    52,    56,    52,    60,    60
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:
#line 132 "shell.yacc"
    { printValue(&(yyvsp[(1) - (1)]));
		 		  CHECK; }
    break;

  case 7:
#line 137 "shell.yacc"
    { (yyvsp[(1) - (1)]).side = RHS; setRHSValue(&(yyval), &(yyvsp[(1) - (1)])); }
    break;

  case 8:
#line 138 "shell.yacc"
    { (yyval) = (yyvsp[(1) - (1)]);
				  (yyval).value.rv = newString((char *) (yyvsp[(1) - (1)]).value.rv);
			  	  CHECK; }
    break;

  case 12:
#line 144 "shell.yacc"
    { (yyval) = (yyvsp[(2) - (3)]); }
    break;

  case 13:
#line 145 "shell.yacc"
    { (yyval) = funcCall(&(yyvsp[(1) - (4)]), &(yyvsp[(3) - (4)]));
				  CHECK; }
    break;

  case 14:
#line 148 "shell.yacc"
    {
				  typeConvert(&(yyvsp[(2) - (2)]), (yyvsp[(1) - (2)]).type, (yyvsp[(1) - (2)]).side);
				  (yyval) = (yyvsp[(2) - (2)]);
				  CHECK;
				}
    break;

  case 15:
#line 153 "shell.yacc"
    { VALUE val;
				  getRHSValue(&(yyvsp[(2) - (2)]), &val);
				  setLHSValue(&(yyval), &val);
				  CHECK; }
    break;

  case 16:
#line 157 "shell.yacc"
    { (yyval).value.rv = (int) getLHSValue(&(yyvsp[(2) - (2)]));
				  (yyval).type = T_INT; (yyval).side = RHS; }
    break;

  case 17:
#line 159 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(2) - (2)]), UMINUS, NULLVAL); }
    break;

  case 18:
#line 160 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(2) - (2)]), '!', NULLVAL); }
    break;

  case 19:
#line 161 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(2) - (2)]), '~', NULLVAL); }
    break;

  case 20:
#line 162 "shell.yacc"
    { VALUE val;
				   setRHSValue(&(yyvsp[(1) - (5)]), &val);
				   setRHSValue(&(yyval), val.value.rv ? &(yyvsp[(3) - (5)]) : &(yyvsp[(5) - (5)])); }
    break;

  case 21:
#line 165 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (4)]), '+', &(yyvsp[(3) - (4)]));
				  typeConvert(&(yyval), T_INT, RHS);
				  setLHSValue(&(yyval), &(yyval)); }
    break;

  case 22:
#line 168 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '+', &(yyvsp[(3) - (3)]));
				  typeConvert(&(yyval), T_INT, RHS);
				  setLHSValue(&(yyval), &(yyval)); }
    break;

  case 23:
#line 171 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '+', &(yyvsp[(3) - (3)])); }
    break;

  case 24:
#line 172 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '-', &(yyvsp[(3) - (3)])); }
    break;

  case 25:
#line 173 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '*', &(yyvsp[(3) - (3)])); }
    break;

  case 26:
#line 174 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '/', &(yyvsp[(3) - (3)])); }
    break;

  case 27:
#line 175 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '%', &(yyvsp[(3) - (3)])); }
    break;

  case 28:
#line 176 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), ROT_LEFT, &(yyvsp[(3) - (3)])); }
    break;

  case 29:
#line 177 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), ROT_RIGHT, &(yyvsp[(3) - (3)])); }
    break;

  case 30:
#line 178 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '&', &(yyvsp[(3) - (3)])); }
    break;

  case 31:
#line 179 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '^', &(yyvsp[(3) - (3)])); }
    break;

  case 32:
#line 180 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '|', &(yyvsp[(3) - (3)])); }
    break;

  case 33:
#line 181 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), AND, &(yyvsp[(3) - (3)])); }
    break;

  case 34:
#line 182 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), OR, &(yyvsp[(3) - (3)])); }
    break;

  case 35:
#line 183 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), EQ, &(yyvsp[(3) - (3)])); }
    break;

  case 36:
#line 184 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), NE, &(yyvsp[(3) - (3)])); }
    break;

  case 37:
#line 185 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), GE, &(yyvsp[(3) - (3)])); }
    break;

  case 38:
#line 186 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), LE, &(yyvsp[(3) - (3)])); }
    break;

  case 39:
#line 187 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '>', &(yyvsp[(3) - (3)])); }
    break;

  case 40:
#line 188 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), '<', &(yyvsp[(3) - (3)])); }
    break;

  case 41:
#line 189 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(2) - (2)]), INCR, NULLVAL);
				  assign(&(yyvsp[(2) - (2)]), &(yyval));
				  CHECK; }
    break;

  case 42:
#line 192 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(2) - (2)]), DECR, NULLVAL);
				  assign(&(yyvsp[(2) - (2)]), &(yyval));
				  CHECK; }
    break;

  case 43:
#line 195 "shell.yacc"
    { VALUE val;
				  val = (yyvsp[(1) - (2)]);
				  binOp(&(yyval), &(yyvsp[(1) - (2)]), INCR, NULLVAL);
				  assign(&(yyvsp[(1) - (2)]), &(yyval));
				  CHECK;
				  (yyval) = val; }
    break;

  case 44:
#line 201 "shell.yacc"
    { VALUE val;
				  val = (yyvsp[(1) - (2)]);
				  binOp(&(yyval), &(yyvsp[(1) - (2)]), DECR, NULLVAL);
				  assign(&(yyvsp[(1) - (2)]), &(yyval));
				  CHECK;
				  (yyval) = val; }
    break;

  case 45:
#line 207 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), ADDA, &(yyvsp[(3) - (3)]));
				  assign(&(yyvsp[(1) - (3)]), &(yyval));
				  CHECK; }
    break;

  case 46:
#line 210 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), SUBA, &(yyvsp[(3) - (3)]));
				  assign(&(yyvsp[(1) - (3)]), &(yyval));
				  CHECK; }
    break;

  case 47:
#line 213 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), ANDA, &(yyvsp[(3) - (3)]));
				  assign(&(yyvsp[(1) - (3)]), &(yyval));
				  CHECK; }
    break;

  case 48:
#line 216 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), ORA, &(yyvsp[(3) - (3)]));
				  assign(&(yyvsp[(1) - (3)]), &(yyval));
				  CHECK; }
    break;

  case 49:
#line 219 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), MODA, &(yyvsp[(3) - (3)]));
				  assign(&(yyvsp[(1) - (3)]), &(yyval));
				  CHECK; }
    break;

  case 50:
#line 222 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), XORA, &(yyvsp[(3) - (3)]));
				  assign(&(yyvsp[(1) - (3)]), &(yyval));
				  CHECK; }
    break;

  case 51:
#line 225 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), MULA, &(yyvsp[(3) - (3)]));
				  assign(&(yyvsp[(1) - (3)]), &(yyval));
				  CHECK; }
    break;

  case 52:
#line 228 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), DIVA, &(yyvsp[(3) - (3)]));
				  assign(&(yyvsp[(1) - (3)]), &(yyval));
				  CHECK; }
    break;

  case 53:
#line 231 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), SHLA, &(yyvsp[(3) - (3)]));
				  assign(&(yyvsp[(1) - (3)]), &(yyval));
				  CHECK; }
    break;

  case 54:
#line 234 "shell.yacc"
    { binOp(&(yyval), &(yyvsp[(1) - (3)]), SHRA, &(yyvsp[(3) - (3)]));
				  assign(&(yyvsp[(1) - (3)]), &(yyval));
				  CHECK; }
    break;

  case 55:
#line 237 "shell.yacc"
    { assign(&(yyvsp[(1) - (3)]), &(yyvsp[(3) - (3)])); (yyval) = (yyvsp[(1) - (3)]); }
    break;

  case 56:
#line 239 "shell.yacc"
    { usymUnprocessed = TRUE; usymValue = (yyvsp[(1) - (1)]); }
    break;

  case 57:
#line 240 "shell.yacc"
    {
				  if ((yyvsp[(1) - (4)]).type != T_UNKNOWN) {
				    fprintf(stderr, "typecast of lhs not allowed.\n");
				    YYERROR;
				  }
				  else {
				    (yyval) = newSym((char*) (yyvsp[(1) - (4)]).value.rv, (yyvsp[(4) - (4)]).type);
				    CHECK;
				    assign(&(yyval), &(yyvsp[(4) - (4)]));
				    CHECK;
				  }
				  usymUnprocessed = FALSE;
				}
    break;

  case 58:
#line 256 "shell.yacc"
    { (yyval) = newArgList(); }
    break;

  case 60:
#line 260 "shell.yacc"
    { (yyval) = newArgList(); addArg(&(yyval), &(yyvsp[(1) - (1)]));
				  CHECK; }
    break;

  case 61:
#line 262 "shell.yacc"
    { addArg(&(yyvsp[(1) - (3)]), &(yyvsp[(3) - (3)]));
				  CHECK; }
    break;

  case 62:
#line 266 "shell.yacc"
    { (yyvsp[(2) - (3)]).side = RHS; (yyval) = (yyvsp[(2) - (3)]); }
    break;

  case 63:
#line 267 "shell.yacc"
    { (yyvsp[(2) - (5)]).side = FHS; (yyval) = (yyvsp[(2) - (5)]); }
    break;


/* Line 1267 of yacc.c.  */
#line 1972 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 270 "shell.yacc"


/*******************************************************************************
 * yystart - Called before the shell is started
 *
 * RETURNS: N/A
 ******************************************************************************/

void yystart(char *line)
{
  lexNewLine(line);
  semError = FALSE;
  usymUnprocessed = FALSE;
  nArgs = 0;
  spawnFlag = FALSE;
}

/*******************************************************************************
 * yyerror - Called by yacc whenever an error occurs
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void yyerror(char *str)
{
  fprintf(stderr, "error: %s\n", str);
}

/*******************************************************************************
 * printSym - Print symbol value
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void printSym(int val, char *prefix, char *suffix)
{
  void *value;
  SYMBOL_ID symId;
  char *name;

  /* If val not minus one */
  if (val != -1) {

    /* Find symbol by value and get name and value */
    if (symFindSymbol(sysSymTable, NULL, (void *) val,
		      SYM_MASK_NONE, SYM_MASK_NONE, &symId) == OK) {

      /* Get symbol name */
      symNameGet(symId, &name);

      /* Get symbol value */
      symValueGet(symId, &value);

      /* If value range resonable */
      if ((int) value != 0 && ( (val - (int) value) < 0x1000) ) {

        printf(prefix);

        if (val == (int) value)
          printf("%s", name);
        else
          printf("%s + 0x%x", name, val - (int) value);

        printf(suffix);

      } /* End if value resonable */

    } /* End if symbol found */

  } /* End if val not minus one */
}

/*******************************************************************************
 * checkLHSValue - Check if value is a left hand side value
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

LOCAL BOOL checkLHSValue(VALUE *pValue)
{

  /* If not left hand side */
  if (pValue->side != LHS) {

    fprintf(stderr, "invalid application of 'address of' operator.\n");
    SET_ERROR;
    return FALSE;

  } /* End if not left hand side */

  return TRUE;
}

/*******************************************************************************
 * checkRHSValue - Check if value is a right hand side value
 *
 * RETURNS: TRUE or FALSE
 ******************************************************************************/

LOCAL BOOL checkRHSValue(VALUE *pValue)
{

  /* If left hand side */
  if (pValue->side == LHS)
    return checkLHSValue(pValue);

  return TRUE;
}

/*******************************************************************************
 * getLHSValue - Get left hand side value
 *
 * RETURNS: VALUE*
 ******************************************************************************/

LOCAL int* getLHSValue(VALUE *pValue)
{
  if (checkLHSValue(pValue))
    return pValue->value.lv;

  return 0;
}

/*******************************************************************************
 * getRHSValue - Get right hand side value
 *
 * RETURNS: VALUE*
 ******************************************************************************/

LOCAL VALUE* getRHSValue(VALUE *pValue, VALUE *pResult)
{
  /* If it is a right hand side value */
  if (pValue->side == RHS) {

    *pResult = *pValue;

  } /* End if it is a right hand side value */

  else {

    /* Setup result specifiers */
    pResult->side = RHS;
    pResult->type = pValue->type;

    /* Select type */
    switch(pValue->type) {

      case T_BYTE:
        pResult->value.byte = *(char *) pValue->value.lv;
      break;

      case T_WORD:
        pResult->value.word = *(short *) pValue->value.lv;
      break;

      case T_INT:
        pResult->value.rv = *pValue->value.lv;
      break;

#ifdef INCLUDE_FLOAT

      case T_FLOAT: pResult->value.fp = *(float *) pValue->value.lv; break;
      case T_DOUBLE: pResult->value.dp = *(double *) pValue->value.lv; break;

#endif /* INCLUDE_FLOAT */

       default: fprintf(stderr, "getRHSValue: invalid rhs."); SET_ERROR; break;

    } /* End select type */

  }

  return pResult;
}

/*******************************************************************************
 * printLHSValue - Print left hand side value
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void printLHSValue(VALUE *pValue)
{
  int *lv;

  /* Get left hand side value */
  lv = getLHSValue(pValue);

  /* Print symbol */
  printSym((int) lv, "", " = ");

  printf("0x%x", (unsigned int) lv);
}

/*******************************************************************************
 * printRHSValue - Print right hand side value
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void printRHSValue(VALUE *pValue)
{
  VALUE value;
  int rv;
  BOOL intFlag;

  /* Get right hand side value */
  getRHSValue(pValue, &value);

  /* Select type */
  switch(pValue->type) {

    case T_BYTE:
      rv = (int) value.value.byte;
      intFlag = TRUE;
    break;

    case T_WORD:
      rv = (int) value.value.word;
      intFlag = TRUE;
    break;

    case T_INT:
      rv = value.value.rv;
      intFlag = TRUE;
    break;

#ifdef INCLUDE_FLOAT

    case T_FLOAT:
      printf("%g", value.value.fp);
      intFlag = FALSE;
    break;

    case T_DOUBLE:
      printf("%g", value.value.dp);
      intFlag = FALSE;
    break;

#endif /* INCLUDE_FLOAT */

    default:
	fprintf(stderr, "printRHSValue: bad type.\n");
	intFlag = FALSE;
	SET_ERROR;
    break;

  } /* End select type */

  /* Print integer value if needed */
  if (intFlag) {

    /* Print value */
    printf("%d = 0x%x", rv, rv);

    /* Print char code if it exists for value */
    if ( __isascii(rv) && __isprint(rv) )
      printf(" = '%c'", rv);

    /* Print symbol */
    printSym(rv, " = ", "");

  }
}

/*******************************************************************************
 * printValue - Print value
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void printValue(VALUE *pValue)
{
  /* If left hand side */
  if (pValue->side == LHS) {

    if ( checkLHSValue(pValue) && checkRHSValue(pValue) ) {

      printLHSValue(pValue);
      printf(": value = ");
      printRHSValue(pValue);
      printf("\n");

    }

    else {

      fprintf(stderr, "invalid lhs.\n");
      SET_ERROR;

    }

  } /* End if left hand side */

  /* Else if right hand side */
  else if (checkRHSValue(pValue)) {

    printf("value = ");
    printRHSValue(pValue);
    printf("\n");

  } /* End if right hand side */

  /* Else invalid */
  else {

    fprintf(stderr, "invalid rhs.\n");
    SET_ERROR;

  } /* End else invalid */
}

/*******************************************************************************
 * newString - Create a new string
 *
 * RETURNS: Integer pointer to string
 ******************************************************************************/

LOCAL int newString(char *str)
{
  char *newStr;
  int len;

  /* Get length */
  len = strlen(str);

  /* Allocate storage */
  newStr = malloc(len);
  if (str == NULL) {

    fprintf(stderr, "not enough memory for string.\n");
    SET_ERROR;

  }

  else {

    strcpy(newStr, str);

  }

  return (int) newStr;
}

/*******************************************************************************
 * newSym - Create a new symbol
 *
 * RETURNS: Symbol value
 ******************************************************************************/

LOCAL VALUE newSym(char *name, TYPE type)
{
  VALUE value;
  void *pAddr;

  pAddr = malloc(MAX_SYMBOL_SIZE);
  if (pAddr == NULL) {

    fprintf(stderr, "not enough memory for new symbol.\n");
    SET_ERROR;

  }

  else {

    if (symAdd(sysSymTable, name, pAddr,
	       (N_BSS | N_EXT), symGroupDefault) != OK) {

      free(pAddr);
      fprintf(stderr, "can't add '%s' to system symbol table - "
		      "error = 0x%x.\n", name, errnoGet());
      SET_ERROR;
    }

    printf("new symbol \"%s\" added to symbol table.\n", name);

    value.side = LHS;
    value.type = type;
    value.value.lv = (int *) pAddr;

  }

  return value;

}

/*******************************************************************************
 * convUnknown - Convert value to unknown type
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void convUnknown(VALUE *pValue)
{
  fprintf(stderr, "convUnknown: bad type.\n");

  SET_ERROR;
}

/*******************************************************************************
 * convByte - Convert value to byte type
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void convByte(VALUE *pValue)
{
  char value;

  /* If type is greater than byte */
  if (pValue->type > T_BYTE) {

    /* First convert type down to word */
    convWord(pValue);

    /* Get byte value from word value */
    value = (char) pValue->value.word;

    /* Set byte value */
    pValue->value.byte = value;

    /* Set type to byte */
    pValue->type = T_BYTE;

  } /* End if type is greater than byte */
}

/*******************************************************************************
 * convWord - Convert value to word type
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void convWord(VALUE *pValue)
{
  short value;

  /* If type is smaller than word */
  if (pValue->type < T_WORD) {

    /* Get word value from byte value */
    value = (short) pValue->value.byte;

    /* Set word value */
    pValue->value.word = value;

    /* Set type to word */
    pValue->type = T_WORD;

  } /* End if type is smaller than word */

  /* Else if type is greater than word */
  else if (pValue->type > T_WORD) {

    /* First convert type down to int */
    convInt(pValue);

    /* Get word value from int value */
    value = (short) pValue->value.rv;

    /* Set word value */
    pValue->value.word = value;

    /* Set type to word */
    pValue->type = T_WORD;

  } /* End else if type is greater than word */
}

/*******************************************************************************
 * convInt - Convert value to integer type
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void convInt(VALUE *pValue)
{
  int value;

  /* If type is smaller than int */
  if (pValue->type < T_INT) {

    /* First convert type up to word */
    convWord(pValue);

    /* Get int value from word value */
    value = (int) pValue->value.word;

    /* Set int value */
    pValue->value.rv = value;

    /* Set type to int */
    pValue->type = T_INT;

  } /* End if type is smaller than int */

  /* Else if type is greater than int */
  else if (pValue->type > T_INT) {

#ifdef INCLUDE_FLOAT

    /* First convert type down to float */
    convFloat(pValue);

    /* Get int value from float value */
    value = (int) pValue->value.fp;

    /* Set int value */
    pValue->value.rv = value;

    /* Set type to int */
    pValue->type = T_INT;

#endif /* INCLUDE_FLOAT */

  } /* End else if type is greater than int */
}

#ifdef INCLUDE_FLOAT

/*******************************************************************************
 * convFloat - Convert value to floating point type
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void convFloat(VALUE *pValue)
{
  float value;

  /* If type is smaller than float */
  if (pValue->type < T_FLOAT) {

    /* First convert type up to int */
    convInt(pValue);

    /* Get float value from int value */
    value = (float) pValue->value.rv;

    /* Set float value */
    pValue->value.fp = value;

    /* Set type to float */
    pValue->type = T_FLOAT;

  } /* End if type is smaller than float */

  /* Else if type is greater than float */
  else if (pValue->type > T_INT) {

    /* First convert type down to float */
    convDouble(pValue);

    /* Get float value from double value */
    value = (float) pValue->value.dp;

    /* Set float value */
    pValue->value.fp = value;

    /* Set type to float */
    pValue->type = T_FLOAT;

  } /* End else if type is greater than float */
}

/*******************************************************************************
 * convDouble - Convert value to double precision floating point type
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void convFloat(VALUE *pValue)
{
  double value;

  /* If type is smaller than double */
  if (pValue->type < T_DOUBLE) {

    /* First convert type up to float */
    convFloat(pValue);

    /* Get double value from float value */
    value = (double) pValue->value.fp;

    /* Set double value */
    pValue->value.dp = value;

    /* Set type to double */
    pValue->type = T_DOUBLE;

  } /* End if type is smaller than double */
}

#endif /* INCLUDE_FLOAT */

LOCAL VOIDFUNCPTR convType[] = {
  convUnknown,
  convByte,
  convWord,

#ifdef INCLUDE_FLOAT

  convInt,
  convFloat,
  convDouble

#else

  convInt

#endif /* INCLUDE_FLOAT */
};

/*******************************************************************************
 * typeConvert - Convert value to specified data type
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void typeConvert(VALUE *pValue, TYPE type, SIDE side)
{

  /* If request convert to right hand side value */
  if (side == FHS) {

    pValue->side = RHS;
    pValue->type = type;

  } /* End if request convert to right hand side value */

  /* Else if request is right hand sided */
  else if (side == RHS) {

    /* Convert if needed */
    if (pValue->side == LHS)
      pValue->type = type;
    else
      (* convType[type]) (pValue);

  } /* End else if request is right hand sided */

  /* Else if value is left hand sided */
  else if (pValue->side == LHS) {

    pValue->type = type;

  } /* End else if value is left hand sided */

  /* Else invalid */
  else {

    fprintf(stderr, "typeConvert: bad type.\n");
    SET_ERROR;

  } /* End else invalid */
}

/*******************************************************************************
 * assign - Assign a value
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void assign(VALUE *pLHSValue, VALUE *pRHSValue)
{
  VALUE value;

  /* If value sideness okay */
  if ( checkLHSValue(pLHSValue) && checkRHSValue(pRHSValue) ) {

    /* Get right hand side value */
    getRHSValue(pRHSValue, &value);

    /* Convert to correct type */
    pLHSValue->type = pRHSValue->type;
    typeConvert(&value, pLHSValue->type, RHS);

    /* Select type */
    switch(pLHSValue->type) {

      case T_BYTE:
        *(char *) getLHSValue(pLHSValue) = value.value.byte;
      break;

      case T_WORD:
        *(short *) getLHSValue(pLHSValue) = value.value.word;
      break;

      case T_INT:
        *(int *) getLHSValue(pLHSValue) = value.value.rv;
      break;

#ifdef INCLUDE_FLOAT

      case T_FLOAT:
        *(float *) getLHSValue(pLHSValue) = value.value.fp;
      break;

      case T_FLOAT:
        *(double *) getLHSValue(pLHSValue) = value.value.dp;
      break;

#endif /* INCLUDE_FLOAT */

      default:
        fprintf(stderr, "bad assignment.\n");
        SET_ERROR;
        break;

    } /* End select type */

  } /* End if value sideness okay */

  /* Else value sideness not okay */
  else {

    fprintf(stderr, "bad assignment.\n");
    SET_ERROR;
    return;

  } /* End else value sideness not okay */
}

/*******************************************************************************
 * setLHSValue - Set right hand side value
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void setLHSValue(VALUE *pValue1, VALUE *pValue2)
{
  /* If value2 not left hand sided */
  if (pValue2->side == LHS) {

    fprintf(stderr, "setLHSValue: invalid lhs.\n");
    SET_ERROR;

  } /* End if value2 not left hand sided */

  /* If type not int */
  if (pValue2->type != T_INT) {

    fprintf(stderr, "setLHSValue: type confilict.\n");
    SET_ERROR;

  } /* End if type not int */

  /* Setup struct */
  pValue1->side = LHS;
  pValue1->type = pValue2->type;
  pValue1->value.lv = (int *) pValue2->value.rv;
}

/*******************************************************************************
 * setRHSValue - Set right hand side value
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void setRHSValue(VALUE *pValue1, VALUE *pValue2)
{
  /* Setup value1 */
  pValue1->side = RHS;
  pValue1->type = pValue2->type;

  /* Select type */
  switch(pValue1->type) {

    case T_BYTE:
      if (pValue1->side == LHS)
        pValue1->value.byte = *(char *) pValue2->value.lv;
      else
        pValue1->value.byte = pValue2->value.byte;
    break;

    case T_WORD:
      if (pValue1->side == LHS)
        pValue1->value.word = *(short *) pValue2->value.lv;
      else
        pValue1->value.word = pValue2->value.word;
    break;

    case T_INT:
      if (pValue1->side == LHS)
        pValue1->value.rv = *pValue2->value.lv;
      else
        pValue1->value.rv = pValue2->value.rv;
    break;

#ifdef INCLUDE_FLOAT

    case T_FLOAT:
      if (pValue1->side == LHS)
        pValue1->value.fp = *(float *) pValue2->value.lv;
      else
        pValue1->value.fp = pValue2->value.fp;
    break;

    case T_DOUBLE:
      if (pValue1->side == LHS)
        pValue1->value.dp = *(double *) pValue2->value.lv;
      else
        pValue1->value.dp = pValue2->value.dp;
    break;

#endif /* INCLUDE_FLOAT */

    default:
      fprintf(stderr, "setRHSValue: bad type.\n");
      SET_ERROR;
      break;

  } /* End select type */
}

typedef struct {
  VALUE (*eval) (VALUE *pValue1, int op, VALUE *pValue2);
} EVAL_TYPE;

LOCAL EVAL_TYPE evalType[] = {
  { evalUnknown },
  { evalByte },
  { evalWord },

#ifdef INCLUDE_FLOAT

  { evalInt },
  { evalFloat },
  { evalDouble }

#else

  { evalInt }

#endif /* INCLUDE_FLOAT */

};

/*******************************************************************************
 * evalExp - Evaluate expression
 *
 * RETURNS: Resulting value
 ******************************************************************************/

LOCAL VALUE evalExp(VALUE *pValue1, int op, VALUE *pValue2)
{
  VALUE *pVal1, *pVal2;

  /* Initialize locals */
  pVal1 = pValue1;
  pVal2 = pValue2;

  /* If unary expression */
  if (pValue2 == NULLVAL) {

    pValue2 = pValue1;
    pVal2 = pValue1;

  } /* End if unary expression */

  /* Make both value in the expression have the same type */
  if ( pVal1->type > pVal2->type)
    typeConvert(pVal2, pVal1->type, pVal1->side);
  else
    typeConvert(pVal1, pVal2->type, pVal2->side);

  return (* evalType[pValue1->type].eval) (pVal1, op, pVal2);
}

/*******************************************************************************
 * binOp - Do binary operator
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void binOp(VALUE *pResult, VALUE *pValue1, int op, VALUE *pValue2)
{
  VALUE tmpResult, tmpValue1, tmpValue2;
  VALUE result;

  getRHSValue(pValue1, &tmpValue1);

  /* If unary expression */
  if (pValue2 == NULLVAL) {

    tmpResult = evalExp(&tmpValue1, op, NULLVAL);

  } /* End if unary expression */

  /* Else binary expression */
  else {

    getRHSValue(pValue2, &tmpValue2);
    tmpResult = evalExp(&tmpValue1, op, &tmpValue2);

  } /* End else binary expression */

  /* Set right hand side value value */
  setRHSValue(pResult, &tmpResult);
}

/*******************************************************************************
 * evalUnknown - Evaluate unknown data type
 *
 * RETURNS: Resulting value
 ******************************************************************************/

LOCAL VALUE evalUnknown(VALUE *pValue1, int op, VALUE *pValue2)
{
  fprintf(stderr, "evalUnknown: bad evaluation.\n");

  SET_ERROR;

  return *pValue1;
}

/*******************************************************************************
 * evalByte - Evaluate byte type value
 *
 * RETURNS: Resulting value
 ******************************************************************************/

LOCAL VALUE evalByte(VALUE *pValue1, int op, VALUE *pValue2)
{
  VALUE *pVal1, *pVal2;
  VALUE result;

  /* Initialize locals */
  pVal1 = pValue1;
  pVal2 = pValue2;

  /* Convert both to int types */
  typeConvert(pVal1, T_INT, RHS);
  typeConvert(pVal2, T_INT, RHS);

  /* Evaluate int operator */
  result = evalInt(pVal1, op, pVal2);

  /* Convert result to word type */
  typeConvert(&result, T_BYTE, RHS);

  return result;
}

/*******************************************************************************
 * evalWord - Evaluate word type value
 *
 * RETURNS: Resulting value
 ******************************************************************************/

LOCAL VALUE evalWord(VALUE *pValue1, int op, VALUE *pValue2)
{
  VALUE *pVal1, *pVal2;
  VALUE result;

  /* Initialize locals */
  pVal1 = pValue1;
  pVal2 = pValue2;

  /* Convert both to int types */
  typeConvert(pVal1, T_INT, RHS);
  typeConvert(pVal2, T_INT, RHS);

  /* Evaluate int operator */
  result = evalInt(pVal1, op, pVal2);

  /* Convert result to word type */
  typeConvert(&result, T_WORD, RHS);

  return result;
}

/*******************************************************************************
 * evalInt - Evaluate int type value
 *
 * RETURNS: Resulting value
 ******************************************************************************/

LOCAL VALUE evalInt(VALUE *pValue1, int op, VALUE *pValue2)
{
  VALUE result;
  int i1, i2;
  int rv;

  /* Get locals */
  i1 = pValue1->value.rv;
  i2 = pValue2->value.rv;

  /* Select operator */
  switch(op) {

    case '+':
    case ADDA:
      rv = i1 + i2;
    break;

    case '-':
    case SUBA:
      rv = i1 - i2;
    break;

    case '*':
    case MULA:
      rv = i1 * i2;
    break;

    case '/':
    case DIVA:
      rv = i1 / i2;
    break;

    case '%':
    case MODA:
      rv = i1 % i2;
    break;

    case '&':
    case ANDA:
      rv = i1 & i2;
    break;

    case '|':
    case ORA:
      rv = i1 | i2;
    break;

    case '^':
    case XORA:
      rv = i1 ^ i2;
    break;

    case ROT_LEFT:
    case SHLA:
      rv = i1 << i2;
    break;

    case ROT_RIGHT:
    case SHRA:
      rv = i1 >> i2;
    break;

    case EQ:
      rv = i1 == i2;
    break;

    case NE:
      rv = i1 != i2;
    break;

    case LE:
      rv = i1 <= i2;
    break;

    case GE:
      rv = i1 >= i2;
    break;

    case AND:
      rv = i1 && i2;
    break;

    case OR:
      rv = i1 || i2;
    break;

    case '<':
      rv = i1 < i2;
    break;

    case '>':
      rv = i1 > i2;
    break;

    case INCR:
      rv = ++i1;
    break;

    case DECR:
      rv = --i1;
    break;

    case UMINUS:
      rv = -i1;
    break;

    case '~':
      rv = ~i1;
    break;

    case '!':
      rv = !i1;
    break;

    default:
      rv = 0;
      fprintf(stderr, "operads have incompatible types.\n");
      SET_ERROR;
    break;

  } /* End select operator */

  /* Setup fieds in value struct */
  result.side = RHS;
  result.type = pValue1->type;
  result.value.rv = rv;

  return result;
}

#ifdef INCLUDE_FLOAT

/*******************************************************************************
 * evalFloat - Evaluate float type value
 *
 * RETURNS: Resulting value
 ******************************************************************************/

LOCAL VALUE evalFloat(VALUE *pValue1, int op, VALUE *pValue2)
{
  return evalDouble(pValue1, op, pValue2);
}

/*******************************************************************************
 * evalDouble - Evaluate double type value
 *
 * RETURNS: Resulting value
 ******************************************************************************/

LOCAL VALUE evalDouble(VALUE *pValue1, int op, VALUE *pValue2)
{
  VALUE result;

  /* Not implemented yet */
  return result;
}

#endif /* INCLUDE_FLOAT */

/*******************************************************************************
 * newArgList - Create function agument list
 *
 * RETURNS: Value of first argument in list
 ******************************************************************************/

LOCAL VALUE newArgList(void)
{
  VALUE value;

  /* Setup value struct */
  value.side = RHS;
  value.type = T_INT;
  value.value.rv = nArgs;

  return value;
}

/*******************************************************************************
 * addArg - Add argument to arglist
 *
 * RETURNS: N/A
 ******************************************************************************/

LOCAL void addArg(VALUE *pArgList, VALUE *pNewArg)
{
  SYMBOL_ID symId;
  SYM_TYPE sType;
  VALUE value;
  int rv;

  if ( nArgs == MAX_SHELL_ARGS ||
       (nArgs - pArgList->value.rv) == MAX_FUNC_ARGS) {

    fprintf(stderr, "too many arguments to functions.\n");
    SET_ERROR;
    return;

  }

  /* Get new argument right hand side value */
  getRHSValue(pNewArg, &value);

  /* If right hand value is okay */
  if ( checkRHSValue(&value) ) {

    /* Select type */
    switch(value.type) {

      case T_BYTE:
        rv = (int) value.value.byte;
      break;

      case T_WORD:
        rv = (int) value.value.word;
      break;

      case T_INT:
        rv = value.value.rv;
      break;

      default:
	rv = 0;
	fprintf(stderr, "addArg: bad type.\n");
	SET_ERROR;
      break;

    } /* End select type */

    /* Store argument and advance */
    argStack[nArgs++] = (ARG) rv;

  } /* End if right hand value is okay */
}

/*******************************************************************************
 * funcCall - Call function
 *
 * RETURNS: Resulting value
 ******************************************************************************/

LOCAL VALUE funcCall(VALUE *pValue, VALUE *pArgList)
{
  VALUE value;
  FUNCPTR pFunc;
  static int funcErrno;
  ARG args[MAX_FUNC_ARGS];
  int i, argNum;
  int rv;
  int oldInFd, oldOutFd;

  /* Store old file descriptos */
  oldInFd = ioGlobalStdGet(STDIN_FILENO);
  oldOutFd = ioGlobalStdGet(STDOUT_FILENO);

  /* Get function to call */
  if (pValue->side == LHS)
    pFunc = (FUNCPTR) getLHSValue(pValue);
  else
    pFunc = (FUNCPTR) pValue->value.rv;

  /* For all arguments on arglist */
  for (argNum = pArgList->value.rv, i = 0; i < MAX_FUNC_ARGS; argNum++, i++) {

    if (argNum < nArgs)
      args[i] = argStack[argNum];
    else
      args[i] = (ARG) 0;

  } /* End for all arguments on arglist */

  /* Get redirections */
  if (redirInFd >= 0)
    ioGlobalStdSet(STDIN_FILENO, redirInFd);

  if (redirOutFd >= 0)
    ioGlobalStdSet(STDOUT_FILENO, redirOutFd);

  /* Setup return value struct */
  value.side = RHS;
  value.type = pValue->type;

  /* Select type */
  switch(pValue->type) {

    case T_BYTE:
    case T_WORD:
    case T_INT:

      /* Call function */
      rv = ( *pFunc) (args[0], args[1], args[2], args[3], args[4], args[5],
		      args[6], args[7], args[8], args[9], args[10], args[11]);

      /* Select type */
      switch(pValue->type) {

        case T_BYTE:
	  value.value.byte = (char) rv;
	break;

        case T_WORD:
	  value.value.word = (short) rv;
	break;

        case T_INT:
	  value.value.rv = rv;
	break;

	default:
	break;

      } /* End select type */

    break;

    default:
      fprintf(stderr, "funcCall: bad function type.\n");
      SET_ERROR;
    break;

  } /* End select type */

  /* Restore default filedescriptors */
  ioGlobalStdSet(STDIN_FILENO, oldInFd);
  ioGlobalStdSet(STDOUT_FILENO, oldOutFd);

  /* Store function errno */
  funcErrno = errnoGet();

  /* Restore args stack */
  nArgs = pArgList->value.rv;

  return value;
}

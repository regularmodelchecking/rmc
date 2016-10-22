/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_K_VAR = 258,
     TOKEN_K_TRUE = 259,
     TOKEN_K_FALSE = 260,
     TOKEN_K_LET = 261,
     TOKEN_NUMBER = 262,
     TOKEN_IDENTIFIER = 263,
     TOKEN_LITERAL = 264,
     TOKEN_K_COMMA = 265,
     TOKEN_K_DOLLAR = 266,
     TOKEN_K_COLON = 267,
     TOKEN_K_SEMICOLON = 268,
     TOKEN_K_EXISTS_SO = 269,
     TOKEN_K_EXISTS_FO = 270,
     TOKEN_K_FORALL_SO = 271,
     TOKEN_K_FORALL_FO = 272,
     TOKEN_K_IFF = 273,
     TOKEN_K_IMPLIES = 274,
     TOKEN_K_OR = 275,
     TOKEN_K_AND = 276,
     TOKEN_K_GT = 277,
     TOKEN_K_GTEQUALS = 278,
     TOKEN_K_LT = 279,
     TOKEN_K_LTEQUALS = 280,
     TOKEN_K_SUBSETEQ = 281,
     TOKEN_K_NOT_IN = 282,
     TOKEN_K_IN = 283,
     TOKEN_K_EQUALS = 284,
     TOKEN_K_PLUS = 285,
     TOKEN_K_EVENTUALLY = 286,
     TOKEN_K_ALWAYS = 287,
     TOKEN_K_NOT = 288,
     TOKEN_K_PRIME = 289,
     TOKEN_K_GUARD = 290,
     TOKEN_K_RIGHT_BRACE = 291,
     TOKEN_K_LEFT_BRACE = 292,
     TOKEN_K_RIGHT_PARENTHESIS = 293,
     TOKEN_K_LEFT_PARENTHESIS = 294,
     TOKEN_K_RIGHT_BRACKET = 295,
     TOKEN_K_LEFT_BRACKET = 296
   };
#endif
#define TOKEN_K_VAR 258
#define TOKEN_K_TRUE 259
#define TOKEN_K_FALSE 260
#define TOKEN_K_LET 261
#define TOKEN_NUMBER 262
#define TOKEN_IDENTIFIER 263
#define TOKEN_LITERAL 264
#define TOKEN_K_COMMA 265
#define TOKEN_K_DOLLAR 266
#define TOKEN_K_COLON 267
#define TOKEN_K_SEMICOLON 268
#define TOKEN_K_EXISTS_SO 269
#define TOKEN_K_EXISTS_FO 270
#define TOKEN_K_FORALL_SO 271
#define TOKEN_K_FORALL_FO 272
#define TOKEN_K_IFF 273
#define TOKEN_K_IMPLIES 274
#define TOKEN_K_OR 275
#define TOKEN_K_AND 276
#define TOKEN_K_GT 277
#define TOKEN_K_GTEQUALS 278
#define TOKEN_K_LT 279
#define TOKEN_K_LTEQUALS 280
#define TOKEN_K_SUBSETEQ 281
#define TOKEN_K_NOT_IN 282
#define TOKEN_K_IN 283
#define TOKEN_K_EQUALS 284
#define TOKEN_K_PLUS 285
#define TOKEN_K_EVENTUALLY 286
#define TOKEN_K_ALWAYS 287
#define TOKEN_K_NOT 288
#define TOKEN_K_PRIME 289
#define TOKEN_K_GUARD 290
#define TOKEN_K_RIGHT_BRACE 291
#define TOKEN_K_LEFT_BRACE 292
#define TOKEN_K_RIGHT_PARENTHESIS 293
#define TOKEN_K_LEFT_PARENTHESIS 294
#define TOKEN_K_RIGHT_BRACKET 295
#define TOKEN_K_LEFT_BRACKET 296




/* Copy the first part of user declarations.  */
#line 3 "parser.yy"

#include "parser-types.h"
#include <sstream>
#include <iostream>
#include <assert.h>
#include <FlexLexer.h>
extern int linecount;

namespace rmc
{
namespace formula
{
namespace parser
{

#define YYDEBUG 1
#define YYPARSE_PARAM lexer
#define YYLEX_PARAM lexer

int yyerror(const char *s);

extern Formula* SyntaxTree;



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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 31 "parser.yy"
typedef union YYSTYPE {
	unsigned int num;
	std::string* str;
	Name* name;
	NodeList* nodeList;
	Formula* formula;
	Declaration* decl;
	TimeIndex* time_index;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 193 "parser.cc"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 41 "parser.yy"

extern int yylex(yystype *, void*);


/* Line 214 of yacc.c.  */
#line 208 "parser.cc"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
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
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  31
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   127

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  14
/* YYNRULES -- Number of rules. */
#define YYNRULES  51
/* YYNRULES -- Number of states. */
#define YYNSTATES  105

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   296

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,     6,     8,    10,    15,    17,    21,
      25,    31,    35,    39,    43,    47,    51,    55,    59,    65,
      73,    85,    87,    89,    93,    97,   101,   105,   108,   111,
     114,   118,   122,   126,   130,   134,   139,   142,   144,   148,
     155,   162,   166,   168,   170,   172,   176,   178,   179,   184,
     186,   188
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      43,     0,    -1,    45,    -1,    -1,    34,    -1,    46,    -1,
      35,    39,    45,    38,    -1,    52,    -1,    53,    28,    53,
      -1,    53,    26,    53,    -1,    53,    29,    53,    30,    54,
      -1,    53,    29,    54,    -1,    53,    29,    11,    -1,    53,
      29,    53,    -1,    53,    24,    53,    -1,    53,    25,    53,
      -1,    53,    22,    53,    -1,    53,    23,    53,    -1,    53,
      44,    41,    53,    40,    -1,    53,    44,    41,    53,    40,
      29,    55,    -1,    53,    44,    41,    53,    40,    29,    53,
      44,    41,    53,    40,    -1,     4,    -1,     5,    -1,    45,
      21,    45,    -1,    45,    20,    45,    -1,    45,    19,    45,
      -1,    45,    18,    45,    -1,    33,    45,    -1,    32,    45,
      -1,    31,    45,    -1,    15,    53,    45,    -1,    17,    53,
      45,    -1,    14,    53,    45,    -1,    16,    53,    45,    -1,
      39,    45,    38,    -1,     6,    47,    28,    45,    -1,    48,
      47,    -1,    48,    -1,    53,    29,    45,    -1,    53,    39,
      51,    38,    29,    45,    -1,    53,    12,    12,    37,    49,
      36,    -1,    50,    10,    49,    -1,    50,    -1,    55,    -1,
      54,    -1,    53,    10,    51,    -1,    53,    -1,    -1,    53,
      39,    51,    38,    -1,     8,    -1,     7,    -1,     9,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    79,    79,    83,    84,    88,    89,    91,    92,    94,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   106,
     108,   118,   119,   120,   122,   124,   125,   126,   128,   130,
     132,   135,   138,   141,   144,   148,   153,   154,   158,   160,
     163,   168,   169,   173,   174,   179,   180,   181,   185,   196,
     200,   204
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOKEN_K_VAR", "TOKEN_K_TRUE",
  "TOKEN_K_FALSE", "TOKEN_K_LET", "TOKEN_NUMBER", "TOKEN_IDENTIFIER",
  "TOKEN_LITERAL", "TOKEN_K_COMMA", "TOKEN_K_DOLLAR", "TOKEN_K_COLON",
  "TOKEN_K_SEMICOLON", "TOKEN_K_EXISTS_SO", "TOKEN_K_EXISTS_FO",
  "TOKEN_K_FORALL_SO", "TOKEN_K_FORALL_FO", "TOKEN_K_IFF",
  "TOKEN_K_IMPLIES", "TOKEN_K_OR", "TOKEN_K_AND", "TOKEN_K_GT",
  "TOKEN_K_GTEQUALS", "TOKEN_K_LT", "TOKEN_K_LTEQUALS", "TOKEN_K_SUBSETEQ",
  "TOKEN_K_NOT_IN", "TOKEN_K_IN", "TOKEN_K_EQUALS", "TOKEN_K_PLUS",
  "TOKEN_K_EVENTUALLY", "TOKEN_K_ALWAYS", "TOKEN_K_NOT", "TOKEN_K_PRIME",
  "TOKEN_K_GUARD", "TOKEN_K_RIGHT_BRACE", "TOKEN_K_LEFT_BRACE",
  "TOKEN_K_RIGHT_PARENTHESIS", "TOKEN_K_LEFT_PARENTHESIS",
  "TOKEN_K_RIGHT_BRACKET", "TOKEN_K_LEFT_BRACKET", "$accept", "start",
  "timeindex", "formula", "formula_with_declarations", "declarations",
  "declaration", "values", "value", "args", "formula_function_app",
  "token_identifier", "token_number", "token_literal", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    42,    43,    44,    44,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    45,    46,    47,    47,    48,    48,
      48,    49,    49,    50,    50,    51,    51,    51,    52,    53,
      54,    55
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     1,     1,     4,     1,     3,     3,
       5,     3,     3,     3,     3,     3,     3,     3,     5,     7,
      11,     1,     1,     3,     3,     3,     3,     2,     2,     2,
       3,     3,     3,     3,     3,     4,     2,     1,     3,     6,
       6,     3,     1,     1,     1,     3,     1,     0,     4,     1,
       1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    21,    22,     0,    49,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     2,     5,     7,     3,     0,
      37,     0,     0,     0,     0,     0,    29,    28,    27,     0,
       0,     1,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     4,    47,     0,     0,    36,     0,     0,
      47,    32,    30,    33,    31,     0,    34,    26,    25,    24,
      23,    16,    17,    14,    15,     9,     8,    50,    12,    13,
      11,     0,    46,     0,    35,     0,    38,     0,     6,     0,
      48,    47,     0,     0,     0,    10,    45,    18,    51,     0,
      42,    44,    43,     0,     0,    40,     0,    39,     3,    19,
      41,     0,     0,     0,    20
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,    14,    45,    15,    16,    19,    20,    89,    90,    71,
      17,    18,    91,    92
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -50
static const yysigned_char yypact[] =
{
      69,   -50,   -50,     8,   -50,     8,     8,     8,     8,    69,
      69,    69,   -20,    69,    25,     2,   -50,   -50,    88,    -1,
       8,     1,    69,    69,    69,    69,   -50,   -50,   -50,    69,
      -9,   -50,    69,    69,    69,    69,     8,     8,     8,     8,
       8,     8,     7,   -50,     8,   -13,    69,   -50,    19,    69,
       8,     2,     2,     2,     2,    31,   -50,     2,     2,    27,
     -50,   -50,   -50,   -50,   -50,   -50,   -50,   -50,   -50,    23,
     -50,    16,    49,     8,   -50,    24,     2,    22,   -50,    56,
     -50,     8,    32,    17,    35,   -50,   -50,    42,   -50,    40,
      70,   -50,   -50,    69,    -2,   -50,    17,     2,    47,   -50,
     -50,    46,     8,    48,   -50
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -50,   -50,    -8,    33,   -50,    72,   -50,    -7,   -50,   -49,
     -50,    -3,   -34,     0
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      21,    77,    22,    23,    24,    25,     4,    88,    70,    32,
      33,    34,    35,    48,    67,     4,     4,    21,    68,    29,
      32,    33,    34,    35,    67,    31,    88,    46,    73,    56,
      49,    75,    86,    61,    62,    63,    64,    65,    66,    69,
      50,    72,    26,    27,    28,    85,    30,    72,    35,    32,
      33,    34,    35,    79,    80,    51,    52,    53,    54,    81,
      84,    83,    55,    67,    93,    57,    58,    59,    60,    78,
      82,    94,    87,     1,     2,     3,    95,     4,    72,    74,
      96,    43,    76,     5,     6,     7,     8,   102,   104,   100,
     101,    98,    47,     0,    99,     0,     0,     0,     0,   103,
       9,    10,    11,     0,    12,     0,     0,     0,    13,     0,
      36,    37,    38,    39,    40,     0,    41,    42,     0,     0,
       0,     0,    43,     0,     0,     0,    97,    44
};

static const yysigned_char yycheck[] =
{
       3,    50,     5,     6,     7,     8,     8,     9,    42,    18,
      19,    20,    21,    12,     7,     8,     8,    20,    11,    39,
      18,    19,    20,    21,     7,     0,     9,    28,    41,    38,
      29,    12,    81,    36,    37,    38,    39,    40,    41,    42,
      39,    44,     9,    10,    11,    79,    13,    50,    21,    18,
      19,    20,    21,    30,    38,    22,    23,    24,    25,    10,
      38,    37,    29,     7,    29,    32,    33,    34,    35,    38,
      73,    29,    40,     4,     5,     6,    36,     8,    81,    46,
      10,    34,    49,    14,    15,    16,    17,    41,    40,    96,
      98,    94,    20,    -1,    94,    -1,    -1,    -1,    -1,   102,
      31,    32,    33,    -1,    35,    -1,    -1,    -1,    39,    -1,
      22,    23,    24,    25,    26,    -1,    28,    29,    -1,    -1,
      -1,    -1,    34,    -1,    -1,    -1,    93,    39
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     4,     5,     6,     8,    14,    15,    16,    17,    31,
      32,    33,    35,    39,    43,    45,    46,    52,    53,    47,
      48,    53,    53,    53,    53,    53,    45,    45,    45,    39,
      45,     0,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    28,    29,    34,    39,    44,    28,    47,    12,    29,
      39,    45,    45,    45,    45,    45,    38,    45,    45,    45,
      45,    53,    53,    53,    53,    53,    53,     7,    11,    53,
      54,    51,    53,    41,    45,    12,    45,    51,    38,    30,
      38,    10,    53,    37,    38,    54,    51,    40,     9,    49,
      50,    54,    55,    29,    29,    36,    10,    45,    53,    55,
      49,    44,    41,    53,    40
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

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
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
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
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
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

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
        case 2:
#line 79 "parser.yy"
    { SyntaxTree = yyvsp[0].formula; }
    break;

  case 3:
#line 83 "parser.yy"
    { yyval.time_index = new TimeIndex(0); }
    break;

  case 4:
#line 84 "parser.yy"
    { yyval.time_index = new TimeIndex(1); }
    break;

  case 6:
#line 90 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new OpGuard(), yyvsp[-1].formula); }
    break;

  case 8:
#line 92 "parser.yy"
    { yyval.formula = new
  FormulaOpApplication(new OpMembership(), (yyvsp[-2].name), (yyvsp[0].name)); }
    break;

  case 9:
#line 94 "parser.yy"
    { yyval.formula = new
FormulaOpApplication(new OpSubset(), (yyvsp[-2].name), (yyvsp[0].name)); }
    break;

  case 10:
#line 96 "parser.yy"
    { yyval.formula = new FormulaFunctionApp(new Name("+"), (yyvsp[-4].name), (yyvsp[-2].name), new FormulaInteger(yyvsp[0].num)); }
    break;

  case 11:
#line 97 "parser.yy"
    { assert(yyvsp[0].num == 0); yyval.formula = new FormulaFunctionApp(new Name("0"), yyvsp[-2].name); }
    break;

  case 12:
#line 98 "parser.yy"
    { yyval.formula = new FormulaFunctionApp(new Name("$"), yyvsp[-2].name); }
    break;

  case 13:
#line 99 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new OpEqual(), (yyvsp[-2].name), (yyvsp[0].name)); }
    break;

  case 14:
#line 100 "parser.yy"
    { yyval.formula = new FormulaFunctionApp(new Name("<"), (yyvsp[-2].name), (yyvsp[0].name)); }
    break;

  case 15:
#line 101 "parser.yy"
    { yyval.formula = new FormulaFunctionApp(new Name("<="), (yyvsp[-2].name), (yyvsp[0].name)); }
    break;

  case 16:
#line 102 "parser.yy"
    { yyval.formula = new FormulaFunctionApp(new Name(">"), (yyvsp[-2].name), (yyvsp[0].name)); }
    break;

  case 17:
#line 103 "parser.yy"
    { yyval.formula = new FormulaFunctionApp(new Name(">="), (yyvsp[-2].name), (yyvsp[0].name)); }
    break;

  case 18:
#line 105 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new OpIndexTrue(), yyvsp[-4].name, yyvsp[-1].name, yyvsp[-3].time_index); }
    break;

  case 19:
#line 107 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new OpIndexValue(), yyvsp[-6].name, yyvsp[-3].name, yyvsp[-5].time_index, yyvsp[0].name); }
    break;

  case 20:
#line 110 "parser.yy"
    { NodeList* args = new NodeList();
 args->push_back(yyvsp[-10].name);
 args->push_back(yyvsp[-7].name);
 args->push_back(yyvsp[-9].time_index);
 args->push_back(yyvsp[-4].name);
 args->push_back(yyvsp[-1].name);
 args->push_back(yyvsp[-3].time_index);
 yyval.formula = new FormulaOpApplication(new OpIndexIndex(), args); }
    break;

  case 21:
#line 118 "parser.yy"
    { yyval.formula = new FormulaTrue(); }
    break;

  case 22:
#line 119 "parser.yy"
    { yyval.formula = new FormulaFalse(); }
    break;

  case 23:
#line 120 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new
OpAnd(), yyvsp[-2].formula, yyvsp[0].formula); }
    break;

  case 24:
#line 122 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new
OpOr(), yyvsp[-2].formula, yyvsp[0].formula); }
    break;

  case 25:
#line 124 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new OpImplies(),yyvsp[-2].formula, yyvsp[0].formula); }
    break;

  case 26:
#line 125 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new OpIff(), yyvsp[-2].formula,yyvsp[0].formula); }
    break;

  case 27:
#line 126 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new OpNot(),
yyvsp[0].formula); }
    break;

  case 28:
#line 128 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new
OpAlways(), yyvsp[0].formula); }
    break;

  case 29:
#line 130 "parser.yy"
    { yyval.formula = new FormulaOpApplication(new
OpEventually(), yyvsp[0].formula); }
    break;

  case 30:
#line 132 "parser.yy"
    { yyval.formula = new
FormulaQuantification(new QuantifierFOExists(), (yyvsp[-1].name),
yyvsp[0].formula); }
    break;

  case 31:
#line 135 "parser.yy"
    { yyval.formula = new
FormulaQuantification(new QuantifierFOForall(), (yyvsp[-1].name),
yyvsp[0].formula); }
    break;

  case 32:
#line 138 "parser.yy"
    { yyval.formula = new
FormulaQuantification(new QuantifierSOExists(), (yyvsp[-1].name),
yyvsp[0].formula); }
    break;

  case 33:
#line 141 "parser.yy"
    { yyval.formula = new
FormulaQuantification(new QuantifierSOForall(), (yyvsp[-1].name),
yyvsp[0].formula); }
    break;

  case 34:
#line 144 "parser.yy"
    {
yyval.formula = yyvsp[-1].formula; }
    break;

  case 35:
#line 148 "parser.yy"
    { yyval.formula = new
  FormulaWithDeclarations(yyvsp[-2].nodeList, yyvsp[0].formula); }
    break;

  case 36:
#line 153 "parser.yy"
    { if (yyvsp[-1].decl) yyvsp[0].nodeList->push_front(yyvsp[-1].decl); yyval.nodeList = yyvsp[0].nodeList; }
    break;

  case 37:
#line 154 "parser.yy"
    { yyval.nodeList = new NodeList(); if (yyvsp[0].decl) yyval.nodeList->push_front(yyvsp[0].decl); }
    break;

  case 38:
#line 158 "parser.yy"
    { yyval.decl = new DeclFunction(yyvsp[-2].name,
  new NodeList(), yyvsp[0].formula); }
    break;

  case 39:
#line 161 "parser.yy"
    { yyval.decl = new
  DeclFunction(yyvsp[-5].name, yyvsp[-3].nodeList, yyvsp[0].formula); }
    break;

  case 40:
#line 164 "parser.yy"
    { yyval.decl = new DeclType(yyvsp[-5].name, yyvsp[-1].nodeList); }
    break;

  case 41:
#line 168 "parser.yy"
    { if (yyvsp[-2].name) yyvsp[0].nodeList->push_front(yyvsp[-2].name); yyval.nodeList = yyvsp[0].nodeList; }
    break;

  case 42:
#line 169 "parser.yy"
    { yyval.nodeList = new NodeList(); if (yyvsp[0].name) yyval.nodeList->push_front(yyvsp[0].name); }
    break;

  case 44:
#line 174 "parser.yy"
    { ostringstream s; s << yyvsp[0].num;yyval.name = new Name(new std::string(s.str())); }
    break;

  case 45:
#line 179 "parser.yy"
    { yyvsp[0].nodeList->push_front(yyvsp[-2].name); yyval.nodeList = yyvsp[0].nodeList; }
    break;

  case 46:
#line 180 "parser.yy"
    { yyval.nodeList = new NodeList(); yyval.nodeList->push_front(yyvsp[0].name); }
    break;

  case 47:
#line 181 "parser.yy"
    { yyval.nodeList = new NodeList(); }
    break;

  case 48:
#line 186 "parser.yy"
    { yyval.formula = new FormulaFunctionApp(yyvsp[-3].name, yyvsp[-1].nodeList); }
    break;

  case 49:
#line 196 "parser.yy"
    { yyval.name = new Name(new std::string(((FlexLexer*)lexer)->YYText())); }
    break;

  case 50:
#line 200 "parser.yy"
    { yyval.num = atoi(((FlexLexer*)lexer)->YYText()); }
    break;

  case 51:
#line 204 "parser.yy"
    { std::string s(((FlexLexer*)lexer)->YYText());yyval.name = new Name(new std::string(s.substr(1, s.size()-2))); }
    break;


    }

/* Line 1000 of yacc.c.  */
#line 1452 "parser.cc"

  yyvsp -= yylen;
  yyssp -= yylen;


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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 207 "parser.yy"

int yyerror(const char *s) {
  (void) fprintf(stderr, "** line %d:%s\n", linecount, s);
  return 1;
}

int yylex(yystype* yylval, void* lexer)
{
	FlexLexer* flexer = (FlexLexer*)lexer;

	return flexer->yylex();
}

}
}
}


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
/* Line 1275 of yacc.c.  */
#line 129 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif






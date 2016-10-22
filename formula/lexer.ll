/* Hi, Emacs!  Please pretend this file is written in -*-C-*- */

%option noyywrap
%option c++
%option prefix="rmc_formula_parser_"
%{
int linecount = 1;
  #include "parser-types.h"
  #include "parser.h"


%}

digit [0-9]
alpha [a-zA-Z]
alphanum {digit}|{alpha}|_
number {digit}+
identifier {alpha}{alphanum}*
literal \'{alphanum}*\'

%%

var return TOKEN_K_VAR;
in return TOKEN_K_IN;
notin return TOKEN_K_NOT_IN;
sub return TOKEN_K_SUBSETEQ;
true return TOKEN_K_TRUE;
false return TOKEN_K_FALSE;
let return TOKEN_K_LET;
guard return TOKEN_K_GUARD;

"$" return TOKEN_K_DOLLAR;
"," return TOKEN_K_COMMA;
"+" return TOKEN_K_PLUS;
"[" return TOKEN_K_LEFT_BRACKET;
"]" return TOKEN_K_RIGHT_BRACKET;
"{" return TOKEN_K_LEFT_BRACE;
"}" return TOKEN_K_RIGHT_BRACE;
"(" return TOKEN_K_LEFT_PARENTHESIS;
")" return TOKEN_K_RIGHT_PARENTHESIS;
"'" return TOKEN_K_PRIME;
"=" return TOKEN_K_EQUALS;
"<=" return TOKEN_K_LTEQUALS;
"<" return TOKEN_K_LT;
">=" return TOKEN_K_GTEQUALS;
">" return TOKEN_K_GT;
"&" return TOKEN_K_AND;
"|" return TOKEN_K_OR;
"~" return TOKEN_K_NOT;
"=>" return TOKEN_K_IMPLIES;
"<=>" return TOKEN_K_IFF;
"alw" return TOKEN_K_ALWAYS;
"ev" return TOKEN_K_EVENTUALLY;
"ex1" return TOKEN_K_EXISTS_FO;
"ex2" return TOKEN_K_EXISTS_SO;
"all1" return TOKEN_K_FORALL_FO;
"all2" return TOKEN_K_FORALL_SO;
":" return TOKEN_K_COLON;
";" return TOKEN_K_SEMICOLON;

{number} { return TOKEN_NUMBER; }
{identifier} { return TOKEN_IDENTIFIER; }
{literal} { return TOKEN_LITERAL; }

"#"[^\n]*\n 	{ linecount++; } /* ignore comments, but count line */

\n 		{ linecount++; }
[ \t\r]		;


. { return yytext[0]; }

%%

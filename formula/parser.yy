%verbose

%{
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

%}

%pure_parser

%union
{
	unsigned int num;
	std::string* str;
	Name* name;
	NodeList* nodeList;
	Formula* formula;
	Declaration* decl;
	TimeIndex* time_index;
}

%{
extern int yylex(yystype *, void*);
%}

%token TOKEN_K_VAR
%token TOKEN_K_TRUE
%token TOKEN_K_FALSE
%token TOKEN_K_LET
%token TOKEN_NUMBER
%token TOKEN_IDENTIFIER
%token TOKEN_LITERAL
%token TOKEN_K_COMMA
%token TOKEN_K_DOLLAR
%token TOKEN_K_COLON
%token TOKEN_K_SEMICOLON

%right TOKEN_K_EXISTS_FO TOKEN_K_EXISTS_SO
%right TOKEN_K_FORALL_FO TOKEN_K_FORALL_SO
%right TOKEN_K_IMPLIES TOKEN_K_IFF
%left TOKEN_K_OR
%left TOKEN_K_AND
%left TOKEN_K_EQUALS TOKEN_K_IN TOKEN_K_NOT_IN TOKEN_K_SUBSETEQ TOKEN_K_LTEQUALS TOKEN_K_LT TOKEN_K_GTEQUALS TOKEN_K_GT
%token TOKEN_K_PLUS
%right TOKEN_K_ALWAYS TOKEN_K_EVENTUALLY
%right TOKEN_K_NOT
%token TOKEN_K_PRIME
%nonassoc TOKEN_K_LEFT_BRACKET TOKEN_K_RIGHT_BRACKET TOKEN_K_LEFT_PARENTHESIS TOKEN_K_RIGHT_PARENTHESIS TOKEN_K_LEFT_BRACE TOKEN_K_RIGHT_BRACE TOKEN_K_GUARD

/* types of non-terminals */
%type <formula> start formula formula_with_declarations formula_function_app
%type <name> token_identifier token_literal value
%type <nodeList> args declarations formulae values
%type <num> token_number
%type <decl> declaration
%type <time_index> timeindex
%%

start:
  formula { SyntaxTree = $1; }
;

timeindex:
  /* empty */ { $$ = new TimeIndex(0); }
| TOKEN_K_PRIME { $$ = new TimeIndex(1); }
;

formula:
  formula_with_declarations
| TOKEN_K_GUARD TOKEN_K_LEFT_PARENTHESIS formula TOKEN_K_RIGHT_PARENTHESIS
{ $$ = new FormulaOpApplication(new OpGuard(), $3); }
| formula_function_app
| token_identifier TOKEN_K_IN token_identifier { $$ = new
  FormulaOpApplication(new OpMembership(), ($1), ($3)); }
| token_identifier TOKEN_K_SUBSETEQ token_identifier { $$ = new
FormulaOpApplication(new OpSubset(), ($1), ($3)); }
| token_identifier TOKEN_K_EQUALS token_identifier TOKEN_K_PLUS token_number { $$ = new FormulaFunctionApp(new Name("+"), ($1), ($3), new FormulaInteger($5)); }
| token_identifier TOKEN_K_EQUALS token_number { assert($3 == 0); $$ = new FormulaFunctionApp(new Name("0"), $1); }
| token_identifier TOKEN_K_EQUALS TOKEN_K_DOLLAR { $$ = new FormulaFunctionApp(new Name("$"), $1); }
| token_identifier TOKEN_K_EQUALS token_identifier { $$ = new FormulaOpApplication(new OpEqual(), ($1), ($3)); }
| token_identifier TOKEN_K_LT token_identifier { $$ = new FormulaFunctionApp(new Name("<"), ($1), ($3)); }
| token_identifier TOKEN_K_LTEQUALS token_identifier { $$ = new FormulaFunctionApp(new Name("<="), ($1), ($3)); }
| token_identifier TOKEN_K_GT token_identifier { $$ = new FormulaFunctionApp(new Name(">"), ($1), ($3)); }
| token_identifier TOKEN_K_GTEQUALS token_identifier { $$ = new FormulaFunctionApp(new Name(">="), ($1), ($3)); }
| token_identifier timeindex TOKEN_K_LEFT_BRACKET token_identifier TOKEN_K_RIGHT_BRACKET
 { $$ = new FormulaOpApplication(new OpIndexTrue(), $1, $4, $2); }
| token_identifier timeindex TOKEN_K_LEFT_BRACKET token_identifier TOKEN_K_RIGHT_BRACKET TOKEN_K_EQUALS token_literal
 { $$ = new FormulaOpApplication(new OpIndexValue(), $1, $4, $2, $7); }
| token_identifier timeindex TOKEN_K_LEFT_BRACKET token_identifier TOKEN_K_RIGHT_BRACKET TOKEN_K_EQUALS
  token_identifier timeindex TOKEN_K_LEFT_BRACKET token_identifier TOKEN_K_RIGHT_BRACKET
{ NodeList* args = new NodeList();
 args->push_back($1);
 args->push_back($4);
 args->push_back($2);
 args->push_back($7);
 args->push_back($10);
 args->push_back($8);
 $$ = new FormulaOpApplication(new OpIndexIndex(), args); }
| TOKEN_K_TRUE { $$ = new FormulaTrue(); }
| TOKEN_K_FALSE { $$ = new FormulaFalse(); }
| formula TOKEN_K_AND formula { $$ = new FormulaOpApplication(new
OpAnd(), $1, $3); }
| formula TOKEN_K_OR formula { $$ = new FormulaOpApplication(new
OpOr(), $1, $3); }
| formula TOKEN_K_IMPLIES formula { $$ = new FormulaOpApplication(new OpImplies(),$1, $3); }
| formula TOKEN_K_IFF formula { $$ = new FormulaOpApplication(new OpIff(), $1,$3); }
| TOKEN_K_NOT formula { $$ = new FormulaOpApplication(new OpNot(),
$2); }
| TOKEN_K_ALWAYS formula { $$ = new FormulaOpApplication(new
OpAlways(), $2); }
| TOKEN_K_EVENTUALLY formula { $$ = new FormulaOpApplication(new
OpEventually(), $2); }
| TOKEN_K_EXISTS_FO token_identifier formula { $$ = new
FormulaQuantification(new QuantifierFOExists(), ($2),
$3); }
| TOKEN_K_FORALL_FO token_identifier formula { $$ = new
FormulaQuantification(new QuantifierFOForall(), ($2),
$3); }
| TOKEN_K_EXISTS_SO token_identifier formula { $$ = new
FormulaQuantification(new QuantifierSOExists(), ($2),
$3); }
| TOKEN_K_FORALL_SO token_identifier formula { $$ = new
FormulaQuantification(new QuantifierSOForall(), ($2),
$3); }
| TOKEN_K_LEFT_PARENTHESIS formula TOKEN_K_RIGHT_PARENTHESIS {
$$ = $2; } ;

formula_with_declarations:
  TOKEN_K_LET declarations TOKEN_K_IN formula { $$ = new
  FormulaWithDeclarations($2, $4); }
;

declarations:
  declaration declarations { if ($1) $2->push_front($1); $$ = $2; }
| declaration { $$ = new NodeList(); if ($1) $$->push_front($1); }
;

declaration:
  token_identifier TOKEN_K_EQUALS formula { $$ = new DeclFunction($1,
  new NodeList(), $3); }
| token_identifier TOKEN_K_LEFT_PARENTHESIS args
  TOKEN_K_RIGHT_PARENTHESIS TOKEN_K_EQUALS formula { $$ = new
  DeclFunction($1, $3, $6); }
| token_identifier TOKEN_K_COLON TOKEN_K_COLON TOKEN_K_LEFT_BRACE values TOKEN_K_RIGHT_BRACE
{ $$ = new DeclType($1, $5); }
;

values:
  value TOKEN_K_COMMA values { if ($1) $3->push_front($1); $$ = $3; }
| value { $$ = new NodeList(); if ($1) $$->push_front($1); }
;

value: 
  token_literal
  | token_number { ostringstream s; s << $1;$$ = new Name(new std::string(s.str())); }
;
  

args:
  token_identifier TOKEN_K_COMMA args { $3->push_front($1); $$ = $3; }
| token_identifier { $$ = new NodeList(); $$->push_front($1); }
| { $$ = new NodeList(); }
;

formula_function_app:
  token_identifier TOKEN_K_LEFT_PARENTHESIS args
  TOKEN_K_RIGHT_PARENTHESIS { $$ = new FormulaFunctionApp($1, $3); }
;

formulae:
  formula TOKEN_K_COMMA formulae { $3->push_front($1); $$ = $3; }
| formula { $$ = new NodeList(); $$->push_front($1); }
| { $$ = new NodeList(); }
;

token_identifier:
  TOKEN_IDENTIFIER { $$ = new Name(new std::string(((FlexLexer*)lexer)->YYText())); }
;

token_number:
  TOKEN_NUMBER { $$ = atoi(((FlexLexer*)lexer)->YYText()); }
;

token_literal:
  TOKEN_LITERAL { std::string s(((FlexLexer*)lexer)->YYText());$$ = new Name(new std::string(s.substr(1, s.size()-2))); }
;

%%
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

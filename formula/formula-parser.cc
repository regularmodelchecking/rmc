/*
 * formula-parser.cc: 
 *
 * Copyright (C) 2004 Marcus Nilsson (marcusn@it.uu.se)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * Authors:
 *    Marcus Nilsson (marcusn@it.uu.se)
 */


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include "formula-parser.h"
#undef yyFlexLexer
#define yyFlexLexer rmc_formula_parser_FlexLexer
#include <FlexLexer.h>

namespace rmc
{
namespace formula
{
namespace parser
{
extern int yyparse(void*);
extern int yydebug;
Formula* SyntaxTree;
}
}
}

namespace rmc
{
namespace formula
{

using namespace rmc::formula::parser;

Formula* Parser::parse(std::istream& is) const
{
//	rmc::formula::parser::yydebug = 1;
	FlexLexer* lexer = new rmc_formula_parser_FlexLexer;
	lexer->switch_streams(&is);
	yyparse(lexer);
	
	return SyntaxTree;
}

}
}


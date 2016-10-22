/*
 * prettyprint.h:
 *
 * Copyright (C) 2003 Marcus Nilsson (marcusn@it.uu.se)
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 * 
 * Authors: Marcus Nilsson (marcusn@it.uu.se), Mayank Saksena
 * (mayanks@it.uu.se)
 */
#ifndef RMCLOGIC_PRETTYPRINT_H
#define RMCLOGIC_PRETTYPRINT_H

#include "formula.h"

namespace rmc
{
	namespace formula
	{

		class VisitorPrettyPrinter : public VisitorDefault
		{
			std::string listPrefix, listInfix, listSuffix;
			FormulaOpApplication* app;
			std::ostream& out;
			
			void print_infix(std::string opname);
		public:
			VisitorPrettyPrinter();
			VisitorPrettyPrinter(std::ostream& out);
			virtual ~VisitorPrettyPrinter();
			virtual void visit(NodeList& n);
			virtual void visit(FormulaWithDeclarations& f);
			virtual void visit(FormulaFunctionApp& f);
			virtual void visit(FormulaTrue& f);
			virtual void visit(FormulaFalse& f);
			virtual void visit(Op& f);
			virtual void visit(OpMembership& f);
			virtual void visit(OpSubset& f);
			virtual void visit(TimeIndex& f);
			virtual void visit(OpIndexTrue& f);
			virtual void visit(OpIndexValue& f);
			virtual void visit(OpIndexIndex& f);
			virtual void visit(FormulaOpApplication& f);
			virtual void visit(FormulaQuantification& f);
			virtual void visit(Name& n);
			virtual void visit(OpEqual& o);
			virtual void visit(OpNot& o);
			virtual void visit(OpGuard& o);
			virtual void visit(OpAlways& o);
			virtual void visit(OpEventually& o);
			virtual void visit(OpAnd& o);
			virtual void visit(OpOr& o);
			virtual void visit(OpImplies& f);
			virtual void visit(OpIff& f);
			virtual void visit(QuantifierFOExists& q);
			virtual void visit(QuantifierFOForall& q);
			virtual void visit(QuantifierSOExists& q);
			virtual void visit(QuantifierSOForall& q);
			virtual void visit(DeclFunction& d);
			virtual void visit(DeclType& d);
		};

		class VisitorPrettyPrinterNoLet : public VisitorPrettyPrinter
		{
		public:
			VisitorPrettyPrinterNoLet();
			VisitorPrettyPrinterNoLet(std::ostream& out);
			virtual ~VisitorPrettyPrinterNoLet();
			virtual void visit(FormulaWithDeclarations& f);
		};


	}
}

#endif /* RMCLOGIC_PRETTYPRINT_H */

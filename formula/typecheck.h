/*
 * typecheck.h:
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
#ifndef RMCLOGIC_TYPECHECK_H
#define RMCLOGIC_TYPECHECK_H

#include "formula.h"
#include "type.h"
#include <stack>
#include <map>

namespace rmc
{
	namespace formula
	{
		class VisitorTypeChecker : public VisitorDefault
		{
			template <class T>
			class SymbolTable : public std::map<Name, T>
			{};
		
			std::stack<Type> typeStack;
			SymbolTable<Type> types;
		public:
			VisitorTypeChecker();
			virtual ~VisitorTypeChecker();
			void handleVariable(Name& n, Type& vartype);
			virtual void visit(NodeList& n);
			virtual void visit(FormulaConstant& f);
			virtual void visit(FormulaOpApplication& f);
			virtual void visit(FormulaQuantification& f);
			virtual void visit(Name& n);
			virtual void visit(Op& o);
			virtual void visit(QuantifierFO& q);
			virtual void visit(QuantifierSO& q);
		};

		void typeError(std::string str);
		
	}
}

#endif /* RMCLOGIC_TYPECHECK_H */

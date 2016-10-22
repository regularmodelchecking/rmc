/*
 * formulabuild.h:
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
#ifndef RMC_FORMULA_FORMULABUILDER_H
#define RMC_FORMULA_FORMULABUILDER_H

#include "formula.h"
#include <stack>

namespace rmc
{
	namespace formula
	{

		template <class T>
		class BuilderStack : public Builder
		{
			std::stack<T> s;
		public:
			virtual ~BuilderStack() {};
			T getResult() { T res = s.top();s.pop();return res; }

			const T& top() const { return s.top(); }
			void pop() { s.pop(); }
			void push(const T& x) { s.push(x); }
			unsigned int size() const { return s.size(); }
		};

		class BuilderFormula : public BuilderStack<Node*>
		{
		public:
			BuilderFormula();
			virtual ~BuilderFormula();
			virtual void buildNodeList(int);
			virtual void buildFormulaWithDeclarations();
			virtual void buildFormulaFunctionApp();
			virtual void buildFormulaConstant(FormulaConstant& c);
			virtual void buildFormulaOpApplication();
			virtual void buildFormulaQuantification();
			virtual void buildName(Name& n);
			virtual void buildTimeIndex(TimeIndex& t);
			virtual void buildVariable(Variable& v);
			virtual void buildOp(Op& op);
			virtual void buildQuantifier(Quantifier& q);
			virtual void buildDeclFunction();
			virtual void buildDeclType();
		};

		class DirectorDefault : public Director, public VisitorDefault
		{
		public:
			DirectorDefault(Builder*);
			virtual ~DirectorDefault();
			virtual void visit(NodeList& ns);
			virtual void visit(FormulaWithDeclarations& f);
			virtual void visit(FormulaFunctionApp& f);
			virtual void visit(FormulaConstant& f);
			virtual void visit(Op& f);
			virtual void visit(TimeIndex& t);
			virtual void visit(Name& n);
			virtual void visit(FormulaOpApplication& f);
			virtual void visit(FormulaQuantification& f);
			virtual void visit(Quantifier& q);
			virtual void visit(DeclFunction& d);
			virtual void visit(DeclType& d);
		};

	}
}

#endif /* RMC_FORMULA_FORMULABUILDER_H */

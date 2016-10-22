/*
 * negationpush.h:
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
#ifndef RMCLOGIC_NEGATIONPUSH_H
#define RMCLOGIC_NEGATIONPUSH_H

#include "formula.h"
#include <stack>

namespace rmc
{
	namespace formula
	{

		class VisitorNegationPusher : public VisitorDefault
		{
			std::stack<Formula*> psiStack;
			Op* op;
			Quantifier* quant;
			bool sawNegation;
			bool opAtomic;

			enum Specialcase {NONE, OP_IMP, OP_IFF};
			Specialcase special;
		public:
			VisitorNegationPusher();
			virtual ~VisitorNegationPusher();
			virtual Formula* getFormula();
			virtual void visit(FormulaWithDeclarations& f);
			virtual void visit(FormulaTrue& f);
			virtual void visit(FormulaFalse& f);
			virtual void visit(FormulaOpApplication& f);
			virtual void visit(FormulaQuantification& f);
			virtual void visit(Formula& f);
			virtual void visit(Op& o);
			virtual void visit(OpNot& o);
			virtual void visit(OpImplies& o);
			virtual void visit(OpIff& o);
			virtual void visit(OpAlways& o);
			virtual void visit(OpEventually& o);
			virtual void visit(OpAnd& o);
			virtual void visit(OpOr& o);
			virtual void visit(QuantifierFOExists& q);
			virtual void visit(QuantifierFOForall& q);
			virtual void visit(QuantifierSOExists& q);
			virtual void visit(QuantifierSOForall& q);
		};

	}
}

#endif /* RMCLOGIC_NEGATIONPUSH_H */

/*
 * negationpush.cc:
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

#include "negationpush.h"
#include <assert.h>

namespace rmc
{

  namespace formula
  {
    
    VisitorNegationPusher::VisitorNegationPusher() :
      op(0), quant(0), sawNegation(false), special(NONE)
    {}
    VisitorNegationPusher::~VisitorNegationPusher() {}
    Formula* VisitorNegationPusher::getFormula()
    {
      assert(!psiStack.empty());
      Formula* psi = psiStack.top();
      psiStack.pop();
      return psi;
    }
    void VisitorNegationPusher::visit(Formula& f)
    {
      Formula* psi = &f;
      if (sawNegation)
	psi = new FormulaOpApplication(new OpNot(), &f);
      psiStack.push(psi);
    }
    void VisitorNegationPusher::visit(FormulaWithDeclarations& f)
    {
      //f.getDecls()->accept(*this);
      f.getSubformula()->accept(*this);
      Formula* psi = psiStack.top();
      psiStack.pop();
      psiStack.push(new FormulaWithDeclarations(f.getDecls(), psi));
    }
    void VisitorNegationPusher::visit(FormulaTrue& f)
    {
      if (sawNegation) {
	psiStack.push(new FormulaFalse());
	return;
      }
      psiStack.push(&f);
    }
    void VisitorNegationPusher::visit(FormulaFalse& f)
    {
      if (sawNegation) {
	psiStack.push(new FormulaTrue());
	return;
      }
      psiStack.push(&f);
    }
    void VisitorNegationPusher::visit(FormulaOpApplication& f)
    {

      op = f.getOp();
      Op* mem = 0;
      const bool saw = sawNegation; // save original value
      opAtomic = false;

      op->accept(*this);

      // Special operators: rewrite
      if (special != NONE) {
	using namespace std;
	Formula* f2 = 0;
	Node *A = f.getArg(0), *B = f.getArg(1);
	if (saw && special == OP_IMP) {
	  f2 = new FormulaOpApplication(new OpNot(), A);
	  f2 = new FormulaOpApplication(new OpOr(), f2, B);
	  special = NONE;
	  f2->accept(*this);
	  return;
	}
	if (saw && special == OP_IFF) {
	  special = NONE;
	  B->accept(*this); // push negation into right arg
	  B = psiStack.top(); psiStack.pop();
	  sawNegation = false;
	  special = NONE;
	  A->accept(*this);
	  A = psiStack.top(); psiStack.pop();
	  psiStack.push(new FormulaOpApplication(new OpIff(), A, B));
	  return;
	}
	special = NONE;
      }

      if (opAtomic)
      {
	      // Atomic, keep it

	      Formula* res_f = &f;
	      if (sawNegation) res_f = new FormulaOpApplication(new OpNot(), res_f);

	      psiStack.push(res_f);

	      sawNegation = saw;

	      return;
      }


      mem = op;
      NodeList* args = f.getArgs();
      NodeList::reverse_iterator it;
      it = args->rbegin();
      while( it != args->rend() ) {
	(*it)->accept(*this);
	sawNegation = saw;
	++it;
      }
      if (mem == 0) // operator vanished; result on top (assumes Negation)
	return;

      int size = args->size();
      NodeList* subs = new NodeList();
      for (int i = 0; i < size; i++) {
	subs->push_back(psiStack.top());
	psiStack.pop();
      }
      psiStack.push(new FormulaOpApplication(mem, subs));
    }

    void VisitorNegationPusher::visit(FormulaQuantification& f)
    {
      quant = f.getQuantifier();
      Quantifier* mem = 0;

      quant->accept(*this); mem = quant;
      f.getSubformula()->accept(*this);
      Formula* psi = psiStack.top(); psiStack.pop();
      psiStack.push(new FormulaQuantification(mem, f.getVariable(), psi));
    }
    void VisitorNegationPusher::visit(OpNot& o)
    {
      sawNegation = !sawNegation;
      op = 0;
    }
    void VisitorNegationPusher::visit(OpImplies& o)
    {
      special = OP_IMP;
    }
    void VisitorNegationPusher::visit(OpIff& o)
    {
      special = OP_IFF;
    }
    void VisitorNegationPusher::visit(OpAlways& o)
    {
      if (sawNegation)
	op = new OpEventually();
    }
    void VisitorNegationPusher::visit(OpEventually& o)
    {
      if (sawNegation)
	op = new OpAlways();
    }
    void VisitorNegationPusher::visit(OpAnd& o)
    {
      if (sawNegation)
	op = new OpOr();
    }
    void VisitorNegationPusher::visit(OpOr& o)
    {
      if (sawNegation)
	op = new OpAnd();
    }

    void VisitorNegationPusher::visit(Op& o)
    {
	    opAtomic = true;
    }
    void VisitorNegationPusher::visit(QuantifierFOExists& q)
    {
      if (sawNegation)
	quant = new QuantifierFOForall();
    }
    void VisitorNegationPusher::visit(QuantifierFOForall& q)
    {
      if (sawNegation)
	quant = new QuantifierFOExists();
    }
    void VisitorNegationPusher::visit(QuantifierSOExists& q)
    {
      if (sawNegation)
	quant = new QuantifierSOForall();
    }
    void VisitorNegationPusher::visit(QuantifierSOForall& q)
    {
      if (sawNegation)
	quant = new QuantifierSOExists();
    }

  }
}

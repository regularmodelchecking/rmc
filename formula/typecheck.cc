/*
 * typecheck.cc:
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

#include "typecheck.h"
#include <assert.h>

using std::cout;

namespace rmc
{

  namespace formula
  {

    void typeError(std::string str)
    {
      cout << "Error";
      cout << "\n" << str << "\n" << "Execution aborted\n";
      exit(-1);
    }
    VisitorTypeChecker::VisitorTypeChecker() {}
    VisitorTypeChecker::~VisitorTypeChecker() {}

    void VisitorTypeChecker::visit(NodeList& n)
    {
      NodeVector v = n.getChildren();
      VisitorDefault::visit(v);
    }
    void VisitorTypeChecker::visit(FormulaConstant& f)
    {
      typeStack.push(typeBoolean);
    }
    void VisitorTypeChecker::visit(FormulaOpApplication& f)
    {
      Op* op = f.getOp();
      assert(op != 0);
      op->accept(*this);
      Type optype = typeStack.top();
      typeStack.pop();
      f.getArgs()->accept(*this);
      int size = op->getArity();
      for (int i = 0; i < size; i++) {
	if (typeStack.top() != optype)
	  typeError("Wrong operand type.\n");
	typeStack.pop();
      }
      typeStack.push(optype);
    }
    void VisitorTypeChecker::visit(FormulaQuantification& f)
    {
      f.getSubformula()->accept(*this);
      typeStack.pop();
      f.getQuantifier()->accept(*this);
      f.getVariable()->accept(*this);
      Type vartype = typeStack.top();
      typeStack.pop();
      if (typeStack.top() != vartype)
	typeError("Type mismatch in Quantification.");
      else {
	typeStack.pop();
	typeStack.push(typeBoolean);
      }
    }
    void VisitorTypeChecker::handleVariable(Name& n, Type& vartype)
    {
      typedef SymbolTable<Type> ST;
      ST::iterator i = types.find(n);

      if (i != types.end()) {
	// found in table
	Type t = i->second;
	if (t != vartype)
	  typeError("Inconsistent type of " + *(n.getName()));
      }
      else {
	std::pair<Name, Type> p(n, vartype);
	types.insert(p);
      }
      typeStack.push(vartype);
    }
    void VisitorTypeChecker::visit(Name& n)
    {
	    // TODO: Fix this, variable concept is gone. Type should be gotten from operation instead.
      Type t(typeFO);
      handleVariable(n, t);
    }
    void VisitorTypeChecker::visit(Op& o)
    {
      typeStack.push(typeBoolean);
    }
    void VisitorTypeChecker::visit(QuantifierFO& q)
    {
      typeStack.push(typeFO);
    }
    void VisitorTypeChecker::visit(QuantifierSO& q)
    {
      typeStack.push(typeSO);
    }

  }
}

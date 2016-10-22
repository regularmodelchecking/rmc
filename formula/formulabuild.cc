/*
 * formulabuild.cc:
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

#include "formulabuild.h"
#include <assert.h>
#include "prettyprint.h"

namespace rmc
{

  namespace formula
  {

    BuilderFormula::BuilderFormula() {}
    BuilderFormula::~BuilderFormula() {}
    void BuilderFormula::buildNodeList(int k)
    {
      NodeList* nodes = new NodeList();
      for (int i = 0; i < k; i++) {
	nodes->push_front(top());
	pop();
      }
      push(nodes);
    }
    void BuilderFormula::buildFormulaWithDeclarations()
    {
      Formula* newSub = dynamic_cast<Formula*>(top());
      pop();
      NodeList* newDecls = dynamic_cast<NodeList*>(top());
      pop();
      push(new FormulaWithDeclarations(newDecls, newSub));
    }
    void BuilderFormula::buildFormulaFunctionApp()
    {
      NodeList* newArgs = dynamic_cast<NodeList*>(top());
      pop();
      Name* newName = dynamic_cast<Name*>(top());
      pop();
      push(new FormulaFunctionApp(newName, newArgs));
    }
    void BuilderFormula::buildFormulaConstant(FormulaConstant& c)
    {
	    push (&c);
    }
    void BuilderFormula::buildFormulaOpApplication()
    {
      Op* newOp = dynamic_cast<Op*>(top());
      pop();
      NodeList* newArgs = dynamic_cast<NodeList*>(top());
      pop();
      push(new FormulaOpApplication(newOp, newArgs));
    }
    void BuilderFormula::buildFormulaQuantification()
    {
      Quantifier* newQuant =  dynamic_cast<Quantifier*>(top());
      pop();
      Variable* newVar = dynamic_cast<Variable*>(top());
      pop();
      Formula* newSub = dynamic_cast<Formula*>(top());
      pop();
      push(new FormulaQuantification(newQuant, newVar, newSub));
    }
    void BuilderFormula::buildVariable(Variable& v)
    {
       push(&v);
    }
    void BuilderFormula::buildName(Name& n)
    {
       push(&n);
    }
    void BuilderFormula::buildTimeIndex(TimeIndex& t)
    {
       push(&t);
    }
    void BuilderFormula::buildOp(Op& op)
    {
       push(&op);
    }
    void BuilderFormula::buildQuantifier(Quantifier& q)
    {
       push(&q);
    }
    void BuilderFormula::buildDeclFunction()
    {
      Formula* newF = dynamic_cast<Formula*>(top());
      pop();
      NodeList* newArgs = dynamic_cast<NodeList*>(top());
      pop();
      Name* newName = dynamic_cast<Name*>(top());
      pop();
      push(new DeclFunction(newName, newArgs, newF));
    }

    void BuilderFormula::buildDeclType()
    {
      NodeList* newValues = dynamic_cast<NodeList*>(top());
      pop();
      Name* newName = dynamic_cast<Name*>(top());
      pop();
      push(new DeclType(newName, newValues));
    }

    DirectorDefault::DirectorDefault(Builder* b) :
      Director(b)
    {}
    DirectorDefault::~DirectorDefault() {}
    void DirectorDefault::visit(NodeList& ns)
    {
      getBuilder()->buildNodeList(ns.size()); // illa! Skicka med faktiskt längd allmännare
    }
    void DirectorDefault::visit(FormulaWithDeclarations& f)
    {
      getBuilder()->buildFormulaWithDeclarations();
    }
    void DirectorDefault::visit(FormulaFunctionApp& f)
    {
       getBuilder()->buildFormulaFunctionApp();
    }
    void DirectorDefault::visit(FormulaConstant& f)
    {
      getBuilder()->buildFormulaConstant(f);
    }
    void DirectorDefault::visit(FormulaOpApplication& f)
    {
      getBuilder()->buildFormulaOpApplication();
    }
    void DirectorDefault::visit(FormulaQuantification& f)
    {
      getBuilder()->buildFormulaQuantification();
    }
    void DirectorDefault::visit(Name& n)
    {
      getBuilder()->buildName(n); 
    }
    void DirectorDefault::visit(Op& op)
    {
      getBuilder()->buildOp(op); 
    }
    void DirectorDefault::visit(TimeIndex& t)
    {
      getBuilder()->buildTimeIndex(t); 
    }
    void DirectorDefault::visit(Quantifier& q)
    {
      getBuilder()->buildQuantifier(q); 
    }
    void DirectorDefault::visit(DeclFunction& d)
    {
      getBuilder()->buildDeclFunction();
    }
    void DirectorDefault::visit(DeclType& d)
    {
	    getBuilder()->buildDeclType();
    }

  }
}

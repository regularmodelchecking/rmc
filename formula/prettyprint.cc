/*
 * prettyprint.cc:
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

#include "prettyprint.h"
#include <assert.h>

namespace rmc
{

  namespace formula
  {

    VisitorPrettyPrinter::VisitorPrettyPrinter() :
	    listPrefix(""), listInfix(""), listSuffix(""), out(std::cout)
    {}

    VisitorPrettyPrinter::VisitorPrettyPrinter(std::ostream& out) :
	    listPrefix(""), listInfix(""), listSuffix(""), out(out)
    {}

    VisitorPrettyPrinter::~VisitorPrettyPrinter() {}

    void VisitorPrettyPrinter::visit(NodeList& n)
    {
      if (n.size() == 0)
	return;
      out << listPrefix;
      NodeList::iterator it = n.begin();
      while( it+1 != n.end() ) {
	(*it)->accept(*this);
	out << listInfix;
	++it;
      }
      (*it)->accept(*this);
      out << listSuffix;
    }

    void VisitorPrettyPrinter::visit(FormulaWithDeclarations& f)
    {
      out << "Let";
      std::string lP = listPrefix, lI = listInfix, lS = listSuffix;
      listPrefix = "\n\t"; listInfix = "\n\t"; listSuffix = "\n";
      f.getDecls()->accept(*this);
      listPrefix = lP; listInfix = lI; listSuffix = lS;
      out << "In\n\t";
      f.getSubformula()->accept(*this);
    }
    void VisitorPrettyPrinter::visit(FormulaFunctionApp& f)
    {
      f.getName()->accept(*this);
      NodeList* args = f.getArgs();
      if (args->size() == 0)
	return;
      out << "(";
      std::string lP = listPrefix, lI = listInfix, lS = listSuffix;
      listPrefix = ""; listInfix = ", "; listSuffix = "";
      args->accept(*this);
      listPrefix = lP; listInfix = lI; listSuffix = lS;
      out << ")";
    }

    void VisitorPrettyPrinter::visit(FormulaTrue& f)
    {
      out << "True";
    }
    void VisitorPrettyPrinter::visit(FormulaFalse& f)
    {
      out << "False";
    }
    void VisitorPrettyPrinter::visit(OpMembership& f)
    {
	    print_infix(" in ");
    }
    void VisitorPrettyPrinter::visit(OpSubset& f)
    {
	    print_infix(" sub ");
    }
    void VisitorPrettyPrinter::visit(OpIndexTrue& f)
    {
	    app->getArg(0)->accept(*this);
	    app->getArg(2)->accept(*this);
	    out << "[";
	    app->getArg(1)->accept(*this);
	    out << "]";
    }

    void VisitorPrettyPrinter::visit(OpIndexValue& f)
    {
	    app->getArg(0)->accept(*this);
	    app->getArg(2)->accept(*this);
	    out << "[";
	    app->getArg(1)->accept(*this);
	    out << "] = '";
	    app->getArg(3)->accept(*this);
	    out << "'";
    }

    void VisitorPrettyPrinter::visit(OpIndexIndex& f)
    {
	    app->getArg(0)->accept(*this);
	    app->getArg(2)->accept(*this);
	    out << "[";
	    app->getArg(1)->accept(*this);
	    out << "]";
	    out << " = ";
	    app->getArg(3)->accept(*this);
	    app->getArg(5)->accept(*this);
	    out << "[";
	    app->getArg(4)->accept(*this);
	    out << "]";
    }

    void VisitorPrettyPrinter::visit(TimeIndex& f)
    {
	    unsigned int t = f.getIndex();
	    while (t > 0)
	    {
		    out << "'";
		    t--;
	    }
    }

    void VisitorPrettyPrinter::visit(Op& f)
    {
      out << "(";
      std::string lP = listPrefix, lI = listInfix, lS = listSuffix;
      listPrefix = ""; listInfix = ", "; listSuffix = "";
      app->getArgs()->accept(*this);
      listPrefix = lP; listInfix = lI; listSuffix = lS;
      out << ")";
    }

    void VisitorPrettyPrinter::print_infix(std::string opname)
    {
	    FormulaOpApplication* thisapp = app;
	    thisapp->getArg(0)->accept(*this);
	    out << opname;
	    thisapp->getArg(1)->accept(*this);
    }

    void VisitorPrettyPrinter::visit(FormulaOpApplication& f)
    {
	    app = &f;
	    f.getOp()->accept(*this);
    }
    void VisitorPrettyPrinter::visit(FormulaQuantification& f)
    {
      f.getQuantifier()->accept(*this);
      f.getVariable()->accept(*this);
      out << " (";
      f.getSubformula()->accept(*this);
      out << ")";
    }

    void VisitorPrettyPrinter::visit(Name& n)
    {
	    out << n.get_desc();
    }
    void VisitorPrettyPrinter::visit(OpEqual& o)
    {
      print_infix(" = ");
    }
    void VisitorPrettyPrinter::visit(OpNot& o)
    {
	    out << "~(";
	    app->getArg(0)->accept(*this);
	    out << ")";
    }
    void VisitorPrettyPrinter::visit(OpGuard& o)
    {
	    out << "guard(";
	    app->getArg(0)->accept(*this);
	    out << ")";
    }
    void VisitorPrettyPrinter::visit(OpAlways& o)
    {
	    out << "alw(";
	    app->getArg(0)->accept(*this);
	    out << ")";
    }
    void VisitorPrettyPrinter::visit(OpEventually& o)
    {
	    out << "ev(";
	    app->getArg(0)->accept(*this);
	    out << ")";
    }
    void VisitorPrettyPrinter::visit(OpAnd& o)
    {
      print_infix(" & ");
    }
    void VisitorPrettyPrinter::visit(OpOr& o)
    {
      print_infix(" | ");
    }
    void VisitorPrettyPrinter::visit(OpImplies& o)
    {
      print_infix(" => ");
    }
    void VisitorPrettyPrinter::visit(OpIff& o)
    {
      print_infix(" <=> ");
    }
    void VisitorPrettyPrinter::visit(QuantifierFOExists& q)
    {
      out << "ex1 ";
    }
    void VisitorPrettyPrinter::visit(QuantifierFOForall& q)
    {
      out << "all1 ";
    }
    void VisitorPrettyPrinter::visit(QuantifierSOExists& q)
    {
      out << "ex2 ";
    }
    void VisitorPrettyPrinter::visit(QuantifierSOForall& q)
    {
      out << "all2 ";
    }

    void VisitorPrettyPrinter::visit(DeclFunction& d)
    {
      d.getName()->accept(*this);
      NodeList* args = d.getArgs();
      if (args->size() != 0) {
	out << "(";
	std::string lP = listPrefix, lI = listInfix, lS = listSuffix;
	listPrefix = ""; listInfix = ", "; listSuffix = "";
	args->accept(*this);
	listPrefix = lP; listInfix = lI; listSuffix = lS;
	out << ")";
      }
      out << " = ";
      d.getSubformula()->accept(*this);
    }
    void VisitorPrettyPrinter::visit(DeclType& d)
    {
      d.getName()->accept(*this);
      out << " :: {";
      std::string lP = listPrefix, lI = listInfix, lS = listSuffix;
      listPrefix = ""; listInfix = ", "; listSuffix = "";
      d.getValues()->accept(*this);
      listPrefix = lP; listInfix = lI; listSuffix = lS;
      out << "}";
    }


    VisitorPrettyPrinterNoLet::VisitorPrettyPrinterNoLet() : VisitorPrettyPrinter()
    {}
    VisitorPrettyPrinterNoLet::VisitorPrettyPrinterNoLet(std::ostream& out) :
	    VisitorPrettyPrinter(out)
    {}
    VisitorPrettyPrinterNoLet::~VisitorPrettyPrinterNoLet() {}
    void VisitorPrettyPrinterNoLet::visit(FormulaWithDeclarations& f)
    {
      f.getSubformula()->accept(*this);
    }

  }
}

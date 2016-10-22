/*
 * formula.cc:
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

#include "formula.h"
#include "prettyprint.h"
#include <typeinfo>
#include <sstream>
//using namespace std;

namespace rmc
{

  namespace formula
  {

	  Visitor::~Visitor() {}

    std::ostream& operator<<(std::ostream& s, const Node& n)
    {
	    VisitorPrettyPrinter pp(s);
	    ((Node&)n).accept(pp);
	    return s;
    }

    void Node::accept(Visitor& v)
    {
      v.visit(*this);
    }
    NodeVector Node::getChildren() const
    {
      return NodeVector();
    }

    bool Node::operator==(const Node& n2) const {
      const Node& n1 = *this;
      /*
	special case(s):
	if typeid and children comparison isn't enough
      */
      const Name *name1, *name2;
      if ((name1 = dynamic_cast<const Name*>(&n1)) &&
	  (name2 = dynamic_cast<const Name*>(&n2)))
	return *name1 == *name2;

      return (typeid(&n1) == typeid(&n2) &&
	      n1.getChildren() == n2.getChildren());
    }

    NodeList::~NodeList(){}
    void NodeList::accept(Visitor& v)
    {
      v.visit(*this);
    }
    NodeVector NodeList::getChildren() const
    {
      NodeVector ns;
      NodeList::const_iterator it = begin();
      while( it != end() ) {
	ns.push_back(*it);
	it++;
      }
      return ns;
    }

    Name::Name(std::string* name): desc(name)
    {}

    Name::Name(const std::string& desc): desc(new std::string(desc))
    {}

    Name::Name(unsigned int n)
    {
	    std::ostringstream os;
	    os << n;
	    
	    desc = new std::string(os.str());
    }

    Name::Name(): desc(new std::string("unnamed"))
    {}

    std::string* Name::getName() const
    {
      return desc;
    }

    const std::string& Name::get_desc() const
    {
      return *desc;
    }
    void Name::accept(Visitor& v)
    {
      v.visit(*this);
    }
    bool operator<(Name n1, Name n2)
    {
	    return (n1.desc < n2.desc);

#if 0
	    if (*n1.desc == "unnamed" && *n2.desc == "unnamed")
	    {
		    return (n1.desc < n2.desc);
	    }
	    else
	    {
		    return *n1.desc < *n2.desc;
	    }
#endif
    }
    bool operator==(Name n1, Name n2)
    {
	    return (n1.desc == n2.desc);

#if 0	   
	    if (*n1.desc == "unnamed" && *n2.desc == "unnamed")
	    {
		    return (n1.desc == n2.desc);
	    }
	    else
	    {
		    return *n1.desc == *n2.desc;
	    }
#endif
    }

    Op::~Op()
    {}
    OpUnary::~OpUnary()
    {}
    OpBinary::~OpBinary()
    {}
    unsigned int OpUnary::getArity() const
    {
      return 1;
    }
    unsigned int OpBinary::getArity() const
    {
      return 2;
    }
    void OpNot::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void OpGuard::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void OpAlways::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void OpEventually::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void OpAnd::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void OpOr::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void OpImplies::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void OpIff::accept(Visitor& v)
    {
      v.visit(*this);
    }

    Quantifier::~Quantifier()
    {}
    QuantifierFO::~QuantifierFO()
    {}
    QuantifierSO::~QuantifierSO()
    {}
    unsigned int QuantifierFO::getOrder() const
    {
      return 1;
    }
    unsigned int QuantifierSO::getOrder() const
    {
      return 2;
    }
    void QuantifierFOExists::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void QuantifierFOForall::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void QuantifierSOExists::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void QuantifierSOForall::accept(Visitor& v)
    {
      v.visit(*this);
    }

    Declaration::~Declaration()
    {}
    void Declaration::accept(Visitor& v)
    {
      v.visit(*this);
    }
    DeclFunction::DeclFunction(Name* n, NodeList* a, Formula* f):
      name(n), args(a), phi(f)
    {}
    Name* DeclFunction::getName() const
    {
      return name;
    }
    NodeList* DeclFunction::getArgs() const
    {
      return args;
    }
    Formula* DeclFunction::getSubformula() const
    {
      return phi;
    }
    NodeVector DeclFunction::getChildren() const
    {
      NodeVector ns;
      ns.push_back(name);
      ns.push_back(args);
      ns.push_back(phi);
      return ns;
    }
    void DeclFunction::accept(Visitor& v)
    {
      v.visit(*this);
    }

    NodeVector DeclType::getChildren() const
    {
      NodeVector ns;
      ns.push_back(name);
      ns.push_back(values);
      return ns;
    }
    void DeclType::accept(Visitor& v)
    {
      v.visit(*this);
    }

    FormulaWithDeclarations::FormulaWithDeclarations(NodeList* d,
						     Formula* f):
      decls(d), phi(f)
    {}
    NodeList* FormulaWithDeclarations::getDecls() const
    {
      return decls;
    }
    Formula* FormulaWithDeclarations::getSubformula() const
    {
      return phi;
    }
    NodeVector FormulaWithDeclarations::getChildren() const
    {
      NodeVector ns;
      ns.push_back(decls);
      ns.push_back(phi);
      return ns;
    }
    void FormulaWithDeclarations::accept(Visitor& v)
    {
      v.visit(*this);
    }

    FormulaFunctionApp::FormulaFunctionApp(Name* n, NodeList* a):
      name(n), args(a)
    {}
	  FormulaFunctionApp::FormulaFunctionApp(Name* n, Node* arg0):
		  name(n), args(new NodeList())
	  {
		  args->push_back(arg0);
	  }

	  FormulaFunctionApp::FormulaFunctionApp(Name* n, Node* arg0, Node* arg1):
		  name(n), args(new NodeList())
	  {
		  args->push_back(arg0);
		  args->push_back(arg1);
	  }

	  FormulaFunctionApp::FormulaFunctionApp(Name* n, Node* arg0, Node* arg1, Node* arg2):
		  name(n), args(new NodeList())
	  {
		  args->push_back(arg0);
		  args->push_back(arg1);
		  args->push_back(arg2);
	  }

    Name* FormulaFunctionApp::getName() const
    {
      return name;
    }
    NodeList* FormulaFunctionApp::getArgs() const
    {
      return args;
    }
    NodeVector FormulaFunctionApp::getChildren() const
    {
      NodeVector ns;
      ns.push_back(getName());
      ns.push_back(args);
      return ns;
    }
    void FormulaFunctionApp::accept(Visitor& v)
    {
      v.visit(*this);
    }

    void FormulaConstant::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void FormulaTrue::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void FormulaFalse::accept(Visitor& v)
    {
      v.visit(*this);
    }
    void FormulaInteger::accept(Visitor& v)
    {
      v.visit(*this);
    }

    void OpMembership::accept(Visitor& v)
    {
      v.visit(*this);
    }

    void OpSubset::accept(Visitor& v)
    {
      v.visit(*this);
    }

    void OpIndex::accept(Visitor& v)
    {
      v.visit(*this);
    }

    void OpIndexTrue::accept(Visitor& v)
    {
      v.visit(*this);
    }

    void OpIndexValue::accept(Visitor& v)
    {
      v.visit(*this);
    }

    void OpIndexIndex::accept(Visitor& v)
    {
      v.visit(*this);
    }

    void TimeIndex::accept(Visitor& v)
    {
      v.visit(*this);
    }

    void OpEqual::accept(Visitor& v)
    {
      v.visit(*this);
    }

    void OpStructure::accept(Visitor& v)
    {
      v.visit(*this);
    }

    unsigned int OpStructure::getArity() const
    {
	    return arity;
    }
    FormulaOpApplication::~FormulaOpApplication()
    {}
    FormulaOpApplication::FormulaOpApplication(Op* op, NodeList* args):
      op(op), args(args)
    {}
    FormulaOpApplication::FormulaOpApplication(Op* op, Node* arg0):
      op(op), args(new NodeList())
    {
      args->push_back(arg0);
    }
    FormulaOpApplication::FormulaOpApplication(Op* op, Node* arg0, Node* arg1):
      op(op), args(new NodeList())
    {
      args->push_back(arg0);
      args->push_back(arg1);
    }
    FormulaOpApplication::FormulaOpApplication(Op* op, Node* arg0, Node* arg1, Node* arg2):
      op(op), args(new NodeList())
    {
      args->push_back(arg0);
      args->push_back(arg1);
      args->push_back(arg2);
    }
    FormulaOpApplication::FormulaOpApplication(Op* op, Node* arg0, Node* arg1, Node* arg2, Node* arg3):
      op(op), args(new NodeList())
    {
      args->push_back(arg0);
      args->push_back(arg1);
      args->push_back(arg2);
      args->push_back(arg3);
    }
    Node* FormulaOpApplication::getArg(unsigned int index) const
    {
      return args->at(index);
    }
    NodeList* FormulaOpApplication::getArgs() const
    {
      return args;
    }
    Op* FormulaOpApplication::getOp() const
    {
      return op;
    }
    NodeVector FormulaOpApplication::getChildren() const
    {
      NodeVector ns;
      ns.push_back(args);
      ns.push_back(op);
      return ns;
    }
    void FormulaOpApplication::accept(Visitor& v)
    {
      v.visit(*this);
    }
    FormulaQuantification::~FormulaQuantification()
    {}
    FormulaQuantification::FormulaQuantification(Quantifier* q, Variable* v, Formula* f):
      q(q), v(v), phi(f)
    {}
    Quantifier* FormulaQuantification::getQuantifier() const
    {
      return q;
    }
    Variable* FormulaQuantification::getVariable() const
    {
      return v;
    }
    Formula* FormulaQuantification::getSubformula() const
    {
      return phi;
    }
    NodeVector FormulaQuantification::getChildren() const
    {
      NodeVector ns;
      ns.push_back(phi);
      ns.push_back(v);
      ns.push_back(q);
      return ns;
    }
    void FormulaQuantification::accept(Visitor& v)
    {
      v.visit(*this);
    }

    Director::Director(Builder* b) : builder(b)
    {}
    Director::~Director() {}
    Builder* Director::getBuilder() const
    {
      return builder;
    }
    void Director::setBuilder(Builder* b)
    {
      builder = b;
    }

    VisitorDefault::VisitorDefault() {}
    VisitorDefault::~VisitorDefault() {}
    void VisitorDefault::visit(NodeVector& ns)
    {
      NodeVector::iterator it = ns.begin();
      while( it != ns.end() ) {
	(*it)->accept(*this);
	it++;
      }    
    }
    void VisitorDefault::visit(Node& n) {}
    void VisitorDefault::visit(NodeList& ns)
    {
      visit(static_cast<Node&>(ns));
    }
    void VisitorDefault::visit(Formula& f)
    {
      visit(static_cast<Node&>(f));
    }
    void VisitorDefault::visit(FormulaWithDeclarations& f)
    {
      visit(static_cast<Formula&>(f));
    }
    void VisitorDefault::visit(FormulaFunctionApp& f)
    {
      visit(static_cast<Formula&>(f));
    }
    void VisitorDefault::visit(FormulaConstant& f)
    {
      visit(static_cast<Formula&>(f));
    }
    void VisitorDefault::visit(FormulaInteger& f)
    {
      visit(static_cast<FormulaConstant&>(f));
    }
    void VisitorDefault::visit(FormulaTrue& f)
    {
      visit(static_cast<FormulaConstant&>(f));
    }
    void VisitorDefault::visit(FormulaFalse& f)
    {
      visit(static_cast<FormulaConstant&>(f));
    }
    void VisitorDefault::visit(OpMembership& f)
    {
      visit(static_cast<OpBinary&>(f));
    }
    void VisitorDefault::visit(OpSubset& f)
    {
      visit(static_cast<OpBinary&>(f));
    }
    void VisitorDefault::visit(OpStructure& f)
    {
      visit(static_cast<Op&>(f));
    }
    void VisitorDefault::visit(OpIndex& f)
    {
      visit(static_cast<Op&>(f));
    }
    void VisitorDefault::visit(OpIndexTrue& f)
    {
      visit(static_cast<OpIndex&>(f));
    }
    void VisitorDefault::visit(OpIndexValue& f)
    {
      visit(static_cast<OpIndex&>(f));
    }

    void VisitorDefault::visit(OpIndexIndex& f)
    {
      visit(static_cast<OpIndex&>(f));
    }
    void VisitorDefault::visit(TimeIndex& f)
    {
      visit(static_cast<Node&>(f));
    }
    void VisitorDefault::visit(OpEqual& f)
    {
      visit(static_cast<OpBinary&>(f));
    }
    void VisitorDefault::visit(FormulaOpApplication& f)
    {
      visit(static_cast<Formula&>(f));
    }
    void VisitorDefault::visit(FormulaQuantification& f)
    {
      visit(static_cast<Formula&>(f));
    }
    void VisitorDefault::visit(Name& n)
    {
      visit(static_cast<Node&>(n));
    }
    void VisitorDefault::visit(Op& o)
    {
      visit(static_cast<Node&>(o));
    }
    void VisitorDefault::visit(OpUnary& o)
    {
      visit(static_cast<Op&>(o));
    }
    void VisitorDefault::visit(OpBinary& o)
    {
      visit(static_cast<Op&>(o));
    }
    void VisitorDefault::visit(OpNot& o)
    {
      visit(static_cast<OpUnary&>(o));
    }
    void VisitorDefault::visit(OpGuard& o)
    {
      visit(static_cast<OpUnary&>(o));
    }
    void VisitorDefault::visit(OpAlways& o)
    {
      visit(static_cast<OpUnary&>(o));
    }
    void VisitorDefault::visit(OpEventually& o)
    {
      visit(static_cast<OpUnary&>(o));
    }
    void VisitorDefault::visit(OpAnd& o)
    {
      visit(static_cast<OpBinary&>(o));
    }
    void VisitorDefault::visit(OpOr& o)
    {
     visit(static_cast<OpBinary&>(o));
    }
    void VisitorDefault::visit(OpIff& o)
    {
      visit(static_cast<OpBinary&>(o));
    }
    void VisitorDefault::visit(OpImplies& o)
    {
      visit(static_cast<OpBinary&>(o));
    }
    void VisitorDefault::visit(Quantifier& q)
    {
      visit(static_cast<Node&>(q));
    }
    void VisitorDefault::visit(QuantifierFO& q)
    {
      visit(static_cast<Quantifier&>(q));
    }
    void VisitorDefault::visit(QuantifierSO& q)
    {
      visit(static_cast<Quantifier&>(q));
    }
    void VisitorDefault::visit(QuantifierFOExists& q)
    {
      visit(static_cast<QuantifierFO&>(q));
    }
    void VisitorDefault::visit(QuantifierFOForall& q)
    {
      visit(static_cast<QuantifierFO&>(q));
    }
    void VisitorDefault::visit(QuantifierSOExists& q)
    {
      visit(static_cast<QuantifierSO&>(q));
    }
    void VisitorDefault::visit(QuantifierSOForall& q)
    {
      visit(static_cast<QuantifierSO&>(q));
    }
    void VisitorDefault::visit(Declaration& d)
    {
      visit(static_cast<Node&>(d));
    }
    void VisitorDefault::visit(DeclFunction& d)
    {
      visit(static_cast<Declaration&>(d));
    }
    void VisitorDefault::visit(DeclType& d)
    {
      visit(static_cast<Declaration&>(d));
    }

  }
}

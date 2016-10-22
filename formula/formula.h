/*
 * formula.h:
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
#ifndef RMC_FORMULA_FORMULA_H
#define RMC_FORMULA_FORMULA_H

#include "ref.h"
#include <deque>
#include <vector>
#include <sstream>
#include <iostream>

namespace rmc
{
	namespace formula
	{

		class Visitor;
		class NodeList;
		class Node;

		typedef std::vector<Shared<Node> > NodeVector;
		class Node
		{
		public:
			virtual ~Node() {}
			virtual void accept(Visitor& v);
			virtual NodeVector getChildren() const;
			bool operator ==(const Node&) const;
			friend std::ostream& operator<<(std::ostream& s, const Node& n);
		};

	        class NodeList : public Node, public std::deque<Node*>
		{
		public:
			virtual ~NodeList();
			virtual void accept(Visitor& v);
			virtual NodeVector getChildren() const;
		};

		class Name : public Node
		{
			std::string* desc;
		public:
			Name();
			Name(unsigned int desc);
			Name(const std::string& desc);
		  	Name(std::string* name);
			std::string* getName() const;
			const std::string& get_desc() const;
			virtual void accept(Visitor& v);
			friend bool operator <(Name, Name);
			friend bool operator ==(Name, Name);
		};

		typedef Name Variable;

		class Op : public Node
		{
		public:
			virtual ~Op();
			virtual unsigned int getArity() const { return 0; };
			virtual void accept(Visitor& v) = 0;
		};

		class OpBinary : public Op
                {
                public:
                        virtual ~OpBinary();
                        virtual unsigned int getArity() const;
			virtual void accept(Visitor& v) = 0;
                };

                class OpUnary : public Op
                {
                public:
                        virtual ~OpUnary();
                        virtual unsigned int getArity() const;
			virtual void accept(Visitor& v) = 0;
                };

		class OpNot : public OpUnary { public: virtual void accept(Visitor& v); };
		class OpGuard : public OpUnary { public: virtual void accept(Visitor& v); };
		class OpAlways : public OpUnary { public: virtual void accept(Visitor& v); };
		class OpEventually : public OpUnary { public: virtual void accept(Visitor& v); };
		class OpAnd : public OpBinary { public: virtual void accept(Visitor& v); };
		class OpOr : public OpBinary { public: virtual void accept(Visitor& v); };
		class OpImplies : public OpBinary { public: virtual void accept(Visitor& v); };
		class OpIff : public OpBinary { public: virtual void accept(Visitor& v); };

		class OpEqual : public OpBinary { public: virtual void accept(Visitor& v); };
		class OpMembership : public OpBinary { public: virtual void accept(Visitor& v); };
		class OpSubset : public OpBinary { public: virtual void accept(Visitor& v); };

		class OpIndex : public Op { public: virtual void accept(Visitor& v); };
		class OpIndexTrue : public OpIndex { public: virtual void accept(Visitor& v); };
		class OpIndexValue : public OpIndex { public: virtual void accept(Visitor& v); };
		class OpIndexIndex : public OpIndex { public: virtual void accept(Visitor& v); };

		class OpStructure : public Op
		{
			unsigned int arity;
		public:
			OpStructure(unsigned int arity):
				arity(arity)
				{}

			virtual void accept(Visitor& v);

			unsigned int getArity() const;
		};

		class TimeIndex : public Node 
		{
			unsigned int index;
		public:
			TimeIndex():
				index(0)
				{}
				
			TimeIndex(unsigned int index):
				index(index)
				{}

			unsigned int getIndex() const { return index; }
			virtual void accept(Visitor& v);
		};

		class Quantifier : public Node
		{
		public:
			virtual ~Quantifier();
			virtual unsigned int getOrder() const = 0;
			virtual void accept(Visitor& v) = 0;
		};

		class QuantifierFO : public Quantifier
		{
		public:
			virtual ~QuantifierFO();
			virtual unsigned int getOrder() const;
			virtual void accept(Visitor& v) = 0;
		};

		class QuantifierSO : public Quantifier
		{
		public:
			virtual ~QuantifierSO();
			virtual unsigned int getOrder() const;
			virtual void accept(Visitor& v) = 0;
		};

		class QuantifierFOExists : public QuantifierFO
		{
		public:
			virtual void accept(Visitor& v);
		};

		class QuantifierFOForall : public QuantifierFO
		{
		public:
			virtual void accept(Visitor& v);
		};

		class QuantifierSOExists : public QuantifierSO
		{
		public:
			virtual void accept(Visitor& v);
		};

		class QuantifierSOForall : public QuantifierSO
		{
		public:
			virtual void accept(Visitor& v);
		};

	        class Declaration : public Node
		{
		public:
		  	virtual ~Declaration();
			virtual void accept(Visitor& v);
		};

		class Formula;

		class DeclFunction : public Declaration
		{
			Name* name;
			NodeList* args;
			Formula* phi;
		public:
		  	DeclFunction(Name*, NodeList*, Formula*);
			Name* getName() const;
		        NodeList* getArgs() const;
		        Formula* getSubformula() const;
			virtual NodeVector getChildren() const;
			virtual void accept(Visitor& v);
		};

		class DeclType : public Declaration
		{
			Name* name;
			NodeList* values; // List of names
		public:
			DeclType(Name* name, NodeList* values):
				name(name),
				values(values)
				{}

			Name* getName() { return name; }
			NodeList* getValues() { return values; }

			virtual NodeVector getChildren() const;
			virtual void accept(Visitor& v);
		};

		class Formula : public Node
		{
		public:
			virtual void accept(Visitor& v) = 0;
		};

		class FormulaWithDeclarations : public Formula
		{
			NodeList* decls;
			Formula* phi;
		public:
			FormulaWithDeclarations(NodeList*, Formula*);
			NodeList* getDecls() const;
			Formula* getSubformula() const;
			virtual NodeVector getChildren() const;
			virtual void accept(Visitor& v);
		};

		class FormulaFunctionApp : public Formula
		{
			Name* name;
			NodeList* args;
		public:
		  	FormulaFunctionApp(Name*, NodeList*);
		  	FormulaFunctionApp(Name*, Node* arg0);
		  	FormulaFunctionApp(Name*, Node* arg0, Node* arg1);
		  	FormulaFunctionApp(Name*, Node* arg0, Node* arg1, Node* arg2);
			Name* getName() const;
		        NodeList* getArgs() const;
			virtual NodeVector getChildren() const;
			virtual void accept(Visitor& v);
		};

		class FormulaConstant : public Formula
		{
		public:
			virtual void accept(Visitor& v);
		};

		class FormulaInteger : public FormulaConstant
		{
			unsigned int v;
		public:
			FormulaInteger(unsigned int v):
				v(v)
				{}


			virtual void accept(Visitor& v);

			unsigned int get_value() const { return v; }
		};

		class FormulaTrue : public FormulaConstant
		{
		public:
			virtual void accept(Visitor& v);
		};

		class FormulaFalse : public FormulaConstant
		{
		public:
			virtual void accept(Visitor& v);
		};

		class FormulaOpApplication : public Formula
                {
                        Op* op;
                        NodeList* args;
                public:
                        virtual ~FormulaOpApplication();
                        FormulaOpApplication(Op* op, NodeList* args);
                        FormulaOpApplication(Op* op, Node* arg0);
                        FormulaOpApplication(Op* op, Node* arg0,Node* arg1);
                        FormulaOpApplication(Op* op, Node* arg0,Node* arg1, Node* arg2);
                        FormulaOpApplication(Op* op, Node* arg0,Node* arg1, Node* arg2, Node* arg3);
                        Node* getArg(unsigned int index) const;
                        NodeList* getArgs() const;
                        Op* getOp() const;
			virtual NodeVector getChildren() const;
			virtual void accept(Visitor& v);
                };

		class FormulaQuantification : public Formula
		{
			Quantifier* q;
			Variable* v;
			Formula* phi;
		public:
			virtual ~FormulaQuantification();
                        FormulaQuantification(Quantifier* q, Variable* v,
					      Formula* f);
			Quantifier* getQuantifier() const;
			Variable* getVariable() const;
			Formula* getSubformula() const;
			virtual NodeVector getChildren() const;
			virtual void accept(Visitor& v);
		};

		class Visitor
		{
		public:
			virtual ~Visitor();
			virtual void visit(NodeVector& ns) = 0;
			virtual void visit(Node& n) = 0;
			virtual void visit(NodeList& ns) = 0;
			virtual void visit(Formula& f) = 0;
			virtual void visit(FormulaWithDeclarations& f) = 0;
			virtual void visit(FormulaFunctionApp& f) = 0;
			virtual void visit(FormulaConstant& f) = 0;
			virtual void visit(FormulaInteger& f) = 0;
			virtual void visit(FormulaTrue& f) = 0;
			virtual void visit(FormulaFalse& f) = 0;
			virtual void visit(FormulaOpApplication& f) = 0;
			virtual void visit(FormulaQuantification& f) = 0;
			virtual void visit(Name& n) = 0;
			virtual void visit(Op& o) = 0;
			virtual void visit(OpUnary& o) = 0;
			virtual void visit(OpBinary& o) = 0;
			virtual void visit(OpNot& f) = 0;
			virtual void visit(OpGuard& f) = 0;
			virtual void visit(OpAlways& f) = 0;
			virtual void visit(OpEventually& f) = 0;
			virtual void visit(OpAnd& f) = 0;
			virtual void visit(OpOr& f) = 0;
			virtual void visit(OpImplies& f) = 0;
			virtual void visit(OpIff& f) = 0;
			virtual void visit(OpMembership& f) = 0;
			virtual void visit(OpSubset& f) = 0;
			virtual void visit(OpStructure& f) = 0;
			virtual void visit(OpIndex& f) = 0;
			virtual void visit(OpIndexTrue& f) = 0;
			virtual void visit(OpIndexValue& f) = 0;
			virtual void visit(OpIndexIndex& f) = 0;
			virtual void visit(TimeIndex& f) = 0;
			virtual void visit(OpEqual& f) = 0;
			virtual void visit(Quantifier& q) = 0;
			virtual void visit(QuantifierFO& q) = 0;
			virtual void visit(QuantifierSO& q) = 0;
			virtual void visit(QuantifierFOExists& q) = 0;
			virtual void visit(QuantifierFOForall& q) = 0;
			virtual void visit(QuantifierSOExists& q) = 0;
			virtual void visit(QuantifierSOForall& q) = 0;
			virtual void visit(Declaration& d) = 0;
			virtual void visit(DeclFunction& d) = 0;
			virtual void visit(DeclType& d) = 0;
		};

		class Builder
		{
		public:
			virtual void buildNodeList(int) = 0;
			virtual void buildFormulaWithDeclarations() = 0;
			virtual void buildFormulaFunctionApp() = 0;
			virtual void buildFormulaOpApplication() = 0;
			virtual void buildFormulaQuantification() = 0;
			virtual void buildFormulaConstant(FormulaConstant& n) = 0;
			virtual void buildName(Name& n) = 0;
			virtual void buildTimeIndex(TimeIndex& t) = 0;
			virtual void buildOp(Op& op) = 0;
			virtual void buildQuantifier(Quantifier& q) = 0;
			virtual void buildDeclFunction() = 0;
			virtual void buildDeclType() = 0;
		};

		class Director
		{
			Builder* builder;
		public:
			Director(Builder*);
			virtual ~Director();
			virtual Builder* getBuilder() const;
			virtual void setBuilder(Builder*);
		};

		class VisitorDefault : public Visitor
		{
		public:
			VisitorDefault();
			virtual ~VisitorDefault();
			virtual void visit(NodeVector& ns);
			virtual void visit(Node& n);
			virtual void visit(NodeList& ns);
			virtual void visit(Formula& f);
			virtual void visit(FormulaWithDeclarations& f);
			virtual void visit(FormulaFunctionApp& f);
			virtual void visit(FormulaConstant& f);
			virtual void visit(FormulaInteger& f);
			virtual void visit(FormulaTrue& f);
			virtual void visit(FormulaFalse& f);
			virtual void visit(FormulaOpApplication& f);
			virtual void visit(FormulaQuantification& f);
			virtual void visit(Name& n);
			virtual void visit(Op& o);
			virtual void visit(OpUnary& o);
			virtual void visit(OpBinary& o);
			virtual void visit(OpNot& f);
			virtual void visit(OpGuard& f);
			virtual void visit(OpAlways& f);
			virtual void visit(OpEventually& f);
			virtual void visit(OpAnd& f);
			virtual void visit(OpOr& f);
			virtual void visit(OpImplies& f);
			virtual void visit(OpIff& f);
			virtual void visit(OpMembership& f);
			virtual void visit(OpSubset& f);
			virtual void visit(OpStructure& f);
			virtual void visit(OpIndex& f);
			virtual void visit(OpIndexTrue& f);
			virtual void visit(OpIndexValue& f);
			virtual void visit(OpIndexIndex& f);
			virtual void visit(TimeIndex& f);
			virtual void visit(OpEqual& f);
			virtual void visit(Quantifier& q);
			virtual void visit(QuantifierFO& q);
			virtual void visit(QuantifierSO& q);
			virtual void visit(QuantifierFOExists& q);
			virtual void visit(QuantifierFOForall& q);
			virtual void visit(QuantifierSOExists& q);
			virtual void visit(QuantifierSOForall& q);
			virtual void visit(Declaration& d);
			virtual void visit(DeclFunction& d);
			virtual void visit(DeclType& d);
		};


	}
}

#endif /* RMC_FORMULA_FORMULA_H */

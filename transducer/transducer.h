/*
 * transducer.h: 
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * Authors:
 *    Marcus Nilsson (marcusn@it.uu.se)
 */

#ifndef RMC_FORMULA_TRANSDUCER_H
#define RMC_FORMULA_TRANSDUCER_H

#include <gbdd/gbdd.h>
#include "alphabet.h"
#include <formula/formulamod.h>
#include <ostream>
#include <memory>

namespace rmc
{
	namespace transducer
	{
		using namespace gbdd;
		using namespace formula;

		class Transducer
		{
			Alphabet a;
			auto_ptr<StructureConstraint> constraint;

			void restrict();
			StructureConstraint* restrict(const StructureConstraint& c) const;
		public:
			typedef Alphabet::IndexedName IndexedName;

			Transducer(const Alphabet& a, StructureConstraint* c);
			Transducer(const Alphabet& new_a, const Transducer& t);
			Transducer(const Transducer& t);
			Transducer& operator=(const Transducer& t);

			StructureRelation get_relation() const;
			const Alphabet& get_alphabet() const;

			Alphabet create_alphabet() const;
			StructureConstraint::Factory* ptr_create_factory() const;

			static Transducer universe(const Alphabet& a, const StructureConstraint::Factory& factory);
			static Transducer singelton(const Alphabet& a, const StructureConstraint::Factory& factory, IndexedName i);
			static Transducer product(const Alphabet& a, const StructureConstraint::Factory& factory, IndexedName i, IndexedName j, bool (*fn)(bool v1, bool v2));
			static Transducer allpos(const Alphabet& a, const StructureConstraint::Factory& factory, Bdd p);

			Transducer product(const Transducer& t2, bool (*fn)(bool v1, bool v2)) const;
			Transducer project(Name var) const;
			Transducer project(unsigned int index) const;
			Transducer operator!() const;

			Transducer operator&(const Transducer& t2) const;
			Transducer operator|(const Transducer& t2) const;

			friend std::ostream& operator<<(std::ostream& s, const Transducer& t);
		};

		class OpTransducer : public formula::OpStructure
		{
		public:
			OpTransducer(unsigned int arity):
				formula::OpStructure(arity)
				{}

			class Args
			{
				vector<Formula::Node*> ns;
			public:
				Args(unsigned int n = 0):
					ns(n, NULL)
					{}

				Args(const vector<Formula::Node*>& ns):
					ns(ns)
					{}

				Formula::Node*& operator[](unsigned int i) { return ns[i]; }
				Formula::Node* operator[](unsigned int i) const { return ns[i]; }

				Name* as_name(unsigned int i) const;
				void push_back(Formula::Node* n) { ns.push_back(n); }
				unsigned int size() const { return ns.size(); }
			};
				
			virtual Transducer apply(const Args& args) const = 0;
		};

		class TransducerBuilder : public BuilderStack<Transducer>, private VisitorDefault
		{
			typedef Transducer::IndexedName IndexedName;
			auto_ptr<StructureConstraint::Factory> factory;
			Alphabet a;
			typedef std::vector<Alphabet::IndexedName> Names;
			Names names;
			OpTransducer::Args args;
			Name formula_name;
			TimeIndex time_index;

			class TOp;

			std::vector<TOp*> ops;

			class TOp 
			{
				unsigned int arity;
			protected:
				virtual Transducer apply(const std::deque<Transducer>& t) const = 0;
			public:
				TOp(unsigned int arity):
					arity(arity)
					{}

				virtual ~TOp() {}

				void apply(TransducerBuilder& builder) const;
			};

			class TOpNegate : public TOp
			{
				virtual Transducer apply(const std::deque<Transducer>& t) const;
			public:
				TOpNegate():
					TOp(1)
					{}
			};

			class TOpProject : public TOp
			{
				Name name;

				virtual Transducer apply(const std::deque<Transducer>& t) const;
			public:
				TOpProject(Name name):
					TOp(1),
					name(name)
					{}
			};

			class TOpGuard : public TOp
			{
				virtual Transducer apply(const std::deque<Transducer>& t) const;
			public:
				TOpGuard():
					TOp(1)
					{}
			};

			class TOpProduct : public TOp
			{
				bool (*fn)(bool, bool);

				virtual Transducer apply(const std::deque<Transducer>& t) const;
			public:
				TOpProduct(bool (*fn)(bool, bool)):
					TOp(2),
					fn(fn)
					{}
			};

			void add_op(TOp* op);
			void apply_ops();

			virtual void visit(Op& f);
			virtual void visit(OpEqual& f);
			virtual void visit(OpNot& f);
			virtual void visit(OpGuard& f);
			virtual void visit(OpImplies& f);
			virtual void visit(OpIff& f);
			virtual void visit(OpAnd& f);
			virtual void visit(OpOr& f);
			virtual void visit(OpMembership& f);
			virtual void visit(OpSubset& f);
			virtual void visit(OpStructure& f);
			virtual void visit(OpIndexTrue& f);
			virtual void visit(OpIndexValue& f);
			virtual void visit(OpIndexIndex& f);
			virtual void visit(FormulaTrue& f);
			virtual void visit(FormulaFalse& f);
			virtual void visit(FormulaInteger& f);
			virtual void visit(QuantifierFOExists& q);
			virtual void visit(QuantifierFOForall& q);
			virtual void visit(QuantifierSOExists& q);
			virtual void visit(QuantifierSOForall& q);

			Transducer product(IndexedName i, IndexedName j, bool (*fn)(bool v1, bool v2)) const;
			Transducer singelton(IndexedName i) const;
		public:
			virtual ~TransducerBuilder() {}
			TransducerBuilder(StructureConstraint::Factory* factory,
					  const Alphabet& a):
				factory(factory),
				a(a)
				{}

			virtual void buildNodeList(int) {};
			virtual void buildFormulaWithDeclarations() {};
			virtual void buildFormulaFunctionApp() {};
			virtual void buildFormulaConstant(FormulaConstant& c);
			virtual void buildFormulaOpApplication();
			virtual void buildFormulaQuantification();
			virtual void buildName(Name& n);
			virtual void buildTimeIndex(TimeIndex& t);
			virtual void buildOp(Op& op);
			virtual void buildQuantifier(Quantifier& q);
			virtual void buildDeclFunction() {};
			virtual void buildDeclType();
		};


		class OpStatic : public OpTransducer
		{
			auto_ptr<StructureConstraint> c;
			gbdd::Space* space;
		public:
			OpStatic(unsigned int arity, StructureConstraint* c, gbdd::Space* space):
				OpTransducer(arity),
				c(c),
				space(space)
				{}

			Transducer apply(const Args& args) const;
		};

		class OpSingelton : public OpStatic
		{
		public:
			OpSingelton(StructureConstraint::Factory* factory,
				    gbdd::Space* space);
		};

		class OpHighorder : public OpTransducer
		{
			std::vector<OpTransducer*> ops;

		protected:
			OpTransducer* get_op(unsigned int i) const { return ops[i]; }
		public:
			OpHighorder(unsigned int arity, OpTransducer* op):
				OpTransducer(arity)
				{ ops.push_back(op); }

			OpHighorder(unsigned int arity, OpTransducer* op1, OpTransducer* op2):
				OpTransducer(arity)
				{ ops.push_back(op1); ops.push_back(op2); }
		};

		class OpProduct : public OpHighorder
		{
			bool (*fn)(bool,bool);
		public:
			OpProduct(OpTransducer* op1,
				  OpTransducer* op2,
				  bool (*fn)(bool,bool)):
				OpHighorder(op1->getArity(), op1, op2),
				fn(fn)
				{}

			Transducer apply(const Args& args) const;
		};

		class OpInverse : public OpHighorder
		{
		public:
			OpInverse(OpTransducer* op):
				OpHighorder(op->getArity(), op)
				{}

			Transducer apply(const Args& args) const;
		};


		class OpIsMin : public OpHighorder
		{
		public:
			OpIsMin(OpTransducer* op):
				OpHighorder(1, op)
				{}

			Transducer apply(const Args& args) const;
		};
	}
}
			

#endif /* RMC_FORMULA_TRANSDUCER_H */

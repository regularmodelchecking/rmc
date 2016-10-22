/*
 * tform-transformer.h: 
 *
 * Copyright (C) 2004 Marcus Nilsson (marcusn@it.uu.se)
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
#ifndef TFORM_TRANSFORMER_H
#define TFORM_TRANSFORMER_H

#include <formula/formulamod.h>
#include <set>
#include <map>
#include <list>

namespace rmc
{
	namespace modelchecker
	{
		using namespace formula;

		class TFormTransformer : public TransformerFormula
		{
			class Context : public VisitorDefault
			{
			public:
				bool holds_t0;
				bool holds_always;
				bool holds_forall;
				typedef std::pair<Name, Quantifier*> CQuantifier;
				std::list<CQuantifier> quantifiers;
				std::set<Name> quantified;
				
				
				Context():
					holds_t0(true),
					holds_always(false),
					holds_forall(true)
					{}

				void visit(Node& n) { holds_t0 = false;holds_always=false;holds_forall=false;}
				void visit(QuantifierFOForall& q) {}
				void visit(QuantifierSOForall& q) {}
				void visit(QuantifierFOExists& q) { holds_forall = false;}
				void visit(QuantifierSOExists& q) { holds_forall = false;}
				void visit(FormulaQuantification& f)
					{
						quantifiers.push_front(CQuantifier(*(f.getVariable()), f.getQuantifier()));
						quantified.insert(*(f.getVariable()));
						f.getQuantifier()->accept(*this);
					}
						
				void visit(OpAnd& op) {}
				void visit(OpAlways& op) { holds_always = holds_t0; }

				Formula* close_formula(Formula* f) const
					{
						for (std::list<CQuantifier>::const_iterator i = quantifiers.begin();i != quantifiers.end();++i)
						{
							f = new FormulaQuantification(i->second, new Name(i->first), f);
						}

						return f;
					}

				bool is_quantified(const Name& n) const { return quantified.find(n) != quantified.end(); }
				bool is_quantified() const { return quantifiers.begin() != quantifiers.end(); }
							
			};

			std::stack<Context> context;
			NodeList* decls;

			const Context& current_context() { return context.top(); }
			void push_context(Node& n) { Context c = context.top();n.accept(c);context.push(c); }
			void pop_context() { context.pop(); }
				

			class Encoding
			{
				Node* encodes;
				Node* encoding_var;
				Name* free_var;
				Op* op_boolean_pos;
				
				Formula* get_encoding(TimeIndex* t) const;
				Formula* get_encoding(TimeIndex* t, Name* index_var) const;
			public:
				Encoding(Node* encodes,
					 Name* encoding_var,
					 Name* free_var,
					 Op* op_boolean_pos):
					encodes(encodes),
					encoding_var(encoding_var),
					free_var(free_var),
					op_boolean_pos(op_boolean_pos)
					{}

				Node* get_encodes() const { return encodes; }
				Formula* get_encoding_current() const { return get_encoding(new TimeIndex(0)); }
				Formula* get_encoding_next() const { return get_encoding(new TimeIndex(1)); }
				Formula* close_formula(Formula* f) const;
				Formula* always_constraint() const;
			};
			typedef std::map<Node*, Encoding> EncodingMap;
			EncodingMap local_encodings;
			EncodingMap ev_encodings;
			EncodingMap formula_encodings;
			std::set<Variable> free_variables;
			unsigned int next_name;

			class Constraints : public std::vector<Formula*>
			{
			public:
				void add(Formula* f) { push_back(f); }

				Formula* conjunction() const
					{
						const_iterator i = begin();

						if (i == end()) return new FormulaTrue();
						Formula* f = *i;
						++i;
						
						for (;i != end();++i)
						{
							f = new FormulaOpApplication(new OpAnd(), f, *i);
						}

						return f;
					}

				Constraints& operator|=(const Constraints& c2)
					{
						for (const_iterator i = c2.begin();i != c2.end();++i)
						{
							add(*i);
						}

						return *this;
					}
			};

			/* We are building t0 & alw(t0_alw) & ev(t1 & alw(t1_alw) & ev(alw(loop_alw) & alw(ev(loop))) */

			Constraints constr_t0;
			Constraints constr_t0_alw;
			Constraints constr_t1;
			Constraints constr_t1_alw;
			Constraints constr_loop;
			Constraints constr_loop_alw;

			/* Global eventualities that has not been scheduled */

			Constraints global_ev;
			Constraints global_alw_ev;

			const Encoding& encode_eventuality(Node* formula, Name* free_var,
							   Constraints& add_realize,
							   Constraints& add_zero);

			void schedule_eventualities(const Constraints& ev, Constraints& add_realize, Constraints& add_done0, Constraints& add_true0, Constraints& add_true1);

			void add_always(Formula* constr);
			const Encoding& encode(EncodingMap& encodings, std::string prefix, Node* formula, Name* free_var);

/**
 * Operator to use to denote positions for the encoding of booleans
 * 
 */
			Op* op_boolean_pos;

		public:
			TFormTransformer(Op* op_boolean_pos):
				next_name(0),
				op_boolean_pos(op_boolean_pos)
				{}
			virtual Node* operator()(Node* n);

			virtual void visit(Name& n);
			virtual void visit(FormulaQuantification& f);
			virtual void visit(FormulaOpApplication& f);
			virtual void visit(FormulaWithDeclarations& f);
			virtual void visit(Op& op);
			virtual void visit(OpAlways& op);
			virtual void visit(OpEventually& op);
			virtual void visit(Declaration& t);
		};
	}
}

#endif /* TFORM_TRANSFORMER_H */

/*
 * transducer.cc: 
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

#include "transducer.h"
#include <memory>

using namespace gautomata;

namespace rmc
{
namespace transducer
{

void TransducerBuilder::TOp::apply(TransducerBuilder& builder) const
{
	assert(arity <= builder.size());
	std::deque<Transducer> ts;
	for (unsigned int i = 0;i < arity;++i)
	{
		ts.push_front(builder.top());
		builder.pop();
	}
	
	builder.push(apply(ts));
}

Transducer TransducerBuilder::TOpProject::apply(const std::deque<Transducer>& t) const
{
	return t[0].project(name);
}

Transducer TransducerBuilder::TOpNegate::apply(const std::deque<Transducer>& t) const
{
	return !t[0];
}

Transducer TransducerBuilder::TOpGuard::apply(const std::deque<Transducer>& t) const
{
	return t[0].project(1);
}

Transducer TransducerBuilder::TOpProduct::apply(const std::deque<Transducer>& t) const
{
	return t[0].product(t[1], fn);
}

void Transducer::restrict()
{
	constraint.reset(restrict(*constraint));
}

StructureConstraint* Transducer::restrict(const StructureConstraint& c) const
{
	std::auto_ptr<StructureConstraint::Factory> factory(c.ptr_factory());

	auto_ptr<StructureConstraint> all (factory->ptr_forall(a.universe().normalized_bdd()));

	return c.ptr_product(*all, StructureConstraint::fn_and);
}

Transducer::Transducer(const Transducer& t):
	a(t.a),
	constraint(t.constraint->ptr_clone())
{}

Transducer& Transducer::operator=(const Transducer& t)
{
	a = t.a;
	constraint.reset(t.constraint->ptr_clone());
	
	return *this;
}

Transducer::Transducer(const Alphabet& new_a, const Transducer& t):
	a(new_a),
	constraint(t.constraint->ptr_clone())
{
	if (!(a == t.a))
	{
		constraint.reset(constraint->ptr_rename(t.a.renaming(new_a)));
	}
}

Transducer::Transducer(const Alphabet& a, StructureConstraint* n_constraint):
	a(a),
	constraint(n_constraint)
{
	if (Nfa* nfa = dynamic_cast<Nfa*>(n_constraint))
	{
		constraint.reset(RefNfa(*nfa).deterministic().ptr_minimize());
	}
}

std::ostream& operator<<(std::ostream& s, const Transducer& t)
{
	s << "Legend: " << t.a.format_symbol_legend() << std::endl;

	auto_ptr<StructureConstraint> c(t.restrict(*(t.constraint)));

	if (Nfa* nfa = dynamic_cast<Nfa*>(c.get())) // Add general printing of structures?
	{
		RefNfa p = RefNfa(*nfa).deterministic().minimize().filter_states_live();
		Alphabet::EnumeratePrintStrategy tmp(new Nfa::PrintStrategy(), t.a);
		p.print_dot(s, tmp);
	}

	return s;
}

StructureRelation Transducer::get_relation() const
{
	return StructureRelation(a.get_domains(), *auto_ptr<StructureConstraint>(restrict(*constraint)));
}

const Alphabet& Transducer::get_alphabet() const
{
	return a;
}

Alphabet Transducer::create_alphabet() const
{
	return Alphabet(a.get_space(), 2);
}

StructureConstraint::Factory* Transducer::ptr_create_factory() const
{
	return constraint->ptr_factory();
}


Transducer Transducer::product(const Transducer& t2, bool (*fn)(bool v1, bool v2)) const
{
	Alphabet new_a = a | t2.a;

	Transducer t1_norm (new_a, *this);
	Transducer t2_norm (new_a, t2);

	return Transducer(new_a, t1_norm.constraint->ptr_product(*t2_norm.constraint, fn));
}

Transducer Transducer::project(Name var) const
{
	Alphabet new_a = a;
	new_a.remove_variable(var);

	auto_ptr<StructureConstraint> restricted (restrict(*constraint));

	return Transducer(new_a, restricted->ptr_project(a.doms_of_var(var).union_all()));
}

Transducer Transducer::project(unsigned int index) const
{
	auto_ptr<StructureConstraint> restricted (restrict(*constraint));

	return Transducer(a, restricted->ptr_project(a.get_domains()[index]));
}

Transducer Transducer::operator!() const
{
	auto_ptr<StructureConstraint::Factory> factory(constraint->ptr_factory());

	Transducer all(a, factory->ptr_forall(a.universe().normalized_bdd()));

	return all.product(*this, StructureConstraint::fn_minus);
}
	
Transducer Transducer::operator&(const Transducer& t2) const
{
	return product(t2, StructureConstraint::fn_and);
}

Transducer Transducer::operator|(const Transducer& t2) const
{
	return product(t2, StructureConstraint::fn_or);
}

#if 0

Transducer Transducer::Factory::subset(IndexedName i, IndexedName j) const
{
	Alphabet a = create_alphabet();
	a.add_variable(i.get_var());
	a.add_variable(j.get_var());

	gautomata::SymbolSet posconstr = a.var_value(i, true).product(a.var_value(j, true), StructureConstraint::fn_implies);

	return Transducer(a, gautomata::RefNfa(factory->ptr_symbol(posconstr)).ptr_kleene());
}

Transducer Transducer::Factory::value(IndexedName x, IndexedName i, Alphabet::Value v) const
{
	Alphabet a = create_alphabet();
	a.add_variable(i.get_var());

	gautomata::SymbolSet posconstr = a.var_value(i, true).product(a.var_value(x, v), StructureConstraint::fn_implies);

	return Transducer(a, gautomata::RefNfa(factory->ptr_symbol(posconstr)).ptr_kleene());
}

Transducer Transducer::Factory::value_equal(IndexedName x, IndexedName i,
					    IndexedName y, IndexedName j) const
{
	Alphabet a = create_alphabet();
	a.add_variable(i.get_var());
	a.add_variable(j.get_var());

	assert(i.get_var() == j.get_var()); // Special case for now

	gautomata::SymbolSet posconstr = a.var_value(i, true).product(a.var_equal(x, y), StructureConstraint::fn_implies);

	return Transducer(a, gautomata::RefNfa(factory->ptr_symbol(posconstr)).ptr_kleene());
}

Transducer Transducer::Factory::singelton(IndexedName i) const
{
	Alphabet a = create_alphabet();
	a.add_variable(i.get_var());

	gautomata::Nfa* nfa = factory->ptr_empty();
	gautomata::State q1 = nfa->add_state(false, true);
	gautomata::State q2 = nfa->add_state(true);

	gautomata::SymbolSet is_i = a.var_value(i, true);

	nfa->add_edge(q1, !is_i, q1);
	nfa->add_edge(q1, is_i, q2);
	nfa->add_edge(q2, !is_i, q2);

	return Transducer(a, nfa);
}

Transducer Transducer::Factory::is_zero(IndexedName i) const
{
	Alphabet a = create_alphabet();
	a.add_variable(i.get_var());

	gautomata::Nfa* nfa = factory->ptr_empty();
	gautomata::State q1 = nfa->add_state(false, true);
	gautomata::State q2 = nfa->add_state(true);

	gautomata::SymbolSet is_i = a.var_value(i, true);

	nfa->add_edge(q1, is_i, q2);
	nfa->add_edge(q2, !is_i, q2);

	return Transducer(a, nfa);
}
	
Transducer Transducer::Factory::is_end(IndexedName i) const
{
	Alphabet a = create_alphabet();
	a.add_variable(i.get_var());

	gautomata::Nfa* nfa = factory->ptr_empty();
	gautomata::State q1 = nfa->add_state(false, true);
	gautomata::State q2 = nfa->add_state(true);

	gautomata::SymbolSet is_i = a.var_value(i, true);

	nfa->add_edge(q1, !is_i, q1);
	nfa->add_edge(q1, is_i, q2);

	return Transducer(a, nfa);
}

Transducer Transducer::Factory::lessthan(IndexedName i, IndexedName j) const
{
	Alphabet a = create_alphabet();
	a.add_variable(i.get_var());
	a.add_variable(j.get_var());

	gautomata::Nfa* nfa = factory->ptr_empty();
	gautomata::State q1 = nfa->add_state(false, true);
	gautomata::State q2 = nfa->add_state(false);
	gautomata::State q3 = nfa->add_state(true);

	gautomata::SymbolSet is_i = a.var_value(i, true);
	gautomata::SymbolSet is_j = a.var_value(j, true);

	nfa->add_edge(q1, !is_i & !is_j, q1);
	nfa->add_edge(q1,  is_i & !is_j, q2);
	nfa->add_edge(q2, !is_i & !is_j, q2);
	nfa->add_edge(q2, !is_i &  is_j, q3);
	nfa->add_edge(q3, !is_i & !is_j, q3);

	return Transducer(a, nfa);
}

Transducer Transducer::Factory::successor(IndexedName i, IndexedName j) const
{
	Alphabet a = create_alphabet();
	a.add_variable(i.get_var());
	a.add_variable(j.get_var());

	gautomata::Nfa* nfa = factory->ptr_empty();
	gautomata::State q1 = nfa->add_state(true, true);
	gautomata::State q2 = nfa->add_state(false);

	gautomata::SymbolSet is_i = a.var_value(i, true);
	gautomata::SymbolSet is_j = a.var_value(j, true);

	nfa->add_edge(q1, !is_i & !is_j, q1);
	nfa->add_edge(q1, !is_i & is_j, q2);
	nfa->add_edge(q2, is_i  & !is_j, q1);
	nfa->add_edge(q2, is_i  & is_j, q2);

	return Transducer(a, nfa);
}

Transducer Transducer::Factory::universe() const
{
	return Transducer(const Alphabet& a, const StructureConstraint::Factory& factory, create_alphabet(), factory->ptr_universal());
}

Transducer Transducer::Factory::empty() const
{
	return Transducer(create_alphabet(), factory->ptr_empty());
}

#endif

void TransducerBuilder::add_op(TOp* op)
{
	ops.push_back(op); 
}

void TransducerBuilder::apply_ops()
{
	for (std::vector<TOp*>::const_iterator i = ops.begin();i != ops.end();++i)
	{
		(*i)->apply(*this);
	}

	ops.clear();
	names.clear();
	args = OpTransducer::Args();
}

void TransducerBuilder::buildFormulaConstant(FormulaConstant& c)
{
	c.accept(*this);
}

void TransducerBuilder::visit(FormulaTrue& f)
{
	push(Transducer::universe(a, *factory));
}

void TransducerBuilder::visit(FormulaFalse& f)
{
	push(!Transducer::universe(a, *factory));
}

void TransducerBuilder::visit(FormulaInteger& f)
{
	args.push_back(&f);
}

Transducer Transducer::universe(const Alphabet& a, const StructureConstraint::Factory& factory)
{
	return allpos(a, factory, a.universe());
}

Transducer Transducer::allpos(const Alphabet& a, const StructureConstraint::Factory& factory, Bdd p)
{
	return Transducer(a, factory.ptr_forall(p));
}
	

Transducer Transducer::product(const Alphabet& a, const StructureConstraint::Factory& factory, IndexedName i, IndexedName j, bool (*fn)(bool v1, bool v2))
{
	Alphabet new_a = a;
	
	new_a.add_variable(i.get_var());
	new_a.add_variable(j.get_var());

	Bdd posconstr = BddSet(Domain::infinite(), new_a.var_value(i, true).product(new_a.var_value(j, true), fn)).get_bdd();

	return allpos(new_a, factory, posconstr);
}

Transducer Transducer::singelton(const Alphabet& a, const StructureConstraint::Factory& factory, IndexedName i)
{
	IndexedName X = i;
	IndexedName Y;
	IndexedName Z;

	// ex Y (Y <= X AND Y != X AND not ex Z (Z <= X AND Z != X AND Z != Y))

	Transducer t1 = product(a, factory, Z, X, StructureConstraint::fn_implies) & !product(a, factory, Z, X, StructureConstraint::fn_iff) & !product(a, factory, Z, Y, StructureConstraint::fn_iff);
	Transducer t2 = product(a, factory, Y, X, StructureConstraint::fn_implies) & !product(a, factory, Y, X, StructureConstraint::fn_iff) & !(t1.project(Z.get_var()));
	Transducer t = t2.project(Y.get_var());
	
	return t;
}	

Transducer TransducerBuilder::product(IndexedName i, IndexedName j, bool (*fn)(bool v1, bool v2)) const
{
	return Transducer::product(a, *factory, i, j, fn);
}

Transducer TransducerBuilder::singelton(IndexedName i) const
{
	return Transducer::singelton(a, *factory, i);
}

void TransducerBuilder::visit(Op& op)
{
	assert(false);
}

void TransducerBuilder::visit(OpEqual& op)
{
	push(product(names[0], names[1], StructureConstraint::fn_iff));
}

void TransducerBuilder::visit(OpMembership& op)
{
	push(product(names[0], names[1], StructureConstraint::fn_implies) & singelton(names[0]));
}

void TransducerBuilder::visit(OpSubset& op)
{
	push(product(names[0], names[1], StructureConstraint::fn_implies));
}

void TransducerBuilder::visit(OpIndexTrue& op)
{
	push(product(names[1], names[0], StructureConstraint::fn_implies));
}

void TransducerBuilder::visit(OpIndexValue& op)
{
	Alphabet new_a = a;
	IndexedName x = names[0];
	IndexedName i = names[1];
	std::string val = *(names[2].get_var().getName());

	// x is assumed to be present in a already
	new_a.add_variable(i.get_var());
	
	Bdd pos = BddSet(Domain::infinite(), new_a.var_value(i, true).product(new_a.var_value(x, val), StructureConstraint::fn_implies)).get_bdd();

	push(Transducer::allpos(new_a, *factory, pos));
}

void TransducerBuilder::visit(OpIndexIndex& op)
{
	Alphabet new_a = a;
	IndexedName x = names[0];
	IndexedName i = names[1];
	IndexedName y = names[2];
	IndexedName j = names[3];

	assert(i.get_var() == j.get_var()); // Special case for now

	new_a.add_variable(i.get_var());
	new_a.add_variable(j.get_var());

	Bdd pos = BddSet(Domain::infinite(), new_a.var_value(i, true).product(new_a.var_equal(x, y), StructureConstraint::fn_implies)).get_bdd();

	push(Transducer::allpos(new_a, *factory, pos));
}

void TransducerBuilder::buildTimeIndex(TimeIndex& t)
{
	// names = ...,x,i
	unsigned int i = names.size() - 2;
	names[i] = Transducer::IndexedName(names[i].get_var(), t.getIndex());
}

void TransducerBuilder::buildFormulaOpApplication()
{
	apply_ops();
}

void TransducerBuilder::buildFormulaQuantification()
{
	apply_ops();
}

void TransducerBuilder::buildName(Name& n)
{
	names.push_back(Transducer::IndexedName(n, 0));
	args.push_back(new Name(n));
	formula_name = n;
}

void TransducerBuilder::buildOp(Op& op)
{
	op.accept(*this);
}

void TransducerBuilder::buildQuantifier(Quantifier& q)
{
	q.accept(*this);
}

void TransducerBuilder::buildDeclType()
{
	Alphabet::ValueSet vs;
	for (unsigned int i = 1;i < names.size();++i)
	{
		vs.insert(*(names[i].get_var().getName()));
	}

	a.add_variable(names[0].get_var(), vs);
	names.clear();
	args = OpTransducer::Args();

}

void TransducerBuilder::visit(OpNot& op)
{
	add_op(new TOpNegate());
}

void TransducerBuilder::visit(OpGuard& op)
{
	add_op(new TOpGuard());
}

void TransducerBuilder::visit(OpAnd& op)
{
	add_op(new TOpProduct(StructureConstraint::fn_and));
}

void TransducerBuilder::visit(OpOr& op)
{
	add_op(new TOpProduct(StructureConstraint::fn_or));
}

void TransducerBuilder::visit(OpImplies& op)
{
	add_op(new TOpProduct(StructureConstraint::fn_implies));
}

void TransducerBuilder::visit(OpIff& op)
{
	add_op(new TOpProduct(StructureConstraint::fn_iff));
}

void TransducerBuilder::visit(OpStructure& f)
{
	try
	{
		OpTransducer& op = dynamic_cast<OpTransducer&>(f);
		if (op.getArity() != args.size())
		{
			cout << op.getArity() << " " << args.size() << endl;
		}

		assert(op.getArity() == args.size());

		push(op.apply(args));
	}
	catch (...)
	{
		assert(false);
	}
}

void TransducerBuilder::visit(QuantifierFOExists& q)
{
	Transducer t = top();pop();
	push(t & singelton(names[0]));

	add_op(new TOpProject(formula_name));
}

void TransducerBuilder::visit(QuantifierFOForall& q)
{
	Transducer t = top();pop();
	push(t | !singelton(names[0]));

	add_op(new TOpNegate());
	add_op(new TOpProject(formula_name));
	add_op(new TOpNegate());
}

void TransducerBuilder::visit(QuantifierSOExists& q)
{
	add_op(new TOpProject(formula_name));
}

void TransducerBuilder::visit(QuantifierSOForall& q)
{
	add_op(new TOpNegate());
	add_op(new TOpProject(formula_name));
	add_op(new TOpNegate());
}

Name* OpTransducer::Args::as_name(unsigned int i) const
{
	assert (i < ns.size());
	return dynamic_cast<Name*>(ns[i]);
}

OpSingelton::OpSingelton(StructureConstraint::Factory* factory,
			 gbdd::Space* space):
	OpStatic(1, 
		 StructureRelation(Domain(0,1), Transducer::singelton(Alphabet(space, 1), *factory, Transducer::IndexedName()).get_relation()).get_bdd_based().ptr_clone(),
		 space)
{}
	
Transducer OpStatic::apply(const Args& args) const
{
	Alphabet a(space, 2);
	Domain::VarMap m;

	Domains d_res;
	Domains d_source;

	for (unsigned int i = 0;i < getArity();++i)
	{
		if (Name* n = args.as_name(i))
		{
			d_source = d_source * Domain(i,1);

			a.add_variable(*n);
			d_res = d_res * a.dom_of_var(*n);
		}
	}

	StructureRelation source(d_source, *c);
	StructureRelation res(d_res, source);

	return Transducer(a, res.get_bdd_based().ptr_clone());
}
	
Transducer OpProduct::apply(const Args& args) const
{
	return get_op(0)->apply(args).product(get_op(1)->apply(args), fn);
}

Transducer OpInverse::apply(const Args& args) const
{
	Args args_inv = args;

	args_inv[0] = args[1];
	args_inv[1] = args[0];

	return get_op(0)->apply(args_inv);
}

Transducer OpIsMin::apply(const Args& args) const
{
	// not ex x : x op i

	Name x;

	Args op_args(2);
	op_args[0] = &x;
	op_args[1] = args[0];

	Transducer t = get_op(0)->apply(op_args);

	auto_ptr<StructureConstraint::Factory> factory (t.ptr_create_factory());
	Transducer x_singelton = Transducer::singelton(t.create_alphabet(), *factory, x);

	return !((t & x_singelton).project(x));
}


}
}

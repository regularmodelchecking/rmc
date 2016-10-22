/*
 * regular-composition.cc: 
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

#include "regular-composition.h"

namespace rmc
{
namespace engine
{

StructureBinaryRelation RegularComposition::Solver::evaluate_atleast_until(const RegularComposition& comp, const StructureBinaryRelation& contains)
{
	return evaluate(comp);
}

void RegularComposition::Solver::set_observer(Observer* observer) {}

RegularComposition::Solver::~Solver()
{}

StructureBinaryRelation RegularComposition::get_relation(unsigned int index) const
{
	return rels[index];
}

StructureBinaryRelation RegularComposition::get_relation(const BddSet& indices) const
{
	assert(!indices.is_empty());

	BddSet::const_iterator i = indices.begin();
	StructureBinaryRelation rel = get_relation(*i);
	++i;
	while (i != indices.end())
	{
		rel |= get_relation(*i);
		++i;
	}

	if (const Nfa* nfa = dynamic_cast<const Nfa*>(&(rel.get_bdd_based())))
	{
		gautomata::RefNfa rnfa(*nfa);
		
		rnfa = rnfa.deterministic().minimize();
		
		rel = gbdd::StructureRelation(rel.get_domains(), rnfa);
	}

	return rel;
}

StructureBinaryRelation RegularComposition::relation_between(StateSet q, StateSet r) const
{
	return get_relation(expr.edge_between(q, r));
}


RegularSet RegularComposition::get_expr() const
{
	return expr;
}

unsigned int RegularComposition::n_relations() const
{
	return rels.size();
}

void RegularComposition::init_expr_zerosym()
{
	auto_ptr<Nfa> dummy (nfa_factory->ptr_empty());

	gbdd::BddSet sym (dummy->get_space());
	sym.insert(0);

	gautomata::RefNfa a (nfa_factory->ptr_symbol(sym.get_bdd()));

	expr = RegularSet(sym.get_domain(), a);
}

RegularComposition::RegularComposition(const RegularComposition& comp2):
	rels(comp2.rels),
	nfa_factory(comp2.nfa_factory)
{}

RegularComposition::RegularComposition(const StructureBinaryRelation& rel,
				       Nfa::Factory* nfa_factory):
	rels(1, rel),
	nfa_factory(nfa_factory)
{
	init_expr_zerosym();
}

RegularComposition::RegularComposition(const BinaryRegularRelation& rel):
	rels(1, rel),
	nfa_factory(rel.get_automaton().ptr_factory())
{
	init_expr_zerosym();
}

RegularComposition::RegularComposition(const vector<StructureBinaryRelation>& rels,
				       const RegularSet& expr):
	rels(rels),
	expr(RegularSet(expr.get_domain(), gautomata::RefNfa(expr.get_automaton()).deterministic().minimize().filter_states_live()))
	{}

RegularComposition RegularComposition::kleene() const
{
	return RegularComposition(rels, 
				  RegularSet(expr.get_domain(), gautomata::RefNfa(expr.get_automaton()).kleene()));
}
	
	
RegularComposition RegularComposition::operator*(const RegularComposition& comp2) const
{
	const RegularComposition& comp1 = *this;

	vector<StructureBinaryRelation> res_rels = comp1.rels;
	unsigned int base_rels2 = res_rels.size();
	res_rels.insert(res_rels.end(), comp2.rels.begin(), comp2.rels.end());

	gbdd::Domain dom(0, gbdd::Bdd::n_vars_needed(res_rels.size()));

	gautomata::RefNfa a1 = RegularSet(dom, comp1.get_expr()).get_automaton();
	auto_ptr<gautomata::Nfa::Factory> factory (a1.ptr_factory());

	const RegularSet& expr2 = comp2.get_expr();


	// Let a2 be like expr2 but with all labels added with the base_rels2 offset
	gautomata::RefNfa a2 = factory->ptr_empty();
	{
		const gautomata::Nfa& a = expr2.get_automaton();

		a2.copy_states(a, a.states(), a.states_accepting(), a.states_starting());
	}
	gautomata::StateSet Q = a2.states();
	for (gautomata::StateSet::const_iterator i = Q.begin();i != Q.end();++i)
	{
		for (gautomata::StateSet::const_iterator j = Q.begin();j != Q.end();++j)
		{
			gbdd::BddSet label = expr2.edge_between(*i, *j);

			gbdd::BddSet new_label(dom, gbdd::Bdd(label.get_space(), false));

			for (gbdd::BddSet::const_iterator k = label.begin();k != label.end();++k)
			{
				new_label.insert(*k + base_rels2);
			}

			a2.add_edge(*i, new_label.get_bdd(), *j);
		}
	}

	return RegularComposition(res_rels, RegularSet(dom, a1 * a2));
}

}

}


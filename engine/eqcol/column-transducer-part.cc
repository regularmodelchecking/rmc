/*
 * column-transducer-part.cc: 
 *
 * Copyright (C) 2003 Marcus Nilsson (marcusn@docs.uu.se)
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

#include "column-transducer-part.h"
#include <algorithm>

using namespace gautomata;

namespace rmc
{
	namespace engine
	{
		namespace eqcol
		{
ColumnTransducerPart::Column ColumnTransducerPart::Column::operator*(const Column& c2) const
{
	Column c = *this;

	c.insert(c.end(), c2.begin(), c2.end());

	return c;
}

bool ColumnTransducerPart::Column::all_member(const gautomata::StateSet& Q) const
{
	for (Column::const_iterator i = begin();i != end();++i)
	{
		if (!Q.member(*i)) return false;
	}

	return true;
}

ColumnTransducerPart::ColumnTransducerPart(const RegularRelation& r):
	RefNfa(auto_ptr<Nfa::Factory>(r.get_automaton().ptr_factory())->ptr_empty()),
	r(r)
{}

ColumnTransducerPart::LabelPrintStrategy::LabelPrintStrategy(Nfa::PrintStrategy* rel_strategy):
	ProxyPrintStrategy(rel_strategy)
{}

string ColumnTransducerPart::LabelPrintStrategy::format_state(const Nfa& nfa, State q) const
{
	const ColumnTransducerPart& part = dynamic_cast<const ColumnTransducerPart&>(nfa);
	string s = "";

	s += '"';

	Column c = part.find_label(q);

	for (Column::const_iterator i = c.begin();i != c.end();++i)
	{
		s += ProxyPrintStrategy::format_state(nfa, *i);
		s += "\\n";
	}

	s += '"';

	return s;
}

ColumnTransducerPart::Column ColumnTransducerPart::find_label(State q) const
{
	assert(state_to_label.find(q) != state_to_label.end());

	return state_to_label.find(q)->second;
}

typedef ColumnTransducerPart::Column Column;
typedef pair<Column,Column> ColumnPair;

using gbdd::Domain;
using gbdd::BddBinaryRelation;
using gbdd::BddRelation;
using gbdd::BddEquivalenceRelation;

typedef map<ColumnPair, BddBinaryRelation> Transitions;

static Transitions add_layer(const Transitions& t, const RegularRelation& r)
{
	Transitions res;
	const Nfa& a = r.get_automaton();

	for (Transitions::const_iterator i = t.begin();i != t.end();++i)
	{
		ColumnPair cs1 = i->first;
		BddBinaryRelation on1 = i->second;

		StateSet Q = a.states();

		for(StateSet::const_iterator j1 = Q.begin();j1 != Q.end();++j1)
		{
			for(StateSet::const_iterator j2 = Q.begin();j2 != Q.end();++j2)
			{
				ColumnPair cs2 = ColumnPair(*j1, *j2);

				SymbolSet edge = a.edge_between(*j1, *j2);
				BddBinaryRelation on2(r.get_domain(0), r.get_domain(1), SymbolSet(Domain::infinite(), edge).get_bdd());

				BddBinaryRelation new_rel = on1.compose(1, on2);

				if (!new_rel.get_bdd().is_false())
				{
					res[ColumnPair(cs1.first * cs2.first, cs1.second * cs2.second)] = new_rel;
				}
			}
		}
	}

	return res;
}

State ColumnTransducerPart::find_or_add_state(const Column& c)
{
	LabelMap::const_iterator i = label_to_state.find(c);

	if (i == label_to_state.end())
	{
		i = label_to_state.insert(pair<Column, State>(c, add_state(c.all_member(r.get_automaton().states_accepting()),
									   c.all_member(r.get_automaton().states_starting())))).first;

		state_to_label[i->second] = c;
	}

	return i->second;
}

void ColumnTransducerPart::add(Level i)
{
	Transitions t;

	t[ColumnPair(Column(), Column())] = BddEquivalenceRelation::identity(r.get_automaton().get_space(),
									     r.get_domain(0),
									     r.get_domain(1));

	while(i > 0)
	{
		t = add_layer(t, r);
		i--;
	}

	for (Transitions::const_iterator j = t.begin();j != t.end();++j)
	{
		ColumnPair cs = j->first;

		State q1 = find_or_add_state(cs.first);
		State q2 = find_or_add_state(cs.second);

		SymbolSet edge(Domain::infinite(),
			       BddRelation(r.get_domains(), j->second).get_bdd());

		add_edge(q1, edge, q2);
	}
}

typedef ColumnTransducerPart::ColumnSimulation ColumnSimulation;

ColumnSimulation ColumnTransducerPart::ColumnSimulation::reverse_columns() const
{
	ColumnSimulation res;

	for (ColumnSimulation::const_iterator i = begin();i != end();++i)
	{
		ColumnPair p = *i;

		::reverse(p.first.begin(), p.first.end());
		::reverse(p.second.begin(), p.second.end());

		res.insert(p);
	}

	return res;
}

ColumnTransducerPart::ColumnSimulation ColumnTransducerPart::state_to_column(const set<Nfa::StatePair>& s) const
{
	ColumnSimulation res;

	for (set<Nfa::StatePair>::const_iterator i = s.begin();i != s.end();++i)
	{
		res.insert(ColumnPair(find_label(i->first), find_label(i->second)));
	}

	return res;
}
	
ColumnSimulation ColumnTransducerPart::find_simulation_forward(const StateSet& among) const
{
	return state_to_column(Nfa::find_simulation_forward(*this, among, among));
}

ColumnSimulation ColumnTransducerPart::find_simulation_backward(const StateSet& among) const
{
	return state_to_column(Nfa::find_simulation_backward(*this, among, among));
}

std::ostream& operator<<(std::ostream&s, const Column& c)
{
	bool is_first = true;
	for (Column::const_iterator i = c.begin();i != c.end();++i)
	{
		if (!is_first) s << " ";
		s << (*i);
		is_first = false;
	}
	return s;
}

std::ostream& operator<<(std::ostream&s, const ColumnSimulation& sim)
{
	for (ColumnSimulation::const_iterator i = sim.begin();i != sim.end();++i)
	{
		s << "(" << i->first << "," << i->second << ")";
	}
	return s;
}



}
}
}

	
		

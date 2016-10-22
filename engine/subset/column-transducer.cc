/*
 * column-transducer.cc: 
 *
 * Copyright (C) 2000 Marcus Nilsson (marcusn@docs.uu.se)
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
 *    Marcus Nilsson (marcusn@docs.uu.se)
 */

#include <column-transducer.h>
#include <signal.h>
#include <iostream>

namespace rmc
{
	namespace engine
	{
		namespace subset
		{

using namespace gautomata;

bool ColumnTransducer::is_accepting(Column x) const
{	
	return !((x & accepting_columns).get_automaton().is_false());
}		

State ColumnTransducer::add_column(bool starting, Column x)
{
	State q = automaton.add_state(is_accepting(x), starting);

	state_labels.push_back(x);

	return q;
}

bool ColumnTransducer::find_column(Column x, State& q)
{
	unsigned int i;
	for (i = 0;i < state_labels.size();++i)
	{
		if (state_labels[i] == x)
		{
			q = i;
			return true;
		}
	}

	return false;
}

static SymbolSet find_copying_states(RegularRelation rel)
{
	SymbolSet change(rel.get_domain(0) | rel.get_domain(1),
			 (!(gbdd::EquivalenceRelation::identity(rel.get_space(),
								rel.get_domain(0),
								rel.get_domain(1)))).get_bdd());
	
	StateSet productive = rel.get_automaton().states_productive();

	StateSet with_change = rel.get_automaton().reachable_predecessors(rel.get_automaton().predecessors(productive, change),
							  rel.get_automaton().alphabet());

	return rel.get_automaton().states() - with_change;
}

static bool is_idempotent(RegularRelation rel)
{
	return rel == rel.compose(1, rel);
}
	
static RegularRelation suff_relation(RegularRelation rel, StateSet states)
{
	return RegularRelation(rel.get_domains(), RefNfa(rel.get_automaton()).with_starting_accepting(states, RefNfa(rel.get_automaton()).states_accepting()).filter_states_live());
}

static StateSet find_idempotent_states(RegularRelation rel, StateSet among)
{
	StateSet idempotent = StateSet::empty(among);

	StateSet::const_iterator i = among.begin();
	for (;i != among.end();++i)
	{
		if (is_idempotent(suff_relation(rel, StateSet(among, *i))))
		{
			idempotent |= StateSet(among, *i);
		}
	}

	return idempotent;
}

void signal_quit(int sig)
{
	exit(0);
}
		
ColumnTransducer::ColumnTransducer(RegularRelation rel, Column starting_columns):
	RefNfa(auto_ptr<Nfa::Factory>(rel.get_automaton().ptr_factory())->ptr_empty()),
	automaton(*this),
	accepting_columns(RegularSet(starting_columns.get_domain(), 
				     RefNfa(auto_ptr<Nfa::Factory>(rel.get_automaton().ptr_factory())->ptr_symbol(rel.get_automaton().states_accepting())).kleene()))
							   
{
	signal(2, signal_quit);

	ColumnRelation successor_rel(rel);

	SymbolSet copying_states = find_copying_states(rel);
	SymbolSet idempotent_states = find_idempotent_states(rel, rel.get_automaton().states() - copying_states) | copying_states;

	(void) add_column(true, starting_columns.saturate(copying_states, idempotent_states));

	unsigned int current_state;
	for(current_state = 0;current_state < state_labels.size();++current_state)
	{
		cout << "State [" << current_state << "/" << state_labels.size() << "]: ";
		cout.flush();
			
		Column x = state_labels[current_state];

		// just to see 
		// cout << "column 'x': " << x.get_automaton() << endl;
		// cout << "successor_rel: " << successor_rel;
		// cout.flush();
		// cout << endl;

		vector<ColumnRelation::Successor> successors = successor_rel.successors(x);

		vector<ColumnRelation::Successor>::const_iterator i = successors.begin();
		while(i != successors.end())
		{
			State next_state;
			Column y = i->col.saturate(copying_states, idempotent_states);

			if(!find_column(y, next_state))
			{
				next_state = add_column(false, y);
			}

			automaton.add_edge(current_state, i->on, next_state);
			
			cout << ".";
			cout.flush();
			++i;
		}
		cout << endl;
	}

	signal(2, SIG_DFL);
}

RegularRelation ColumnTransducer::prepare_relations(vector<RegularRelation> rels,
						    vector<StateSet>& out_state_map)
{
	assert(rels.size() > 0);

	RegularRelation rel1 = *(rels.begin());
	gbdd::Space* space = rel1.get_space();

	Domain prefix_dom = rel1.get_domain(0) | rel1.get_domain(1);
	Space::Var discriminator = prefix_dom.higher();

	SymbolSet is_a_prefix(Domain::infinite(), gbdd::Bdd::var_true(space, discriminator));
	BNfa is_not_a_prefix = BNfa::symbol(space, !is_a_prefix).kleene();

	assert (rels.size() <= (unsigned int)(1 << prefix_dom.size()));

	BNfa res(space);

	vector<RegularRelation>::const_iterator i;
	unsigned int prefix = 0;
	for (i = rels.begin();i != rels.end();++i)
	{
		SymbolSet prefix_sym(Domain::infinite(), gbdd::Bdd::value(space, prefix_dom, prefix));
		BNfa prefix_rel = BNfa::symbol(space, prefix_sym & is_a_prefix);

		res = res | (prefix_rel * (BNfa(i->get_automaton()) & is_not_a_prefix));

		++prefix;
	}

	res = res.reverse().deterministic().minimize().reverse().filter_states_live();

	res = res.project(Domain(discriminator));

	out_state_map = vector<StateSet>();
	for (prefix = 0;prefix < rels.size();++prefix)
	{
		SymbolSet prefix_sym(prefix_dom, gbdd::Bdd::value(space, prefix_dom, prefix));

		out_state_map.push_back(res.successors(res.states_starting(), prefix_sym));
	}

	RegularRelation res_rel = RegularRelation(rel1.get_domains(), res);

	return res_rel;
}

}

}

}

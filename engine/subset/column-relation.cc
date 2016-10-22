/*
 * column-relation.cc: 
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
 *    Marcus Nilsson (marcusn@docs.uu.se), Mayank Saksena (mayanks@docs.uu.se)
 */

#include "column-relation.h"
#include <queue>

namespace rmc
{
	namespace engine
	{
		namespace subset
		{
using namespace gautomata;
using gbdd::Bdd;

State ColumnRelation::find_state(BddRelation label) const
{
  label = BddRelation(label_q.get_domains(), label);
	assert(label_to_state.find(label.get_bdd()) != label_to_state.end());

	return label_to_state.find(label.get_bdd())->second;
}

BddRelation ColumnRelation::find_label(State q) const
{
	assert(state_to_label.find(q) != state_to_label.end());

	return BddRelation(label_q.get_domains(), state_to_label.find(q)->second);
}

State ColumnRelation::add_state(BddRelation label,
				bool accepting,
				bool starting = false)
{
	State q = successor_automaton.add_state(accepting, starting);
	Bdd bdd = BddRelation(label_q.get_domains(), label).get_bdd();
	state_to_label.insert(pair<State, Bdd>(q, bdd));
	label_to_state.insert(pair<Bdd, State>(bdd, q));

	return q;
}

State ColumnRelation::find_or_add_state(BddRelation label, bool accepting, bool starting = false)
{
	hash_map<Bdd, State>::const_iterator i = label_to_state.find(BddRelation(label_q.get_domains(), label).get_bdd());

	if (i == label_to_state.end())
	{
		return add_state(label,
				 accepting,
				 starting);
	}
	else
	{
		return  i->second;
	}
}

bool ColumnRelation::label_exists(BddRelation label) const
{
	return label_to_state.find(BddRelation(label_q.get_domains(), label).get_bdd()) != label_to_state.end();
}


ColumnRelation::ColumnRelation(BinaryRegularRelation rel_trans):
	colrel(rel_trans),
	label_q(rel_trans.get_space(), Domains()),
	tape_q(rel_trans.get_space(), Domains()),
	tape_r(rel_trans.get_space(), Domains()),
	successor_automaton(rel_trans.get_space())
{
	Space *space = rel_trans.get_space();

	// Domains
	Bdd::VarPool pool;
	gbdd::Bdd::Vars vars(space);
	unsigned int n_vars_states = colrel.get_automaton().states().get_domain().size();
	unsigned int n_vars_symbols = colrel.get_domain(0).size();
	label_q = gbdd::Bdd::FiniteVars(vars[colrel.get_domains()]);
	tape_q = gbdd::Bdd::FiniteVars(vars[pool.alloc_interleaved(n_vars_symbols, 2)]);
	tape_r = gbdd::Bdd::FiniteVars(vars[pool.alloc_interleaved(n_vars_states, 2)]);

	StateSet visited(space);
	queue<State> unprocessed;
	{
	  using namespace gbdd;
	  
	  BddRelation id(label_q, label_q[0] == label_q[1]);
	  State final = add_state(id, true);
	  unprocessed.push(final);
	}

	State initial = successor_automaton.add_state(false, true); /* without label */
	
	const StateSet Q = colrel.get_automaton().states();
	
	while (!unprocessed.empty()) {

	  State s = unprocessed.front();

	  if (!visited.member(s)) {

	    BddRelation R2 = find_label(s);
	    
	    // Add transition from the initial state to s with label(s) on the edge.
	    successor_automaton.add_edge(initial, BddRelation(tape_q.get_domains(), R2).get_bdd(), s);
	    
	    for (StateSet::const_iterator r1 = Q.begin(); r1 != Q.end(); ++r1) {
	      for (StateSet::const_iterator r2 = Q.begin(); r2 != Q.end(); ++r2) {
		
		BddRelation top_bottom = colrel.edge_between(*r1, *r2);
		if (!top_bottom.is_false()) {
		  
		  BddRelation R1 = top_bottom.compose(1, R2);

		  State source;
		  if (!label_exists(R1)) {
		    source = add_state(R1, false);
		    unprocessed.push(source);
		  }
		  else
		    source = find_state(R1);
		  
		  successor_automaton.add_edge(source, tape_r[0] == *r1 & tape_r[1] == *r2, s); /* R1 -(r1, r2)-> R2 */
		}
	      }
	    }

	    visited.insert(s);
	  }

	  unprocessed.pop(); // next state...
	}

	successor_automaton = successor_automaton.deterministic().minimize();
}


vector<ColumnRelation::Successor> ColumnRelation::successors(Column col0) const
{
	vector<Successor> res;
	
	BNfa any_sym = BNfa::symbol(col0.get_space(),
				      col0.get_automaton().alphabet());
	Column col(RegularSet(col0.get_domain(), any_sym * col0.get_automaton()));

	BNfa sym_image(col0.get_space());
	{
	  sym_image = BNfa(RegularSet(tape_r[0].get_domain(), col).get_automaton());
	}

	sym_image = sym_image.filter_states_productive();

	sym_image = (sym_image & successor_automaton).project(tape_r[0].get_domain());

	sym_image = sym_image.minimize();
	sym_image = sym_image.deterministic();
	sym_image = sym_image.minimize();
	  
	StateSet states_one = sym_image.successors(sym_image.states_starting(),
						     sym_image.alphabet());
	
	StateSet::const_iterator i = states_one.begin();
	while (i != states_one.end()) {
	  Bdd on = sym_image.edge_between(sym_image.states_starting(),
					  StateSet(sym_image.states(), *i));

	  BddRelation on_rel(tape_q.get_domains(), on);
	  
	  Column y = Column(RegularSet(tape_r[1].get_domain(),
				       sym_image.with_starting_accepting(StateSet(states_one, *i),
									 sym_image.states_accepting()).filter_states_live()));
	  
	  res.push_back(Successor(y, SymbolSet(on_rel.get_bdd())));
	  
	  ++i;
	}
	
	return res;
}


ostream& operator<<(ostream& s, const ColumnRelation& r)
{
  s << "successor automaton: " << r.successor_automaton;
  return s;
}

}

}

}

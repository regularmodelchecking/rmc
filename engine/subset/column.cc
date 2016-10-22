/*
 * column.cc: 
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

#include "column.h"

namespace rmc
{
	namespace engine
	{
		namespace subset
		{
using gbdd::Bdd;
using namespace gautomata;

Column::Column(RegularSet s):
	RegularSet(Domain(0, s.get_domain().size()), s)
{
	is_canonical = false;
}

Column Column::saturate(SymbolSet copying_states, SymbolSet idempotent_states) const
{
	Column saturated(*this);
	{
		RefNfa new_saturated = saturated.get_automaton();
		// q -> qq for all idempotent states but not copying (since the loops takes care of this case)

		SymbolSet expand_states = idempotent_states - copying_states;

		SymbolSet::const_iterator i = expand_states.begin();
		for(;i != expand_states.end();++i)
		{
			Relation sat_transitions = new_saturated.transitions();

			gbdd::BinaryRelation has_expand_edge(sat_transitions.get_domain(0),
							     sat_transitions.get_domain(2),
							     sat_transitions.restrict(1, BddSet(expand_states, *i)).project(1).get_bdd());

			StateSet dest = has_expand_edge.image();

			StateSet::const_iterator current_dest = dest.begin();
			for(;current_dest != dest.end();++current_dest)
			{
				StateSet sources = has_expand_edge.range_under(StateSet(dest, *current_dest));

				State q = new_saturated.add_state(false, false);

				sources = sources.extend_domain(new_saturated.transitions().get_domain(0));

				new_saturated.add_edge(sources, BddSet(expand_states, *i), BddSet(saturated.get_automaton().states(), q));
				new_saturated.add_edge(q, BddSet(expand_states, *i), q);
				new_saturated.add_edge(q, BddSet(expand_states, *i), *current_dest);
			}
		}
		saturated = Column(RegularSet(saturated.get_domain(), new_saturated));
	}
		

	{
		RefNfa new_saturated = saturated.get_automaton();
		

		// qq -> q, for all idempotent states q

		// extra(q1, a, q2) <=> a \in idempotent_states \AND \exists q3:delta(q1, a, q3) \AND delta(q3, a, q2)
		
		Relation delta = get_automaton().transitions().restrict(1, idempotent_states);
		
		Domain dom_q1 = delta.get_domain(0);
		Domain dom_q2 = delta.get_domain(2);
		Domain dom_sym = delta.get_domain(1).first_n(copying_states.get_domain().size());
		
		Bdd::VarPool pool;
		pool.alloc(dom_q1|dom_q2|dom_sym);
		
		Domain dom_q3 = pool.alloc(dom_q1.size());
		
		Bdd extra1 = Relation(dom_q1 * dom_sym * dom_q3, delta).get_bdd();
		Bdd extra2 = Relation(dom_q3 * dom_sym * dom_q2, delta).get_bdd();
		
		Domains rel_domains = dom_q1 * dom_sym * dom_q2;
		Relation extra(rel_domains, (extra1 & extra2).project(dom_q3));
		
		new_saturated.add_transitions(extra);

		// Add loops for all copying states

		Relation loops(rel_domains,
			       Bdd::vars_equal(get_space(), dom_q1, dom_q2) &
			       (gbdd::Set(dom_sym, copying_states).get_bdd()));
		
		new_saturated.add_transitions(loops);

		saturated = Column(RegularSet(saturated.get_domain(), new_saturated));
	}



	Column min_saturated(RegularSet(saturated.get_domain(), RefNfa(saturated.get_automaton()).deterministic().minimize()));

	min_saturated.is_canonical = true;

	return min_saturated;
}

bool operator==(const Column& x, const Column &y)
{
	if (x.is_canonical && y.is_canonical)
	{
		if (x.get_automaton().states().size() != y.get_automaton().states().size())
		{
			return false;
		}
	}

	return (x.get_automaton() == y.get_automaton());
}

Column operator&(const Column& x, const Column &y)
{
	return Column((const RegularSet&)x & (const RegularSet&)y);
}

Column operator|(const Column& x, const Column &y)
{
	return Column((const RegularSet&)x | (const RegularSet&)y);
}

Column operator*(const Column& x, const Column &y)
{
	return Column(RegularSet(x.get_domain(), RefNfa(x.get_automaton()) * y.get_automaton()));
}
		
Column Column::kleene() const
{
	return Column(RegularSet(get_domain(), RefNfa(get_automaton()).kleene()));
}
	
}

}

}

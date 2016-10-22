/*
 * eq-column-transducer.cc: 
 *
 * Copyright (C) 2002 Marcus Nilsson (marcusn@docs.uu.se)
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

#include "eq-column-transducer.h"
#include "column-transducer-part.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

typedef std::pair<gbdd::Bdd,gbdd::Bdd> BddPair;

DECL_NAMESPACE_SGI
{
  template<>
	struct hash<BddPair>
	{
		size_t operator()(BddPair p) const
			{
				return hash<gbdd::Bdd>()(p.first) + hash<gbdd::Bdd>()(p.second);
			}
	};
}

namespace rmc
{
	namespace engine
	{
		namespace eqcol
		{

using namespace gautomata;

void EqColumnTransducer::SimpleWorkbench::add(gautomata::State from, gautomata::State to, const Transition& t)
{
	current.push(t);
}

void EqColumnTransducer::SimpleWorkbench::get(const EqColumnTransducer& t, Transitions& out)
{
	out = current;
	current = Transitions();
}

void EqColumnTransducer::LiveFilterWorkbench::add(gautomata::State from, gautomata::State to, const Transition& t)
{
	current.push(t);
}

void EqColumnTransducer::LiveFilterWorkbench::get(const EqColumnTransducer& et, Transitions& out)
{
	StateSet live = et.states_live();

	queue<Transition> to_process = current;
	current = queue<Transition>();

	while (!to_process.empty())
	{
		const Transition& t = to_process.front();

		if (live.member(et.find_state(t.source())))
		{
			out.push(t);
		}
		else
		{
			current.push(t);
		}

		to_process.pop();
	}
}

string EqColumnTransducer::LiveFilterWorkbench::describe()
{
	ostringstream s;
	s << current.size();
	return s.str();
}


void EqColumnTransducer::LiveSmartWorkbench::add(gautomata::State from, gautomata::State to, const Transition& t)
{
	current[from].push(t);
}

void EqColumnTransducer::LiveSmartWorkbench::get(const EqColumnTransducer& et, Transitions& out)
{
	StateSet live = et.states_live();

	for (StateSet::const_iterator i = live.begin();i != live.end();++i)
	{
		queue<Transition> to_process = current[*i];

		while (!to_process.empty())
		{
			if (live.member(et.find_state(to_process.front().destination())))
			{
				out.push(to_process.front());
			}
			else
			{
				current[*i].push(to_process.front());
			}

			to_process.pop();
		}
	}
}

string EqColumnTransducer::LiveSmartWorkbench::describe()
{
	unsigned int size = 0;

	for (hash_map<gautomata::State, queue<Transition> >::const_iterator i = current.begin();i != current.end();++i)
	{
		size += i->second.size();
	}

	ostringstream s;
	s << size;

	return s.str();
}

	

State EqColumnTransducer::add_state(EqColumnSequence label,
				    bool accepting,
				    bool starting)
{
	State q = automaton.add_state(accepting,starting);

	state_to_label[q] = label;
	label_to_state[label] = q;

	return q;
}

State EqColumnTransducer::find_state(EqColumnSequence label) const
{
	assert(label_to_state.find(label) != label_to_state.end());

	return label_to_state.find(label)->second;
}

const EqColumnSequence& EqColumnTransducer::find_label(gautomata::State q) const
{
	assert(state_to_label.find(q) != state_to_label.end());

	return state_to_label.find(q)->second;
}

State EqColumnTransducer::find_or_add_state(EqColumnSequence label)
{
	hash_map<EqColumnSequence, State>::const_iterator i = label_to_state.find(label);

	if (i == label_to_state.end())
	{
		return add_state(label,
				 label.exist_member(F),
				 label.exist_member(I));
	}
	else
	{
		return  i->second;
	}
}

bool EqColumnTransducer::label_exists(EqColumnSequence label) const
{
	return label_to_state.find(label) != label_to_state.end();
}

static RegularRelation states_relation(const RegularRelation& rel, const StateSet& states, bool forward)
{
	RefNfa automaton = rel.get_automaton();
	if (forward)
	{
		return RegularRelation(rel.get_domains(),
				       automaton.with_starting_accepting(states, automaton.states_accepting()));
	}
	else
	{
		return RegularRelation(rel.get_domains(),
				       automaton.with_starting_accepting(automaton.states_starting(), states));
	}
}		
		
static StateSet iterate_idempotency(const RegularRelation& rel, const StateSet& current, bool forward)
{
	const Nfa& automaton = rel.get_automaton();
	StateSet Q = automaton.states();
	StateSet next = StateSet::empty(Q);
	for (StateSet::const_iterator i = current.begin();i != current.end();++i)
	{
		bool all_r = true;
		for (StateSet::const_iterator j = Q.begin();j != Q.end();++j)
		{
			gbdd::BddRelation edge_rel = forward ? rel.edge_between(*i, *j) : rel.edge_between(*j, *i);

			if (edge_rel.is_false()) continue;

			if (!(current.member(*j) &&
			      edge_rel.compose(1, edge_rel) == edge_rel))
			{
				all_r = false; break; // *i does not represent idempotent relation
			}
			
			// Check that *i cannot go to two separate states that can be composed (kills simulation)

			StateSet other_states = forward ? 
				automaton.successors(StateSet(Q, *i), automaton.alphabet()) :
				automaton.predecessors(StateSet(Q, *i), automaton.alphabet());

			StateSet combinable_states = StateSet::empty(Q);

			for (StateSet::const_iterator k = other_states.begin();k != other_states.end();++k)
			{
				if (*j == *k) continue;

				BddRelation other_edge_rel = forward ? 
					rel.edge_between(*i, *k) :
					rel.edge_between(*k, *i);
			
			
				if (!(other_edge_rel.compose(1, edge_rel).is_false() &&
				      edge_rel.compose(1, other_edge_rel).is_false()))
				{
					// *i -> *j can be combined with *i -> *k
					combinable_states.insert(*k);
				}
			}

			RegularRelation combinable_states_rel = states_relation(rel, combinable_states, forward);
			RegularRelation states_rel = states_relation(rel, StateSet(Q, *j), forward);
			
				
			if (!(states_rel.compose(1, combinable_states_rel).get_automaton().is_false() &&
			      combinable_states_rel.compose(1, states_rel).get_automaton().is_false()))
			{
				all_r = false;break;
			}
		}
		if (all_r) next.insert(*i);
	}

	return next;
}

static StateSet find_idempotent_states(const RegularRelation& rel, bool forward)
{
	StateSet current = rel.get_automaton().states();
	StateSet prev = StateSet::empty(current);
	while (!(prev == current))
	{
		prev = current;
		current = iterate_idempotency(rel, current, forward);
	}

	return current;
}


static void find_copying_states(const RegularRelation& rel,
				StateSet& out_left_copying,
				StateSet& out_right_copying)
{
	SymbolSet change(rel.get_domain(0) | rel.get_domain(1),
			 (!(gbdd::EquivalenceRelation::identity(rel.get_space(),
								rel.get_domain(0),
								rel.get_domain(1)))).get_bdd());

	StateSet productive = rel.get_automaton().states_productive();
	StateSet with_suff_change = rel.get_automaton().reachable_predecessors(rel.get_automaton().predecessors(productive, change),
							       rel.get_automaton().alphabet());

	out_right_copying = rel.get_automaton().states() - with_suff_change;


	StateSet reachable = rel.get_automaton().states_reachable();
	StateSet with_pref_change = rel.get_automaton().reachable_successors(rel.get_automaton().successors(reachable, change),
							     rel.get_automaton().alphabet());

	out_left_copying = rel.get_automaton().states() - with_pref_change;
}

EqColumnTransducer::Edge EqColumnTransducer::Edge::operator*(const Edge& e2) const
{
	return Edge(compose(1, e2));
}

EqColumnTransducer::Transition EqColumnTransducer::Transition::operator*(const Transition& t2) const
{
	return Transition(source() * t2.source(),
			  Edge(edge().get_domains(), edge() * t2.edge()),
			  destination() * t2.destination());
}

ostream& operator<<(ostream &s, const EqColumnTransducer::Transition &t)
{
	s << "<" << t.source() << "," << t.edge() << "," << t.destination() << ">";

	return s;
}


void EqColumnTransducer::push_current(queue<Transition>& queue,
				      const Domain& base,
				      const Domain& d1,
				      const Domain& d2) const
{
	Domains dom_edge = Domain(base, d1) * Domain(base, d2);

	StateSet Q = automaton.states();
	for (StateSet::const_iterator i_q1 = Q.begin();i_q1 != Q.end();++i_q1)
	{
		State q1 = *i_q1;
		StateSet Q2 = automaton.successors(StateSet(Q, q1),
						   automaton.alphabet());
		
		for (StateSet::const_iterator i_q2 = Q2.begin();i_q2 != Q2.end();++i_q2)
		{
			State q2 = *i_q2;
			
			SymbolSet edge = automaton.edge_between(q1, q2);

			queue.push(Transition(find_label(q1), 
					      Edge(dom_edge, SymbolSet(base, edge).get_bdd()), 
					      find_label(q2)));
		}
	}
}			

void EqColumnTransducer::push_current_bench(Workbench& bench,
					    const Domain& base,
					    const Domain& d1,
					    const Domain& d2) const
{
	queue<Transition> ts;

	push_current(ts, base, d1, d2);
	while (!ts.empty())
	{
	const Transition& t = ts.front();

		bench.add(find_state(t.source()), find_state(t.destination()), t);

		ts.pop();
	}
}


bool EqColumnTransducer::is_bad_transition(const Transition& t) const
{
	return t.source().is_dead() || t.destination().is_dead();
}
	
template <class Key, class Data>
class HashCache
{
	hash_map<Key, Data> cache;
	unsigned int hits;
	unsigned int misses;

	typedef typename hash_map<Key, Data>::const_iterator const_iterator;
protected:
	virtual void compute(const Key& key, Data& out) = 0;
public:
	virtual ~HashCache() {}
	HashCache() :
		hits(0),
		misses(0)
		{}
	
	const Data& find(const Key& key);

	unsigned int get_hits() const { return hits; }
	unsigned int get_misses() const { return misses; }

	string statistics()
		{
			ostringstream s;

			s << "[" << hits << "/" << misses << "]";

			return s.str();
		}
};

template <class Key, class Data>
const Data& HashCache<Key, Data>::find(const Key& key)
{
	const_iterator i = cache.find(key);
	
	if (i == cache.end())
	{
		misses++;

		Data& d = cache[key];
		compute(key, d);
		return d;
	}
	else
	{
		hits++;
		return i->second;
	}
}

typedef gbdd::Bdd Bdd;

template <class Data>
class BddPairCache : public HashCache<BddPair, Data>
{
public:
	const Data& find(Bdd p, Bdd q)
		{
			return HashCache<BddPair, Data>::find(BddPair(p, q));
		}

	void compute(const BddPair& pair, Data& out)
		{
			compute(pair.first, pair.second, out);
		}
		
	virtual void compute(Bdd p, Bdd q, Data& out) = 0;
};

bool EqColumnTransducer::combine_and_add_transitions(Workbench& bench,
						     Transitions with)
{
	class DiffCache : public BddPairCache<bool>
	{
	public:
		void compute(Bdd new_edge, Bdd old_edge, bool& out)
		{
			 out = !(new_edge - old_edge).is_false();
		}
	};

	class ComposeCache : public BddPairCache<Bdd>
	{
		Domains edge_domains;
	public:
		ComposeCache(const Domains& doms) :
			edge_domains(doms)
			{}

		void compute(Bdd e1, Bdd e2, Bdd& out)
		{
			out = (Edge(edge_domains, e1) * Edge(edge_domains, e2)).get_bdd();
		}

		Edge find(Bdd e1, Bdd e2)
		{
			return Edge(edge_domains, BddPairCache<Bdd>::find(e1, e2));
		}
				
	};
				
	ComposeCache compose_cache(with.begin()->second[0].edge().get_domains());
	DiffCache diff_cache;

	Domain dom_alphabet = automaton.alphabet().get_domain();

	Transitions from;
	bench.get(*this, from);
	bool new_transition = false;
	
	string description = bench.describe();
        Transitions::size_type n_edges = from.size();
	unsigned int processing_edge = 0;

	for (Transitions::const_iterator i = from.begin();i != from.end();++i)
	{
		typedef vector<Transition> TVec;
		const TVec& ts1 = i->second;
		assert(ts1.size() > 0);
		const Edge& e1 = ts1[0].edge();

		processing_edge += ts1.size();

		{
			ostringstream s;

			s << "Combining: [" << processing_edge << "/" << n_edges << "/" << description << "] " 
			  << "Cache: " << compose_cache.statistics() << " "
			  << "DiffCache: " << diff_cache.statistics() << " "
			  << "\r";

			observer->progress_init(s.str());
		}

		for (Transitions::const_iterator j = with.begin();j != with.end();++j)
		{
			const TVec& ts2 = j->second;
			assert(ts2.size() > 0);
			const Edge& e2 = ts2[0].edge();

			Edge e = compose_cache.find(e1.get_bdd(), e2.get_bdd());
			if (!e.is_false())
			{
				for (TVec::const_iterator ti = ts1.begin();ti != ts1.end();++ti)
				{
					const Transition& t1 = *ti;
					for (TVec::const_iterator tj = ts2.begin();tj != ts2.end();++tj)
					{
						const Transition& t2 = *tj;

						Transition t (t1.source() * t2.source(),
							      e,
							      t1.destination() * t2.destination());

						if ((!label_exists(t.source()) || !label_exists(t.destination()))
						    &&
						    is_bad_transition(t))
							continue;
						
						State q = find_or_add_state(t.source());
						State r = find_or_add_state(t.destination());
						
						// Remember : We are guaranteed that all edges have the domains of orig_rel
						//            mapped onto the alphabet of automaton
						
						const Edge& new_edge = t.edge();
						Edge old_edge(new_edge.get_domains(), SymbolSet(dom_alphabet, automaton.edge_between(q, r)).get_bdd());
						
						// old_edge and new_edge now have the same domains
						
						if (diff_cache.find(new_edge.get_bdd(), old_edge.get_bdd()))
						{
							automaton.add_edge(q, SymbolSet(dom_alphabet, new_edge.get_bdd()), r);
							
							bench.add(q, r, t);							
							new_transition = true;
						}
					}
				}
			}
		}
	}
	
	observer->progress_init("\n");
	return new_transition;
}



static vector<SymbolSet> find_discriminators(const Nfa::Factory& factory, const vector<SymbolSet>& sets)
{
	RefNfa a = factory.ptr_empty();

	State q_i = a.add_state(false, true);
	State q_f = a.add_state(true);

	Domain dom_value(0, gbdd::Bdd::n_vars_needed(sets.size()));

	unsigned int index = 0;
	for (vector<SymbolSet>::const_iterator i = sets.begin();i != sets.end();++i)
	{
		State q = a.add_state(false);

		a.add_edge(q_i, *i, q);

		SymbolSet edge(dom_value, gbdd::Bdd::value(a.get_space(), dom_value, index));

		a.add_edge(q_i, edge, q_f);

		++index;
	}

	RefNfa a_det = a.deterministic();

	vector<SymbolSet> res;

	StateSet I = a_det.states_starting();
	StateSet Succ = a_det.successors(I, a_det.alphabet());


	assert(I.size() == 1);

	for (StateSet::const_iterator i = Succ.begin();i != Succ.end();++i)
	{
		res.push_back(a_det.edge_between(*(I.begin()), *i));
	}

	return res;
}


static vector<SymbolSet> find_discriminators_predecessors(const Nfa& nfa, const StateSet& to, const StateSet& among)
{
	StateSet Q = nfa.predecessors(to, nfa.alphabet()) & among;

	vector<SymbolSet> symbols;
	for (StateSet::const_iterator i = Q.begin();i != Q.end();++i)
	{
		symbols.push_back(nfa.edge_between(StateSet(Q, *i), to));
	}

	auto_ptr<Nfa::Factory> factory(nfa.ptr_factory());

	return find_discriminators(*factory, symbols);
}

static RefNfa right_determinisic(const Nfa& nfa, StateSet right_states)
{
	auto_ptr<Nfa::Factory> factory(nfa.ptr_factory());

	RefNfa res(factory->ptr_empty());

	StateSet other_states = nfa.states() - right_states;
	StateSet I = nfa.states_starting();
	StateSet F = nfa.states_accepting();

	hash_map<State,State> old_to_new = res.copy_states_and_transitions(nfa, other_states, F, I);

	hash_map<gbdd::Bdd,State> powerstate_to_state;
	Domain dom_states = F.get_domain();

	queue<StateSet> to_explore;

	StateSet right_start = F & right_states;
	powerstate_to_state[StateSet(dom_states, right_start).get_bdd()] = res.add_state(true, !(right_start & I).is_empty());
	to_explore.push(right_start);

	while(!to_explore.empty())
	{
		StateSet Q(dom_states, to_explore.front());
		to_explore.pop();

		// Invariant : to_explore contains only subsets of right_states

		assert((Q - right_states).is_empty());

		vector<SymbolSet> symbolsets = find_discriminators_predecessors(nfa, Q, right_states);

		for (vector<SymbolSet>::const_iterator i = symbolsets.begin();i != symbolsets.end();++i)
		{
			StateSet R = StateSet(dom_states, nfa.predecessors(Q, *i) & right_states);

			if (!R.is_empty())
			{
				if (powerstate_to_state.find(R.get_bdd()) == powerstate_to_state.end())
				{
					powerstate_to_state[R.get_bdd()] = res.add_state(false, !(R & I).is_empty());
					to_explore.push(R);
				}
				
				res.add_edge(powerstate_to_state[R.get_bdd()], *i, powerstate_to_state[Q.get_bdd()]);
			}
		}
	}

	for (hash_map<gbdd::Bdd, State>::const_iterator i = powerstate_to_state.begin();i != powerstate_to_state.end();++i)
	{
		StateSet Q(dom_states, i->first);
		State q = i->second;

		StateSet pred = nfa.predecessors(Q, nfa.alphabet()) - right_states;

		for (StateSet::const_iterator j = pred.begin();j != pred.end();++j)
		{
			assert(old_to_new.find(*j) != old_to_new.end());

			State r = old_to_new[*j];
			StateSet R(Q, *j); // R = {*j}, *j in pred

			res.add_edge(r, nfa.edge_between(R, Q), q);
		}
	}

	return res;
}

			
static StateSet map_stateset(const StateSet& from, hash_map<State,State>& map)
{
	StateSet to(from.get_space());
	for (StateSet::const_iterator i = from.begin();i != from.end();++i)
	{
		to.insert(map[*i]);
	}

	return to;
}

void EqColumnTransducer::set_relation_regex(vector<RegularRelation> rels, const RegularSet& orig_rels_composition)
{
	assert(rels.size() > 0);

	n_extra_startpos = 1;

	RegularSet rels_composition(orig_rels_composition.get_domain(),
				    RefNfa(orig_rels_composition.get_automaton()).deterministic().minimize().filter_states_live());
	
	auto_ptr<Nfa::Factory> nfa_factory (rels[0].get_automaton().ptr_factory());
	Space* space = rels[0].get_space();

	// Make sure all relations have the same domain
	Domains doms = rels[0].get_domains();
	for(vector<RegularRelation>::iterator i = rels.begin();i != rels.end();++i)
	{
		*i = RegularRelation(doms, *i);
	}

	final_domains = doms;

	BddBased::VarPool pool;
	for (Domains::const_iterator i = doms.begin();i != doms.end();++i) pool.alloc(*i);

	Bdd::FiniteVar w1(space, doms[0]);
	Bdd::FiniteVar w2(space, doms[1]);

	/*
	 * init_constraint is what we start with. Default to copying.
	 */
	RefNfa init_constraint = RefNfa(nfa_factory->ptr_symbol(SymbolSet(Domain::infinite(), w1 == w2))).kleene();

	{
		RefNfa comp_a = rels_composition.get_automaton();
		StateSet I = comp_a.states_starting();
		StateSet F = comp_a.states_accepting();
		StateSet Succ = comp_a.successors(I, comp_a.alphabet());

		
		if ((I & F).is_empty() &&
		    ((Succ == F) || (Succ & F).is_empty()))
		{
			using gbdd::BddSet;
			BddSet first_relations(rels_composition.get_domain(), 
					       SymbolSet(Domain::infinite(), comp_a.edge_between(I, Succ)));

			init_constraint = RefNfa(nfa_factory->ptr_empty());
			for (BddSet::const_iterator i = first_relations.begin(); i != first_relations.end();++i)
			{
				init_constraint = init_constraint | rels[*i].get_automaton();
			}

			rels_composition = RegularSet(rels_composition.get_domain(),
						      comp_a.with_starting_accepting(Succ, F).deterministic().minimize().filter_states_live());
		}
	}

	Bdd::FiniteVars rc_transition(space, pool.alloc_interleaved(rels_composition.get_automaton().states().get_domain().size(), 2));
	Bdd::FiniteVar q = rc_transition[0];
	Bdd::FiniteVar r = rc_transition[1];
	doms[0] |= q.get_domain();
	doms[1] |= r.get_domain();

	// Build A as the union of (q,r) dot rels[rels.composition.edge_between(q,r)]
	RefNfa A(nfa_factory->ptr_empty());
	RefNfa copy_qr = RefNfa(nfa_factory->ptr_symbol(SymbolSet(Domain::infinite(), q == 0 & q == r))).kleene();
	const Nfa& rc_nfa = rels_composition.get_automaton();
	StateSet rc_Q = rc_nfa.states();
	for (StateSet::const_iterator i = rc_Q.begin();i != rc_Q.end();++i)
	{
		for (StateSet::const_iterator j = rc_Q.begin();j != rc_Q.end();++j)
		{
			gbdd::BddSet relations (rels_composition.get_domain(), rc_nfa.edge_between(*i, *j));
			for (gbdd::BddSet::const_iterator r_i = relations.begin();r_i != relations.end();++r_i)
			{
				A = A | 
					RefNfa(nfa_factory->ptr_symbol(SymbolSet(Domain::infinite(), w1 == w2 & q == *i & r == *j))) *
					(rels[*r_i].get_automaton() & copy_qr);
			}
		}
	}

	A = A.deterministic().minimize().filter_states_live();

	// Run A through prepare strategy.
	// TODO: A contains is_first which is not in doms. The prepare strategy should
	//       perhaps work on automaton and not on regular relations. We assume here that the domains don't change
	RegularRelation A_rel = prepare_st->prepare(RegularRelation(doms, A));
	assert(A_rel.get_domains() == doms);

	A = A_rel.get_automaton();

	// Create automaton, the start of the algorithm.

	Bdd q_is_initial = SymbolSet(q.get_domain(), rc_nfa.states_starting()).get_bdd();
	automaton =
		RefNfa (nfa_factory->ptr_symbol(SymbolSet(Domain::infinite(), q_is_initial & q == r & w1 == w2))) *
		init_constraint;
	automaton = automaton.deterministic().minimize().filter_states_live();

	Bdd r_is_final = SymbolSet(r.get_domain(), rc_nfa.states_accepting()).get_bdd();
	final_restriction =
		RefNfa (nfa_factory->ptr_symbol(SymbolSet(Domain::infinite(), r_is_final))) *
		RefNfa (nfa_factory->ptr_universal());


	
	init_rel = RegularRelation(doms, A);
	current_r = RegularRelation(doms, automaton);
	rel_domains = doms;

	RefNfa labeling_automaton(nfa_factory->ptr_empty());
	hash_map<State,State> init_labels = labeling_automaton.copy_states_and_transitions(A, A.states(), A.states_accepting(), A.states_starting());
	hash_map<State,State> current_labels = labeling_automaton.copy_states_and_transitions(automaton, automaton.states(), automaton.states_accepting(), automaton.states_starting());

	// I and F are the set of labels that are considered to be initial and accepting

	I = labeling_automaton.states_starting();
	F = labeling_automaton.states_accepting();

	find_copying_states(init_rel, left_copying, right_copying);
	left_copying = left_copying - right_copying; 	// Ensure disjoint, keep right_copying

	left_copying = map_stateset(left_copying, init_labels);
	right_copying = map_stateset(right_copying, init_labels);

	StateSet states_compress = left_copying | right_copying;
	{
		ostringstream s;
		s << "States compress: " <<  states_compress << endl;
		observer->progress_init(s.str());
	}

	seq_eq_st.reset(eq_st->create(*this));
	seq_dead_st.reset(dead_st->create(*this));

	// Set labels for states in automaton

	state_to_label.clear();
	label_to_state.clear();
	{
		StateSet Q = automaton.states();

		for(StateSet::const_iterator i = Q.begin();i != Q.end();++i)
		{
			State q = *i;
			
			EqColumnSequence label(*seq_eq_st, *seq_dead_st, current_labels[q]);
			state_to_label[q] = label;
			label_to_state[label] = q;
		}
	}

	column_states = labeling_automaton.states();

	bench.reset(new LiveSmartWorkbench());
	push_current_bench(*bench, automaton.alphabet().get_domain(), doms[0], doms[1]);

	t_0 = Transitions();

	Domain dom_base = automaton.alphabet().get_domain();
	Domains dom_edge = Domain(dom_base, doms[0]) * Domain(dom_base, doms[1]);
	StateSet Q = A.states();
	for (StateSet::const_iterator i_q1 = Q.begin();i_q1 != Q.end();++i_q1)
	{
		State q1 = *i_q1;
		EqColumnSequence label1(*seq_eq_st, *seq_dead_st, init_labels[q1]);
		StateSet Q2 = A.successors(StateSet(Q, q1),
					   A.alphabet());
		
		for (StateSet::const_iterator i_q2 = Q2.begin();i_q2 != Q2.end();++i_q2)
		{
			State q2 = *i_q2;
			EqColumnSequence label2(*seq_eq_st, *seq_dead_st, init_labels[q2]);
			
			SymbolSet edge = A.edge_between(q1, q2);

			t_0.push(Transition(label1,
					    Edge(dom_edge, SymbolSet(dom_base, edge).get_bdd()), 
					    label2));
		}
	}
	
}

void EqColumnTransducer::set_relation_reachability(const RegularRelation& initial,
						   const RegularRelation& transition)
{
	Space* space = initial.get_automaton().get_space();
	auto_ptr<Nfa::Factory> nfa_factory(initial.get_automaton().ptr_factory());

	std::vector<RegularRelation> rels;
	rels.push_back(initial);
	rels.push_back(transition);

	BddSet sym_rels(space);
	sym_rels.insert(0);
	sym_rels.insert(1);

	BddSet sym_initial(sym_rels, 0);
	BddSet sym_always(sym_rels, 1);

	RegularSet regex(sym_rels.get_domain(),
			 (RefNfa(nfa_factory->ptr_symbol(sym_initial)) *
			  RefNfa(nfa_factory->ptr_symbol(sym_always)).kleene()).deterministic().minimize().filter_states_live());

	set_relation_regex(rels, regex);
}

BinaryRegularRelation EqColumnTransducer::get_result() const
{
	Domain internal_vars = rel_domains.union_all() - final_domains.union_all();

	RefNfa res = RefNfa(current_r.get_automaton()) & final_restriction;
	StateSet I = res.states_starting();
	for (unsigned int i = 0;i < n_extra_startpos;++i)
	{
		I = res.successors(I, res.alphabet());
	}

	res = res.with_starting_accepting(I, res.states_accepting()).project(internal_vars);

	res = res.deterministic().minimize();

	return RegularRelation(final_domains, res);
}
	
void EqColumnTransducer::set_relation(const RegularRelation& rel0)
{
	n_extra_startpos = 0;

	RegularRelation rel1 = RegularRelation(rel0.get_domains(), RefNfa(rel0.get_automaton()).deterministic().minimize().filter_states_live());
	RegularRelation rel = prepare_st->prepare(rel1);

	init_rel = rel;

	automaton = rel.get_automaton();
	rel_domains = rel.get_domains();

	I = automaton.states_starting();
	F = automaton.states_accepting();
	StateSet Q = automaton.states();
	
	find_copying_states(rel, left_copying, right_copying);

	// Ensure disjoint, keep right_copying
	left_copying = left_copying - right_copying;


	StateSet states_compress = left_copying | right_copying;
	{
		ostringstream s;
		s << "States compress: " <<  states_compress << endl;
		observer->progress_init(s.str());
	}

	seq_eq_st.reset(eq_st->create(*this));
	seq_dead_st.reset(dead_st->create(*this));

	for(StateSet::const_iterator i = Q.begin();i != Q.end();++i)
	{
		State q = *i;

		EqColumnSequence label(*seq_eq_st, *seq_dead_st, q);
		state_to_label[q] = label;
		label_to_state[label] = q;
	}

	column_states = rel.get_automaton().states();

	bench.reset(new LiveSmartWorkbench());

	Domain dom_alphabet = automaton.alphabet().get_domain();

	push_current_bench(*bench, dom_alphabet, rel.get_domain(0), rel.get_domain(1));
	
	{
		queue<Transition> t_0_queue;
		push_current(t_0_queue, dom_alphabet, rel.get_domain(0), rel.get_domain(1));
		t_0 = Transitions(t_0_queue);
	}

	RefNfa automaton_filtered(automaton.ptr_clone());
	current_r = RegularRelation(rel.get_domains(), automaton_filtered);

	observer->changed(*this);
}		
		
EqColumnTransducer::PrepareStrategy::~PrepareStrategy()
{}

RegularRelation EqColumnTransducer::NullPrepareStrategy::prepare(const RegularRelation& r) const
{
	return r;
}

RegularRelation EqColumnTransducer::RightDeterministic::prepare(const RegularRelation& r) const
{
	gautomata::StateSet left_copying;
	gautomata::StateSet right_copying;

	find_copying_states(r, left_copying, right_copying);

	return RegularRelation(r.get_domains(), right_determinisic(r.get_automaton(), right_copying));
}

RegularRelation EqColumnTransducer::RightIdemDeterministic::prepare(const RegularRelation& r) const
{
	gautomata::StateSet left_copying;
	gautomata::StateSet right_copying;

	find_copying_states(r, left_copying, right_copying);

	gautomata::StateSet left = find_idempotent_states(r, false);
	gautomata::StateSet right = find_idempotent_states(r, true);

	/*
	 * To make this compatible with LRDeadStrategy we have to make sure that after preparation
	 *
	 * 1. Right copying => right deterministic
	 * 2. Left copying & ~Right copying => left deterministic (since right is given priority)
	 *
	 * (1) is easy, just include all states that are right copying now
	 * (2) For idempotency, we only take states that are right idempotent but not left idempotent. Since
	 *     left copying implies left idempotency, we leave all left copying states (that are not right copying) alone
	 */
	
	return RegularRelation(r.get_domains(), right_determinisic(r.get_automaton(), right_copying|(right-left)));
}

EqColumnTransducer::DeadStrategy::~DeadStrategy()
{}

EqColumnSequence::DeadStrategy* EqColumnTransducer::NullDeadStrategy::create(const EqColumnTransducer& t) const
{
	return new EqColumnSequence::NoDeadStrategy();
}

EqColumnSequence::DeadStrategy* EqColumnTransducer::LRDeadStrategy::create(const EqColumnTransducer& t) const
{
	vector<StateSet> dead_sets;
	dead_sets.push_back(t.left_copying);
	dead_sets.push_back(t.right_copying);

	return new EqColumnSequence::ConsequtiveDeadStrategy(dead_sets);
}

EqColumnSequence::DeadStrategy* EqColumnTransducer::PairDeadStrategy::create(const EqColumnTransducer& t) const
{
	EqColumnSequence::ColumnDeadStrategy* st = new EqColumnSequence::ColumnDeadStrategy();

	ColumnTransducerPart t2(t.init_rel);
	t2.add(2);

	StateSet Dead = t2.states() - t2.states_live();
	for (StateSet::const_iterator i = Dead.begin();i != Dead.end();++i)
	{
		st->add_column(t2.find_label(*i));
	}
	
	return st;
}

EqColumnTransducer::EquivalenceStrategy::~EquivalenceStrategy()
{}

EqColumnSequence::EquivalenceStrategy* EqColumnTransducer::NullEquivalenceStrategy::create(const EqColumnTransducer& t) const
{
	return new EqColumnSequence::NoEquivalenceStrategy();
}

typedef ColumnTransducerPart::ColumnSimulation ColumnSimulation;

typedef ColumnTransducerPart::ColumnPair ColumnPair;
typedef ColumnTransducerPart::Column Column;

EqColumnSequence::EquivalenceStrategy* EqColumnTransducer::LREquivalenceStrategy::create(const EqColumnTransducer& t) const
{
	ostringstream s;

	StateSet right_idempotent_states = find_idempotent_states(t.init_rel, true);
	s << "Right Idempotent States: " << right_idempotent_states << endl;
	StateSet left_idempotent_states = find_idempotent_states(t.init_rel, false);
	s << "Left Idempotent States: " << left_idempotent_states << endl;
	s << t.init_rel.get_automaton();

	t.observer->progress_init(s.str());


	assert((t.right_copying - right_idempotent_states).is_empty());
	assert((t.left_copying - left_idempotent_states).is_empty());

	return new EqColumnSequence::CompressedEquivalenceStrategy(left_idempotent_states|right_idempotent_states);
}


class RepresentativeMap
{
public:
	typedef map<State,State> MapT;
	typedef vector<State> StateVector;
	typedef pair<Column, Column> Context;
	
	static const State epsilon = EqColumnSequence::EquivalenceStrategy::epsilon;
private:
	MapT rep_map;

	Column column_for(const Context& c, State q) const
		{
			StateVector v;
			if (q != epsilon) v.push_back(q);

			return Column(c.first) * Column(v) * Column(c.second);
		}

	bool is_bisim(const ColumnSimulation& sim, const Context& c, State q1, State q2) const
		{
			Column c1 = column_for(c, q1);
			Column c2 = column_for(c, q2);

			return sim.member(ColumnPair(c1, c2)) &&
				sim.member(ColumnPair(c2, c1));
		}

	void find_and_store_representative_for(const ColumnSimulation& sim, const Context& c, State q)
		{
			for (MapT::const_iterator j = rep_map.begin();j != rep_map.end();++j)
			{
				if (is_bisim(sim, c, q, j->first))
				{
					rep_map[q] = j->second;
					return;
				}
			}

			// No representative, create new

			rep_map[q] = q;
		}
public:
	typedef MapT::const_iterator const_iterator;
	const_iterator begin() const { return rep_map.begin(); }
	const_iterator end() const { return rep_map.end(); }
	const_iterator find(State q) const { return rep_map.find(q); }

	RepresentativeMap() {}

	RepresentativeMap(const ColumnSimulation& sim,
			  const StateSet& eq_states,
			  const Context& c)
		{
			for (StateSet::const_iterator i = eq_states.begin();i != eq_states.end();++i)
			{
				find_and_store_representative_for(sim, c, *i);
			}

			find_and_store_representative_for(sim, c, epsilon);
		}
};

typedef RepresentativeMap::Context Context;

typedef map<Context, RepresentativeMap> SpineRepresentativeMap;

static SpineRepresentativeMap create_representatives(const ColumnSimulation& sim, 
						     const StateSet& spine_states,
						     const StateSet& eq_states)
{
	SpineRepresentativeMap res;

	for (StateSet::const_iterator spine_i = spine_states.begin();spine_i != spine_states.end();++spine_i)
	{
		Context c_down = Context(Column(*spine_i), Column());
		Context c_up = Context(Column(), Column(*spine_i));

		res[c_down] = RepresentativeMap(sim, eq_states, c_down);
		res[c_up] = RepresentativeMap(sim, eq_states, c_up);

		for (StateSet::const_iterator spine_j = spine_states.begin();spine_j != spine_states.end();++spine_j)
		{
			Context c = Context(Column(*spine_i), Column(*spine_j));

			res[c] = RepresentativeMap(sim, eq_states, c);
		}
	}

	return res;
}

EqColumnSequence::EquivalenceStrategy* EqColumnTransducer::SimulationEquivalenceStrategy::create(const EqColumnTransducer& t) const
{
	t.observer->progress_init("Simulation Equivalence Strategy: Building approximation of T...");
	ColumnTransducerPart t2(t.init_rel);
	t2.add(1);
	t2.add(2);
	t2.add(3);
	t.observer->progress_init("\n");


	t.observer->progress_init("Simulation Equivalence Strategy: Finding right simulation...");
	ColumnSimulation sim_right = t2.find_simulation_forward(t2.states_live());
	t.observer->progress_init("\n");

	t.observer->progress_init("Simulation Equivalence Strategy: Finding left simulation...");
	ColumnSimulation sim_left = t2.find_simulation_backward(t2.states_live());
	t.observer->progress_init("\n");

	bool left_largest = sim_left.size() > sim_right.size();

	StateSet Q = t.init_rel.get_automaton().states();
	ColumnSimulation& sim = left_largest ? sim_left : sim_right;
	StateSet eq_states = left_largest ? t.left_copying : t.right_copying;
	StateSet alt_states = left_largest ? t.right_copying : t.left_copying;
	StateSet spine_states = Q - eq_states;
	StateSet context_states = Q - (t.left_copying|t.right_copying);
	
	class Strategy : public EqColumnSequence::SpineEquivalenceStrategy
	{
		SpineRepresentativeMap rep_map;
		StateSet spine_states;
		StateSet eq_states;
		StateSet context_states;
		StateSet alt_states;
	public:
		typedef EqColumnSequence::StateVector StateVector;
		Strategy(const SpineRepresentativeMap& rep_map,
			 const StateSet& states_compress,
			 const StateSet& spine_states,
			 const StateSet& eq_states,
			 const StateSet& context_states,
			 const StateSet& alt_states):
			SpineEquivalenceStrategy(states_compress, eq_states, spine_states),
			rep_map(rep_map),
			spine_states(spine_states),
			eq_states(eq_states),
			context_states(context_states),
			alt_states(alt_states)
			{}

		State representative_context(const Context& c, State q) const
			{
				assert (rep_map.find(c) != rep_map.end());
				assert (rep_map.find(c)->second.find(q) != rep_map.find(c)->second.end());

				return rep_map.find(c)->second.find(q)->second;
			}
				
		
		State representative_after(State s1, State q) const
			{
				if (!context_states.member(s1)) return q;

				return representative_context(Context(Column(s1), Column()), q);

			}
				
		State representative_before(State s2, State q) const
			{
				if (!context_states.member(s2)) return q;

				return representative_context(Context(Column(), Column(s2)), q);
			}
		State representative_between(State s1, State s2, State q) const
			{
				if (!context_states.member(s1)) return representative_before(s2, q);
				if (!context_states.member(s2)) return representative_after(s1, q);

				return representative_context(Context(Column(s1), Column(s2)), q);
			}

		StateVector concat(const StateVector& v1, const StateVector& v2) const
			{
				StateVector v = SpineEquivalenceStrategy::concat(v1, v2);

				// Look for alternation

				deque<State> pipeline;
				StateVector v_res;

				for (StateVector::const_iterator i = v.begin();i != v.end();++i)
				{
					pipeline.push_back(*i);
					if (pipeline.size() > 5)
					{
						v_res.push_back(pipeline.front());
						pipeline.pop_front();
					}

					// Check for pipeline == "RLRLR"
					
					if (pipeline.size() == 5 &&
					    pipeline[0] == pipeline[2] &&
					    pipeline[2] == pipeline[4] &&
					    pipeline[1] == pipeline[3] &&
					    alt_states.member(pipeline[0]) &&
					    eq_states.member(pipeline[1]))
					{
						// We have an alternation
						
						pipeline.pop_back();
						pipeline.pop_back();
					}
				}

				while (pipeline.size() > 0)
				{
					v_res.push_back(pipeline.front());
					pipeline.pop_front();
				}

				return v_res;
			}
	};

	SpineRepresentativeMap rep_map = create_representatives(sim, spine_states, eq_states);

	t.observer->progress_init("Simulation Strategy: (");
	t.observer->progress_init(left_largest ? "Left" : "Right");
	t.observer->progress_init(")");
	t.observer->progress_init("\n");

	return new Strategy(rep_map,
			    t.left_copying|t.right_copying,
			    spine_states,
			    eq_states,
			    context_states,
			    alt_states);
}

bool EqColumnTransducer::is_fixpoint() const
{
	observer->progress_init("composing...\r");
	RegularRelation composed(current_r.get_domains(),
				 current_r.compose(1, init_rel));

	RefNfa res = RefNfa(composed.get_automaton());

	observer->progress_init("min.........\r");res = res.minimize();
	observer->progress_init("det.........\r");res = res.deterministic();
	observer->progress_init("min2........\r");res = res.minimize();

	observer->progress_init("test........\r");
	return res <= current_r.get_automaton();
}

bool EqColumnTransducer::iterate(int max_iterations)
{
	int n_iter = 0;
	RegularRelation rel(rel_domains, automaton);

	bool reached_fixpoint = false;
	bool new_transitions = true;
	
	while((max_iterations == -1 || n_iter < max_iterations) && !(reached_fixpoint = is_fixpoint()) && new_transitions)
	{			
		observer->progress_init("Combining\r");
		new_transitions = combine_and_add_transitions(*bench, t_0);

		RefNfa automaton_filtered = automaton.filter_states_live();
		current_r = RegularRelation(rel.get_domains(), automaton_filtered);

		observer->progress_init("n_states: ");
		observer->progress_init(automaton.n_states());
		observer->progress_init(" ");
		observer->progress_init(automaton_filtered.n_states());

		RefNfa automaton_min = automaton_filtered.minimize();
		observer->progress_init(" ");
		observer->progress_init(automaton_min.n_states());

		RefNfa automaton_det = automaton_min.deterministic();
		observer->progress_init(" ");
		observer->progress_init(automaton_det.n_states());

		RefNfa automaton_detmin = automaton_det.minimize();
		observer->progress_init(" ");
		observer->progress_init(automaton_detmin.n_states());

		observer->progress_init("\n");

		ostringstream s;
		s << automaton.n_states();
		s << "/" << automaton_filtered.n_states();
		s << "/" << automaton_min.n_states();
		s << "/" << automaton_detmin.n_states();
		stats = s.str();

		current_r = RegularRelation(rel.get_domains(), automaton_detmin);

		observer->changed(*this);
		n_iter++;
	}

	if (!new_transitions)
		return true;
	
	return reached_fixpoint;
}
	
void EqColumnTransducer::set_strategy(EquivalenceStrategy* st)
{
	eq_st.reset(st);
}

void EqColumnTransducer::set_strategy(DeadStrategy* st)
{
	dead_st.reset(st);
}

void EqColumnTransducer::set_strategy(PrepareStrategy* st)
{
	prepare_st.reset(st);
}
	

#define QUEUE_METHOD

EqColumnTransducer::EqColumnTransducer(Observer* observer):
	RefNfa(NULL),
	eq_st(new LREquivalenceStrategy()),
	dead_st(new LRDeadStrategy()),
	prepare_st(new RightIdemDeterministic()),
	observer(observer),
	automaton(*this),
	final_restriction(NULL),
	debug_print_strategy(new gautomata::Nfa::PrintStrategy()),
	max_iterations(-1)
{}

EqColumnTransducer::EqColumnTransducer(const RegularRelation& rel,
				       Observer* observer):
	// Take rel as the initial automaton and set labels
	RefNfa(rel.get_automaton()),
	eq_st(new LREquivalenceStrategy()),
	dead_st(new LRDeadStrategy()),
	prepare_st(new RightIdemDeterministic()),
	observer(observer),
	automaton(*this),
	final_restriction(NULL),
	debug_print_strategy(new gautomata::Nfa::PrintStrategy()),
	max_iterations(-1)
{
	set_relation(rel);
}

EqColumnTransducer::~EqColumnTransducer()
{}

StructureBinaryRelation EqColumnTransducer::evaluate(const RegularComposition& comp)
{
	return evaluate_atleast_until(comp, gautomata::RegularRelation::universal(comp.get_expr()));
}

StructureBinaryRelation EqColumnTransducer::evaluate_atleast_until(const RegularComposition& comp, const StructureBinaryRelation& contains_str)
{
	stats = "";
	observer->progress_begin();

	BinaryRegularRelation contains(contains_str);

	vector<RegularRelation> rels;
	for (unsigned int i = 0;i < comp.n_relations();++i)
	{
		rels.push_back(comp.get_relation(i));
	}

	set_relation_regex(rels, comp.get_expr());

	unsigned int init_n_states = n_states();

	bool contains_is_all = contains.get_automaton().is_true();

	bool finished = false;
	n_iterations = 0;

	do
	{
		if (max_iterations != -1 && n_iterations >= max_iterations) break;

		observer->progress_iteration(n_iterations, n_states());

		finished = iterate(1);
		n_iterations++;

		if (!contains_is_all)
		{
			if (!((get_result() & contains).get_automaton().is_false())) finished = true;
		}

	} while (!finished);

	ostringstream s;
	s << init_n_states << "->" << stats;

	observer->progress_end(s.str());

	return get_result();
}


EqColumnTransducer::LabelPrintStrategy::LabelPrintStrategy(const EqColumnTransducer& t):
	state_to_label(t.state_to_label)
{}

string EqColumnTransducer::LabelPrintStrategy::format_state(const Nfa& nfa, State q) const
{
	assert(state_to_label.find(q) != state_to_label.end());

	const EqColumnTransducer& t = dynamic_cast<const EqColumnTransducer&>(nfa);

	EqColumnSequence::StateLabels labels;
	StateSet Q = t.column_states;
	for (StateSet::const_iterator i = Q.begin();i != Q.end();++i)
	{
		ostringstream s;
		if (t.left_copying.member(*i)) s << "L";
		if (t.right_copying.member(*i)) s << "R";
		s << *i;

		labels[*i] = s.str();
	}

	return state_to_label.find(q)->second.format(labels);
}



}
}
}



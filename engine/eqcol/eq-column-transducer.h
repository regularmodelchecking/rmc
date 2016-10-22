/*
 * eq-column-transducer.h: 
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

#ifndef RMC_ENGINE_EQCOL_EQ_COLUMN_TRANSDUCER_H
#define RMC_ENGINE_EQCOL_EQ_COLUMN_TRANSDUCER_H

#include <gautomata/gautomata.h>
#include "eq-column-sequence.h"
#include <engine/regular-composition.h>
#include <queue>
#include <vector>
#include <iostream>
#include <sstream>

namespace rmc
{
	namespace engine
	{
		namespace eqcol
		{
			using namespace std;
			
			using gautomata::RegularRelation;
			using gautomata::BinaryRegularRelation;
			
			class EqColumnTransducer : public gautomata::RefNfa, public RegularComposition::Solver
			{
			public:
				class PrepareStrategy
				{
				public:
					virtual ~PrepareStrategy();
					virtual RegularRelation prepare(const RegularRelation& r) const = 0;
				};
				
				class NullPrepareStrategy : public PrepareStrategy
				{
				public:
					RegularRelation prepare(const RegularRelation& r) const;
				};
				
				class RightDeterministic : public PrepareStrategy
				{
				public:
					RegularRelation prepare(const RegularRelation& r) const;
				};
				
				class RightIdemDeterministic : public PrepareStrategy
				{
				public:
					RegularRelation prepare(const RegularRelation& r) const;
				};
				
				class DeadStrategy
				{
				public:
					virtual ~DeadStrategy();

					virtual EqColumnSequence::DeadStrategy* create(const EqColumnTransducer& t) const = 0;
				};

				class LRDeadStrategy : public DeadStrategy
				{
				public:
					EqColumnSequence::DeadStrategy* create(const EqColumnTransducer& t) const;
				};

				class PairDeadStrategy : public DeadStrategy
				{
				public:
					EqColumnSequence::DeadStrategy* create(const EqColumnTransducer& t) const;
				};

				class NullDeadStrategy : public DeadStrategy
				{
				public:
					EqColumnSequence::DeadStrategy* create(const EqColumnTransducer& t) const;
				};

				class EquivalenceStrategy
				{
				public:
					virtual ~EquivalenceStrategy();
					virtual EqColumnSequence::EquivalenceStrategy* create(const EqColumnTransducer& t) const = 0;
				};

				class LREquivalenceStrategy : public EquivalenceStrategy
				{
				public:
					EqColumnSequence::EquivalenceStrategy* create(const EqColumnTransducer& t) const;
				};

				class SimulationEquivalenceStrategy : public EquivalenceStrategy
				{
				public:
					EqColumnSequence::EquivalenceStrategy* create(const EqColumnTransducer& t) const;
				};

				class NullEquivalenceStrategy : public EquivalenceStrategy
				{
				public:
					EqColumnSequence::EquivalenceStrategy* create(const EqColumnTransducer& t) const;
				};


			private:
				gautomata::State find_state(EqColumnSequence label) const;
				gautomata::State add_state(EqColumnSequence label, bool accepting, bool starting);
				gautomata::State find_or_add_state(EqColumnSequence label);
				bool label_exists(EqColumnSequence label) const;

				const EqColumnSequence& find_label(gautomata::State q) const;

				class Edge : public gbdd::BinaryRelation
				{
				public:
					Edge() {}

					Edge(const gbdd::Domain& d1, const gbdd::Domain& d2, const gautomata::SymbolSet& on):
						gbdd::BinaryRelation(d1, d2,
								     gbdd::Bdd(gbdd::Set(gbdd::Domain::infinite(),
											 on).get_bdd()))
						{}
					Edge(const gbdd::Domains& ds, const gautomata::SymbolSet& on):
						gbdd::BinaryRelation(ds[0], ds[1],
								     gbdd::Bdd(gbdd::Set(gbdd::Domain::infinite(),
											 on).get_bdd()))
						{}
					Edge(const gbdd::BinaryRelation& r):
						gbdd::BinaryRelation(r)
						{}

					Edge(const gbdd::Domains& ds, const Edge& e):
						gbdd::BinaryRelation(ds[0],ds[1], e)
						{}

					Edge(const gbdd::Domains& ds, const gbdd::Bdd& b):
						gbdd::BinaryRelation(ds[0], ds[1], b)
						{}
			
					Edge operator*(const Edge& e2) const;
				};
		
				class Transition
				{
					EqColumnSequence e1;
					Edge on;
					EqColumnSequence e2;
				public:
					Transition(const EqColumnSequence& e1, const Edge& on, const EqColumnSequence& e2):
						e1(e1),
						on(on),
						e2(e2)
						{}
			
					const EqColumnSequence& source () const { return e1; }
					const EqColumnSequence& destination () const { return e2; }
					const Edge& edge () const { return on; }
			
					Transition operator*(const Transition& t2) const;

					friend ostream& operator<<(ostream &s, const Transition &t);
				};
				friend ostream& operator<<(ostream &s, const Transition &t);


				class Transitions : public hash_map<gbdd::Bdd, vector<Transition> >
					{
					public:
						Transitions() {}
						Transitions(queue<Transition> ts)
							{
								while (!ts.empty())
								{
									push(ts.front());
									ts.pop();
								}
							}

						void push(const Transition& t) { (*this)[t.edge().get_bdd()].push_back(t); }
						typedef unsigned int size_type;

						size_type size() const
							{
								size_type s = 0;
								for (const_iterator i = begin();i != end();++i)
								{
									s += i->second.size();
								}

								return s;
							}
					};

				friend class Workbench;

				class Workbench
				{
				public:
					virtual ~Workbench() {}
					virtual void add(gautomata::State from, gautomata::State to, const Transition& t) = 0;
					virtual void get(const EqColumnTransducer& t, Transitions& out) = 0;
					virtual string describe() { return ""; };
				};

				class SimpleWorkbench : public Workbench
				{
					Transitions current;
				public:
					virtual void add(gautomata::State from, gautomata::State to, const Transition& t);
					virtual void get(const EqColumnTransducer& t, Transitions& out);
				};

				class LiveFilterWorkbench : public Workbench
				{
					queue<Transition> current;
				public:
					virtual void add(gautomata::State from, gautomata::State to, const Transition& t);
					virtual void get(const EqColumnTransducer& t, Transitions& out);
					virtual string describe();
				};

				class LiveSmartWorkbench : public Workbench
				{
					hash_map<gautomata::State, queue<Transition> > current;
				public:
					virtual void add(gautomata::State from, gautomata::State to, const Transition& t);
					virtual void get(const EqColumnTransducer& t, Transitions& out);
					virtual string describe();
				};

/**
 * Equivalence strategy, defines the equivalences between states.
 * 
 */
				auto_ptr<EquivalenceStrategy> eq_st;
/**
 * Dead strategy, defines when to declare states as dead. Transitions that have a dead source or a dead target will
 * not be added to the transducer.
 * 
 */
				auto_ptr<DeadStrategy> dead_st;
/**
 * Prepare strategy. The initial transducer will be prepared using this strategy, and the output is the transducer
 * that will actually be used in the algorithm.
 * 
 */
				auto_ptr<PrepareStrategy> prepare_st;

/**
 * Equivalence strategy for sequences, derived from eq_st
 * 
 */
				auto_ptr<EqColumnSequence::EquivalenceStrategy> seq_eq_st;
/**
 * Dead strategy for sequences, derived from dead_st
 * 
 */
				auto_ptr<EqColumnSequence::DeadStrategy> seq_dead_st;
/**
 * Observer to be used when the transducer changes, as the algorithm iterates
 * 
 */
				auto_ptr<Observer> observer;
/**
 * Automaton we are working on, points to same as this
 * 
 */
				gautomata::RefNfa& automaton;
/**
 * The domains of the relation we are working on. automaton and rel_domains defines the relation.
 * 
 */
				gbdd::Domains rel_domains;
/**
 * The initial relation. This is the relation we add each time to our current relation.
 * 
 */
				RegularRelation init_rel;

/**
 * The final domains of the relation, after projecting away all extra internal variables added by set_relation_..
 * 
 */

				gbdd::Domains final_domains;
				RefNfa final_restriction;
				unsigned int n_extra_startpos;
/**
 * Maps labels to state in automaton
 * 
 */
				hash_map<EqColumnSequence, gautomata::State> label_to_state;
/**
 * Maps state in automaton to its label
 * 
 */
				hash_map<gautomata::State, EqColumnSequence> state_to_label;

/**
 * Imaginary column states, used to create columns. Used for printing.
 * 
 */
				gautomata::StateSet column_states;

/**
 * The set of left copying states in labeling automaton
 * 
 */
				gautomata::StateSet left_copying;
/**
 * The set of right copying states in labeling automaton
 * 
 */
				gautomata::StateSet right_copying;

/**
 * The set of initial states of the labeling transducer. This is used to decide whether a new state is initial or not.
 * 
 */
				gautomata::StateSet I;
/**
 * The set of final states of the labeling transducer. This is used to decide whether a new states is final or not.
 * 
 */
				gautomata::StateSet F;
		
/**
 * Workbench that defines the strategy used to keep track of transitions we have to combine with the initial transducer.
 * 
 */
				auto_ptr<Workbench> bench;
/**
 * The transitions of the initial transducer. Transitions from bench are combined with these transitions in every iteration.
 * 
 */
				Transitions t_0;
/**
 * The relation so far. This may be totally different from automaton, but has the same language. It is used to compare
 * with init_rel to check for fixpoint.
 * 
 */
				RegularRelation current_r;

				auto_ptr<gautomata::Nfa::PrintStrategy> debug_print_strategy;

/**
 * Statistics about current iteration. Is passed to progress_end() at end of evaluation
 * 
 */

				std::string stats;
/**
 * Number of iterations so far, for statistics
 * 
 */
				int n_iterations;

/**
 * Max number of iterations, -1 for unlimited
 * 
 */

				int max_iterations;


				bool is_bad_transition(const Transition& t) const;

				bool combine_and_add_transitions(Workbench& bench, Transitions with);

				void push_current(queue<Transition>& queue, const gbdd::Domain& base, const gbdd::Domain& d1, const gbdd::Domain& d2) const;
				void push_current_bench(Workbench& bench, const gbdd::Domain& base, const gbdd::Domain& d1, const gbdd::Domain& d2) const;

			public:

				EqColumnTransducer(Observer* observer = new StreamObserver(std::cout));
				EqColumnTransducer(const RegularRelation& rel, Observer* observer = new StreamObserver(std::cout));
				~EqColumnTransducer();

				StructureBinaryRelation evaluate(const RegularComposition& comp);
				StructureBinaryRelation evaluate_atleast_until(const RegularComposition& comp, const StructureBinaryRelation& contains);

				unsigned int get_n_iterations() const { return (unsigned int)n_iterations; }

				void set_relation(const RegularRelation& rel);
				void set_relation_regex(vector<RegularRelation> rels, const gautomata::RegularSet& rels_composition);
				void set_relation_reachability(const RegularRelation& initial,
							       const RegularRelation& transition);

				void set_max_iterations(int max_iterations = -1)
					{
						this->max_iterations = max_iterations;
					}

				void set_strategy(EquivalenceStrategy* st);
				void set_strategy(DeadStrategy* st);
				void set_strategy(PrepareStrategy* st);
				void set_strategy(gautomata::Nfa::PrintStrategy* st) { debug_print_strategy.reset(st); }

				void set_observer(Observer* ob) { observer.reset(ob); }
				void set_context(std::string context) { observer->set_context(context); }

				bool iterate(int max_iterations = -1);
				bool is_fixpoint() const;
				BinaryRegularRelation get_result() const;

				gautomata::StateSet get_left_copying() const { return left_copying; }
				gautomata::StateSet get_right_copying() const { return right_copying; }
				RegularRelation get_initial_relation() const { return init_rel; }

				friend class LabelPrintStrategy;
				
				class LabelPrintStrategy : public Nfa::PrintStrategy
				{
					hash_map<gautomata::State, EqColumnSequence> state_to_label;
				public:
					virtual ~LabelPrintStrategy() {};
					LabelPrintStrategy(const EqColumnTransducer& t);

					virtual string format_state(const gautomata::Nfa& nfa, gautomata::State q) const;
				};
			};
		}
	}
}

#endif /* RMC_ENGINE_EQCOL_EQ_COLUMN_TRANSDUCER_H */

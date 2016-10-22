/*
 * eq-column-sequence.h: 
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

#ifndef RMC_ENGINE_EQCOL_EQ_COLUMN_SEQUENCE_H
#define RMC_ENGINE_EQCOL_EQ_COLUMN_SEQUENCE_H

#include <gautomata/gautomata.h>
#include <vector>
#include <string>
#include <map>

namespace rmc
{
	namespace engine
	{
		namespace eqcol
		{
			using namespace std;
	
			class EqColumnSequence
			{
			public:
				typedef gautomata::State State;
				typedef map<gautomata::State, string> StateLabels;
				typedef vector<gautomata::State> StateVector;
				typedef gautomata::StateSet StateSet;

				class EquivalenceStrategy
				{
				public:
					static const State epsilon = (unsigned int)-1;
			
					virtual ~EquivalenceStrategy() = 0;

					virtual StateVector concat(const StateVector& v1,
								   const StateVector& v2) const;

					virtual bool exist_member(const StateVector& v, gautomata::StateSet s) const;
					virtual bool exist_member_with_context(State q, 
									       gautomata::StateSet s,
									       const StateVector& before,
									       const StateVector& after) const;

					virtual StateVector initial(State q) const;

					virtual string format(const StateLabels& labels, gautomata::State q) const;
					virtual string format_with_context(const StateLabels& labels, gautomata::State q, const StateVector& before, const StateVector& after) const;
					virtual string format_before(const StateLabels& labels, gautomata::State q) const;
					virtual string format_between(const StateLabels& labels, gautomata::State q, gautomata::State r) const;
					virtual string format_after(const StateLabels& labels, gautomata::State q) const;
				};

				class DeadStrategy
				{
				public:
					virtual ~DeadStrategy() = 0;

					virtual bool is_dead(const StateVector& v) const;
				};

				class NoEquivalenceStrategy : public EquivalenceStrategy
				{
				public:
					virtual ~NoEquivalenceStrategy();
				};

				class CompressedEquivalenceStrategy : public EquivalenceStrategy
				{
					StateSet _states_compress;
				public:
					CompressedEquivalenceStrategy(const StateSet& states_compress);
					virtual ~CompressedEquivalenceStrategy();

					virtual string format(const StateLabels& labels, gautomata::State q) const;
					virtual StateVector concat(const StateVector& v1, const StateVector& v2) const;
				};

				class SpineEquivalenceStrategy : public CompressedEquivalenceStrategy
				{
					StateSet eq_states;
					StateSet spine_states;
					string format_with_context(const StateLabels& labels, 
								   gautomata::State q, 
								   const StateVector& before, 
								   const StateVector& after) const;
			
					State representative_with_context(const StateVector& before,
									  const StateVector& after,
									  State q) const;
				protected:
					virtual State representative_after(State s1, State q) const = 0;
					virtual State representative_before(State s2, State q) const = 0;
					virtual State representative_between(State s1, State s2, State q) const = 0;
					bool is_epsilon(State q) const { return q == epsilon; }
				public:
					SpineEquivalenceStrategy(const StateSet& states_compress,
								 const StateSet& eq_states,
								 const StateSet& spine_states);

					virtual StateVector concat(const StateVector& v1, const StateVector& v2) const;
					virtual bool exist_member_with_context(State q, 
									       gautomata::StateSet s,
									       const StateVector& before,
									       const StateVector& after) const;
					virtual StateVector initial(State q) const;
				};

				class NoDeadStrategy : public DeadStrategy
				{
				public:
					virtual ~NoDeadStrategy();
				};

				class ConsequtiveDeadStrategy : public DeadStrategy
				{
					vector<gautomata::StateSet> dead_sets;
				public:
					virtual ~ConsequtiveDeadStrategy();

					ConsequtiveDeadStrategy(const vector<gautomata::StateSet>& dead_sets);
					bool is_dead(const StateVector& v) const;
				};

				class ColumnDeadStrategy : public DeadStrategy
				{
					set<StateVector> dead_columns;
				public:
					virtual ~ColumnDeadStrategy();
					ColumnDeadStrategy() {};

					void add_column(const StateVector& v);
					bool is_dead(const StateVector& v) const;
				};
			


			private:

				/// Vector of states that must be in the column
				vector<gautomata::State> _sequence;

/**
 * Strategy to use for quotening
 * 
 */
				const EquivalenceStrategy* eq_st;
				const DeadStrategy* dead_st;

				EqColumnSequence(const EquivalenceStrategy& eq_st, 
						 const DeadStrategy& dead_st, 
						 const vector<gautomata::State>& sequence);

			public:
				EqColumnSequence();


				friend EqColumnSequence operator*(const EqColumnSequence& s1,
								  const EqColumnSequence& s2);

				friend bool operator==(const EqColumnSequence& s1,
						       const EqColumnSequence& s2);

				string format(const StateLabels& labels) const;

				bool exist_member(gautomata::StateSet s) const;
				bool is_dead() const;

				EqColumnSequence(const EquivalenceStrategy& eq_st, 
						 const DeadStrategy& dead_st,
						 gautomata::State q);

				friend struct hash<EqColumnSequence>;

				friend ostream& operator<<(ostream &s, const EqColumnSequence &seq);
			};
		}
	}
}
DECL_NAMESPACE_SGI
{
template<>
struct hash<rmc::engine::eqcol::EqColumnSequence>
{
	size_t operator()(rmc::engine::eqcol::EqColumnSequence s) const;
};

}


#endif /* RMC_ENGINE_EQCOL_EQ_COLUMN_SEQUENCE_H */

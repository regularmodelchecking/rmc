/*
 * column-transducer-part.h: 
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
#ifndef RMC_ENGINE_EQCOL_COLUMN_TRANSDUCER_PART_H
#define RMC_ENGINE_EQCOL_COLUMN_TRANSDUCER_PART_H

#include <gautomata/gautomata.h>
#include <map>

namespace rmc
{
	namespace engine
	{
		namespace eqcol
		{
			using namespace std;

			class ColumnTransducerPart : public gautomata::RefNfa
			{
			public:
				typedef gautomata::State State;
				class Column : public vector<State>
				{
				public:
					Column()
						{}
				
					Column(State q):
						vector<State>(1, q)
						{}

					Column(const vector<State>& v):
						vector<State>(v)
						{}

					Column operator*(const Column& c2) const;

					bool all_member(const gautomata::StateSet& Q) const;
					friend std::ostream& operator<<(std::ostream&s, const Column& c);


				};
				typedef pair<Column,Column> ColumnPair;

				class ColumnSimulation : public set<ColumnPair>
				{
				public:
					ColumnSimulation reverse_columns() const;

					bool member(const ColumnPair& p) const { return find(p) != end(); }

					friend std::ostream& operator<<(std::ostream&s, const ColumnSimulation& sim);
				};

			private:
				ColumnSimulation state_to_column(const set<Nfa::StatePair>& s) const;

				typedef map<State, Column> StateMap;
				typedef map<Column, State> LabelMap;
				StateMap state_to_label;
				LabelMap label_to_state;

				gautomata::RegularRelation r;

				State find_or_add_state(const Column& c);
			public:
				ColumnTransducerPart(const gautomata::RegularRelation& r);

				friend class LabelPrintStrategy;

				class LabelPrintStrategy : public gautomata::Nfa::ProxyPrintStrategy
				{
				public:
					virtual ~LabelPrintStrategy() {}
					LabelPrintStrategy(gautomata::Nfa::PrintStrategy* rel_strategy);

					virtual string format_state(const gautomata::Nfa& nfa, gautomata::State q) const;
				};

				typedef unsigned int Level;

				void add(Level i);
				Column find_label(State q) const;

				ColumnSimulation find_simulation_forward(const gautomata::StateSet& among) const;
				ColumnSimulation find_simulation_backward(const gautomata::StateSet& among) const;
			};
		}
	}
}

#endif /* RMC_ENGINE_EQCOL_COLUMN_TRANSDUCER_PART_H */

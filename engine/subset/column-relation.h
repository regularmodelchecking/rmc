/*
 * column-relation.h: 
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

#ifndef RMC_ENGINE_SUBSET_COLUMN_RELATION_H
#define RMC_ENGINE_SUBSET_COLUMN_RELATION_H

#include <gautomata/gautomata.h>
#include "column.h"

#include <vector>
#include <gbdd/bdd.h>

namespace rmc
{
	namespace engine
	{
		namespace subset
		{
			using namespace std;
			
			using gautomata::RegularRelation;
			using gautomata::BinaryRegularRelation;
			using gautomata::BNfa; 
			using gautomata::State;
			using gautomata::BddRelation;
			using gbdd::Domain;
			using gbdd::Domains;
			using gbdd::Bdd;
			
			class ColumnRelation
			{
				RegularRelation colrel;
				
				gbdd::Bdd::FiniteVars label_q;
				gbdd::Bdd::FiniteVars tape_q;
				gbdd::Bdd::FiniteVars tape_r;
				
				gautomata::BNfa successor_automaton;
				
			public:
				ColumnRelation(BinaryRegularRelation rel);
				
				class Successor
				{
				public:
					Successor(Column col, gautomata::SymbolSet on):
						col(col), on(on)
						{}
					
					Column col;
					gautomata::SymbolSet on;
				};
				
				vector<Successor> successors(Column col) const;
				
				friend ostream& operator<<(ostream& s, const ColumnRelation& r);
				
			private:
				State find_state(BddRelation label) const;
				BddRelation find_label(State q) const;
				State add_state(BddRelation label, bool accepting, bool starting);
				State find_or_add_state(BddRelation label, bool accepting, bool starting);
				bool label_exists(BddRelation label) const;
				
				/**
				 * Maps label to state in automaton. Each label represents a BddRelation.
				 * 
				 */
				hash_map<Bdd, State> label_to_state;
				
				/**
				 * Maps state in automaton to its label. Each label represents a BddRelation.
				 * 
				 */
				hash_map<State, Bdd> state_to_label;
			};
		}
	}
}

#endif /* RMC_ENGINE_SUBSET_COLUMN_RELATION_H */

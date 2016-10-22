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
 *    Marcus Nilsson (marcusn@it.uu.se), Mayank Saksena (mayanks@it.uu.se)
 */

#include "column-transducer-solver.h"

namespace rmc
{
	namespace engine
	{
		namespace subset
		{

using namespace gautomata;

ColumnTransducerSolver::~ColumnTransducerSolver()
{}

StructureBinaryRelation ColumnTransducerSolver::evaluate(const RegularComposition& comp)
{
	observer->progress_begin();

	vector<RegularRelation> rels;
	for (unsigned int i = 0; i < comp.n_relations(); ++i)
	{
		rels.push_back(comp.get_relation(i));
	}

	vector<gautomata::StateSet> state_map;
	RegularRelation rel = ColumnTransducer::prepare_relations(rels, state_map);

	gbdd::BddBinaryRelation rel_state_map = BddRelation::enumeration(state_map);
	rel_state_map = rel_state_map.inverse();

	RefNfa::Factory factory(rel.get_automaton().ptr_factory());

	BinaryRegularRelation regrel_state_map =
		BinaryRegularRelation(rel_state_map.get_domain(0), rel_state_map.get_domain(1),
				      RefNfa(factory.ptr_symbol(rel_state_map.get_bdd())).kleene());

	RegularSet initial(regrel_state_map.image_under(comp.get_expr()));

	ColumnTransducer transducer(rel, initial);
	BinaryRegularRelation res(rel.get_domain(0), rel.get_domain(1), transducer);

	observer->progress_end();

	return res;
}

}
}
}





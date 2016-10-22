/*
 * eqsize-solver.cc: 
 *
 * Copyright (C) 2005 Marcus Nilsson (marcus@it.uu.se)
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
 *    Marcus Nilsson (marcus@it.uu.se)
 */

#include "eqsize-solver.h"

namespace rmc
{
	namespace engine
	{
		namespace eqcol
		{

using namespace gautomata;

EqsizeSolver::~EqsizeSolver()
{}		      
			
StructureBinaryRelation EqsizeSolver::_evaluate(const RegularComposition& comp, const StructureBinaryRelation* contains)
{
	ostringstream stats;

	observer->progress_begin();

	class StatsObserver : public rmc::engine::RegularComposition::Solver::Observer
	{
		Observer* o;
	public:
		StatsObserver(Observer* o):
			o(o)
			{};

		std::string stats;

		void progress_iteration(unsigned int iteration, unsigned int size) const
			{
				o->progress_iteration(iteration, size);
			}

		void progress_end(std::string stats) const
			{
				((StatsObserver*)this)->stats = stats;
			}
	};

	StatsObserver* eq_o = new StatsObserver(observer.get());

	EqColumnTransducer eq_solver(eq_o);
	StructureBinaryRelation res = contains == NULL ? eq_solver.evaluate(comp) : eq_solver.evaluate_atleast_until(comp, *contains);

	stats << eq_o->stats;

	unsigned int n_iterations = eq_solver.get_n_iterations();

	eq_solver.set_max_iterations(n_iterations);
	eq_solver.set_strategy(new EqColumnTransducer::NullEquivalenceStrategy());
	if (contains == NULL) eq_solver.evaluate(comp);
	else eq_solver.evaluate_atleast_until(comp, *contains);

	stats << " " << eq_o->stats;
	
	observer->progress_end(stats.str());

	return res;
}


		}
	}
}

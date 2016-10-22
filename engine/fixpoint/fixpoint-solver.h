/*
 * fixpoint-solver.h: 
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
 *    Marcus Nilsson (marcusn@it.uu.se)
 *    Ahmed Rezine   (rahmed@it.uu.se)
 *    Mayank Saksena (mayanks@it.uu.se)
 */

#ifndef RMC_ENGINE_FIXPOINT_FIXPOINT_SOLVER_H
#define RMC_ENGINE_FIXPOINT_FIXPOINT_SOLVER_H

#include <engine/regular-composition.h>
#include <engine/acceleration-strategy.h>

namespace rmc
{
	namespace engine
	{
		namespace fixpoint
		{
			using namespace std;
			using rmc::engine::AccelerationStrategy;

			class FixpointSolver : public RegularComposition::Solver
			{
				auto_ptr<Observer> observer;
				auto_ptr<AccelerationStrategy> accst;
				unsigned int MAX_ACC;
				int maxsize;
				
				StructureBinaryRelation evaluate_opt_contains(const RegularComposition& comp, const StructureBinaryRelation* contains);
				std::vector<StructureBinaryRelation> meta_actions(const StructureBinaryRelation& T);
				StructureBinaryRelation round_robin(const StructureBinaryRelation& I, const std::vector<StructureBinaryRelation>& T, unsigned int n_iter);
			public:
				FixpointSolver(AccelerationStrategy* s = new NullAccelerationStrategy(), unsigned int max_acc = 1, int maxsize = -1) : accst(s), MAX_ACC(max_acc), maxsize(maxsize) {}
				~FixpointSolver();

				StructureBinaryRelation evaluate(const RegularComposition& comp);
				StructureBinaryRelation evaluate_atleast_until(const RegularComposition& comp, const StructureBinaryRelation& contains);
				void set_observer(Observer* ob) { observer.reset(ob); }
				void set_context(std::string context) { observer->set_context(context); }
				void set_strategy(AccelerationStrategy* st) { accst.reset(st); }
				void set_maxsize(int n) { maxsize = n; }
				void set_maxacc(unsigned int n) { MAX_ACC = n; }
			};
		}
	}
}

#endif /* RMC_ENGINE_FIXPOINT_FIXPOINT_SOLVER_H */

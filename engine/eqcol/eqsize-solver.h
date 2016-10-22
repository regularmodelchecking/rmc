/*
 * eqsize-solver.h: 
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
#ifndef RMC_ENGINE_EQCOL_EQSIZE_SOLVER_H
#define RMC_ENGINE_EQCOL_EQSIZE_SOLVER_H

#include <engine/regular-composition.h>
#include "eq-column-transducer.h"

namespace rmc
{
	namespace engine
	{
		namespace eqcol
		{
			using namespace std;
			
			using gautomata::RegularRelation;
			using gautomata::BinaryRegularRelation;
			
			class EqsizeSolver : public RegularComposition::Solver
			{
				auto_ptr<Observer> observer;
				
				StructureBinaryRelation _evaluate(const RegularComposition& comp, const StructureBinaryRelation* contains);
			public:
				~EqsizeSolver();
				
				StructureBinaryRelation evaluate(const RegularComposition& comp)
					{
						return _evaluate(comp, NULL);
					}

				StructureBinaryRelation evaluate_atleast_until(const RegularComposition& comp, const StructureBinaryRelation& contains)
					{
						return _evaluate(comp, &contains);
					}

				void set_observer(Observer* ob) { observer.reset(ob); }
				void set_context(std::string context) { observer->set_context(context); }
			};
		}
	}
}


#endif /* RMC_ENGINE_EQCOL_EQSIZE_SOLVER_H */

/*
 * column-transducer-solver.h: 
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

#ifndef RMC_ENGINE_SUBSET_COLUMN_TRANSDUCER_SOLVER_H
#define RMC_ENGINE_SUBSET_COLUMN_TRANSDUCER_SOLVER_H

#include <gautomata/gautomata.h>
#include "column-transducer.h"
#include <engine/regular-composition.h>

namespace rmc
{
	namespace engine
	{
		namespace subset
		{
			using namespace std;
			
			using gautomata::RegularRelation;
			using gautomata::BinaryRegularRelation;
			
			class ColumnTransducerSolver : public RegularComposition::Solver
			{
				auto_ptr<Observer> observer;
				
			public:
				~ColumnTransducerSolver();
				
				StructureBinaryRelation evaluate(const RegularComposition& comp);
				void set_observer(Observer* ob) { observer.reset(ob); }
				void set_context(std::string context) { observer->set_context(context); }
			};
		}
	}
}

#endif /* RMC_ENGINE_SUBSET_COLUMN_TRANSDUCER_SOLVER_H */

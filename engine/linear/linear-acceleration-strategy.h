/*
 * linear-acceleration-strategy.h: 
 *
 * Copyright (C) 2004 Marcus Nilsson (marcus@it.uu.se)
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
 *    Mayank Saksena (mayanks@it.uu.se)
 *    Ahmed Rezine   (rahmed@it.uu.se)
 */
#ifndef LINEAR_ACCELERATION_STRATEGY_H
#define LINEAR_ACCELERATION_STRATEGY_H


#include <gautomata/gautomata.h>
#include <engine/eqcol/eq-column-sequence.h>
#include <engine/regular-composition.h>
#include <queue>
#include <vector>
#include <iostream>
#include <sstream>

#include <engine/acceleration-strategy.h>
#include <engine/eqcol/eq-column-transducer.h>
#include <gautomata/gautomata.h>

namespace rmc
{
	namespace engine
	{
		class LinearAccelerationStrategy : public AccelerationStrategy
		{			
			class Observer : public RegularComposition::Solver::Observer
			{
			public:
				void progress_iteration(unsigned int iteration, unsigned int size) const
					{
						printf("[%6d] ", size);
						std::cout << get_context() << ": ";
						for (unsigned int i = 0;i < iteration;++i) std::cout << "#";
						std::cout << "\r";
						std::cout.flush();
					}
				
				void progress_end() const
					{
						std::cout << "\n";
					}
			};

			Nfa::Factory* nfa_factory;
			Observer* observer;

			virtual void acc_binary_linear(const StructureBinaryRelation& R, std::vector<StructureBinaryRelation>& out);
		public:
			LinearAccelerationStrategy(Nfa::Factory* nfa_factory): nfa_factory(nfa_factory) {}
			virtual ~LinearAccelerationStrategy();
			virtual std::vector<StructureBinaryRelation> accelerate_vector(const std::vector<StructureBinaryRelation>& rels);

			virtual void accelerate_semantic(const StructureBinaryRelation& R, std::vector<StructureBinaryRelation>& out, bool acc = true);
		};
	}
}

#endif /* LINEAR_ACCELERATION_STRATEGY_H */

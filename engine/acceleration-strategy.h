/*
 * acceleration-strategy.h: 
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
 *    Marcus Nilsson (marcusn@it.uu.se)
 */

#ifndef ACCELERATION_STRATEGY_H
#define ACCELERATION_STRATEGY_H

#include <formula/formulamod.h>
#include <engine/regular-composition.h>

namespace rmc
{
	namespace engine
	{
		using namespace gbdd;

		class AccelerationStrategy
		{
		protected:
			RegularComposition::Solver* solver;
		public:
			virtual ~AccelerationStrategy();
			
			void set_solver(RegularComposition::Solver* s) { solver = s; }

/// Accelerate a set of binary relation
/**
 * The parameter \a rels represents transition relation and this method should return an accelerated version
 * of \a rels that can be used in a reachability analysis.
 *
 * @param rels Relations to accelerate
 * 
 * @return A vector of relations R1, R2, ..., Rn s.t. with union R s.t. 1. the union of rels is a subset of R 2. R is a subset of the transitive closure of the union of rels
 */
			virtual std::vector<StructureBinaryRelation> accelerate_vector(const std::vector<StructureBinaryRelation>& rels) = 0;
			StructureBinaryRelation accelerate(const std::vector<StructureBinaryRelation>& rels);
		};

		class NullAccelerationStrategy : public AccelerationStrategy
		{
		public:
			virtual ~NullAccelerationStrategy();

			virtual std::vector<StructureBinaryRelation> accelerate_vector(const std::vector<StructureBinaryRelation>& rels);
		};
	}
}



#endif /* ACCELERATION_STRATEGY_H */

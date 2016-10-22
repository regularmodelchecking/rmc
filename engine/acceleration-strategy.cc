/*
 * acceleration-strategy.cc: 
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

#include "acceleration-strategy.h"

namespace rmc
{
namespace engine
{

AccelerationStrategy::~AccelerationStrategy()
{}

StructureBinaryRelation AccelerationStrategy::accelerate(const std::vector<StructureBinaryRelation>& rels)
{
	assert(rels.size() > 0);
	std::vector<StructureBinaryRelation> arels = accelerate_vector(rels);
	assert(arels.size() > 0);

	std::vector<StructureBinaryRelation>::const_iterator i = arels.begin();
	StructureBinaryRelation res = *i;
	++i;
	
	while(i != arels.end())
	{
		res |= *i;
		if (const Nfa* nfa = dynamic_cast<const Nfa*>(&(res.get_bdd_based())))
		{
			gautomata::RefNfa rnfa(*nfa);
			res = StructureRelation(res.get_domains(), rnfa.deterministic().minimize());
		}
		++i;
	}

	return res;
}

NullAccelerationStrategy::~NullAccelerationStrategy()
{}

std::vector<StructureBinaryRelation> NullAccelerationStrategy::accelerate_vector(const std::vector<StructureBinaryRelation>& rels)
{
	return rels;
}

}
}

/*
 * column.h: 
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
 *    Marcus Nilsson (marcusn@docs.uu.se)
 */

#ifndef RMC_ENGINE_SUBSET_COLUMN_H
#define RMC_ENGINE_SUBSET_COLUMN_H

#include <gautomata/gautomata.h>

namespace rmc
{
	namespace engine
	{
		namespace subset
		{
			using gautomata::RegularSet;
			class Column : public RegularSet
			{
				bool is_canonical;
			public:
				Column(RegularSet s);
				
				Column saturate(gautomata::SymbolSet copying_states,
						gautomata::SymbolSet idempotent_states) const;
				
				friend bool operator==(const Column& x, const Column &y);
				
				friend Column operator&(const Column& x, const Column &y);
				friend Column operator|(const Column& x, const Column &y);
				friend Column operator*(const Column& x, const Column &y);
				
				Column kleene() const;
			};
		}
	}
}

#endif /* RMC_ENGINE_SUBSET_COLUMN_H */

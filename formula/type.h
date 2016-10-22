/*
 * type.h: 
 *
 * Copyright (C) 2003 Marcus Nilsson (marcusn@it.uu.se)
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
 * Authors: Marcus Nilsson (marcusn@it.uu.se), Mayank Saksena
 * (mayanks@it.uu.se)
 */
#ifndef RMCLOGIC_TYPE_H
#define RMCLOGIC_TYPE_H

//#include "constant.h"

namespace rmc
{
	namespace formula
	{

	  enum Type
	    {
	      typeBoolean, typeInteger, typeFO, typeSO, typeConfig
	    };
	  /*
	    0 bool
	    1 int
	    2 FO
	    3 SO
	    4 Config
	  */


#if 0
		class Type
		{
		public:
			virtual ~Type() = 0;
		};

		class TypeBoolean : public Type
		{
		public:
			virtual ~TypeBoolean();
			TypeBoolean();
		};

		class TypeInteger : public Type
		{
		public:
			virtual ~TypeInteger();
			TypeInteger();
		};

		class TypeFO : public Type
		{
		public:
			virtual ~TypeFO();
			TypeFO();
		};

		class TypeSO : public Type
		{
		public:
			virtual ~TypeSO();
			TypeSO();
		};

		class TypeConfig : public Type
		{
		public:
			virtual ~TypeConfig();
			TypeConfig();
		};
#endif
	}
}

#endif /* RMCLOGIC_TYPE_H */

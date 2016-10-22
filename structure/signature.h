/*
 * signature.h: 
 *
 * Copyright (C) 2004 Marcus Nilsson (marcusn@it.uu.se)
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
#ifndef RMC_STRUCTURE_SIGNATURE_H
#define RMC_STRUCTURE_SIGNATURE_H

#include <formula/formulamod.h>
#include <transducer/transducer.h>
namespace rmc
{
	namespace structure
	{
		using namespace gbdd;

		using namespace rmc::transducer;
		using namespace rmc::formula;

		class Signature
		{
			typedef std::map<string, OpTransducer*> OpMap;
/**
 * As a temporary solution, we store a mapping from string to an op. In general, this should be any declaration.
 * 
 */
			OpMap ops;
		protected:

			void add_op(const std::string& name, OpTransducer* op);
			OpTransducer* get_op(const std::string& name) const;
		public:
			Signature();
			virtual ~Signature();

			Node* operator()(Node* f) const;

			virtual StructureConstraint::Factory* structure_factory() const = 0;
			virtual Op* op_boolean_pos() const = 0;
		};
	}
}
			

#endif /* RMC_STRUCTURE_SIGNATURE_H */

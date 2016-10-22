/*
 * signature.cc: 
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

#include "signature.h"
#include <sstream>

namespace rmc
{
namespace structure
{

Signature::Signature()
{}

Signature::~Signature()
{}

	
Node* Signature::operator()(Node* f) const
{
	NodeList* l = new NodeList();
	for (OpMap::const_iterator i = ops.begin();i != ops.end();++i)
	{
		Op* op = i->second;
		NodeList* args = new NodeList();

		for (unsigned int j = 0;j < op->getArity();++j)
		{
			std::stringstream s;
			s << "sigarg" << j;
			args->push_back(new Name(new std::string(s.str())));
		}

		l->push_back(new DeclFunction(new Name(new std::string(i->first)),
					      args,
					      new FormulaOpApplication(op, args)));
	}

	return new FormulaWithDeclarations(l, dynamic_cast<Formula*>(f));
}

void Signature::add_op(const std::string& name, OpTransducer* op)
{
	ops[name] = op;
}

OpTransducer* Signature::get_op(const std::string& name) const
{
	OpMap::const_iterator i = ops.find(name);

	assert(i != ops.end());

	return i->second;
}

}
}



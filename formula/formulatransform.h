/*
 * formulatransform.h:
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 * 
 * Authors: Marcus Nilsson (marcusn@it.uu.se), Mayank Saksena
 * (mayanks@it.uu.se)
 */
#ifndef RMC_FORMULA_FORMULATRANSFORMER_H
#define RMC_FORMULA_FORMULATRANSFORMER_H

#include "formula.h"
#include "formulabuild.h"
#include <stack>

namespace rmc
{
	namespace formula
	{

		class Transformer : public VisitorDefault
		{
			DirectorDefault* director;
		public:
			Transformer(Builder* builder);
			virtual ~Transformer();
			virtual Builder* getBuilder();
			DirectorDefault* getDirector() { return director; }
			virtual void visit(Node& n);
		};

		class TransformerFormula : protected Transformer
		{
		protected:
			BuilderFormula* getBuilder() { return dynamic_cast<BuilderFormula*>(Transformer::getBuilder()); }
			Node* getResult() { return getBuilder()->getResult(); }
		public:
			TransformerFormula():
				Transformer(new BuilderFormula())
				{}

			virtual Node* operator()(Node* n) { n->accept(*this);return getResult(); }
		};

	}
}

#endif /* RMC_FORMULA_FORMULATRANSFORMER_H */

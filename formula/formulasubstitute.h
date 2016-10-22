/*
 * formulasubstitute.h:
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
#ifndef RMC_FORMULA_FORMULASUBSTITUTER_H
#define RMC_FORMULA_FORMULASUBSTITUTER_H

#include "formula.h"
#include "formulabuild.h"
#include "formulatransform.h"
#include <map>
#include <vector>
#include <gbdd/gbdd.h> // hash map


namespace rmc
{
	namespace formula
	{
		class Binder : public TransformerFormula
		{
			typedef std::map<std::string, Name> BindTable;
			typedef std::vector<BindTable> BindTables;

			BindTables current_tables;
			
			// Symbol table for free (toplevel) configuration variables
			BindTable toplevel_table;

			Name* lookup(const std::string& n) const;
			BindTable& push_table();
			void pop_table();
		public:
			Binder();
			virtual ~Binder();

			Name* lookup_toplevel(const std::string& n) const;

			virtual void visit(DeclFunction& f);
			virtual void visit(FormulaWithDeclarations& f);
			virtual void visit(FormulaQuantification& f);
			virtual void visit(Name& n);
		};

		class LetRemover : public TransformerFormula
		{
			typedef Name Id;
			typedef std::pair<NodeList*, Node*> Def;
			typedef std::multimap<Id, Def> AliasTable;
			AliasTable aliasTable;
		public:
			LetRemover();
			virtual ~LetRemover();
			virtual void visit(FormulaWithDeclarations& f);
			virtual void visit(FormulaFunctionApp& f);
		};

		typedef std::map<Name, Node*> NameNodeSubstitution;
		class NameNodeSubstituter : public TransformerFormula
		{
			NameNodeSubstitution substs;
		public:
			NameNodeSubstituter(NameNodeSubstitution);
			virtual ~NameNodeSubstituter();
			virtual void visit(Name& n);
		};

		typedef std::map<std::string, Node*> StringNodeSubstitution;
		class BodyStringSubstituter : public TransformerFormula
		{
			StringNodeSubstitution substs;
		public:
			BodyStringSubstituter(StringNodeSubstitution substs):
				substs(substs)
				{}

			virtual ~BodyStringSubstituter();

			virtual void visit(FormulaWithDeclarations& f);
			virtual void visit(Name& n);
		};

#if 0
		typedef std::map<Name, Name> NameSubstitution;
		class NameSubstituter : public TransformerFormula
		{
			NameSubstitution substs;
		public:
			NameSubstituter(NameSubstitution);
			virtual ~NameSubstituter();
			virtual void visit(Name& n);
		};
#endif

	}
}

#endif /* RMC_FORMULA_FORMULASUBSTITUTER_H */

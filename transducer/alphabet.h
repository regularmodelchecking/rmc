/*
 * alphabet.h: 
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
#ifndef RMC_FORMULA_ALPHABET_H
#define RMC_FORMULA_ALPHABET_H

#include <gbdd/gbdd.h>
#include <gautomata/gautomata.h> // Get rid of this later
#include <string>
#include <formula/formulamod.h>
#include <memory>
#include <map>

namespace rmc
{
	namespace formula
	{
		class Alphabet
		{
		public:
			class IndexedName
			{
				Name var;
				unsigned int index;
			public:
				IndexedName(Name var, unsigned int index = 0):
					var(var),
					index(index)
					{}
				IndexedName(unsigned int index = 0):
					var(),
					index(index)
					{}

				unsigned int get_index() const { return index; }
				const Name& get_var() const { return var; }
 			};

			typedef std::string Value;
			typedef std::set<Value> ValueSet;
		private:
			typedef gbdd::Domains Domains;
			typedef gbdd::Domain Domain;
		
		
			gbdd::Space* _space;
			
			std::map<Name, ValueSet> var_to_possible_values;
			std::map<Name, Domains > var_to_domains;
			gbdd::Bdd::VarPool free_vars;
			
			gautomata::SymbolSet symbols_from_bdd(gbdd::Bdd p) const;

			unsigned int arity;
		public:
			Alphabet(gbdd::Space* space, unsigned int arity);
			
			friend class EnumeratePrintStrategy;

			class EnumeratePrintStrategy : public gautomata::Nfa::EnumeratePrintStrategy
			{
				std::auto_ptr<gautomata::Nfa::PrintStrategy> next;
				const Alphabet& a;
			public:
				virtual ~EnumeratePrintStrategy() {}
				EnumeratePrintStrategy(gautomata::Nfa::PrintStrategy* next,
						       const Alphabet& a):
					gautomata::Nfa::EnumeratePrintStrategy(a.get_domains()),
					next(next),
					a(a)
					{}
				
				std::string format_state(const gautomata::Nfa& nfa, gautomata::State q) const;
				std::string format_edge_symbol(const gautomata::Nfa& nfa, unsigned int domain_i, unsigned int v) const;
				std::string format_attributes() const;
			};

			bool operator==(const Alphabet& a2) const;
			
			
			gbdd::Space* get_space(void) const { return _space; }
			unsigned int get_arity(void) const { return arity; }
			
			void add_variable(Name varname, ValueSet possible_values);
			void add_variable(Name varname);
			void remove_variable(Name varname);
			
			Domains doms_of_var(Name name) const;
			Domain dom_of_var(IndexedName var) const;
			ValueSet values_of_var(Name name) const;

			Domains get_domains() const;
			
			std::string var_value_name(Name var, unsigned int v) const;
			
			gautomata::SymbolSet var_equal(IndexedName v1, IndexedName v2) const;
			gautomata::SymbolSet var_value(IndexedName var, std::string value) const;
			gautomata::SymbolSet var_value(IndexedName var, bool value) const;
			gautomata::SymbolSet universe() const;

			Domain::VarMap renaming(const Alphabet& a2) const;
			
			Alphabet operator|(const Alphabet& a2) const;

			std::string format_legend() const;
			std::string format_symbol_legend() const;


			// Automata specific, push into properties of StructureConstraint
			std::string format_symbol(const gautomata::SymbolSet& singelton, unsigned int index) const;
			std::string format_word(const gautomata::RegularSet& singelton, unsigned int index) const;
			gautomata::SymbolSet assign_values(const gautomata::SymbolSet& s, unsigned int index) const;
						  

		};
	}
}

#endif /* RMC_FORMULA_ALPHABET_H */

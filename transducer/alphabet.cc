/*
 * alphabet.cc: 
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

#include "alphabet.h"

using namespace std;

namespace rmc
{
	namespace formula
	{

string Alphabet::EnumeratePrintStrategy::format_state(const gautomata::Nfa& nfa, gautomata::State q) const
{
	return next->format_state(nfa, q);
}

string Alphabet::EnumeratePrintStrategy::format_attributes() const
{
	return next->format_attributes();
}

string Alphabet::EnumeratePrintStrategy::format_edge_symbol(const gautomata::Nfa& nfa, unsigned int domain_i, unsigned int v) const
{
	gbdd::Bdd::FiniteVar x(nfa.get_space(), a.get_domains()[0]);

	return a.format_symbol(a.symbols_from_bdd(x == v), 0);
}

Alphabet::Alphabet(gbdd::Space* space, unsigned int arity):
	_space(space),
	arity(arity)
{}

void Alphabet::add_variable(Name varname, ValueSet possible_values)
{
	if (var_to_domains.find(varname) != var_to_domains.end())
	{
		assert(possible_values == values_of_var(varname));
		return;
	}

	var_to_possible_values[varname] = possible_values;
	var_to_domains[varname] = free_vars.alloc_interleaved(gbdd::Bdd::n_vars_needed(possible_values.size()),
							      arity);
}

void Alphabet::add_variable(Name varname)
{
	ValueSet values;
	values.insert("0");
	values.insert("1");
	add_variable(varname, values);
}

void Alphabet::remove_variable(Name varname)
{
	var_to_possible_values.erase(varname);
	var_to_domains.erase(varname);
}

Alphabet::Domains Alphabet::doms_of_var(Name name) const
{
	map<Name, Domains>::const_iterator domains_i = var_to_domains.find(name);
	if(domains_i == var_to_domains.end())
	{
		cerr << "Could not find var: " << *(name.getName()) << " among ";
		for (domains_i = var_to_domains.begin();domains_i != var_to_domains.end();++domains_i) cerr << *(domains_i->first.getName()) << " ";
		cerr << endl;
		assert(false);
	}

	return domains_i->second;
}

Alphabet::Domains Alphabet::get_domains() const
{
	Domains doms(arity);
	for(map<Name,Domains>::const_iterator var_i = var_to_domains.begin();var_i != var_to_domains.end();++var_i)
	{
		for (unsigned int i = 0;i < arity;++i)
		{
			doms[i] |= (var_i->second)[i];
		}
	}

	return doms;
}

string Alphabet::var_value_name(Name var, unsigned int v) const
{
	map<Name,ValueSet>::const_iterator var_i = var_to_possible_values.find(var);
	assert(var_i != var_to_possible_values.end());

	const ValueSet& strings = var_i->second;

	ValueSet::const_iterator i = strings.begin();

	while (v > 0)
	{
		assert(i != strings.end());
		++i;
		--v;
	}

	if (i == strings.end()) return "?";

	assert(i != strings.end());

	return *i;
}

static unsigned int index_of(Alphabet::ValueSet strings, string str)
{
	unsigned int index = 0;
	Alphabet::ValueSet::const_iterator i;
	for(i = strings.begin();i != strings.end();++i)
	{
		if (*i == str) return index;
		++index;
	}

	cout << "Could not find: " << str << endl;

	assert(false);
}

gautomata::SymbolSet  Alphabet::symbols_from_bdd(gbdd::Bdd p) const
{
	return gautomata::SymbolSet(Domain::infinite(), p);
}

gautomata::SymbolSet Alphabet::var_value(IndexedName var, string value) const
{
	return symbols_from_bdd(gbdd::Bdd::value(_space,
						 dom_of_var(var),
						 index_of(values_of_var(var.get_var()), value))) & universe();
}

gautomata::SymbolSet Alphabet::var_value(IndexedName var, bool value) const
{
	return var_value(var, value ? string("1") : string("0"));
}
	
Alphabet::Domain Alphabet::dom_of_var(IndexedName var) const
{
	return doms_of_var(var.get_var())[var.get_index()];
}

Alphabet::ValueSet Alphabet::values_of_var(Name name) const
{
	map<Name, ValueSet>::const_iterator values_i = var_to_possible_values.find(name);
	assert(values_i != var_to_possible_values.end());

	return values_i->second;
}

gautomata::SymbolSet Alphabet::var_equal(IndexedName v1, IndexedName v2) const
{
	Domain dom1 = dom_of_var(v1);
	Domain dom2 = dom_of_var(v2);

	return symbols_from_bdd(gbdd::Bdd::vars_equal(_space, 
						      dom_of_var(v1),
						      dom_of_var(v2))) & universe();
}

gautomata::SymbolSet Alphabet::universe() const
{
	gbdd::Bdd sym_total = gbdd::Bdd(_space, true);

	// Rule out impossible combinations

	map<Name, ValueSet>::const_iterator i;
	for (i = var_to_possible_values.begin();i != var_to_possible_values.end();++i)
	{
		Name name = i->first;
		unsigned int n_values = i->second.size();
		for (unsigned int j = 0;j < arity;++j)
		{
			Domain dom = dom_of_var(IndexedName(name, j));
			unsigned int dom_highest_n = (1 << dom.size()) - 1;
		
			sym_total &= !gbdd::Bdd::value_range(_space, dom, n_values, dom_highest_n);
		}
	}
		
	return symbols_from_bdd(sym_total);
}

gbdd::Domain::VarMap Alphabet::renaming(const Alphabet& a2) const
{
	assert(arity == a2.arity);
	Domain::VarMap map;

	for (std::map<Name, Domains>::const_iterator i = var_to_domains.begin();i != var_to_domains.end();++i)
	{
		Domains source_domains = i->second;
		Domains target_domains = a2.doms_of_var(i->first);

		for (unsigned int i = 0;i < arity;++i)
		{
			Domain s = source_domains[i];
			Domain t = target_domains[i];

			if (s != t) map = map | Domain::map_vars(source_domains[i], target_domains[i]);
		}
	}

	return map;
}

Alphabet Alphabet::operator|(const Alphabet& a2) const
{
	Alphabet a = a2;

	for (std::map<Name, Domains>::const_iterator i = var_to_domains.begin();i != var_to_domains.end();++i)
	{
		Name varname = i->first;

		std::map<Name, Domains>::const_iterator j = a.var_to_domains.find(varname);
		if (j == a.var_to_domains.end())
		{
			a.add_variable(varname, values_of_var(varname));
		}
		else
		{
			assert(values_of_var(varname) == a.values_of_var(varname));
		}
	}

	return a;
}

std::string Alphabet::format_legend() const
{
	typedef gbdd::Bdd::Var Var;
	typedef map<Var, IndexedName> UsageMap;

	UsageMap umap;
	unsigned int highest = 0;

	for (std::map<Name, Domains>::const_iterator i = var_to_domains.begin();i != var_to_domains.end();++i)
	{
		Domains doms = i->second;
		for (unsigned int j = 0;j < doms.size();++j)
		{
			IndexedName v(i->first, j);
			const Domain& dom = doms[j];

			for (Domain::const_iterator k = dom.begin();k != dom.end();++k)
			{
				umap[*k] = v;

				highest = highest > *k ? highest : *k;
			}
		}
	}

	std::ostringstream res;

	for (unsigned int i = 0;i <= highest;++i)
	{
		UsageMap::const_iterator j = umap.find(i);
		if (j == umap.end()) res << ".";
		else
		{
			res << *(j->second.get_var().getName());
			res << ((j->second.get_index() == 0) ? "" : "'");
		}
		res << " ";
	}
	
	return res.str();
}

std::string Alphabet::format_symbol_legend() const
{
	std::ostringstream res;
	for (std::map<Name, Domains>::const_iterator i = var_to_domains.begin();i != var_to_domains.end();++i)
	{
		res << i->first << " ";
	}

	return res.str();
}

std::string Alphabet::format_symbol(const gautomata::SymbolSet& singelton, unsigned int index) const
{
	std::ostringstream res;
	gautomata::SymbolSet singelton_one = assign_values(singelton, index);

	for (std::map<Name, Domains>::const_iterator i = var_to_domains.begin();i != var_to_domains.end();++i)
	{
		IndexedName v(i->first, index);
		ValueSet values = values_of_var(i->first);
		for (ValueSet::const_iterator j = values.begin();j != values.end();++j)
		{
			gbdd::BddSet x = (var_value(v, *j) & singelton_one);
			if (!x.is_empty())
			{
				res << *j;
				break;
			}
		}
	}
	
	return res.str();
}

std::string Alphabet::format_word(const gautomata::RegularSet& s, unsigned int index) const
{
	gautomata::RegularSet alphabet_s (get_domains()[index], s);
	std::ostringstream res;
	const gautomata::Nfa& a = alphabet_s.get_automaton();
	gautomata::StateSet Live = a.states_live();
	gautomata::StateSet q = a.states_starting() & Live;
	gautomata::StateSet F = a.states_accepting();
	while ((q & F).is_empty())
	{
		gautomata::StateSet r = a.successors(q, a.alphabet()) & Live;
		assert (r.begin() != r.end());
		r = gautomata::StateSet(r, *(r.begin()));
		res << format_symbol(a.edge_between(q, r), index) << " ";
		q = r;
	}

	return res.str();
}

gautomata::SymbolSet Alphabet::assign_values(const gautomata::SymbolSet& s, unsigned int index) const
{
	gautomata::SymbolSet res = s;

	for (std::map<Name, Domains>::const_iterator i = var_to_domains.begin();i != var_to_domains.end();++i)
	{
		IndexedName v(i->first, index);
		ValueSet values = values_of_var(i->first);

		for (ValueSet::const_iterator j = values.begin();j != values.end();++j)
		{
			if (!(var_value(v,*j) & res).is_empty())
			{
				res = res & var_value(v, *j);
				break;
			}
			assert (j != values.end());
		}
	}

	return res;
}

bool Alphabet::operator==(const Alphabet& a2) const
{
	return var_to_possible_values == a2.var_to_possible_values &&
		var_to_domains == a2.var_to_domains;
}


}
}

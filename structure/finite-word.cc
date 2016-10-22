/*
 * finite-word.cc: 
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

#include "finite-word.h"

namespace rmc
{
namespace structure
{

static Nfa* create_successor(Nfa::Factory* factory)
{
	Nfa* nfa = factory->ptr_empty();
	gautomata::State q1 = nfa->add_state(true, true);
	gautomata::State q2 = nfa->add_state(false);

	Bdd::Vars x(nfa->get_space());

	Bdd is_i = x[0];
	Bdd is_j = x[1];

	nfa->add_edge(q1, !is_i & !is_j, q1);
	nfa->add_edge(q1, !is_i & is_j, q2);
	nfa->add_edge(q2, is_i  & !is_j, q1);
	nfa->add_edge(q2, is_i  & is_j, q2);

	return nfa;
}	

static Nfa* create_lessthan(Nfa::Factory* factory)
{
	Nfa* nfa = factory->ptr_empty();
	gautomata::State q1 = nfa->add_state(false, true);
	gautomata::State q2 = nfa->add_state(false);
	gautomata::State q3 = nfa->add_state(true);

	Bdd::Vars x(nfa->get_space());

	Bdd is_i = x[0];
	Bdd is_j = x[1];

	nfa->add_edge(q1, !is_i & !is_j, q1);
	nfa->add_edge(q1,  is_i & !is_j, q2);
	nfa->add_edge(q2, !is_i & !is_j, q2);
	nfa->add_edge(q2, !is_i &  is_j, q3);
	nfa->add_edge(q3, !is_i & !is_j, q3);

	return nfa;
}	

static Nfa* create_equal(Nfa::Factory* factory)
{
	Nfa* nfa = factory->ptr_empty();
	gautomata::State q = nfa->add_state(true, true);

	Bdd::Vars x(nfa->get_space());

	Bdd is_i = x[0];
	Bdd is_j = x[1];

	nfa->add_edge(q, is_i & is_j, q);
	nfa->add_edge(q, !is_i & !is_j, q);

	return nfa;
}	

FiniteWord::FiniteWord(Nfa::Factory* factory, gbdd::Space* space):
	factory(factory)
{
	OpTransducer* equal = new OpStatic(2, create_equal(factory), space);

	add_op("+", new OpStatic(3, create_successor(factory), space));
	add_op("<", new OpStatic(2, create_lessthan(factory), space));
	add_op("0", new OpIsMin(get_op("<")));
	add_op("$", new OpIsMin(new OpInverse(get_op("<"))));
	add_op("<=", new OpProduct(get_op("<"), equal, StructureConstraint::fn_or));
	add_op(">", new OpInverse(get_op("<")));
	add_op(">=", new OpInverse(get_op("<=")));
}

FiniteWord::~FiniteWord()
{}

StructureConstraint::Factory* FiniteWord::structure_factory() const
{
	auto_ptr<Nfa> dummy (factory->ptr_empty()); // TODO: Add ptr_clone in factory

	return dummy->ptr_factory();
}

Op* FiniteWord::op_boolean_pos() const
{
	return get_op("0");
}

}

}

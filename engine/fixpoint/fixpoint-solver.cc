/*
 * fixpoint-solver.cc: 
 *
 * Copyright (C) 2002 Marcus Nilsson (marcusn@docs.uu.se)
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
 *    Marcus Nilsson (marcus@it.uu.se)
 *    Ahmed Rezine   (rahmed@it.uu.se)
 *    Mayank Saksena (mayanks@it.uu.se)
 */

#include "fixpoint-solver.h"

namespace rmc
{
	namespace engine
	{
		namespace fixpoint
		{

using namespace gautomata;

FixpointSolver::~FixpointSolver()
{}

StructureBinaryRelation FixpointSolver::evaluate_atleast_until(const RegularComposition& comp, const StructureBinaryRelation& contains)
{
	return evaluate_opt_contains(comp, &contains);
}

StructureBinaryRelation FixpointSolver::evaluate(const RegularComposition& comp)
{
	return evaluate_opt_contains(comp, NULL);
}

StructureBinaryRelation reduce(StructureBinaryRelation A)
{
	if (const Nfa* nfa = dynamic_cast<const Nfa*>(&(A.get_bdd_based())))
	{
		gautomata::RefNfa rnfa(*nfa);
		rnfa = rnfa.minimize().deterministic().minimize().filter_states_live();
		A = StructureRelation(A.get_domains(), rnfa);
	}
	return A;
}

// get meta actions from T, T^2, T^3 etc. up to T^MAX_ACC
std::vector<StructureBinaryRelation> FixpointSolver::meta_actions(const StructureBinaryRelation& T)
{
	StructureBinaryRelation Tk = T;
	std::vector<StructureBinaryRelation> out;

	for (unsigned int k = 1; k <= MAX_ACC; ++k)
	{
		std::vector<StructureBinaryRelation> rels, arels;
		rels.push_back(Tk);
		arels = accst->accelerate_vector(rels);

		/*
		  Meta action reduction:
		  - remove every action in arels_2 which is contained in the composition of two actions of arels_1 (more memory-friendly than checking (U arels_1)^2
		  - remove Tk from arels for every k
		*/
#if 0
		if (k == 2) {
			std::vector<StructureBinaryRelation> prev2;

			for (std::vector<StructureBinaryRelation>::const_iterator i = out.begin(); i != out.end(); ++i)
				for (std::vector<StructureBinaryRelation>::const_iterator j = out.begin(); j != out.end(); ++j)
					prev2.push_back(reduce(i->compose(1, *j)));

			bool saw_Tk = false;
			for (std::vector<StructureBinaryRelation>::const_iterator i = arels.begin(); i != arels.end(); ++i) {

				if (!saw_Tk && (*i == Tk)) {
					saw_Tk = true;
					continue;
				}

				bool sub = false;
				for (std::vector<StructureBinaryRelation>::const_iterator j = prev2.begin(); j != prev2.end(); ++j) {
					if ((*j & *i) == *i) {
						sub = true;
						break;
					}
				}

				if (!sub)
					out.push_back(*i); // i is not a subautomaton of any j
			}
		}
		else
#endif
			for (std::vector<StructureBinaryRelation>::iterator i = arels.begin(); i != arels.end(); ++i)
				if (!(*i == Tk))
					out.push_back(*i);

		Tk = Tk.compose(1, T) - Tk;
		Tk = reduce(Tk);
	}
	
	return out;
}

// vector steps
StructureBinaryRelation FixpointSolver::round_robin(const StructureBinaryRelation& I, const std::vector<StructureBinaryRelation>& T, unsigned int n_iter)
{
	StructureBinaryRelation Prev;
	StructureBinaryRelation C = I;

	unsigned int size = 0;

	Prev = C;

	for (std::vector<StructureBinaryRelation>::const_iterator t = T.begin(); t != T.end(); ++t) {

		C = C.compose(1, *t) | C;
		//C = Prev.compose(1, *t) | C; // alt

		if (const Nfa* nfa = dynamic_cast<const Nfa*>(&(C.get_bdd_based())))
		{
			RefNfa rnfa(*nfa);
			rnfa = rnfa.minimize().deterministic().minimize().filter_states_live();
			size = rnfa.n_states();
			C = StructureRelation(C.get_domains(), rnfa);
		}

		observer->progress_iteration(n_iter, size);
	}
	
	return C;
}

StructureBinaryRelation FixpointSolver::evaluate_opt_contains(const RegularComposition& comp, const StructureBinaryRelation* contains)
{
	RegularSet expr = comp.get_expr();
	const Nfa &expr_nfa = expr.get_automaton();

	// Init = Q1 --> Q2 --> Q3, we require Q2 = Q3, i.e. a loop
	
	StateSet Q1 = expr_nfa.states_starting();
	StateSet Q2 = expr_nfa.successors(Q1, expr_nfa.alphabet());
	StateSet Q3 = expr_nfa.successors(Q2, expr_nfa.alphabet());
	
	assert(Q2 == Q3 && Q2 == expr_nfa.states_accepting());
	
	// Relation is I o T^*
	
	const StructureBinaryRelation I = comp.relation_between(Q1, Q2);
	const StructureBinaryRelation T = comp.relation_between(Q2, Q3);

	StructureBinaryRelation Prev;
	StructureBinaryRelation C = I;
	const std::vector<StructureBinaryRelation> T_meta = meta_actions(T);
	unsigned int n_iter = 0;

	do
	{
		unsigned int size = 0;

		C = round_robin(C, T_meta, n_iter);
		
		Prev = C;
		C = Prev.compose(1, T) | C;

		if (const Nfa* nfa = dynamic_cast<const Nfa*>(&(C.get_bdd_based())))
		{
			RefNfa rnfa(*nfa);
			rnfa = rnfa.minimize().deterministic().minimize().filter_states_live();
			size = rnfa.n_states();
			C = StructureRelation(C.get_domains(), rnfa);
			
			if (maxsize >= 0 && int(size) > maxsize)
				throw Failure(reduce(C - Prev));
		}

		observer->progress_iteration(n_iter, size);
		++n_iter;

	} while (!(C == Prev) && 
		 (contains == NULL || (C - *contains) == C));

	observer->progress_end();
	
	return C;
}

}
}
}

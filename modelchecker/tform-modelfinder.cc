/*
 * tform-modelfinder.cc: 
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
 *    Mayank Saksena (mayanks@it.uu.se)
 */

#include "tform-modelfinder.h"
#include <fstream>
#include <ostream>
#include <iostream>

using namespace gbdd;
using namespace gautomata;
using namespace std;

namespace rmc
{
namespace modelchecker
{

	using namespace transducer;
	
void TFormModelfinder::visit(Formula& f)
{
	if (inside_always) return;

	initial = new FormulaOpApplication(new OpAnd(), initial, &f);
}

void TFormModelfinder::visit(FormulaOpApplication& f)
{
	app = &f;
	f.getOp()->accept(*this);
}

void TFormModelfinder::visit(Op& op)
{
	if (inside_always) return;
	initial = new FormulaOpApplication(new OpAnd(), initial, app);
}

void TFormModelfinder::visit(OpAnd& op)
{
	if (inside_always) return;
	NodeVector nv = app->getArgs()->getChildren();
	VisitorDefault::visit(nv);
}

void TFormModelfinder::visit(OpAlways& f)
{
	inside_always = true;
	seen_eventuality = false;

	FormulaOpApplication* thisapp = app;

	NodeVector nv = app->getArgs()->getChildren();
	VisitorDefault::visit(nv);
	if (!seen_eventuality)
	{
		// No eventuality inside
		if (*always == FormulaTrue()) always = thisapp->getArg(0);
		else always = new FormulaOpApplication(new OpAnd(), always, thisapp->getArg(0));
	}

	inside_always = false;

}

void TFormModelfinder::visit(OpEventually& f)
{
  if (inside_always) {
    always_eventuality = app->getArg(0);
    seen_eventuality = true;
  }
  else
    eventuality = app->getArg(0);
}

TFormModelfinder::TFormModelfinder(RegularComposition::Solver* solver,
				   gautomata::Nfa::Factory* nfa_factory):
	nfa_factory(nfa_factory),
	solver(solver),
	solver_reachability(solver),
	acceleration(new NullAccelerationStrategy())
{
	space = auto_ptr<gautomata::Nfa>(nfa_factory->ptr_empty())->get_space();
}
	
Transducer TFormModelfinder::s1s_to_transducer(gautomata::Nfa::Factory* nfa_factory, Node* f)
{
	struct tms start_time,end_time;
	times(&start_time);

	auto_ptr<Nfa> dummy(nfa_factory->ptr_empty());
	TransducerBuilder* builder = new TransducerBuilder(nfa_factory, Alphabet(dummy->get_space(), 2));
	Transformer transf(builder);

	f->accept(transf);

	times(&end_time);
	s1s_ticks += end_time.tms_utime - start_time.tms_utime;
	return builder->getResult();
}

static gautomata::BinaryRegularRelation identity(gbdd::Space* space, gautomata::Nfa::Factory* nfa_factory, const gbdd::Domains& doms)
{
	gbdd::Bdd::FiniteVar w1(space, doms[0]);
	gbdd::Bdd::FiniteVar w2(space, doms[1]);
	gautomata::SymbolSet edge(gbdd::Domain::infinite(), w1 == w2);

	gautomata::RefNfa sym(nfa_factory->ptr_symbol(edge));

	return gautomata::RegularRelation(doms, sym * sym.kleene());
}

// find a shortest sequence of states (q_i)[0,k] of R s.t. q_0 initial and q_k final
static std::deque<gautomata::State> get_shortest_path(gautomata::RegularSet R)
{
	deque<State> out;

	const RefNfa A = R.get_automaton();
	const StateSet accept = A.states_accepting();
	if (accept.is_empty())
		return out;

	const SymbolSet a = A.alphabet();
	State closest;
	std::vector<StateSet> frw;
	{
		StateSet S = A.states_starting();
		while ((S & accept).is_empty()) {
			frw.push_back(S);
			S = A.successors(S, a) - S;
		}

		closest = *(S & accept).begin();
	}
	{
		State s = closest;
		const StateSet Q = A.states();
		for (std::vector<StateSet>::reverse_iterator i = frw.rbegin(); i != frw.rend(); ++i) {
			StateSet one(Q, s);
			StateSet S = A.predecessors(one, a) & *i;
			s = *S.begin();
			out.push_front(s);
		}

		out.push_back(closest);
	}

	return out;
}

gautomata::RegularSet TFormModelfinder::get_shortest_member(gautomata::RegularSet R) const
{
	// return a shortest member of R

	const RefNfa A(R.get_automaton());
	RefNfa shortest_member(nfa_factory->ptr_epsilon());

	const deque<State> path = get_shortest_path(R);
	deque<State>::const_iterator it = path.begin();
	bool done = (it == path.end());
	while (!done) {
		deque<State>::const_iterator prev = it;
		++it;
		done = (it == path.end());
		if (!done) {
			BddSet edge(R.get_domain(), A.edge_between(*prev, *it).normalized_bdd());
			BddSet singleton(edge, *(edge.begin()));
			shortest_member = shortest_member * RefNfa(nfa_factory->ptr_symbol(singleton.get_bdd()));
			shortest_member = shortest_member.deterministic().minimize();
		}
	}

	return RegularSet(R.get_domain(), shortest_member);
}

std::vector<gautomata::RegularSet> TFormModelfinder::get_shortest_word(RegularSet I, BinaryRegularRelation R, RegularSet F) const
{
	// gives a shortest sequence of singletons connecting I with F by R

	vector<RegularSet> preimages;

	RegularSet X = F;
	const RegularSet empty = RegularSet::empty(X);
	
	while ((X & I) == empty) {
		preimages.push_back(X);
		X = R.range_under(X);
		X = RegularSet(X.get_domain(), RefNfa(X.get_automaton()).deterministic().minimize());
	}

	X = X & I;

	vector<RegularSet> shortest_word;
	RegularSet single = get_shortest_member(X);
	shortest_word.push_back(single);

	for (vector<RegularSet>::reverse_iterator it = preimages.rbegin(); it != preimages.rend(); it++){
		X = R.image_under(single) & (*it);
		single = get_shortest_member(X);
		shortest_word.push_back(single);
	}
	
	return shortest_word;
}

static BinaryRegularRelation reduce(const BinaryRegularRelation& R)
{
	return BinaryRegularRelation(R.get_domain(0), R.get_domain(1), RefNfa(R.get_automaton()).deterministic().minimize());
}

BinaryRegularRelation TFormModelfinder::reach(BinaryRegularRelation I, BinaryRegularRelation A, BinaryRegularRelation Meta)
{
	solver_reachability->set_context("Reachability");
	BinaryRegularRelation result = solve_r1_r2star(reduce((I & A).project(0)), Meta, solver_reachability);
	return RegularRelation(result.get_domains(), result.inverse());
}

BinaryRegularRelation TFormModelfinder::reach_pairs(BinaryRegularRelation I, BinaryRegularRelation A, BinaryRegularRelation Meta)
{
	return reduce(reduce(reach(I, A, Meta) & A) | reduce(I & A));
}

TFormModelfinder::Example TFormModelfinder::Example::operator*(const Example& c2) const
{
	assert(false);
	return c2;
}

void TFormModelfinder::Example::operator*=(const vector<RegularSet>& v)
{
	for (vector<RegularSet>::const_iterator i = v.begin(); i != v.end(); i++)
		push_back(*i);
}


std::ostream& operator<<(std::ostream& s, const TFormModelfinder::Example& c)
{
	s << c.a.format_symbol_legend() << endl;

	for (vector<RegularSet>::const_iterator i = c.begin(); i != c.end(); i++) {
		s << c.a.format_word(*i, 0) << endl;
	}

	return s;
}

BinaryRegularRelation TFormModelfinder::solve_r1_r2star(BinaryRegularRelation r1,
							BinaryRegularRelation r2,
							RegularComposition::Solver* solver,
							BinaryRegularRelation *terminate_on)
{
#if 1
	RegularComposition comp = RegularComposition(r1) * RegularComposition(r2).kleene();

	BinaryRegularRelation res;
	if (terminate_on == NULL)
		res = solver->evaluate(comp);
	else
		res = solver->evaluate_atleast_until(comp, *terminate_on);

	return res;
#endif
#if 0
	const unsigned int max_states = 10000;
	const unsigned int max_iter = 15;
	const unsigned int min_iter = 5;

	set_relation_reachability(r1, r2);

	bool finished = false;
	unsigned int n_iter = 0;
	unsigned int total_iter = 0;

	do
	{
		printf("[%6d] ", n_states());
		std::cout << "Searching: ";
		for (unsigned int i = 0;i < total_iter;++i) std::cout << "#";
		std::cout << "\r";
		std::cout.flush();

		finished = iterate(1);
		n_iter++;
		total_iter++;

		if (terminate_on != NULL)
		{
			if (!((get_result() & *terminate_on).get_automaton().is_false())) finished = true;
		}

		if (!finished && n_iter >= min_iter && (n_iter >= max_iter || n_states() >= max_states))
		{
			// Restart

			set_relation_reachability(get_result(), r2);
			n_iter = 0;
		}
	} while (!finished);

	std::cout << "\n";
	return get_result();
#endif
}

TFormModelfinder::Example TFormModelfinder::solve(Node* F, const Alphabet& in_a, BinaryRegularRelation A, BinaryRegularRelation I)
{
  // returns an example x_1...x_n s.t. (x_1, x_2) in I, and each (x_i, x_i+1) is in A constrained by formula F

	bool prev_A_is_true = A.get_automaton().is_true();
  initial = new FormulaTrue();
  always = new FormulaTrue();
  eventuality = NULL;
  always_eventuality = NULL;
  inside_always = false;

  F->accept(*this);

  if (always_eventuality == NULL) always_eventuality = new FormulaTrue();

#if 0
  std::cout << "initial: " << *initial << std::endl;
  std::cout << "always: " << *always << std::endl;
  std::cout << "always_ev: " << *always_eventuality << std::endl;
#endif
  
  Transducer t_always = s1s_to_transducer(nfa_factory, always);
  Transducer t_initial = s1s_to_transducer(nfa_factory, initial) & t_always;
  Transducer t_always_eventuality = s1s_to_transducer(nfa_factory, always_eventuality);

  
  // Unify alphabet
  
  Alphabet a = 
	  in_a |
	  t_initial.get_alphabet() | 
	  t_always.get_alphabet() |
	  t_always_eventuality.get_alphabet() |
	  Alphabet(space, 2);
  t_initial = Transducer(a, t_initial);
  t_always = Transducer(a, t_always);
  t_always_eventuality = Transducer(a, t_always_eventuality);
  

  // Adapt A and I to new alphabet

  A = RegularRelation(a.get_domains(), RefNfa(A.get_automaton()).rename(in_a.renaming(a)));
  I = RegularRelation(a.get_domains(), RefNfa(I.get_automaton()).rename(in_a.renaming(a)));

  // Add new t_initial and t_always constraints
  
  I = I & BinaryRegularRelation(t_initial.get_relation());
  A = A & BinaryRegularRelation(t_always.get_relation());
  
  I = RegularRelation(I.get_domains(), RefNfa(I.get_automaton()).deterministic().minimize());
  A = RegularRelation(A.get_domains(), RefNfa(A.get_automaton()).deterministic().minimize());
  
  if (eventuality == NULL)
  {
	  // case: F = F_I & alw F_A & alw ev F_AE 
	  
	  BinaryRegularRelation F_AE(t_always_eventuality.get_relation());
	  BinaryRegularRelation Identity(identity(space, nfa_factory, I.get_domains()));

	  // Examine the set of possible first two rows first, there may be a loop with length one

	  BinaryRegularRelation first = reduce(I & A & F_AE);

	  if (first.get_automaton().is_false()) return Example(a);

	  {
		  RegularSet first_loops((first & Identity).project_on(0));
		  if (!(first_loops.get_automaton().is_false()))
		  {
			  RegularSet one = get_shortest_member(first_loops);
			  Example ex(a);
			  ex.push_back(one);
			  ex.push_back(one);
			  return ex;
		  }
	  }

	  // No immediate loop, do reachability to get possible start state of loop

	  BinaryRegularRelation start;
	  try {
		  start = reach(I, A, A) & (A & F_AE);
	  }
	  catch (RegularComposition::Solver::Failure f) {
		  RegularSet partial(f.get_result().project_on(1));
		  std::vector<RegularSet> w = get_shortest_word(I.project_on(0), A, partial);
		  Example trace(a, true);
		  trace *= w;
		  return trace;
	  }

	  {
		  // Can we find a loop immediately?
		  RegularSet start_loops((start & Identity).project_on(0));
		  if (!(start_loops.get_automaton().is_false()))
		  {
			  RegularSet one = get_shortest_member(start_loops);
			  Example ex(a);
			  ex.push_back(one);
			  ex.push_back(one);
			  return ex;
		  }
	  }

	  // Do repeated reachability

	  solver->set_context("Repeated Reachability");
	  RegularSet loops;
	  Example path(a);
	  try {
		  loops = (solve_r1_r2star(start, A, solver, &Identity) & Identity).project_on(0);
	  }
	  catch (RegularComposition::Solver::Failure f) {
		  loops = RegularSet(f.get_result().project_on(1));
		  path.set_partial_result();
	  }

	  if (loops.get_automaton().is_false())
	  {
		  // no models, return the "none" example
		  return Example(a);
	  }
	  
	  // Now find example from second to loops using A as transition relation
	  {
		  RegularSet second(a.get_domains()[0], I.project_on(1));
		  vector<RegularSet> second_to_loops = get_shortest_word(second, A, loops);

		  RegularSet x(second_to_loops.back());
		  vector<RegularSet> loops_to_loops;
		  if (!path.is_partial_result())
			  loops_to_loops = get_shortest_word((A & F_AE).image_under(x), A, x);

		  RegularSet init_to_second = get_shortest_member(I.range_under(second_to_loops.front()));

		  path.push_back(init_to_second);
		  for (vector<RegularSet>::iterator it = second_to_loops.begin(); it != second_to_loops.end(); it++)
			  path.push_back(*it);
		  for (vector<RegularSet>::iterator it = loops_to_loops.begin(); it != loops_to_loops.end(); it++)
			  path.push_back(*it);
		  return path;
	  }
  }
  
  // else: recur
  {
	  // case: F = F_I & alw F_A & ev F'

	  class DisjunctsFinder : public VisitorDefault
	  {
		  NodeList* decls;
		  Quantifier* q;
		  Variable* q_v;
		  bool is_or;
		  bool is_exists;
	  public:
		  DisjunctsFinder():
			  decls(new NodeList()),
			  q(NULL)
			  {}

		  std::vector<Node*> disjuncts;
		  void visit(OpOr& op) { is_or = true; }
		  void visit(Op& op) { is_or = false; }
		 
		  void visit(Quantifier& q) { is_exists = false; }
		  void visit(QuantifierSOExists& q ) { is_exists = true; }
		  void visit(QuantifierFOExists& q ) { is_exists = true; }
 
		  virtual void visit(FormulaQuantification& f)
			  {
				  f.getQuantifier()->accept(*this);

				  if (is_exists && q == NULL)
				  {
					  q = f.getQuantifier();
					  q_v = f.getVariable();
					  f.getSubformula()->accept(*this);
				  }
				  else
				  {
					  visit(static_cast<Formula&>(f));
				  }
			  };
					  
		  virtual void visit(FormulaOpApplication& f)
			  {
				  f.getOp()->accept(*this);
				  
				  if (is_or)
				  {
					  NodeVector v = f.getArgs()->getChildren();
					  VisitorDefault::visit(v);
				  }
				  else
				  {
					  visit(static_cast<Formula&>(f));
				  }
			  };

		  virtual void visit(Formula& f)
			  {
				  Formula* final = &f;

				  if (q != NULL)
				  {
					  final = new FormulaQuantification(q, q_v, final);
				  }
				  
				  if (decls->size() > 0)
				  {
					  final = new FormulaWithDeclarations(decls, final);
				  }


				  disjuncts.push_back(final);
			  };
		  virtual void visit(Declaration& f)
			  {
				  decls->push_back(&f);
			  }
		  virtual void visit(FormulaWithDeclarations& f)
			  {
				  NodeVector v = f.getDecls()->getChildren();
				  VisitorDefault::visit(v);
				  
				  f.getSubformula()->accept(*this);
			  }
	  } dfinder;


	  BinaryRegularRelation Meta = A;

	  if (prev_A_is_true &&  // A is "always & prev_A", acceleration only works if "always" represents A
	      dynamic_cast<NullAccelerationStrategy*>(acceleration.get()) == NULL) // Check for NullAccelerationStrategy for effeciency (avoids s1s transformation)
	  {
		  always->accept(dfinder);

		  // Accelerate
		  
		  std::vector<StructureBinaryRelation> rels;
		  for (std::vector<Node*>::const_iterator i = dfinder.disjuncts.begin();i != dfinder.disjuncts.end();++i)
		  {
			  Transducer t(a, s1s_to_transducer(nfa_factory, *i));
			  rels.push_back(t.get_relation());
		  }

		  Meta = acceleration->accelerate(rels);
	  }
	  
	  BinaryRegularRelation Inv;
	  Example tail(a);
	  BinaryRegularRelation AI;
	  Alphabet ex_a = a;
	  try {
		  Inv = reach_pairs(I, A, A|Meta);

		  // Recur to find example for F'
		  tail = solve(eventuality, a, A, Inv);

		  if (tail.size() == 0) return tail;

		  // Return example ending with tail

		  ex_a = tail.get_alphabet();

		  A = RegularRelation(ex_a.get_domains(), RefNfa(A.get_automaton()).rename(a.renaming(ex_a)));
		  I = RegularRelation(ex_a.get_domains(), RefNfa(I.get_automaton()).rename(a.renaming(ex_a)));

		  AI = A & I;
		  assert(tail.size() >= 2);
		  if ( (AI.range_under(tail.at(1)) & tail.front()) == tail.front() ) // (x1, x2) in A & I
			  return tail;
	  }
	  catch (RegularComposition::Solver::Failure f) {
		  // (reach_pairs contains a solver call as a subexpression)
		  RegularSet partial(f.get_result().project_on(1));
		  tail.set_partial_result();
		  tail.push_back(get_shortest_member(partial));
		  AI = A & I;
	  }

	  RegularSet second = AI.project_on(1);
	  vector<RegularSet> to_tail = get_shortest_word(second, A, tail.front());
	  assert(!to_tail.empty());
	  RegularSet to_to_tail = get_shortest_member(AI.range_under(to_tail.front()));
	  to_tail.pop_back();
	  
	  Example path(ex_a, tail.is_partial_result());
	  path.push_back(to_to_tail);
	  for (vector<RegularSet>::iterator it = to_tail.begin(); it != to_tail.end(); it++)
		  path.push_back(*it);
	  for (vector<RegularSet>::iterator it = tail.begin(); it != tail.end(); it++)
		  path.push_back(*it);
	  return path;
  }
}

void TFormModelfinder::set_tform(Node* tform)
{
	s1s_ticks = 0;
	RefNfa nfa_universal(nfa_factory->ptr_universal());
	Domain d;
	BinaryRegularRelation Init(d, d, nfa_universal);

	Example example = solve(tform, Alphabet(space, 2), Init, Init);

	if (example.size() == 0)
	{
		cout << "Formula has no models" << endl;
	}
	else
	{
		cout << example.get_title() << endl;
		cout << example;
	}
}

}
}

	

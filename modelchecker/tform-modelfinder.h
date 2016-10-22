/*
 * tform-modelfinder.h: 
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
#ifndef TFORM_MODELFINDER_H
#define TFORM_MODELFINDER_H

#include <formula/formulamod.h>
#include <engine/engine.h>
#include <transducer/transducer.h>
#include <ostream> 
#include <memory>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>

namespace rmc
{
	namespace modelchecker
	{
		using namespace formula;
		using namespace engine;
		using namespace transducer;

		class TFormModelfinder : private VisitorDefault
		{
			gbdd::Space* space;
			gautomata::Nfa::Factory* nfa_factory;

 			RegularComposition::Solver* solver;
			RegularComposition::Solver* solver_reachability;
			std::auto_ptr<AccelerationStrategy> acceleration;

			Node* initial;
			Node* always;
			Node* eventuality;
			Node* always_eventuality;
			bool inside_always;
			bool seen_eventuality;
			
			FormulaOpApplication* app;

			clock_t s1s_ticks;
			Transducer s1s_to_transducer(gautomata::Nfa::Factory* nfa_factory, Node* f);

			virtual void visit(Formula& f);
			virtual void visit(FormulaOpApplication& f);
			virtual void visit(Op& op);
			virtual void visit(OpAnd& op);
			virtual void visit(OpAlways& f);
			virtual void visit(OpEventually& f);
			
			gautomata::RegularSet get_shortest_member(gautomata::RegularSet R) const;
			std::vector<gautomata::RegularSet> get_shortest_word(gautomata::RegularSet I, gautomata::BinaryRegularRelation R, gautomata::RegularSet F) const;

			class Example : public std::vector<gautomata::RegularSet>
			{
				Alphabet a;
				bool partial_result;
			public:
				Example(Alphabet a, bool p = false):
					a(a), partial_result(p)
					{}

				const Alphabet& get_alphabet() const { return a; }

				Example operator*(const Example& c2) const;
				void operator*=(const std::vector<gautomata::RegularSet>& v);
				friend std::ostream& operator<<(std::ostream& s, const TFormModelfinder::Example& c);
				void set_partial_result(bool p = true) { partial_result = p; }
				bool is_partial_result() { return partial_result; }
				string get_title() { if (!partial_result) return "Formula has model:"; return "Aborting run. Partial result:"; }
			};

			friend std::ostream& operator<<(std::ostream& s, const TFormModelfinder::Example& c);

			gautomata::BinaryRegularRelation solve_r1_r2star(gautomata::BinaryRegularRelation r1,
									 gautomata::BinaryRegularRelation r2,
									 RegularComposition::Solver* solver,
									 gautomata::BinaryRegularRelation* terminate_on = NULL);


			Example solve(Node*,
				      const Alphabet& in_a,
				      gautomata::BinaryRegularRelation,
				      gautomata::BinaryRegularRelation);
			gautomata::BinaryRegularRelation reach(gautomata::BinaryRegularRelation I,
							       gautomata::BinaryRegularRelation A,
							       gautomata::BinaryRegularRelation Meta);
			gautomata::BinaryRegularRelation reach_pairs(gautomata::BinaryRegularRelation I,
								     gautomata::BinaryRegularRelation A,
								     gautomata::BinaryRegularRelation Meta);
		public:
			TFormModelfinder(RegularComposition::Solver* solver,
					 gautomata::Nfa::Factory* nfa_factory);

			void set_strategy(AccelerationStrategy* st) { acceleration.reset(st); }
			void set_solver_reachability(RegularComposition::Solver* solver) { solver_reachability = solver; }

			void set_tform(Node* tform);

			unsigned int get_s1s_ms() const { return s1s_ticks * 1000 / sysconf(_SC_CLK_TCK); }

		};
	}
}


#endif /* TFORM_MODELFINDER_H */

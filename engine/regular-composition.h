/*
 * regular-composition.h: 
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
#ifndef RMC_ENGINE_REGULAR_COMPOSITION_H
#define RMC_ENGINE_REGULAR_COMPOSITION_H

#include <gautomata/gautomata.h>
#include <iostream>
#include <sstream>
#include <vector>

namespace rmc
{
	namespace engine
	{
		using namespace std;
		using gautomata::RegularSet;
		using gautomata::Nfa;
		using gautomata::BinaryRegularRelation;
		using gbdd::StructureBinaryRelation;
		using gbdd::BddSet;
		using gautomata::StateSet;
		
		class RegularComposition
		{
			vector<StructureBinaryRelation> rels;
			Nfa::Factory* nfa_factory;
			RegularSet expr;

			void init_expr_zerosym();
			RegularComposition(const vector<StructureBinaryRelation>& rels,
					   const RegularSet& expr);
		public:
			RegularComposition(const RegularComposition& comp2);
			RegularComposition(const StructureBinaryRelation& rel,
					   Nfa::Factory* nfa_factory);
			RegularComposition(const BinaryRegularRelation& rel);

			RegularComposition kleene() const;
			RegularComposition operator*(const RegularComposition& comp2) const;
			
			unsigned int n_relations() const;
			StructureBinaryRelation get_relation(unsigned int index) const;
			StructureBinaryRelation get_relation(const BddSet& indices) const;
			StructureBinaryRelation relation_between(StateSet q, StateSet r) const;

			RegularSet get_expr() const;
			
			class Solver
			{
			public:
				virtual ~Solver();
				virtual StructureBinaryRelation evaluate(const RegularComposition& comp) = 0;
/// Partly evaluate composition until it contains elements from a certain set
/**
 * This method evaluates a composition like evaluate but may terminate earlier and return a smaller relation that contains some elements we are searching for. The idea is that if we know what we are looking for, we don't need to evaulate the entire composition. Note that searching for the universe relation (using \a contains as universe) means testing the composition for emptiness.
 *
 * @param comp Composition to evaulate
 * @param contains Elements to search for
 * 
 * @return A subset R of evaluate(comp) such that R and \a contains have common elements, or evaluate(comp) if no such R exists
 */
				virtual StructureBinaryRelation evaluate_atleast_until(const RegularComposition& comp, const StructureBinaryRelation& contains);
				
				class Observer
				{
					std::string context;
				protected:
					const std::string& get_context() const { return context; }
				public:
					virtual ~Observer() {}
					virtual void set_context(std::string context_descr) { context = context_descr; }
					virtual void progress_iteration(unsigned int iteration, unsigned int size) const {};
					virtual void progress_iterate(std::string descr) const {};
					// progress_init is used just to provide information to user, and may be called several times during progress
					virtual void progress_init(std::string descr) const {};
					virtual void progress_init(unsigned int n) { std::ostringstream s;s << n; progress_init(s.str()); }

					// progress_begin/progress_end is called at start and end of computation, and may be used to measure time
					virtual void progress_begin() const {};
					virtual void progress_end(std::string stats) const {};
					void progress_end() { progress_end(""); };
					virtual void changed(const Solver& s) const {};
				};
				
				typedef Observer NullObserver;
				
				class StreamObserver : public NullObserver
				{
					std::ostream& out;
				public:
					StreamObserver(std::ostream& out):
						out(out)
						{}
					
					virtual void progress_init(std::string descr) const { out << descr;out.flush(); }
					virtual void progress_iterate(std::string descr) const { out << descr;out.flush(); }
				};
				
				virtual void set_observer(Observer* observer);				
				virtual void set_context(std::string context) = 0;

				class Failure
				{
					StructureBinaryRelation res;
				public:
					Failure(StructureBinaryRelation partial_result) : res(partial_result) {}
					StructureBinaryRelation get_result() { return res; }
				};
			};
		};
	}
}

#endif /* RMC_ENGINE_REGULAR_COMPOSITION_H */

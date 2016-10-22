/*
 * linear-acceleration-strategy.cc: 
 *
 * Copyright (C) 2004 Marcus Nilsson (marcus@it.uu.se)
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
 *    Mayank Saksena (mayanks@it.uu.se)
 *    Ahmed Rezine   (rahmed@it.uu.se)
 */

#include "linear-acceleration-strategy.h"

namespace rmc
{
namespace engine
{
	using namespace gautomata;

LinearAccelerationStrategy::~LinearAccelerationStrategy()
{}

void add_embed_disjunct(RefNfa &disjunct, const Domains &rel_domains, std::vector<StructureBinaryRelation> &to, const RefNfa &prefix, const RefNfa &postfix)
{
	assert(!disjunct.is_false());

	// embed disjunct
	disjunct = prefix.ptr_concatenation(disjunct);
	disjunct = disjunct.ptr_concatenation(postfix);

	disjunct = disjunct.deterministic().minimize().filter_states_live();
	
	RegularRelation r(rel_domains, disjunct);
	to.push_back(r);
}

void cut_correctness(const RefNfa cut, const RefNfa rnfa, const StateSet ql, const StateSet qr,
		     bool add_prefix, bool add_postfix, const RefNfa prefix, const RefNfa postfix,
		     const RefNfa cut_acc, const Domains rel_domains, bool exact = false)
{
	assert(!cut.is_false());

	// cut is inside rnfa(ql, qr)
	RefNfa inner(rnfa.ptr_with_starting_accepting(ql, qr));
	assert((inner & cut) == cut);

	// embed cut with prefix and postfix and check inclusion in rnfa
	
	RefNfa cut_embed = cut;

	if (add_prefix)
		cut_embed = RefNfa(prefix).ptr_concatenation(cut_embed);
						
	if (add_postfix)
		cut_embed = cut_embed.ptr_concatenation(RefNfa(postfix));
	
	cut_embed = cut_embed.deterministic().minimize().filter_states_live();
	const RefNfa cut_embed_rnfa(RefNfa(cut_embed & rnfa).deterministic().minimize().filter_states_live());

	if (!(cut_embed_rnfa == cut_embed)) {
		// error: cut_embed is not a subautomaton
		cout << "*** embedded cut not a subautomaton ***" << endl;
		cout << "bad cut: " << cut << endl;
		cout << "embedded: " << cut_embed << endl;
		cout << "prefix: " << prefix << endl;
		cout << "postfix: " << postfix << endl;
		cout << "restricted automaton: " << inner << endl;
		cout << "inner intersection: " <<
			(inner & cut).deterministic().minimize().filter_states_live() << endl;
		cout << "cut equals inner intersection? " << (cut == (inner & cut)) << endl;

		assert(false);
	}

	// cut is inside cut_acc
	if (!((cut - cut_acc).is_false())) {
		cout << "cut_acc doesn't contain cut" << endl;
		cout << "cut: " << cut << endl;
		cout << "cut acc: " << cut_acc << endl;
		assert(false);
	}

	// cut_acc is closed w.r.t. to cut
	const RegularRelation cut_rel(rel_domains, cut);
	const RegularRelation clos_rel(rel_domains, cut_acc);
	RegularRelation closcut_rel(clos_rel.compose(1, cut_rel) | clos_rel);
	closcut_rel = RegularRelation(rel_domains, RefNfa(closcut_rel.get_automaton()).deterministic().minimize().filter_states_live());
	
	if (!(closcut_rel == clos_rel)) {
		cout << "*** WARNING: closure candidate is not closed w.r.t. to cut (strict underapproximation of closure)***";
		if (exact) {
			// error: we expected the exact closure
			RefNfa closcut_aut(closcut_rel.get_automaton());
			cout << endl;
			cout << "closure | closure o cut:" << closcut_aut << endl;
			cout << "cut:" << cut << endl;
			cout << "closure:" << cut_acc << endl;
			cout << "closure o cut - cut:" << RefNfa((closcut_aut - cut_acc).deterministic().minimize().filter_states_live()) << endl;

			assert(closcut_rel == clos_rel);
		}
	}
}

void print_edge(const string& label, const Relation& edge, const Nfa::Factory& nfa_factory)
{
	cout << label << ": " << RefNfa(nfa_factory.ptr_symbol(edge.get_bdd())).deterministic().minimize().filter_states_live() << endl;
}

// accelerate linear binary actions in R, add to out (actually only "right" ones)
void LinearAccelerationStrategy::acc_binary_linear(const StructureBinaryRelation& R, std::vector<StructureBinaryRelation>& out)
{
/*
  Cut out and accelerate every disjunct of form:
  - (x/x)*.(z/x).(y/z).(y/y)*, with |z| = |x| = |y| = 1 (right-linear)
*/
	const RegularRelation rel(R);
	const RefNfa rnfa(rel.get_automaton());
	const RefNfa::Factory nfa_factory(rnfa.ptr_factory());
	
	const StateSet I = rnfa.states_starting();
	const StateSet F = rnfa.states_accepting();

	// this stuff can be computed once for the LinearAccelerationStrategy, I think (at least at a higher level than this)
	const gbdd::Bdd::FiniteVar w1(rel.get_space(), (rel.get_domains())[0]);
	const gbdd::Bdd::FiniteVar w2(rel.get_space(), (rel.get_domains())[1]);
	const SymbolSet copy(gbdd::Domain::infinite(), w1 == w2);
	const Relation copy_rel(rel.get_domains(), w1 == w2);
	const SymbolSet sigma(rnfa.alphabet());
	const SymbolSet notcopy(sigma - copy);
	const Relation notcopy_rel(!copy_rel);
	// end "this stuff..."

	const StateSet left_side = rnfa.reachable_successors(I, copy);
	const StateSet right_side = rnfa.reachable_predecessors(F, copy);

	// left states with self loops
	StateSet left_self = left_side;
	for (StateSet::iterator qit = left_side.begin(); qit != left_side.end(); ++qit)
	{
		StateSet q(rnfa.states(), *qit);
		if ((rnfa.edge_between(q, q) & copy).is_empty())
			left_self -= q;
	}

	for (StateSet::iterator lit = left_self.begin(); lit != left_self.end(); ++lit)
	{
		const StateSet qL(rnfa.states(), *lit); // fix one left state
		
		// prefix computation (for acceleration below)
		const RefNfa id(RefNfa(nfa_factory.ptr_symbol(copy)->ptr_kleene()).deterministic().minimize().filter_states_live());
		const RefNfa prefix = (RefNfa(rnfa.ptr_with_starting_accepting(I, qL)) & id).deterministic().minimize().filter_states_live();

		const StateSet super_middle((rnfa.successors(qL, notcopy) & rnfa.predecessors(right_side, notcopy)) - qL); // overapprox. of middle states

		for (StateSet::iterator mit = super_middle.begin(); mit != super_middle.end(); ++mit)
		{
			const StateSet qM(rnfa.states(), *mit); // fix one middle state

			const Relation qL_qM(rel.edge_between(qL, qM) & notcopy_rel);
			const Relation qL_copy(rel.edge_between(qL, qL) & copy_rel);
			const Relation zx(qL_qM.compose(1, qL_copy)); // all occuring "zx"-type edges

			if (zx.is_false())
				continue;

			std::vector<Relation> zx_subs;
#if 0
			unary_idempotent_subrels(zx, zx_subs);
			assert(zx_subs.size() > 0);
#else
			zx_subs.push_back(zx);
#endif
			std::vector<Relation>::const_iterator i = zx_subs.begin();
			while (i != zx_subs.end()) {
				const Relation zx_sub(*i);
				const BddSet z_zx_sub(zx_sub.project_on(0));
				
				const StateSet super_right((rnfa.successors(qM, (notcopy_rel.restrict(1, z_zx_sub)).get_bdd()) - qM) & right_side); // overapprox. of right states

				for (StateSet::iterator rit = super_right.begin(); rit != super_right.end(); ++rit)
				{
					const StateSet qR(rnfa.states(), *rit); // fix one right state

					//const bool add_postfix = (qR & F).is_empty();
					//const RefNfa postfix = add_postfix ? (RefNfa(rnfa.ptr_with_starting_accepting(qR, F)) & id) : nfa_factory.ptr_empty();
					const RefNfa postfix = RefNfa(rnfa.ptr_with_starting_accepting(qR, F)) & id;

					const Relation qM_qR(rel.edge_between(qM, qR) & notcopy_rel);
					const Relation qR_copy(rel.edge_between(qR, qR) & copy_rel);
					const Relation yz(qR_copy.compose(1, qM_qR).restrict(1, z_zx_sub)); // all occuring "yz"-type edges

					if (yz.is_false())
						continue;

					std::vector<Relation> yz_subs;
#if 0
					unary_idempotent_subrels(yz, yz_subs);
					assert(yz_subs.size() > 0);
#else
					yz_subs.push_back(yz);
#endif
					std::vector<Relation>::const_iterator j = yz_subs.begin();
					while (j != yz_subs.end()) {
						const Relation yz_sub(*j);
						
						// found a binary right disjunct
										
						RefNfa cut(nfa_factory.ptr_empty());
						const State cut_init = cut.add_state(false, true);
						const State cut_one = cut.add_state(false);
						const State cut_final = cut.add_state(true);
						
						cut.add_edge(cut_init, qL_copy.get_bdd(), cut_init);
						cut.add_edge(cut_init, zx_sub.get_bdd(), cut_one);
						cut.add_edge(cut_one, yz_sub.get_bdd(), cut_final);
						cut.add_edge(cut_final, qR_copy.get_bdd(), cut_final);
						
						// cut acceleration
#if 1
						// manual acceleration
						const Relation yx(yz_sub.compose(1, zx_sub));
						assert(!(yx.is_false()));
						
						RefNfa acc = cut;
						acc.add_edge(cut_one, yx.get_bdd(), cut_one);
						acc = acc.deterministic().minimize().filter_states_live();
#else
						// accelerate with eqcol
						const RegularRelation acc_me(rel.get_domains(), cut);
						//const RegularRelation acc_me(rel.get_domains(), rnfa.with_starting_accepting(qL, qR));
						
						StructureBinaryRelation acc_rel = acc_linear_eqcol(acc_me);
						RefNfa acc = RefNfa(RegularRelation(acc_rel).get_automaton()).deterministic().minimize().filter_states_live();
#endif
#if 0
						cout << "acc input: " << cut << endl;
						cout << "acc output: " << acc << endl;
						
						// debug: check correctness of the cut
						cut_correctness(cut, rnfa, qL, qR, add_prefix, add_postfix, prefix, postfix, acc, rel.get_domains());
#endif
						add_embed_disjunct(acc, rel.get_domains(), out, prefix, postfix);
						//cout << "binary acc: " << acc << endl;

						++j;
					}
				}
				++i;
			}
		}
	}
}

// t-c
StructureBinaryRelation acc_tc(const StructureBinaryRelation& L)
{
 	eqcol::EqColumnTransducer a;
 	Nfa::Factory* nfa_factory(RegularRelation(L).get_automaton().ptr_factory());
 
 	RegularComposition T(L, nfa_factory);
 	StructureBinaryRelation res = a.evaluate(T * T.kleene());

 	if (const Nfa* nfa = dynamic_cast<const Nfa*>(&(res.get_bdd_based())))
 	{
 		gautomata::RefNfa rnfa(*nfa);
 		res = StructureRelation(res.get_domains(), rnfa.deterministic().minimize().filter_states_live());
 	}
 
 	return res;
}

// unary acceleration
RefNfa acc_unary_one(const Relation& qL_qL, const Relation& qL_qR, const Relation& qR_qR, const Nfa::Factory& nfa_factory, const Domains& domains)
{
	RefNfa clos(nfa_factory.ptr_empty());
	
	const Relation copy = EquivalenceRelation::identity(qL_qR.get_space(), qL_qR.get_domain(0), qL_qR.get_domain(1));
	const Relation qR_copy(qR_qR & copy);
	const BddSet sigmaR(qR_copy.project_on(0));
	const Relation qL_copy(qL_qL & copy);
	const Relation qLR_copy(qL_copy & qR_copy);
	const BddSet sigmaL(qL_copy.project_on(0));

	const Relation t(qL_qR);
	const Relation t_cR(t.restrict(0, sigmaR));
	const Relation t_c2R(t.restrict(1, sigmaR));
	const Relation t_cL(t.restrict(0, sigmaL));
	const Relation t_c2L(t.restrict(1, sigmaL));

	// quick abort: sometimes we cannot get loops
	if ((t_cL.is_false() || t_c2R.is_false()) && (t_c2L.is_false() || t_cR.is_false()))
	{
		//cout << "skipping not acceleratable action" << endl;
		return clos;
	}

	const State qL = clos.add_state(false, true);
	const State qR = clos.add_state(true);
	const State q1 = clos.add_state(false);
	const State q2 = clos.add_state(false);
	const State q3 = clos.add_state(false);
	const State q4 = clos.add_state(false);

	const Relation t_cL_c2L(t.restrict(0, sigmaL).restrict(1, sigmaL));
	const Relation t_c2L_cRc2R(t.restrict(1, sigmaL).restrict(0, sigmaR) | t.restrict(1, sigmaL).restrict(1, sigmaR));
	const Relation t_cL_c2L_cRc2R(t.restrict(0, sigmaL).restrict(1, sigmaL).restrict(0, sigmaR) | t.restrict(0, sigmaL).restrict(1, sigmaL).restrict(1, sigmaR));
	const Relation t_cL_cRc2R(t.restrict(0, sigmaL).restrict(0, sigmaR) | t.restrict(0, sigmaL).restrict(1, sigmaR));
	const Relation t_c2L_cR(t.restrict(1, sigmaL).restrict(0, sigmaR));
	const Relation t_cR_cLc2L(t.restrict(0, sigmaR).restrict(0, sigmaL) | t.restrict(0, sigmaR).restrict(1, sigmaL));
	const Relation t_cL_c2R(t.restrict(0, sigmaL).restrict(1, sigmaR));
	const Relation t_c2R_cLc2L(t.restrict(1, sigmaR).restrict(0, sigmaL) | t.restrict(1, sigmaR).restrict(1, sigmaL));
	const Relation t_cR_c2R_cLc2L(t.restrict(0, sigmaR).restrict(1, sigmaR).restrict(0, sigmaL) | t.restrict(0, sigmaR).restrict(1, sigmaR).restrict(1, sigmaL));
	const Relation t_cR_c2R(t.restrict(0, sigmaR).restrict(1, sigmaR));

	struct
	{
		State from;
		Relation edge;
		State to;
	} edge_info[] = {
		{q1, qLR_copy, q1},
		{q2, qLR_copy, q2},
		{q3, qLR_copy, q3},
		{q4, qLR_copy, q4},
		{qL, t, qR},
		{qL, t_cL_c2L, q1},
		{qL, t_c2L, q2},
		{qL, t_cL, q3},
		{q1, t_cL_c2L_cRc2R, q1},
		{q1, t_c2L_cRc2R, q2},
		{q1, t_cL_cRc2R, q3},
		{q2, t_cR, qR},
		{q2, t_c2L_cR, q2},
		{q2, t_cR_cLc2L, q4},
		{q3, t_c2R, qR},
		{q3, t_cL_c2R, q3},
		{q3, t_c2R_cLc2L, q4},
		{q4, t_cR_c2R, qR},
		{q4, t_cR_c2R_cLc2L, q4},
		{qL, qL_copy, qL},
		{qR, qR_copy, qR}
	};

	for (unsigned int i = 0;  i < (sizeof(edge_info) / sizeof(edge_info[0])); ++i)
	{
		clos.add_edge(edge_info[i].from, edge_info[i].edge.get_bdd(), edge_info[i].to);
	}

	const RefNfa ugly(clos);
	clos = clos.deterministic().minimize().filter_states_live();

#if 0
	{
		// correctness

		RefNfa t_rnfa(nfa_factory.ptr_empty());
		const State t_init = t_rnfa.add_state(false, true);
		const State t_final = t_rnfa.add_state(true);
		t_rnfa.add_edge(t_init, qL_copy.get_bdd(), t_init);
		t_rnfa.add_edge(t_init, t.get_bdd(), t_final);
		t_rnfa.add_edge(t_final, qR_copy.get_bdd(), t_final);
		t_rnfa = t_rnfa.deterministic().minimize().filter_states_live();

		const RegularRelation acc_me(domains, t_rnfa);
		const RegularRelation acc_result(domains, clos);

		// accelerate with eqcol
		RegularRelation acc_rel(acc_tc(acc_me));
		RefNfa acc = RefNfa(acc_rel.get_automaton());

		if (!(acc == clos))
		{
			cout << "acceleration was not exact!" << endl;
			cout << "manual: " << clos << endl;
			cout << "exact: " << acc << endl;
			cout << "exact - manual: " << (acc - clos).deterministic().minimize().filter_states_live() << endl;
			cout << "manual - exact: " << (clos - acc).deterministic().minimize().filter_states_live() << endl;
			cout << "cut: " << t_rnfa << endl;

			print_edge("t_cL", t_cL, nfa_factory);
			print_edge("t_c2L", t_c2L, nfa_factory);
			print_edge("t_cR", t_cR, nfa_factory);
			print_edge("t_c2R", t_c2R, nfa_factory);
			cout << "unmin: " << ugly << endl;

			if (!((acc_rel.compose(1, acc_me) - acc_rel).get_automaton().is_false()))
				cout << "eqcol closure is not closed under cut" << endl;
			else
				cout << "eqcol closure is closed under cut" << endl;

			if (!((acc_result.compose(1, acc_me) - acc_result).get_automaton().is_false()))
				cout << "manual closure is not closed under cut" << endl;
			else
				cout << "manual closure is closed under cut" << endl;

			assert(qL_qL == qL_copy && qR_qR == qR_copy);

			assert(false);
		}
		assert(acc == clos);

		if (!((acc_result.compose(1, acc_me) - acc_result).get_automaton().is_false()))
		{
			const RegularRelation ugly_rel(domains, ugly);
			print_edge("t", t, nfa_factory);
			print_edge("t^2", t.compose(1, t), nfa_factory);
			print_edge("q2_qR", ugly_rel.edge_between(q2, qR), nfa_factory);
			print_edge("q3_qR", ugly_rel.edge_between(q3, qR), nfa_factory);
			print_edge("q4_qR", ugly_rel.edge_between(q4, qR), nfa_factory);

			cout << "unmin closure: " << ugly << endl;
#if 0
			print_edge("t", t, nfa_factory);
			print_edge("t^2", t.compose(1, t), nfa_factory);
			print_edge("qLR_copy", qLR_copy, nfa_factory);
			print_edge("cL: ", qL_copy, nfa_factory);
			print_edge("cR: ", qR_copy, nfa_factory);

			print_edge("t_c2L", t_c2L, nfa_factory);
			print_edge("t_cL_c2L", t_cL_c2L, nfa_factory);
			print_edge("t_cL", t_cL, nfa_factory);

			print_edge("t_cL_c2L_cRc2R", t_cL_c2L_cRc2R, nfa_factory);
			print_edge("t_c2L_cRc2R", t_c2L_cRc2R, nfa_factory);
			print_edge("t_cR_c2R_cLc2L", t_cR_c2R_cLc2L, nfa_factory);

			print_edge("t_c2L_cR", t_c2L_cR, nfa_factory);
			print_edge("t_cR", t_cR, nfa_factory);
			print_edge("t_cR_cLc2L", t_cR_cLc2L, nfa_factory);

			print_edge("t_cL_c2R", t_cL_c2R, nfa_factory);
			print_edge("t_c2R_cLc2L", t_c2R_cLc2L, nfa_factory);
			print_edge("t_c2R", t_c2R, nfa_factory);

			print_edge("t_cR_c2R_cLc2L", t_cR_c2R_cLc2L, nfa_factory);
			print_edge("t_cR_c2R", t_cR_c2R, nfa_factory);

			cout << "cut:" << t_rnfa << endl;
			cout << "closure unmin: " << ugly << endl;
#endif
			cout << "closure:" << clos << endl;
			cout << "closure o cut:" << RefNfa(acc_result.compose(1, acc_me).get_automaton()).deterministic().minimize().filter_states_live() << endl;
			cout << "closure o cut - cut:" << RefNfa((acc_result.compose(1, acc_me) - acc_result).get_automaton()).deterministic().minimize().filter_states_live() << endl;
			assert(false);
		}
	}
#endif
	return clos;
}

void LinearAccelerationStrategy::accelerate_semantic(const StructureBinaryRelation& R, std::vector<StructureBinaryRelation>& out, bool accelerate)
{
	RegularRelation rel(R);
	rel = RegularRelation(rel.get_domains(), RefNfa(rel.get_automaton()).deterministic().minimize().filter_states_live());

#if 1
	{
#if 0
		// extract right states (if we want something else than right idempotent states)

		const RefNfa rnfa = RefNfa(rel.get_automaton()).filter_states_live();
		
		const SymbolSet copy(rel.get_domain(0) | rel.get_domain(1), gbdd::EquivalenceRelation::identity(rel.get_space(), rel.get_domain(0), rel.get_domain(1)).get_bdd());
		const SymbolSet notcopy(rnfa.alphabet() - copy);

		// left states
		StateSet left = rnfa.reachable_successors(rnfa.states_starting(), copy);
		{
			StateSet left_self = left; // with self loops
			for (StateSet::iterator qit = left.begin(); qit != left.end(); ++qit)
			{
				StateSet q(rnfa.states(), *qit);
				if ((rnfa.edge_between(q, q) & copy).is_empty())
					left_self -= q;
			}
			left = left_self;
		}
		
		// right states
		StateSet right = StateSet::empty(rnfa.states());
		{
			const Relation notcopy_rel(!(gbdd::EquivalenceRelation::identity(rel.get_space(), rel.get_domain(0), rel.get_domain(1))));

			const StateSet right_super = rnfa.reachable_predecessors(rnfa.states_accepting(), copy);

			for (StateSet::iterator lit = left.begin(); lit != left.end(); ++lit)
			{
				const StateSet qL(rnfa.states(), *lit);
				const StateSet sux = (rnfa.successors(qL, notcopy) - qL) & right_super; // noncopying outgoing edges from qL to productive {qr}
				
				for (StateSet::iterator rit = sux.begin(); rit != sux.end(); ++rit)
				{
					const StateSet qR(rnfa.states(), *rit);
					const Relation tau = rel.edge_between(qL, qR);
					if ((tau & notcopy_rel) == tau) // all of tau is noncopying
						right |= qR;
				}
			}
		}

		eqcol::EqColumnTransducer::RightDeterministicParam ps(right);
#else
		eqcol::EqColumnTransducer::RightIdemDeterministic ps;
#endif
		rel = ps.prepare(rel); // right determinize
	}
#endif

	// extract disjuncts

	const RefNfa rnfa = RefNfa(rel.get_automaton()).filter_states_live();	
	const RefNfa::Factory nfa_factory(rnfa.ptr_factory());

	const StateSet I = rnfa.states_starting();
	const StateSet F = rnfa.states_accepting();

	const SymbolSet copy(rel.get_domain(0) | rel.get_domain(1), gbdd::EquivalenceRelation::identity(rel.get_space(), rel.get_domain(0), rel.get_domain(1)).get_bdd());
	const SymbolSet notcopy(rnfa.alphabet() - copy);

	const StateSet right_super = rnfa.reachable_predecessors(rnfa.states_accepting(), copy);

	// left states
	StateSet left = rnfa.reachable_successors(I, copy);
	{
		StateSet left_self = left; // with self loops
		for (StateSet::iterator qit = left.begin(); qit != left.end(); ++qit)
		{
			StateSet q(rnfa.states(), *qit);
			if ((rnfa.edge_between(q, q) & copy).is_empty())
				left_self -= q;
		}
		left = left_self;
	}

	const Relation copy_rel(gbdd::EquivalenceRelation::identity(rel.get_space(), rel.get_domain(0), rel.get_domain(1)));
	const Relation notcopy_rel(!copy_rel);

	for (StateSet::iterator lit = left.begin(); lit != left.end(); ++lit)
	{
		const StateSet qL(rnfa.states(), *lit);
		const Relation qL_copy(rel.edge_between(qL, qL) & copy_rel);

		const StateSet QR((rnfa.successors(qL, notcopy) - qL) & right_super); // noncopying outgoing edges from qL to productive {qr}_qL
		for (StateSet::iterator rit = QR.begin(); rit != QR.end(); ++rit)
		{
			const StateSet qR(rnfa.states(), *rit);
			const Relation qR_copy(rel.edge_between(qR, qR) & copy_rel);

			const Relation tau = rel.edge_between(qL, qR);

#if 0
			RefNfa acc(acc_unary_one(qL_copy, tau, qR_copy, nfa_factory, rel.get_domains()));
			if (acc.is_false())
				continue;
#else
			RefNfa tau_rnfa(nfa_factory.ptr_empty());
			const State tau_init = tau_rnfa.add_state(false, true);
			const State tau_final = tau_rnfa.add_state(true);
			tau_rnfa.add_edge(tau_init, qL_copy.get_bdd(), tau_init);
			tau_rnfa.add_edge(tau_init, tau.get_bdd(), tau_final);
			tau_rnfa.add_edge(tau_final, qR_copy.get_bdd(), tau_final);

			// accelerate with eqcol
			const RegularRelation acc_me(rel.get_domains(), tau_rnfa);
			StructureBinaryRelation acc_rel = acc_tc(acc_me);
			RefNfa acc = RefNfa(RegularRelation(acc_rel).get_automaton());
#endif
			const RefNfa id(RefNfa(nfa_factory.ptr_symbol(copy)->ptr_kleene()).deterministic().minimize().filter_states_live());
			const RefNfa prefix = (RefNfa(rnfa.ptr_with_starting_accepting(I, qL)) & id).deterministic().minimize().filter_states_live();
			const RefNfa postfix = RefNfa(rnfa.ptr_with_starting_accepting(qR, F)) & id;

			if (!accelerate)
			{
				RefNfa tau_rnfa(nfa_factory.ptr_empty());
				const State tau_init = tau_rnfa.add_state(false, true);
				const State tau_final = tau_rnfa.add_state(true);
				tau_rnfa.add_edge(tau_init, qL_copy.get_bdd(), tau_init);
				tau_rnfa.add_edge(tau_init, tau.get_bdd(), tau_final);
				tau_rnfa.add_edge(tau_final, qR_copy.get_bdd(), tau_final);

				add_embed_disjunct(tau_rnfa, rel.get_domains(), out, prefix, postfix);
				return;
			}

			add_embed_disjunct(acc, rel.get_domains(), out, prefix, postfix);
		}
	}
}

std::vector<StructureBinaryRelation> LinearAccelerationStrategy::accelerate_vector(const std::vector<StructureBinaryRelation>& rels)
{
	/*
	  Return the union of accelerated semantic disjuncts (cut out from the union of rels) + the union of rels
	*/

	assert(rels.size() > 0);

	StructureBinaryRelation sum = *rels.begin();
	for (std::vector<StructureBinaryRelation>::const_iterator i = rels.begin(); i != rels.end(); ++i)
		sum |= *i;

	if (const Nfa* nfa = dynamic_cast<const Nfa*>(&(sum.get_bdd_based())))
	{
		gautomata::RefNfa rnfa(*nfa);
		sum = StructureRelation(sum.get_domains(), rnfa.deterministic().minimize().filter_states_live());
	}

	std::vector<StructureBinaryRelation> disjuncts;
	disjuncts.push_back(sum); // by AccelerationStrategy specification, we should include the union of actions

	accelerate_semantic(sum, disjuncts);
	//acc_binary_linear(sum, disjuncts);

	return disjuncts;
}

}
}

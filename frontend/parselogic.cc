/*
 * parselogic.cc: 
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * Authors:
 *    Marcus Nilsson (marcus@it.uu.se)
 *    Ahmed Rezine   (rahmed@it.uu.se)
 *    Mayank Saksena (mayanks@it.uu.se)
 */

//#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include <modelchecker/modelchecker.h>
#include <engine/engine.h>
#include <structure/structure.h>
#undef yyFlexLexer
#define yyFlexLexer rmclogic_ir_parser_FlexLexer
#include <FlexLexer.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <getopt.h>

extern char *optarg;
extern int optind, opterr, optopt;

using namespace std;
using namespace rmc::formula;
using namespace rmc::modelchecker;
using namespace rmc::engine;
using namespace rmc::transducer;

int main(int argc, char** argv)
{
	std::string* subst_from = NULL;
	Node* subst_to = NULL;
	bool verbose = false;
	gbdd::Space* space = gbdd::Space::create_default();
	gautomata::Nfa::Factory* nfa_factory = new gautomata::BNfa::Factory(space);
	
	typedef map<std::string, rmc::engine::RegularComposition::Solver*> SolverMap;
	typedef map<std::string, rmc::engine::AccelerationStrategy*> AccelerationMap;

	AccelerationMap accelerations;
	accelerations["none"] = new rmc::engine::NullAccelerationStrategy();
	accelerations["tc"] = new rmc::engine::TCAccelerationStrategy(nfa_factory);
	accelerations["linear"] = new rmc::engine::LinearAccelerationStrategy(nfa_factory);
	
	SolverMap solvers;
	rmc::engine::fixpoint::FixpointSolver* fixpoint_solver = new rmc::engine::fixpoint::FixpointSolver();
	solvers["eqcol"] = new rmc::engine::eqcol::EqColumnTransducer();
	solvers["eqsize"] = new rmc::engine::eqcol::EqsizeSolver();
	solvers["subset"] = new rmc::engine::subset::ColumnTransducerSolver();
	solvers["fixpoint"] = fixpoint_solver;

	int fixpoint_maxsize = -1;
	
	// Defaults
	rmc::engine::RegularComposition::Solver* solver = solvers["eqcol"];
	rmc::engine::RegularComposition::Solver* solver_reachability = NULL;
	rmc::engine::RegularComposition::Solver* solver_acceleration = NULL;
	rmc::engine::AccelerationStrategy* acceleration = accelerations["none"];
	rmc::engine::AccelerationStrategy* fixpoint_acceleration = accelerations["none"];

	rmc::structure::Signature* sig = new rmc::structure::FiniteWord(nfa_factory, space);

	bool opt_print_initial = false;
	bool opt_print_tform = false;
	
	while (1)
	{
		int option_index = 0;
		static struct option long_options[] = {
			{"subst_from", 1, 0, 0},
			{"subst_to", 1, 0, 0},
			{"verbose", 0, 0, 0},
			{"help", 0, 0, 0},
			{"solver", 1, 0, 0},
			{"solver-reachability", 1, 0, 0},
			{"solver-acceleration", 1, 0, 0},
			{"print-initial", 0, 0, 0},
			{"print-tform", 0, 0, 0},
			{"acceleration", 1, 0, 0},
			{"fixpoint-acceleration", 1, 0, 0},
			{"fixpoint-maxstates", 1, 0, 0},
			{0, 0, 0, 0}
		};
		
		const unsigned int n_options = sizeof(long_options) / sizeof(struct option) - 1;

		std::string solvervals;
		for (SolverMap::const_iterator i = solvers.begin();i != solvers.end();)
		{
			solvervals += i->first;
			++i;
			if (i != solvers.end()) solvervals += ",";
		}

		std::string accelerationvals;
		for (AccelerationMap::const_iterator i = accelerations.begin();i != accelerations.end();)
		{
			accelerationvals += i->first;
			++i;
			if (i != accelerations.end()) accelerationvals += ",";
		}

		static struct optioninfo
		{
			std::string help;
		} info_options[n_options] = {
			{"Substitute from val"},
			{"Substitute to val"},
			{"Enable verbose debug output"},
			{"Display this help messsage"},
			{"Choose solve engine val from {" + solvervals + "}"},
			{"Choose reachability solve engine val from {" + solvervals + "}"},
			{"Choose acceleration solve engine val from {" + solvervals + "}"},
			{"Print structure constraint for formula without temporal operators"},
			{"Print final formula for modelfinder"},
			{"Choose acceleration strategy val from {" + accelerationvals + "}"},
			{"Choose acceleration strategy val for the fixpoint solver from {" + accelerationvals + "}"},
			{"For a fixpoint solver, bound the number of states of automata before aborting a run"}
		};
		
		int c = getopt_long (argc, argv, "",
				long_options, &option_index);
		if (c == -1)
			break;
		
		switch (c) 
		{
		case 0:
			switch(option_index)
			{
			case 0:
				subst_from = new std::string(optarg);
				break;
			case 1:
				subst_to = new Name(new std::string(optarg));
				break;
			case 2:
				verbose = true;
				break;
			case 3:
				std::cout << "Usage: " << argv[0] << " [options] [filename]" << std::endl << std::endl;
				std::cout << "Options are given as --optionname=val where valid optionnames are:" << std::endl << std::endl;

			       for(unsigned int i = 0;i < n_options;++i)
			       {
				       struct optioninfo* info = &info_options[i];

				       printf("%-23s %s\n", long_options[i].name, info->help.c_str());
			       }

			       return 1;
		       case 4:
		       {
			       SolverMap::const_iterator i = solvers.find(optarg);

			       if (i == solvers.end())
			       {
				       std::cout << "No such solver: " << optarg << "\n";
				       return 1;
			       }

			       solver = i->second;
		       }
		       break;
		       case 5:
		       {
			       SolverMap::const_iterator i = solvers.find(optarg);

			       if (i == solvers.end())
			       {
				       std::cout << "No such solver: " << optarg << "\n";
				       return 1;
			       }

			       solver_reachability = i->second;
		       }
		       break;
		       case 6:
		       {
			       SolverMap::const_iterator i = solvers.find(optarg);

			       if (i == solvers.end())
			       {
				       std::cout << "No such solver: " << optarg << "\n";
				       return 1;
			       }

			       solver_acceleration = i->second;
		       }
		       break;
		       case 7:
			       opt_print_initial = true;
			       break;
		       case 8:
			       opt_print_tform = true;
			       break;
		       case 9:
		       {
			       AccelerationMap::const_iterator i = accelerations.find(optarg);

			       if (i == accelerations.end())
			       {
				       std::cout << "No such acceleration: " << optarg << "\n";
				       return 1;
			       }
			       
			       acceleration = i->second;

			       if (strcmp(optarg, "none") != 0)
			       {
				       if (solver_reachability == NULL) solver_reachability = solvers["fixpoint"];
				       if (solver_acceleration == NULL) solver_acceleration = solvers["eqcol"];
			       }

		       }
		       break;
		       case 10:
		       {
			       AccelerationMap::const_iterator i = accelerations.find(optarg);

			       if (i == accelerations.end())
			       {
				       std::cout << "No such acceleration: " << optarg << "\n";
				       return 1;
			       }

			       fixpoint_acceleration = i->second;

			       if (strcmp(optarg, "none") != 0)
			       {
				       if (solver_reachability == NULL) solver_reachability = solvers["fixpoint"];
				       if (solver_acceleration == NULL) solver_acceleration = solvers["eqcol"];
			       }

			       if (strcmp(optarg, "linear") == 0)
				       fixpoint_solver->set_maxacc(1);
		       }
		       break;
		       case 11:
		       {
			       int n = atoi(optarg);

			       if (n < 0)
			       {
				       std::cout << "State bound must be non-negative.\n";
				       return 1;
			       }
			       
			       fixpoint_maxsize = n;
		       }
		       break;
		       default:
			       printf ("?? getopt returned character code 0%o ??\n", c);
			       break;
	               }
	       }
	}
	
	if (solver_reachability == NULL) solver_reachability = solver;				 
	if (solver_acceleration == NULL) solver_acceleration = solver;

	std::istream* is = &std::cin;
	if (optind < argc)
	{
		std::ifstream* s = new std::ifstream(argv[optind]);
		if(!*s)
		{
			fprintf(stderr, "Error opening: %s\n", argv[1]);
			exit(1);
		}

		is = s;
	}

	// Configure components

	acceleration->set_solver(solver_acceleration);
	fixpoint_acceleration->set_solver(solver_acceleration); // need to set an acceleration solver (e.g. tc needs one)
	fixpoint_solver->set_strategy(fixpoint_acceleration);
	fixpoint_solver->set_maxsize(fixpoint_maxsize);

	// Parse

	rmc::formula::Parser parser;
	Node* n = parser.parse(*is);

	assert (n != NULL);

	// Add signature
	n = (*sig)(n);

	// Arg substitution
	if (subst_from != NULL && subst_to != NULL)
	{
		StringNodeSubstitution s;
		s[*subst_from] = subst_to;
		n = BodyStringSubstituter(s)(n);
	}

	// Bind

	Binder binder;
	n = binder(n);

	// Remove Let
	n = LetRemover()(n);

	// Push negations
	VisitorNegationPusher* negv = new VisitorNegationPusher();
	n->accept(*negv);
	n = negv->getFormula();

	if (opt_print_initial)
	{
		cout << "Formula: " << *n << endl;

		TransducerBuilder* builder = new TransducerBuilder(sig->structure_factory(), Alphabet(space, 2));
		Transformer transf(builder);

		n->accept(transf);
		
		Transducer res = builder->getResult();

		cout << res << endl;

		return 0;
	}
		

	// Move temporal operators to root
	n = TFormTransformer(sig->op_boolean_pos())(n);

	if (opt_print_tform)
	{
		cout << *n << endl;
	}


	// Find models
	{
		TFormModelfinder finder(solver, nfa_factory);
		finder.set_strategy(acceleration);
		finder.set_solver_reachability(solver_reachability);
		
		RegularComposition::Solver::Observer* observer;

		if (verbose) observer = new rmc::engine::RegularComposition::Solver::StreamObserver(cout);
		else
		{
			using rmc::engine::RegularComposition;
			class Observer : public RegularComposition::Solver::Observer
			{
				auto_ptr<struct tms> start_time;
			public:
				Observer():
					start_time(new struct tms())
					{}

				void progress_iteration(unsigned int iteration, unsigned int size) const
					{
						std::cout << "\r";
						printf("[%6d] ", size);
						std::cout << get_context() << ": ";
						for (unsigned int i = 0;i < iteration;++i) std::cout << "#";
						std::cout.flush();
					}

				void progress_begin() const
					{
						times(start_time.get());
					}
						
				void progress_end(std::string stats) const
					{
						struct tms end_time;
						times(&end_time);

						clock_t total_time_ticks = end_time.tms_utime - start_time->tms_utime;
						unsigned int total_time_msecs = total_time_ticks * 1000 / sysconf(_SC_CLK_TCK);

						std::cout << " [" << stats << "] " << "(" << total_time_msecs << " ms)\n";
					}
			};
			
			observer = new Observer();
		}

		solver->set_observer(observer);
		solver_reachability->set_observer(observer);
		solver_acceleration->set_observer(observer);

		struct tms start_time;
		struct tms end_time;

		times(&start_time);
		finder.set_tform(n);
		times(&end_time);

		clock_t total_time_ticks = end_time.tms_utime - start_time.tms_utime;
		unsigned int total_time_msecs = total_time_ticks * 1000 / sysconf(_SC_CLK_TCK);

		cout << "S1S transform time (ms):" << finder.get_s1s_ms() << endl;
		cout << "Total time (ms):" << total_time_msecs << endl;
		cout << "Total time - S1S (ms):" << total_time_msecs - finder.get_s1s_ms() << endl;
	}

	return 0;
}

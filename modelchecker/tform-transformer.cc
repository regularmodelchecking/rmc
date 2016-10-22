/*
 * tform-transformer.cc: 
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

#include "tform-transformer.h"
#include <sstream>
#include <assert.h>

namespace rmc
{
namespace modelchecker
{

void TFormTransformer::visit(Name& n)
{
	TransformerFormula::visit(n);
	if (current_context().is_quantified(n))
	{
		free_variables.insert(n);
	}
}
	
void TFormTransformer::visit(FormulaQuantification& f)
{
	push_context(f);

 	TransformerFormula::visit(f);
	free_variables.erase(*(f.getVariable()));
 
	pop_context();
}
      

void TFormTransformer::visit(Op& op)
{
	TransformerFormula::visit(op);
	getBuilder()->buildFormulaOpApplication();
}

Formula* TFormTransformer::Encoding::get_encoding(TimeIndex* t, Name* index_var) const
{
	return new FormulaOpApplication(new OpIndexTrue,
					encoding_var,
					index_var,
					t);
}
		
Formula* TFormTransformer::Encoding::get_encoding(TimeIndex* t) const
{
	Name* index_var = free_var == NULL ? new Name(new std::string("i")) : free_var;
	
	Formula* enc_f = get_encoding(t, index_var);

	if (free_var == NULL)
	{
		enc_f = new FormulaQuantification(new QuantifierFOForall(),
						  index_var,
						  new FormulaOpApplication(new OpImplies(),
									   new FormulaOpApplication(op_boolean_pos, index_var),
									   enc_f));
	}

	return enc_f;
}

Formula* TFormTransformer::Encoding::close_formula(Formula* f) const
{
	if (free_var != NULL)
	{
		f = new FormulaQuantification(new QuantifierFOForall(),
					      free_var,
					      f);
	}

	return f;
}

Formula* TFormTransformer::Encoding::always_constraint() const
{
	return NULL;

	if (free_var == NULL && encoding_var != NULL)
	{
		Name* index1 = new Name(new std::string("i"));
		Name* index2 = new Name(new std::string("j"));

		Formula* f1 = new FormulaOpApplication(new OpIff(),
						      get_encoding(new TimeIndex(0), index1),
						      get_encoding(new TimeIndex(0), index2));

		f1 = new FormulaQuantification(new QuantifierFOForall(), index1,
					      new FormulaQuantification(new QuantifierFOForall(), index2,
									f1));

		Formula* f2 = new FormulaOpApplication(new OpIff(),
						      get_encoding(new TimeIndex(1), index1),
						      get_encoding(new TimeIndex(1), index2));

		f2 = new FormulaQuantification(new QuantifierFOForall(), index1,
					      new FormulaQuantification(new QuantifierFOForall(), index2,
									f2));

//		return new FormulaOpApplication(new OpAnd(), f1, f2);
		return f1;
	}

	return NULL;
};
									   


const TFormTransformer::Encoding& TFormTransformer::encode(EncodingMap& encodings, std::string prefix, Node* formula, Name* free_var)
{
	EncodingMap::iterator i = encodings.find(formula);
	
	if (i == encodings.end())
	{
		Name* encoding_var;
		{
			std::ostringstream s;
			s << prefix << (unsigned int)next_name;
			next_name++;
			encoding_var = new Name(new std::string(s.str()));
		}
		
		
		i = encodings.insert(std::pair<Node*, Encoding>(formula, Encoding(formula, encoding_var, free_var, op_boolean_pos))).first;
	}
	
	if (Formula* f = i->second.always_constraint()) add_always(f);

	return i->second;
}

void TFormTransformer::add_always(Formula* constr)
{
	constr_t0_alw.add(constr);
}

void TFormTransformer::visit(OpAlways& op)
{
	NodeList& args = dynamic_cast<NodeList&>(*(getResult()));

	if (current_context().holds_t0 && current_context().holds_forall)
	{
		add_always(current_context().close_formula(dynamic_cast<Formula*>(args.at(0))));
		getBuilder()->push(new FormulaTrue());
		return;
	}

	const Encoding& enc = encode(local_encodings, "x", args.at(0), free_variables.empty() ? NULL : new Name(*(free_variables.begin())));
	
	getBuilder()->push(enc.get_encoding_current());

	add_always(enc.close_formula(new FormulaOpApplication(new OpIff(), 
							      new FormulaOpApplication(new OpAnd(),
										       enc.get_encodes(),
										       enc.get_encoding_next()),
							      enc.get_encoding_current())));
}

void TFormTransformer::schedule_eventualities(const Constraints& ev, Constraints& add_realize, Constraints& add_done0, Constraints& add_true0, Constraints& add_true1)
{
	for (Constraints::const_iterator i = ev.begin();i != ev.end();++i)
	{
		const Encoding& ev_enc = encode_eventuality(*i, NULL, add_realize, add_done0);
		add_true0.add(ev_enc.get_encoding_current());
		add_true1.add(ev_enc.get_encoding_next());
	}
}

const TFormTransformer::Encoding& TFormTransformer::encode_eventuality(Node* phi, Name* free_var, Constraints& add_realize, Constraints& add_zero)
{
	// Add eventuality variable y

	const Encoding& ev_enc = encode(ev_encodings, "y", phi, free_var);

#if 1
	Encoding phienc = encode(formula_encodings, "z", phi, free_var);


	// forall1 i z[i] <=> phi[i]
	add_realize.add(phienc.close_formula(new FormulaOpApplication(new OpIff(),
								      phienc.get_encoding_current(),
								      phi)));
			
	// forall1 i (~y'[i] & y[i] => z[i])
	add_realize.add(ev_enc.close_formula(new FormulaOpApplication(new OpImplies(),
								      new FormulaOpApplication(new OpAnd(),
											       new FormulaOpApplication(new OpNot(),
															ev_enc.get_encoding_next()),
											       ev_enc.get_encoding_current()),
								      phienc.get_encoding_current())));
#else
// skip z-bit
// forall1 i (~y'[i] & y[i] => phi[i])
	add_realize.add(ev_enc.close_formula(
				new FormulaOpApplication(
					new OpImplies(),
					new FormulaOpApplication(
						new OpAnd(),
						new FormulaOpApplication(new OpNot(), ev_enc.get_encoding_next()),
						ev_enc.get_encoding_current()),
					phi)));
#endif

	add_zero.add(ev_enc.close_formula(new FormulaOpApplication(new OpNot(),
								   ev_enc.get_encoding_current())));

	return ev_enc;
}
	

void TFormTransformer::visit(OpEventually& op)
{
	NodeList& args = dynamic_cast<NodeList&>(*(getResult()));
	Formula* phi = dynamic_cast<Formula*>(args.at(0));

	// Add eventuality variable y

	Constraints c_realize;
	Constraints c_zero;
	const Encoding& ev_enc = encode_eventuality(phi, free_variables.empty() ? NULL : new Name(*(free_variables.begin())),
						    c_realize,
						    c_zero);


	if (!(current_context().is_quantified()) &
	    (current_context().holds_t0 | current_context().holds_always))
	{
		// Global property

		if (current_context().holds_always)
		{
			global_alw_ev.add(phi);
		}
		else if (current_context().holds_t0)
		{
			global_ev.add(phi);
		}
		else
		{
			assert(false);
		}

		getBuilder()->push(new FormulaTrue());
	}
	else if (current_context().holds_always && current_context().holds_forall)
	{
		constr_loop_alw |= c_realize;
		constr_loop |= c_zero;

		{
			Formula* f = new FormulaOpApplication(new OpIff(),
							      ev_enc.get_encoding_next(),
							      new FormulaOpApplication(new OpNot(), phi));

			
			constr_loop.add(ev_enc.close_formula(f));
		}

		getBuilder()->push(new FormulaTrue());
	}
	else if (current_context().holds_t0)
	{
		constr_t0_alw |= c_realize;
		constr_t1 |= c_zero;
		getBuilder()->push(ev_enc.get_encoding_current());
	}
	else
	{
		constr_t1_alw |= c_realize;
		constr_loop |= c_zero;

		Encoding lenc = encode(local_encodings, "x", phi, free_variables.empty() ? NULL : new Name(*(free_variables.begin())));
		// x <=> phi or x'
		
		add_always(lenc.close_formula(new FormulaOpApplication(new OpIff(), 
								       new FormulaOpApplication(new OpOr(),
												lenc.get_encodes(),
												lenc.get_encoding_next()),
								       lenc.get_encoding_current())));
		// Reset: y' <=> x' and ~phi
		{
			Formula* f = new FormulaOpApplication(new OpIff(),
							      ev_enc.get_encoding_next(),
							      new FormulaOpApplication(new OpAnd(),
										       lenc.get_encoding_next(),
										       new FormulaOpApplication(new OpNot(), phi)));

			constr_loop.add(lenc.close_formula(f));
		}

		getBuilder()->push(lenc.get_encoding_current());
	}
}

void TFormTransformer::visit(FormulaOpApplication& f)
{
	push_context(*(f.getOp()));

	f.getArgs()->accept(*this);

	pop_context();

	f.getOp()->accept(*this);
}

void TFormTransformer::visit(Declaration& f)
{
	decls->push_back(&f);
}

void TFormTransformer::visit(FormulaWithDeclarations& f)
{
      NodeVector v = f.getDecls()->getChildren();
      VisitorDefault::visit(v);

      f.getSubformula()->accept(*this);
}

Node* TFormTransformer::operator()(Node* n)
{
	constr_t0 = Constraints();
	constr_t0_alw = Constraints();
	constr_t1 = Constraints();
	constr_t1_alw = Constraints();
	Constraints constr_t2 = Constraints();
	constr_loop = Constraints();
	constr_loop_alw = Constraints();
	global_ev = Constraints();
	global_alw_ev = Constraints();

	decls = new NodeList();

	context = std::stack<Context>();
	context.push(Context());
	
	{
		Formula* f_initial = dynamic_cast<Formula*>(TransformerFormula::operator()(n));
		if (!(*f_initial == FormulaTrue())) constr_t0.add(f_initial);
	}

	constr_t2 = constr_loop;

	if (constr_loop.size() == 0 && global_alw_ev.size() > 0)
	{
		constr_loop.add(global_alw_ev.back());
		global_alw_ev.pop_back();
	}

	if (constr_t1.size() == 0 && global_ev.size() > 0)
	{
		constr_t1.add(global_ev.back());
		global_ev.pop_back();
	}

	{
		Constraints dummy;
		//                                      realize         done0        true0     true1
		schedule_eventualities(global_ev,     constr_t0_alw,   constr_t1,   constr_t0, dummy      );
		schedule_eventualities(global_alw_ev, constr_loop_alw, constr_loop, dummy,     constr_loop);
	}

	if (constr_t1.empty())
	{
		constr_t1 = constr_t0;
		constr_t0 = Constraints();

		constr_t1_alw |= constr_t0_alw;
		constr_t0_alw = Constraints();
	}

	if (constr_loop.empty() && constr_loop_alw.empty())
	{
		constr_t2 = constr_t1;
		constr_loop_alw = constr_t1_alw;

		constr_t1 = Constraints();
		constr_t1_alw = Constraints();
	}

	Formula* f_t0 = constr_t0.conjunction();
	Formula* f_t0_alw = constr_t0_alw.conjunction();
	Formula* f_t1 = constr_t1.conjunction();
	Formula* f_t1_alw = constr_t1_alw.conjunction();
	Formula* f_t2 = constr_t2.conjunction();
	Formula* f_loop = constr_loop.conjunction();
	Formula* f_loop_alw = constr_loop_alw.conjunction();

	if (decls->size() > 0)
	{
		f_t0 = new FormulaWithDeclarations(decls, f_t0);
		f_t0_alw = new FormulaWithDeclarations(decls, f_t0_alw);
		f_t1 = new FormulaWithDeclarations(decls, f_t1);
		f_t1_alw = new FormulaWithDeclarations(decls, f_t1_alw);
		f_t2 = new FormulaWithDeclarations(decls, f_t2);
		f_loop_alw = new FormulaWithDeclarations(decls, f_loop_alw);
		f_loop = new FormulaWithDeclarations(decls, f_loop);
	}

	Constraints res;

	res.add(f_t2);
	res.add(new FormulaOpApplication(new OpAlways(), f_loop_alw));
	res.add(new FormulaOpApplication(new OpAlways(),
					 new FormulaOpApplication(new OpEventually(), f_loop)));

	if (!constr_t1.empty() || !constr_t1_alw.empty())
	{
		Constraints final_t1;
		final_t1.add(f_t1);
		final_t1.add(new FormulaOpApplication(new OpAlways(), f_t1_alw));
		final_t1.add(new FormulaOpApplication(new OpEventually(), res.conjunction()));

		res = final_t1;
	}

	if (!constr_t0.empty() || !constr_t0_alw.empty())
	{
		Constraints final_t0;
		final_t0.add(f_t0);
		final_t0.add(new FormulaOpApplication(new OpAlways(), f_t0_alw));
		final_t0.add(new FormulaOpApplication(new OpEventually(), res.conjunction()));

		res = final_t0;
	}

	return res.conjunction();
}

}
}

	      
	      
	

/*
 * formulasubstitute.cc:
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 * 
 * Authors: Marcus Nilsson (marcusn@it.uu.se), Mayank Saksena
 * (mayanks@it.uu.se)
 */

#include "formulasubstitute.h"
#include <vector>
#include "prettyprint.h"

using namespace std;

namespace rmc
{

  namespace formula
  {


	  BodyStringSubstituter::~BodyStringSubstituter()
	  {}

	  void BodyStringSubstituter::visit(FormulaWithDeclarations& f)
	  {
		  getBuilder()->push(f.getDecls());
		  f.getSubformula()->accept(*this);
		  f.accept(*getDirector());
	  }
	  
	  void BodyStringSubstituter::visit(Name& n)
	  {
		  StringNodeSubstitution::const_iterator i = substs.find(n.get_desc());
		  ((i != substs.end()) ? (*(i->second)) : n).accept(*getDirector());
	  }

    Binder::Binder() {}
    Binder::~Binder() {}

	  Name* Binder::lookup(const std::string& n) const
	  {
		  for (BindTables::const_reverse_iterator i = current_tables.rbegin();i != current_tables.rend();++i)
		  {
			  BindTable::const_iterator j = i->find(n);
			  if (j != i->end())
			  {
				  // Need copy, should really replace all this with count refs (or pointers) in visitors

				  return new Name(j->second);
			  }
		  }

		  return NULL;
	  }

	  Name* Binder::lookup_toplevel(const std::string& n) const
	  {
		  BindTable::const_iterator i = toplevel_table.find(n);
		  return i == toplevel_table.end() ? NULL : new Name(i->second);
	  }

	  Binder::BindTable& Binder::push_table()
	  {
		  current_tables.push_back(BindTable());
		  return current_tables.back();
	  }

	  void Binder::pop_table()
	  {
		  current_tables.pop_back();
	  }
	  

	  void Binder::visit(DeclFunction& f)
	  {
		  f.getName()->accept(*getDirector());

		  BindTable& t = push_table();

		  NodeList* args = f.getArgs();

		  for (NodeList::const_iterator i = args->begin();i != args->end();++i)
		  {
			  Name& name = dynamic_cast<Name&>(**i);

			  t[name.get_desc()] = name;

			  name.accept(*getDirector());
		  }

		  args->accept(*getDirector());

		  f.getSubformula()->accept(*this);

		  pop_table();

		  f.accept(*getDirector());
	  }

	  void Binder::visit(FormulaWithDeclarations& f)
	  {
		  class DeclVisitor : public VisitorDefault
		  {
			  BindTable& t;
		  public:
			  DeclVisitor(BindTable& t):
				  t(t)
				  {}

			  void visit(DeclFunction& d)
				  {
					  t[d.getName()->get_desc()] = *(d.getName());
				  }
		  };

		  BindTable& t = push_table();

		  DeclVisitor dv(t);
		  NodeVector decls = f.getDecls()->getChildren();
		  dv.VisitorDefault::visit(decls); // Visit all declarations

		  // Note that left-hand names of declarations (the ones now in t), are in scope of both declaration bodies and subformula

		  VisitorDefault::visit(*(f.getDecls()));
		  f.getSubformula()->accept(*this);

		  pop_table();
		  
		  f.accept(*getDirector());
	  }


	  void Binder::visit(FormulaQuantification& f)
	  {
		  BindTable& t = push_table();

		  Name v = *(f.getVariable());
		  t[v.get_desc()] = v;

		  TransformerFormula::visit(f);

		  pop_table();
	  }

	  void Binder::visit(Name& n)
	  {
		  Name* new_n = lookup(n.get_desc());

		  if (new_n == NULL)
		  {
			  BindTable::const_iterator i = toplevel_table.find(n.get_desc());
			  if (i != toplevel_table.end())
			  {
				  new_n = new Name(i->second);
			  }
			  else
			  {
				  new_n = new Name(n);
				  toplevel_table[n.get_desc()] = *new_n;
			  }
		  }

		  if (new_n == NULL) assert(false);

		  TransformerFormula::visit(*new_n);
	  }


    LetRemover::LetRemover() {}
    LetRemover::~LetRemover() {}

    void LetRemover::visit(FormulaWithDeclarations& f)
    {
      /*
	collect function declarations
	visit In: substitute declaration instances
	(build new formula -- implicitly done by visiting subformula)
	remove function declarations
      */
      vector<AliasTable::iterator> insertions;
      NodeList *decls = f.getDecls(), *keep = new NodeList();

      for (NodeList::iterator it = decls->begin(); it != decls->end(); ++it) {
	DeclFunction* d = 0;
	if ((d = dynamic_cast<DeclFunction*>(*it))) {
	  Id id(*(d->getName()));
	  Def def(d->getArgs(), d->getSubformula());
	  AliasTable::iterator inspos =
	    aliasTable.insert(aliasTable.end(), pair<Id, Def>(id, def));
	  insertions.push_back(inspos);
	}
	else
	  keep->push_back(*it);
#if 0
	cout << "Debug: LetRemover defining " << *id.getName() << ":\n";
	Visitor* pp = new VisitorPrettyPrinter();
	def.second->accept(*pp);
	cout << "\n";
	cout.flush();
#endif
      }

#if 0
      cout << "Debug: LetRemover visiting In-part:\n";
      Visitor* pp = new VisitorPrettyPrinter();
      f.getSubformula()->accept(*pp);
      cout << "\n";
      cout.flush();
#endif

      f.getSubformula()->accept(*this);
      if (!keep->empty()) { // kept some declarations
	BuilderFormula* builder = this->getBuilder();
	Node* newformula = builder->top(); builder->pop();
	builder->push(keep); builder->push(newformula);
	this->getDirector()->visit(f);
      }

      for (vector<AliasTable::iterator>::iterator insIt = insertions.begin();
	   insIt != insertions.end(); ++insIt)
	aliasTable.erase(*insIt);
    }

    void LetRemover::visit(FormulaFunctionApp& f)
    {
      /*
	find alias (last occurence) and substitute formal->actual parameters
	visit the new formula
      */

      Id id(*(f.getName()));
      AliasTable::reverse_iterator pos = aliasTable.rend();
      for (AliasTable::reverse_iterator it = aliasTable.rbegin(); it != aliasTable.rend(); it++) {
	if (it->first == id) {
	  pos = it;
	  break;
	}
      }

      if (pos == aliasTable.rend()) {
	Transformer::visit(f);
	return;
      }

      Def def = pos->second; // hit
      if(def.first->size() != f.getArgs()->size())
      {
	      cout << "Number of args mismatch for " << id << " def: " << def.first->size() << " app: " << f.getArgs()->size() << endl;
	      assert(false);
      }

      // collect substitutions of formal to actual parameters
      NodeList* formal = def.first;
      NodeList* actual = f.getArgs();
      NameNodeSubstitution substs;
      for (NodeList::iterator fIt = formal->begin(), aIt = actual->begin();
	   aIt != actual->end(); fIt++, aIt++) {
	Name* fArg = dynamic_cast<Name*>(*fIt);
	Node* aArg = *aIt;
	substs.insert(pair<Name, Node*>(*fArg, aArg));
#if 0
	cout << "Debug: LetRemover recording the mapping " << *fArg->getName()
	     << " to\n";
	Visitor* pp = new VisitorPrettyPrinter();
	aArg->accept(*pp);
	cout << "\n";
	cout.flush();
#endif
      }

      TransformerFormula* substituter = new NameNodeSubstituter(substs);
#if 0
	  cout << "Debug: substituting parameters in\n";
	  Visitor* pp = new VisitorPrettyPrinter();
	  def.second->accept(*pp);
	cout << "\n";
	cout.flush();
#endif
	
	Node* newFormula = (*substituter)(def.second);
#if 0
	cout << "Debug: substituter returned ";
	newFormula->accept(*pp);
	cout << "\n";
	cout.flush();
#endif

	// visit new formula
	newFormula->accept(*this);
	// getBuilder()->push(newFormula);
	//newFormula->accept(*static_cast<Transformer*>(this));
    }

    NameNodeSubstituter::NameNodeSubstituter(NameNodeSubstitution s) : substs(s)
    {}
    NameNodeSubstituter::~NameNodeSubstituter() {}
    void NameNodeSubstituter::visit(Name& n)
    {
      NameNodeSubstitution::iterator pos = substs.find(n);
      Node* newNode = &n;
      if (pos != substs.end())
	newNode = pos->second;
      newNode->accept(*(getDirector()));
    }

#if 0
    NameSubstituter::NameSubstituter(NameSubstitution s) : substs(s)
    {}
    NameSubstituter::~NameSubstituter() {}
    void NameSubstituter::visit(Name& n)
    {
      NameSubstitution::iterator pos = substs.find(n);
      Name newName = n;
      if (pos != substs.end())
	newName = pos->second;
      getDirector()->visit(newName);
    }
#endif

  }
}

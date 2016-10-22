/*
 * eq-column-sequence.cc: 
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
 *    Marcus Nilsson (marcusn@docs.uu.se)
 */

#include "eq-column-sequence.h"
#include <numeric>
#include <sstream>
#include <iterator>

namespace rmc
{
	namespace engine
	{
		namespace eqcol
		{

typedef EqColumnSequence::EquivalenceStrategy EquivalenceStrategy;
typedef EqColumnSequence::DeadStrategy DeadStrategy;
typedef EqColumnSequence::StateVector StateVector;

StateVector EquivalenceStrategy::concat(const StateVector& v1,
					const StateVector& v2) const
{
	StateVector v = v1;
	v.insert(v.end(), v2.begin(), v2.end());

	return v;
}

bool EquivalenceStrategy::exist_member(const StateVector& v, gautomata::StateSet s) const
{
	StateVector before;
	vector<gautomata::State>::const_iterator i = v.begin();

	while (i != v.end())
	{
		StateVector after;
		{
			StateVector::const_iterator i2 = i;
			i2++;
			after.insert(after.end(), i2, v.end());
		}

		if (!exist_member_with_context(*i, s, before, after)) return false;
		before.push_back(*i);
		++i;
	}

	return true;
}

bool EquivalenceStrategy::exist_member_with_context(State q, gautomata::StateSet s, const StateVector& before, const StateVector& after) const
{
	return s.member(q);
}

StateVector EquivalenceStrategy::initial(State q) const
{
	return StateVector(1, q);
}
	
string EquivalenceStrategy::format(const StateLabels& labels, gautomata::State q) const
{
	assert(labels.find(q) != labels.end());
	return labels.find(q)->second;
}

string EquivalenceStrategy::format_before(const StateLabels& labels, gautomata::State q) const
{
	return "";
}

string EquivalenceStrategy::format_between(const StateLabels& labels, gautomata::State q, gautomata::State r) const
{
	return "";
}

string EquivalenceStrategy::format_after(const StateLabels& labels, gautomata::State q) const
{
	return "";
}

string EquivalenceStrategy::format_with_context(const StateLabels& labels, 
						gautomata::State q, 
						const StateVector& before, 
						const StateVector& after) const
{
	return format(labels, q);
}


EqColumnSequence::EquivalenceStrategy::~EquivalenceStrategy()
{}

EqColumnSequence::NoEquivalenceStrategy::~NoEquivalenceStrategy()
{}


EqColumnSequence::CompressedEquivalenceStrategy::~CompressedEquivalenceStrategy()
{}

EqColumnSequence::CompressedEquivalenceStrategy::CompressedEquivalenceStrategy(const StateSet& states_compress):
	_states_compress(states_compress)
{}

StateVector EqColumnSequence::CompressedEquivalenceStrategy::concat(const StateVector& v1, const StateVector& v2) const
{
	StateVector new_sequence = v1;
	vector<gautomata::State>::const_iterator v2_from = v2.begin();

	if (v1.back() == v2.front() &&
	    _states_compress.member(v1.back()))
	{
		v2_from++;
	}

	new_sequence.insert(new_sequence.end(),
			    v2_from,
			    v2.end());

	return new_sequence;
}

string EqColumnSequence::CompressedEquivalenceStrategy::format(const StateLabels& labels, gautomata::State q) const
{
	ostringstream s;

	s << labels.find(q)->second;
	if (_states_compress.member(q)) s << "+";

	return s.str();
}

EqColumnSequence::SpineEquivalenceStrategy::SpineEquivalenceStrategy(const StateSet& states_compress,
								     const StateSet& eq_states,
								     const StateSet& spine_states):
	CompressedEquivalenceStrategy(states_compress),
	eq_states(eq_states),
	spine_states(spine_states)
{}

EqColumnSequence::StateVector EqColumnSequence::SpineEquivalenceStrategy::concat(const StateVector& _v1, const StateVector& _v2) const
{
	StateVector v1 = _v1;
	StateVector v2 = _v2;

	StateVector::reverse_iterator spine_v1 = v1.rbegin();
	while (spine_v1 != v1.rend() && !spine_states.member(*spine_v1)) spine_v1++;

	StateVector::iterator spine_v2 = v2.begin();
	while (spine_v2 != v2.end() && !spine_states.member(*spine_v2)) spine_v2++;


	bool v1_has_spine = (spine_v1 != v1.rend());
	bool v2_has_spine = (spine_v2 != v2.end());

	StateVector::reverse_iterator v1_i = spine_v1;
	StateVector::iterator v2_i = spine_v2;

	if (!v1_has_spine && !v2_has_spine) return CompressedEquivalenceStrategy::concat(v1, v2);

	while (v1_i != v1.rbegin())
	{
		v1_i--;
		State representative;

		if (v1_has_spine && v2_has_spine) representative = representative_between(*spine_v1, *spine_v2, *v1_i);
		if (v1_has_spine && !v2_has_spine) representative = representative_after(*spine_v1, *v1_i);
		if (!v1_has_spine && v2_has_spine) representative = representative_before(*spine_v2, *v1_i);

		*v1_i = representative;
	}

	while (v2_i != v2.begin())
	{
		v2_i--;
		State representative;

		if (v1_has_spine && v2_has_spine) representative = representative_between(*spine_v1, *spine_v2, *v2_i);
		if (v1_has_spine && !v2_has_spine) representative = representative_after(*spine_v1, *v2_i);
		if (!v1_has_spine && v2_has_spine) representative = representative_before(*spine_v2, *v2_i);

		*v2_i = representative;
	}

	return CompressedEquivalenceStrategy::concat(v1, v2);
}

bool EqColumnSequence::SpineEquivalenceStrategy::exist_member_with_context(State q, 
									   gautomata::StateSet s,
									   const StateVector& v1,
									   const StateVector& v2) const
{
	if (!eq_states.member(q)) return CompressedEquivalenceStrategy::exist_member_with_context(q, s, v1, v2);

	if (representative_with_context(v1, v2, epsilon) == q) return true;

	for (StateSet::const_iterator i = s.begin();i != s.end();++i)
	{
		if (!eq_states.member(*i)) continue;

		State rep = representative_with_context(v1, v2, *i);

		if (rep == q)
		{
			if (s.member(*i)) return true;
		}
	}

	return false;
};


StateVector EqColumnSequence::SpineEquivalenceStrategy::initial(State q) const
{
	if (eq_states.member(q) || !spine_states.member(q)) return StateVector(1, q);

	// Take care of epsilons

	State epsilon_up_rep = representative_with_context(StateVector(), StateVector(1, q), epsilon);
	State epsilon_down_rep = representative_with_context(StateVector(1, q), StateVector(), epsilon);

	StateVector v;

	if (epsilon_up_rep != epsilon) v.push_back(epsilon_up_rep);
	v.push_back(q);
	if (epsilon_down_rep != epsilon) v.push_back(epsilon_down_rep);

	return v;
}

EqColumnSequence::State EqColumnSequence::SpineEquivalenceStrategy::representative_with_context(const StateVector& v1,
												const StateVector& v2,
												State q) const
{
	StateVector::const_reverse_iterator spine_v1 = v1.rbegin();
	while (spine_v1 != v1.rend() && !spine_states.member(*spine_v1)) spine_v1++;
	
	StateVector::const_iterator spine_v2 = v2.begin();
	while (spine_v2 != v2.end() && !spine_states.member(*spine_v2)) spine_v2++;


	bool v1_has_spine = (spine_v1 != v1.rend());
	bool v2_has_spine = (spine_v2 != v2.end());
	
	State representative;

	if (!v1_has_spine && !v2_has_spine) representative = q;
	if (v1_has_spine && v2_has_spine) representative = representative_between(*spine_v1, *spine_v2, q);
	if (v1_has_spine && !v2_has_spine) representative = representative_after(*spine_v1, q);
	if (!v1_has_spine && v2_has_spine) representative = representative_before(*spine_v2, q);

	return representative;
}
	
	
string EqColumnSequence::SpineEquivalenceStrategy::format_with_context(const StateLabels& labels, 
								       gautomata::State q, 
								       const StateVector& before, 
								       const StateVector& after) const
{
	ostringstream s;

	if (!eq_states.member(q))
	{
		s << CompressedEquivalenceStrategy::format_with_context(labels, q, before, after);
	}
	else
	{
		vector<string> elements;
		bool has_epsilon = representative_with_context(before, after, epsilon) == q;

		for (StateSet::const_iterator i = eq_states.begin();i != eq_states.end();++i)
		{
			if (representative_with_context(before, after, *i) == q) 
			{
				elements.push_back(labels.find(*i)->second + (has_epsilon ? "*" : "+"));
			}
		}

		if (elements.size() > 0)
		{
			string last = elements.back();
			elements.pop_back();

			std::copy(elements.begin(), elements.end(), ostream_iterator<string>(s, ","));
			s << last;
		}
	}

	return s.str();
}

DeadStrategy::~DeadStrategy()
{}

bool DeadStrategy::is_dead(const StateVector& v) const
{
	return false;
}

EqColumnSequence::NoDeadStrategy::~NoDeadStrategy()
{}

EqColumnSequence::ConsequtiveDeadStrategy::~ConsequtiveDeadStrategy()
{}

EqColumnSequence::ConsequtiveDeadStrategy::ConsequtiveDeadStrategy(const vector<gautomata::StateSet>& dead_sets):
	dead_sets(dead_sets)
{}

bool EqColumnSequence::ConsequtiveDeadStrategy::is_dead(const StateVector& v) const
{
	for (vector<gautomata::StateSet>::const_iterator dead_i = dead_sets.begin();dead_i != dead_sets.end();++dead_i)
	{
		const gautomata::StateSet& s = *dead_i;

		gautomata::State last;

		vector<gautomata::State>::const_iterator i = v.begin();
		if (i == v.end()) continue;

		last = *i;
		++i;

		while (i != v.end())
		{
			if (last != *i && s.member(last) && s.member(*i)) return true;
			last = *i;
			++i;
		}
	}

	return false;
}
	
EqColumnSequence::ColumnDeadStrategy::~ColumnDeadStrategy()
{}


void EqColumnSequence::ColumnDeadStrategy::add_column(const StateVector& v)
{
	assert(v.size() == 2); // For now
	dead_columns.insert(v);
}

bool EqColumnSequence::ColumnDeadStrategy::is_dead(const StateVector& v) const
{
	StateVector::const_iterator i = v.begin();

	if (i == v.end()) return false;
	gautomata::State prev = *i;
	++i;
	for(;i != v.end();++i)
	{
		StateVector part;
		part.push_back(prev);
		part.push_back(*i);

		if (dead_columns.find(part) != dead_columns.end()) return true;

		prev = *i;
	}

	return false;
}
	

EqColumnSequence::EqColumnSequence()
{}

/// Constructs a sequence given a vector of states
/**
 * 
 *
 * @param sequence The sequence to use
 * 
 */

EqColumnSequence::EqColumnSequence(const EquivalenceStrategy& eq_st, 
				   const DeadStrategy& dead_st,
				   const vector<gautomata::State>& sequence):
	_sequence(sequence),
	eq_st(&eq_st),
	dead_st(&dead_st)
{}

/// Create a column sequence of the class which a columnof length one belongs to
/**
 *
 * @param q State in sequence
 * 
 * @return A sequence of length one containing \a q
 */

EqColumnSequence::EqColumnSequence(const EquivalenceStrategy& eq_st, 
				   const DeadStrategy& dead_st,
				   gautomata::State q):
	_sequence(eq_st.initial(q)),
	eq_st(&eq_st),
	dead_st(&dead_st)

{}


/// Concatenate two column sequences
/**
 * Concatenation will compress consequtive states in the compressed state set give
 * at creation of column sequences.
 *
 * @param s1 First sequence
 * @param s2 Second sequence
 * 
 * @return Concatenated sequence
 */

EqColumnSequence operator*(const EqColumnSequence& s1,
			   const EqColumnSequence& s2)
{
#if 0
	for (StateVector::const_iterator i = s1._sequence.begin();i != s1._sequence.end();++i) cout << *i;
	cout << " * ";
	for (StateVector::const_iterator i = s2._sequence.begin();i != s2._sequence.end();++i) cout << *i;
	cout << " = ";
	StateVector s = s1.eq_st->concat(s1._sequence, s2._sequence);
	for (StateVector::const_iterator i = s.begin();i != s.end();++i) cout << *i;
	cout << endl;
#endif
	
	return EqColumnSequence(*s1.eq_st, *s2.dead_st, s1.eq_st->concat(s1._sequence, s2._sequence));
}

/// Equivalence
/**
 * 
 *
 * @param s1 First sequence
 * @param s2 Second sequence
 * 
 * @return Whether \a s1 and \a s2 are equal.
 */
bool operator==(const EqColumnSequence& s1,
		const EqColumnSequence& s2)
{
	return s1._sequence == s2._sequence;
}

/// Format column sequence in human readable form
/**
 * 
 * @return Human readable format
 */

string EqColumnSequence::format(const StateLabels& labels) const
{
	ostringstream s;

	s << '"';

	vector<gautomata::State> before;

	vector<gautomata::State>::const_iterator i = _sequence.begin();

	while (i != _sequence.end())
	{
		if (i == _sequence.begin())
		{
			string before = eq_st->format_before(labels, *i);
			
			if (before != "") s << before << "\\n";
		}
		
		gautomata::State last = *i;
		
		++i;
		vector<gautomata::State> after;
		after.insert(after.begin(), i, _sequence.end());

		s << eq_st->format_with_context(labels, last, before, after);


		s << "\\n";
		
		if (i != _sequence.end())
		{
			string between = eq_st->format_between(labels, last, *i);
			if (between != "") s << between << "\\n";
		}
		else
		{
			string after = eq_st->format_after(labels, last);
			if (after != "") s << after << "\\n";
		}
		before.push_back(*i);
	}

	s << '"';

	return s.str();
}	

	
/// Send textual representation of column sequence to stream
/**
 * 
 *
 * @param s Stream to send to
 * @param seq Sequence to send
 * 
 * @return \a s
 */

ostream& operator<<(ostream &s, const EqColumnSequence &seq)
{
	s << "<";
	
	vector<gautomata::State>::const_iterator i = seq._sequence.begin();

	while (i != seq._sequence.end())
	{
		s << *i;
		++i;

		if (i != seq._sequence.end()) s << ",";
	}

	s << ">";

	return s;
}

/// Tests if exist states in column is member of a set
/**
 * 
 *
 * @param s Set to test for
 * 
 * @return Whether there are states in column that are members of \a s+
 */
bool EqColumnSequence::exist_member(gautomata::StateSet s) const
{
	return eq_st->exist_member(_sequence, s);
}


bool EqColumnSequence::is_dead() const
{
	return dead_st->is_dead(_sequence);
}


}
}
}

namespace std
{

/// Hash function for column sequences
/**
 * @param s Sequence to hash
 * 
 * @return A hash value for \a s
 */

size_t hash<rmc::engine::eqcol::EqColumnSequence>::operator()(rmc::engine::eqcol::EqColumnSequence s) const
{
	return accumulate(s._sequence.begin(),
			  s._sequence.end(),
			  0);
}

}


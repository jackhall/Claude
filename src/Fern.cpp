/*
    Claude: a real-to-discrete coding scheme based on spiraling trees
    Copyright (C) 2012  Jack Hall

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    e-mail: jackwhall7@gmail.com
*/

#include "Claude.h"

namespace clau {

	Fern::Fern()
		: root( new Fork(NULL, false) ), upper_bound(0.0),
		  lower_bound(0.0), max_bin(0) {}
	
	Fern::Fern(const num_type upperBound, const num_type lowerBound, 
		   const bin_type numBins)
		: root( new Fork(NULL, true) ), upper_bound(upperBound),
		  lower_bound(lowerBound), max_bin(num_bins-1) {
		
		root->update_boundary(lower_bound, upper_bound);
		root->update_max_bin(max_bin);
	}
	
	Fern::Fern(const Fern& rhs)
		: root( new Fork(rhs.root) ), upper_bound(rhs.upper_bound), 
		  lower_bound(rhs.lower_bound), max_bin(rhs.max_bin) {}
	
	Fern& Fern::operator=(const Fern& rhs) {
		if(this != &rhs) {
			delete root;
			root = new Fork(rhs.root);
			upper_bound = rhs.upper_bound;
			lower_bound = rhs.lower_bound;
			max_bin = rhs.max_bin;
		}	
		return *this;
	}
	
	Fern::~Fern() {
		delete root;
	}

	void Fern::mutate() 
	
	void Fern::crossover(const Fern& other) 

	////////////////// iterator methods ///////////////////
	Fern::iterator&  Fern::iterator::operator=(const Fern::iterator& rhs) {
		if(this != &rhs) current = rhs.current;
		return *this;
	}
	
	Fern::iterator&  Fern::iterator::up() {
		if(current->parent != NULL)
		return *this;
	}
	
	Fern::iterator&  Fern::iterator::left()
	
	Fern::iterator&  Fern::iterator::right()

} //namespace clau



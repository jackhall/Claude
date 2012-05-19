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
	
	Node::iterator Fern::select_random_node() {
		//record a traversal of the whole tree
		std::deque<Node::iterator> 
		
		//pick a random node
		std::random_device rd;
		std::mt19937 gen(rd);
	}
	
	void Fern::mutate() {
		auto locus = select_random_node();
		//finish...
	}
	
	void Fern::crossover(const Fern& other) {
		auto locus1 = select_random_node();
		auto locus2 = other.select_random_node();
		
		
	}

} //namespace clau



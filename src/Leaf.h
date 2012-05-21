#ifndef Leaf_h
#define Leaf_h

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

#include <random>
#include <iostream>
#include "Node.h"

namespace clau {

	class Leaf : public Node {
	/*
		A Leaf uses the Node interface, adding a bin number. Its version
		of update_boundary does nothing, as it has no boundary. 
		Its version of query returns the bin number. 
	*/
	private:
		bin_type max_bin, bin;
		
		Node* up() { return parent; }
		Node* left_child() { return this; }
		Node* right_child() { return this; }
		
	public:
		Leaf() = delete;
		explicit Leaf(Fork* pParent);
		Leaf(const Leaf& rhs, Fork* pParent);
		Leaf& operator=(const Leaf& rhs);
		~Leaf();
		
		bool is_leaf() const { return true; }
		void update_boundary(const num_type lower_bound, const num_type upper_bound) {}
		void update_max_bin(const bin_type nMaxBin);
		bin_type query(const num_type number=0.0) const { return bin; }
		void mutate(rng_type& gen);
		void split(const bool bValue);
		void merge() {}
		void print(std::ostream& out) const { out << "-: " << bin << std::endl; }
		
	}; //class Bin

} //namespace clau

#endif

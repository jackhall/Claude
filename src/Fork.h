#ifndef Fork_h
#define Fork_h

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

#include <cmath>
#include "Node.h"

namespace clau {

	class Fork : public Node {
	/*
		The Fork class uses the Node interface. It also stores its boundary as a 
		num_type and a bool that tells whether the boundary is on the left or 
		right of the current interval.
	*/
	private: 
		Node* left, right;
		bool value;
		num_type boundary;
		
		Node* up();
		Node* left() { return left; }
		Node* right() { return right; }
		
	public:
		Fork();
		Fork(Fork* pParent, const bool bValue);
		Fork(const Fork& rhs, Fork* pParent=NULL);
		Fork& operator=(const Fork& rhs);
		~Fork();
		
		bool is_leaf() const { return false; }
		void update_boundary(const num_type lower_bound, const num_type upper_bound);
		void update_max_bin(const bin_type nMaxBin);
		bin_type query(const num_type number) const;
		void mutate(const num_type random);
		
	}; //class Fork
	
} //namespace clau

#endif


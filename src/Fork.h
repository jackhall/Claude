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

namespace clau {

	class Fork : public Node {
	private: 
		Node* left, right;
		float boundary;
		
		float infer_boundary() const;
		
	public:
		bool value;
		
		Fork();
		Fork(Fork* pParent, const bool child);
		Fork(const bool bValue, const float range, Fork* child0=NULL, Fork* child1=NULL);
		Fork(const Fork& rhs);
		Fork(Fork&& rhs);
		Fork& operator=(const Fork& rhs);
		Fork& operator=(Fork&& rhs);
		~Fork();
		
		void update_boundary(const float lower_bound, const float upper_bound);
		void update_leaves();
		unsigned short query(const float number) const;
		
	}; //class Fork
	
} //namespace clau

#endif


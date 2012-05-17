#ifndef ClaudeNode_h
#define ClaudeNode_h

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

	class Node {
	/*
		The node class describes the behavior that roots, forks, and leaves
		have in common. This is mainly an common interface for communication
		between nodes. All nodes also have a parent; the root node is simply
		sets this pointer to NULL.
	*/
	private:
		Node* parent; //this is only used by iterators
		
	public:
		Node();
		Node(Node* pParent);
		Node(const Node& rhs);
		Node& operator=(const Node& rhs);
		virtual ~Node();
		
		virtual bool is_leaf() const = 0;
		virtual void update_boundary(const num_type lower_bound, const num_type upper_bound) = 0;
		virtual unsigned short query(const num_type number) const = 0;
		
	}; //class Node
	
} //namespace clau

#endif


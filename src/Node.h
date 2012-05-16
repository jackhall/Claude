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
	private:
		Node* parent;
		unsigned short branch_leaves; 
		
	public:
		Node();
		Node(Node* pParent);
		Node(const Node& rhs);
		Node(Node&& rhs);
		Node& operator=(const Node& rhs);
		Node& operator=(Node&& rhs);
		virtual ~Node();
		
		bool is_leaf() const;
		void update_leaves();
		virtual unsigned short query(const float number) const = 0;
		
	}; //class Node
	
} //namespace clau

#endif

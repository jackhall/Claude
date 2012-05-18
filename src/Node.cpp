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

	Node::Node() : parent(NULL) {}
	
	Node::Node(Node* pParent) : parent(pParent) {}
	
	Node::Node(const Node& rhs) : parent(rhs.parent) {}

	Node& Node::operator=(const Node& rhs) {
		if(this != &rhs) parent = rhs.parent;
		return *this;
	}
	
	Node::~Node() {}
	
	Node::iterator&  Node::iterator::left() {
		if( !current->is_leaf() ) {
			temp_fork //Node doesn't know what a Fork is, so how can it access left or right?
			//but making iterator polymorphic would entail changing the iterator's type
			//could replace Node::iterator with Fern::iterator, but that would require violation
			//of encapsulation to mess with the Node::parent pointer, which would in turn require
			//circular declarations
			//should I just make Node, Fork, and Leaf private members of Fern?
		}
		return *this;
	}
	
	Node::iterator&  Node::iterator::right() {
	
	}
	
} //namespace clau


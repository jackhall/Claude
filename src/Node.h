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

#include <random>
#include <iostream>

namespace clau {

	class Node {
	/*
		The node class describes the behavior that roots, forks, and leaves
		have in common. This is mainly an common interface for communication
		between nodes. All nodes also have a parent; the root node is simply
		sets this pointer to NULL. All new nodes must be adjusted to their 
		context after creation, whether it be update_max_bin() for leaves or
		update_boundary() for forks.
	*/
	protected:
		Node* parent; //this is only used by iterators
		
		virtual Node* up() = 0;
		virtual Node* left_child() = 0;
		virtual Node* right_child() = 0;
		
	public:
		Node();
		explicit Node(Node* pParent);
		Node(const Node& rhs);
		Node& operator=(const Node& rhs);
		virtual ~Node();
		
		virtual bool is_leaf() const = 0;
		virtual void update_boundary(const num_type lower_bound, 
					     const num_type upper_bound) = 0;
		virtual void update_max_bin(const bin_type nMaxBin) = 0;
		virtual bin_type query(const num_type number) const = 0;
		virtual void mutate(rng_type& gen) = 0;
		virtual void split(const bool bValue) = 0;
		virtual void merge() = 0;
		virtual void copy(const Node& other) = 0;
		virtual void print(std::ostream& out) const = 0;
		
		friend std::ostream& operator<<(std::ostream& out, const Node& node);
		
		class iterator {
		protected:
			Node* current;
			
		public:
			iterator() : current(nullptr) {}
			iterator(Node* node) : current(node) {}
			iterator(const iterator& rhs) : current(rhs.current) {}
			iterator& operator=(const iterator& rhs) 
				{ if(this != &rhs) current = rhs.current; return *this; }
			~iterator() = default;
			
			Node& operator*() { return *current; }
			Node* operator->() { return current; }
			
			iterator& up() 	  { current = current->up(); 	return *this; }
			iterator& left()  { current = current->left_child();  return *this; }
			iterator& right() { current = current->right_child(); return *this; }
			
			void replace(Node* new_node);
			bool at_root() { return current->parent == nullptr; }
		};
		
	}; //class Node
	
} //namespace clau

#endif


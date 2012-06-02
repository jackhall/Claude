#ifndef Fern_h
#define Fern_h

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
#include <random>
#include <vector>
#include <array>
#include <iostream>

namespace clau {
	
	typedef float num_type;
	typedef unsigned short bin_type;
	typedef unsigned short dim_type;
	typedef std::mt19937 rng_type;
	
	template<dim_type D>
	class Fern {
	public:
		class iterator; //forward declaration as a friend class for Node and Fork
		
		struct Interval { 
			num_type lower;
			num_type upper;
		};
		
	private:	
		struct Node {
		/*
			The node class describes the behavior that roots, forks, and leaves
			have in common. This is mainly a common interface for communication
			between nodes. All nodes also have a parent; the root node is simply
			sets this pointer to NULL. All new nodes must be adjusted to their 
			context after creation, whether it be update_max_bin() for leaves or
			update_boundary() for forks.
		*/
		protected:
			Node* parent; //this is only used by iterators
			bool leaf;
		
			friend class iterator;
		
		public:
			Node();
			explicit Node(Node* pParent);
			Node(const Node& rhs);
			Node& operator=(const Node& rhs);
			virtual ~Node();
			
			virtual void mutate(rng_type& gen) = 0;
			virtual void copy_from(const Node& other) = 0;
			virtual void print(std::ostream& out) const = 0;
		}; //class Node

		struct Fork : public Node {
		/*
			The Fork class uses the Node interface. It also stores its boundary as a 
			num_type and a bool that tells whether the boundary is on the left or 
			right of the current interval.
		*/
		private: 
			Node *left, *right;
			bool value;
			dim_type dimension;
			num_type boundary;
			
			friend class iterator;
			
			void replace_left(Node* child);
			void replace_right(Node* child);
		
		public:
			Fork();
			Fork(Fork* pParent, const bool bValue);
			Fork(const Fork& rhs, Fork* pParent=nullptr);
			Fork& operator=(const Fork& rhs);
			virtual ~Fork();
		
			void update_boundary(const Interval bounds);
			bin_type query(const array<num_type, D> point) const;
			void merge();
			
			virtual void mutate(rng_type& gen);
			virtual void copy_from(const Node& other);
			virtual void print(std::ostream& out) const { out << value << ": " << boundary << std::endl; }
		}; //class Fork
		
		struct Leaf : public Node {
		/*
			A Leaf uses the Node interface, adding a bin number. Its version
			of update_boundary does nothing, as it has no boundary. 
			Its version of query returns the bin number. 
		*/
		private:
			bin_type max_bin, bin;
		
		public:
			Leaf() = delete;
			explicit Leaf(Fork* pParent);
			Leaf(const Leaf& rhs, Fork* pParent);
			Leaf& operator=(const Leaf& rhs);
			virtual ~Leaf();
		
			void update_max_bin(const bin_type nMaxBin);
			bin_type query() const { return bin; }
			void split(const bool bValue, const dim_type dimension=1);
			
			virtual void mutate(rng_type& gen);
			virtual void copy_from(const Node& other);
			virtual void print(std::ostream& out) const { out << "L: " << bin << "/" << max_bin << std::endl; }
		}; //class Leaf

		Fork* root;
		array<Interval, D> root_region;
		bin_type max_bin;
		
		friend iterator;
		
	public:
		Fern();
		Fern(const array<num_type, D> lowerBound, const array<num_type, D> upperBound, 
		     const bin_type numBins);
		Fern(const Fern& rhs);
		Fern& operator=(const Fern& rhs);
		~Fern();
		
		void set_bounds(const array<Interval, D> bounds);
		void set_bins(const bin_type numBins);
		
		array<Interval, D> get_bounds() const { return root_region; }
		Interval get_bounds(const dim_type dimension) const 
			{ return root_region[dimension-1] };
		
		void mutate();
		void crossover(const Fern& other);
		bin_type query(const array<num_type, D> point) { return root->query(point); }
		
		friend std::ostream& operator<<(std::ostream& out, const Fern& fern);

		class iterator {
		private:
			Node* current;
			iterator(Node* root) : current(root) {}
			friend iterator Fern::begin();
			
		public:
			iterator() : current(nullptr) {}
			iterator(const iterator& rhs) = default;
			iterator& operator=(const iterator& rhs) = default;
			~iterator = default;
			
			Node& operator*() { return *current; }
			Node* operator->() { return current; }
			
			void up() { if(current->parent != nullptr) current = fork->parent; }
			
			void left() { 
				if(!current->leaf) { 
					Fork* fork_ptr = static_cast<Fork*>(current);
					current = fork_ptr->left; 
				}
			}
			
			void right() { 
				if(!current->leaf) {
					Fork* fork_ptr = static_cast<Fork*>(current);
					current = fork_ptr->right; 
				}
			}
			
			void next_leaf();
			
			bool is_leaf() const { return current->leaf; }
			bool is_root() const { return current->parent == nullptr; }
			
		}; //class iterator
		
		iterator begin() { return iterator(root); }
		
	}; //class Fern
	
} //namespace clau

#endif


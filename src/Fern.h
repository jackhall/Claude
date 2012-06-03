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
	
	struct Interval { 
		num_type lower;
		num_type upper;
	};
	
	template<dim_type D>
	class Fern {
	public:
		class node_handle; //forward declaration as a friend class for Node and Fork
		
	private:
		struct Division {
			bool bit;
			dim_type dimension;
		};
	
		struct Node {
		/*
			The Node class provides a common interface for Forks and Leaves. 
		*/
		protected:
			Node* parent; //this is only used by node_handles
			bool leaf;
			friend class node_handle;
		
		public:
			Node() = delete;
			Node(Node* pParent, const bool bLeaf) : parent(pParent), leaf(bLeaf) {}
			Node(const Node& rhs) = default;
			Node& operator=(const Node& rhs) = default;
			virtual ~Node() = default;
		}; //class Node
	
		struct Fork : public Node;

		struct Leaf : public Node {
		/*
			A Leaf adds a bin number to the Node interface. Its version of query 
			returns the bin number. 
		*/
		private:
			bin_type bin;
			friend class node_handle;
		
		public:
			Leaf() = delete;
			Leaf(Fork* pParent, const bin_type nBin) : Node(pParent, true), bin(nBin) {}
			Leaf(const Leaf& rhs) = default;
			Leaf& operator=(const Leaf& rhs) = default;
			virtual ~Leaf() = default;
			
			bin_type query() const { return bin; }
		}; //class Leaf
	
		struct Fork : public Node {
		/*
			The Fork class stores its boundary as a num_type and a bool that tells 
			whether the boundary is on the left or right of the current region in
			the dimension specified. Note that the boundary is stored for convenience 
			only; it is not an independent property. 
		*/
		private: 
			Node *left, *right;
			Division value;
			num_type boundary;
			friend class node_handle;
		
		public:
			Fork() = delete;
			Fork(Fork* pParent, const Division cValue, 
			     const bin_type left_bin, const bin_type right_bin);
			Fork(const Fork& rhs);
			Fork& operator=(const Fork& rhs);
			virtual ~Fork();
			
			bin_type query(const array<num_type, D> point) const;
			void update_boundary(const array<Interval, D> bounds);
		}; //class Fork

		Fork* root;
		array<Interval, D> root_region;
		bin_type max_bin;
		rng_type generator;
		
		friend node_handle;
		
	public:
		Fern();
		Fern(const array<num_type, D> lowerBound, const array<num_type, D> upperBound, 
		     const bin_type numBins);
		Fern(const Fern& rhs);
		Fern& operator=(const Fern& rhs);
		~Fern();
		
		void set_bounds(const array<Interval, D> bounds);
		void set_num_bins(const bin_type numBins);
		
		array<Interval, D> get_bounds() const { return root_region; }
		Interval get_bounds(const dim_type dimension) const 
			{ return root_region[dimension-1] };
		bin_type get_num_bins() const { return max_bin+1; }
		
		void mutate();
		void crossover(const Fern& other);
		bin_type query(const array<num_type, D> point) { return root->query(point); }
		
		friend std::ostream& operator<<(std::ostream& out, const Fern& fern);

		class node_handle {
		/*
			node_handle provides safe external access to Nodes. 
		*/
		private:
			Node* current;
			Fern* fern;
			
			node_handle(Node* root) : current(root) {}
			friend node_handle Fern::begin();
			
		public:
			node_handle() : current(nullptr) {}
			node_handle(const node_handle& rhs) = default;
			node_handle& operator=(const node_handle& rhs) = default;
			~node_handle = default;
			
			node_handle& up() { 
				if(current->parent != nullptr) current = fork->parent; 
				return *this;
			}
			
			node_handle& left() { 
				if(!current->leaf) { 
					Fork* fork_ptr = static_cast<Fork*>(current);
					current = fork_ptr->left; 
				}
				return *this;
			}
			
			node_handle& right() { 
				if(!current->leaf) {
					Fork* fork_ptr = static_cast<Fork*>(current);
					current = fork_ptr->right; 
				}
				return *this;
			}
			
			void mutate();
			void splice(const node_handle& other);
			
			bool split_leaf();
			bool set_leaf_bin(const bin_type new_bin);
			
			bool merge_fork();
			bool set_fork_dimension(const dim_type new_dimension);
			bool set_fork_value(const bool new_value);
			
			bool is_leaf() const { return current->leaf; }
			bool is_root() const { return current->parent == nullptr; }
		}; //class node_handle
		
		node_handle begin() { return node_handle(root); }
		
	}; //class Fern
	
} //namespace clau

#endif


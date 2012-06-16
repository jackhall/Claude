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
		
		Interval() : lower(0.0), upper(0.0) {}
		Interval(const num_type fLower, const num_type fUpper) 
			: lower(fLower), upper(fUpper) {}
		bool operator==(const Interval& rhs) const
			{ return (lower==rhs.lower) && (upper==rhs.upper); }
		bool operator!=(const Interval& rhs) const { return !(*this==rhs); }
		
		friend std::ostream& operator<<(std::ostream& out, const Interval& interval);
	};
	
	std::ostream& operator<<(std::ostream& out, const Interval& interval) {
		out << "[ " << interval.lower << " , " << interval.upper << " ]";
		return out;
	}
	
	template<dim_type D>
	struct Region {
		std::array<Interval, D> limits;
			
		Region() { 
			Interval null_interval;
			for(int i=D-1; i>=0; --i) limits[i] = null_interval;
		}
		
		Region(const Region& rhs) = default;
		Region& operator=(const Region& rhs) = default;
		~Region() = default;
		
		bool operator==(const Region& rhs) const { 
			for(int i=D-1; i>=0; --i) if(limits[i] != rhs.limits[i]) return false;
			return true;
		}
		bool operator!=(const Region& rhs) const { return !(*this==rhs); }
		template<dim_type T> 
		friend std::ostream& operator<<(std::ostream& out, const Region<T>& region);
		
		void set_uniform(const Interval interval) 
			{ for(int i=D-1; i>=0; --i) limits[i] = interval; }
		Interval& operator()(const unsigned int dimension) 
			{ return limits[dimension-1]; }
		const Interval& operator()(const unsigned int dimension) const 
			{ return limits[dimension-1]; }
	};
	
	template<dim_type T>
	std::ostream& operator<<(std::ostream& out, const Region<T>& region) {
		for(int i=0; i<T; ++i) out << "[" << region.limits[i] << "]";
		return out;
	}
	
	template<dim_type D>
	struct Point {
		std::array<num_type, D> coordinates;
		
		Point() { for(int i=D-1; i>=0; --i) coordinates = 0.0; }
		Point(const Point& rhs) = default;
		Point& operator=(const Point& rhs) = default;
		~Point() = default;
		
		num_type& operator()(const unsigned int dimension) 
			{ return coordinates[dimension-1]; }
		const num_type& operator()(const unsigned int dimension) const 
			{ return coordinates[dimension-1]; }
	};
	
	template<dim_type D>
	class Fern {
	public:
		class node_handle; //forward declaration as a friend class for Node and Fork
		
		struct Division {
			bool bit;
			dim_type dimension;
			
			Division(const bool bBit, const dim_type dim=1) : bit(bBit), dimension(dim) 
				{ if(dim <= 0 || dim > D) dimension = 1; }
		};
		
	private:
		struct Node {
		/*
			The Node class provides a common interface for Forks and Leaves. 
		*/
			Node* parent;
			const bool leaf;
		
			Node() = delete;
			Node(Node* pParent, const bool bLeaf) : parent(pParent), leaf(bLeaf) {}
			Node(const Node& rhs) = default;
			Node& operator=(const Node& rhs) = default;
			virtual ~Node() = default;
			
			virtual void print(std::ostream& out, unsigned int depth) const {}
		}; //class Node
	
		struct Fork;

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
			Leaf(Fork* pParent, const bin_type nBin) 
				: Node(pParent, true), bin(nBin) {}
			Leaf(const Leaf& rhs) = default;
			Leaf& operator=(const Leaf& rhs) = default;
			virtual ~Leaf() noexcept = default;
			
			virtual void print(std::ostream& out, unsigned int depth) const;
			bin_type query() const { return bin; }
		}; //class Leaf
	
		struct Fork : public Node {
		/*
			The Fork class stores its boundary as a num_type and a bool that tells 
			whether the boundary is on the left or right of the current region in
			the dimension specified. Note that the boundary is stored for 
			convenience only; it is not an independent property. 
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
			virtual ~Fork() noexcept;
			
			virtual void print(std::ostream& out, unsigned int depth) const;
			bin_type query(const Point<D> point) const;
			void update_boundary(const Region<D> bounds);
		}; //class Fork

		Fork* root;
		Region<D> root_region;
		bin_type max_bin;
		rng_type generator;
		
		friend node_handle;
		
	public:
		Fern(const bin_type numBins=1.0);
		Fern(const Region<D> bounds, const bin_type numBins=1.0);
		Fern(const Fern& rhs);
		Fern& operator=(const Fern& rhs);
		~Fern();
		
		void set_bounds(const Region<D> bounds);
		//left out a way to change the number of bins, might need to add it back later
		
		Region<D> get_bounds() const { return root_region; }
		Interval get_bounds(const dim_type dimension) const 
			{ if(dimension <= D && dimension > 0) return root_region[dimension-1];
			  else return Interval(); }
		bin_type get_num_bins() const { return max_bin+1; }
		
		void mutate();
		void crossover(const Fern& other);
		bin_type query(const Point<D> point) { return root->query(point); }
		
		template<dim_type T>
		friend std::ostream& operator<<(std::ostream& out, const Fern<T>& fern);

		class node_handle {
		/*
			node_handle provides safe external access to Nodes. 
		*/
		private:
			Node* current;
			Fern* fern;
			
			node_handle(Node* root) : current(root) {}
			friend node_handle Fern::begin() const;
			
		public:
			node_handle() : current(nullptr) {}
			node_handle(const node_handle& rhs) = default;
			node_handle& operator=(const node_handle& rhs) = default;
			~node_handle() = default;
			
			bool operator==(const node_handle& rhs) { return current == rhs.current; }
			bool operator!=(const node_handle& rhs) { return current != rhs.current; }
			
			node_handle& up() { 
				if(current->parent != nullptr) current = current->parent; 
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
			
			node_handle& random_node();
			node_handle& root() { while( !is_root() ) up(); return *this; }
			
			void mutate_value();
			bool mutate_structure();
			bool splice(const node_handle& other);
			
			bool split_leaf(const Division new_value);
			bin_type get_leaf_bin() const;
			bool set_leaf_bin(const bin_type new_bin);
			
			bool merge_fork();
			dim_type get_fork_dimension() const;
			bool get_fork_bit() const;
			bool set_fork_dimension(const dim_type new_dimension);
			bool set_fork_bit(const bool new_bit);
			
			bool is_leaf() const { return current->leaf; }
			bool is_root() const { return current->parent == nullptr; }
			bool is_ghost() const;
			bool belongs_to(const Fern& owner) { return fern==&owner; }
		}; //class node_handle
		
		node_handle begin() const { return node_handle(root); }
		bool random_analagous(node_handle one, node_handle two); //needs access to rng generator
	}; //class Fern
	
} //namespace clau

#include "Fern.cpp"

#endif


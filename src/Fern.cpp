#ifndef Fern_cpp
#define Fern_cpp

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

namespace clau {

	//=================== Fern methods ======================
	Fern::Fern()
		: root( new Fork(NULL, false) ), upper_bound(0.0),
		  lower_bound(0.0), max_bin(0) {}
	
	Fern::Fern(const num_type lowerBound, const num_type upperBound, 
		   const bin_type numBins)
		: root( new Fork(NULL, false) ), upper_bound(upperBound),
		  lower_bound(lowerBound), max_bin(numBins-1) {
		
		root->update_boundary(lower_bound, upper_bound);
		root->update_max_bin(max_bin);
	}
	
	Fern::Fern(const Fern& rhs)
		: root( new Fork(*rhs.root) ), upper_bound(rhs.upper_bound), 
		  lower_bound(rhs.lower_bound), max_bin(rhs.max_bin) {}
	
	Fern& Fern::operator=(const Fern& rhs) {
		if(this != &rhs) {
			delete root;
			root = new Fork(*rhs.root);
			upper_bound = rhs.upper_bound;
			lower_bound = rhs.lower_bound;
			max_bin = rhs.max_bin;
		}
		return *this;
	}
	
	Fern::~Fern() {
		delete root;
	}
	
	void Fern::set_bounds(const num_type lowerBound, const num_type upperBound) {
		lower_bound = lowerBound;
		upper_bound = upperBound;
		root->update_boundary(lower_bound, upper_bound);
	}
	
	void Fern::set_num_bins(const bin_type numBins) { //rewrite using node_handles
		max_bin = numBins-1;
		root->update_max_bin(max_bin);
	}
	
	void Fern::mutate() { //rewrite using node_handles
		Node::iterator itn(root); //new node
		itn.left(); //excluding the root node from the search
		Node::iterator locus(itn); //current choice
		
		std::vector<bool> branch_stack; //stack recording traversal choices: left=false, right=true
		branch_stack.push_back(false); //starting from left child of root
		
		unsigned int n=1; //number of nodes traversed so far
		
		std::random_device rd;
		rng_type gen(rd());
		
		bool stop = false;
		while(!stop) {
			//test current node
			num_type chance = 1/n;
			if( chance >= std::generate_canonical<num_type,16>(gen) ) locus = itn;
			++n;
			
			//find new node
			if(itn->is_leaf()) {
				itn.up();
			
				while(branch_stack.back()) { //until we reach an unexplored right branch
					itn.up();
					branch_stack.pop_back();
					if(branch_stack.size() == 0) { //no more unexplored branches
						stop = true;
						break;
					}
				}
				
				if(!stop) { 
					itn.right();
					branch_stack.back() = true;
				}
			} else {
				itn.left();
				branch_stack.push_back(false);
			}
		}
		
		locus->mutate(gen); //this may invalidate locus iterator, so don't touch it after this
		root->update_max_bin(max_bin); //necessary for all merging and splitting mutations
		root->update_boundary(lower_bound, upper_bound); //this will do nothing if mutation was a merge
	}
	
	void Fern::crossover(const Fern& other) { //rewrite using node_handles
		Node::iterator itn1(root); //new node, this tree
		itn1.left(); //excluding the root node from the search
		Node::iterator itn2(other.root); //new node, other tree (need const_iterator?)
		itn2.left();
		
		Node::iterator locus1(itn1); //current choices
		Node::iterator locus2(itn2); 
		
		std::vector<bool> branch_stack; //stack recording traversal choices: left=false, right=true
		branch_stack.push_back(false); //starting from left child of root
		
		unsigned int n=1; //number of nodes traversed so far
		
		std::random_device rd;
		std::mt19937 gen(rd());
		
		bool stop = false;
		while(!stop) {
			if(itn1->is_leaf() || itn2->is_leaf()) { 
				itn1.up();
				itn2.up();
			
				while(branch_stack.back()) { //until we reach an unexplored right branch
					itn1.up();
					itn2.up();
					branch_stack.pop_back();
					if(branch_stack.size() == 0) { //no more unexplored branches
						stop = true;
						break;
					}
				}
				
				if(!stop) { 
					itn1.right();
					itn2.right();
					branch_stack.back() = true;
				}
			} else {
				//test current node
				num_type chance = 1/n;
				if( chance >= std::generate_canonical<num_type,16>(gen) ) {
					locus1 = itn1;
					locus2 = itn2;
				}
				++n;
				
				//next left
				itn1.left();
				itn2.left();
				branch_stack.push_back(false);
			}
		}
		
		//splice branch
		locus1->copy(*locus2);
		root->update_boundary(lower_bound, upper_bound);
		root->update_max_bin(max_bin);
	}
	
	std::ostream& operator<<(std::ostream& out, const Fern& fern) {
		std::vector<bool> branch_stack;
		Node::iterator locus(fern.root);
		
		out << "Interval: [" << fern.lower_bound << ", " << fern.upper_bound << "]" << std::endl;
		out << "Number of bins: " << fern.max_bin+1 << std::endl;
		
		bool stop = false;
		while(!stop) {
			for(int i = branch_stack.size(); i>0; --i) out << "=";
			out << *locus;
			
			//find new node
			if(locus->is_leaf()) {
				locus.up();
			
				while(branch_stack.back()) { //until we reach an unexplored right branch
					locus.up();
					branch_stack.pop_back();
					if(branch_stack.size() == 0) { //no more unexplored branches
						stop = true;
						break;
					}
				}
				
				if(!stop) { 
					locus.right();
					branch_stack.back() = true;
				}
			} else {
				locus.left();
				branch_stack.push_back(false);
			}
		}
		
		return out;
	}
	
	/*
	bool Fern::test_splitting() {
		Node::iterator locus(root);
		Node::iterator root_locus(locus);
		locus.left();
		locus->split(true);
		
		locus = root_locus;
		locus.left().right();
		locus->split(false);
		
		root_locus->update_boundary(lower_bound, upper_bound);
		root_locus->update_max_bin(max_bin);
		return true;
	}
	
	bool Fern::test_copying(const Fern& source) {
		Node::iterator locus1(root), locus2(source.root);
		locus1.left();
		locus2.left();
		locus1->copy(*locus2);	
		return true;
	}
	
	bool Fern::test_merging() {
		Node::iterator locus(root);
		locus.left();
		locus->merge();
		
		root->update_max_bin(max_bin);
		return true;
	} 
	*/
	
	//==================== Fern::Fork methods ===================
	template<dim_type D>
	Fern<D>::Fork::Fork(Fork* pParent, const Division cValue, 
			    const bin_type left_bin, const bin_type right_bin) 
		: Node(pParent, false), value(cValue) {
		//does not set boundary! This should be done by node_handle from the root node
		left  = new Leaf(this, left_bin);
		right = new Leaf(this, right_bin);
	}
	
	template<dim_type D>
	Fern<D>::Fork::Fork(const Fork& rhs, Fork* pParent=nullptr) 
		: Node(pParent, false), value(rhs.value), boundary(rhs.boundary) {
		//copy left subtree
		if( rhs.left->is_leaf() ) left = new Leaf( *static_cast<Leaf*>(rhs.left) );
		else 			  left = new Fork( *static_cast<Fork*>(rhs.left) );
		left->parent = this;
		
		//copy right subtree
		if( rhs.right->is_leaf() ) right = new Leaf( *static_cast<Leaf*>(rhs.right) );
		else 			   right = new Fork( *static_cast<Fork*>(rhs.right) );
		right->parent = this;
	}
	
	template<dim_type D>
	Fern<D>::Fork&  Fern<D>::Fork::operator=(const Fork& rhs) {
		//keeps its place in its original tree (does not copy parent pointer)
		value = rhs.value;
		boundary = rhs.boundary; //assumes identical context, may need to update after copy
		
		//replace left subtree
		delete left;
		left = nullptr;
	
		if( rhs.left->is_leaf() ) left = new Leaf( *static_cast<Leaf*>(rhs.left) );
		else 			  left = new Fork( *static_cast<Fork*>(rhs.left) );
		left->parent = this;
		
		//replace right subtree
		delete right;
		right = nullptr;
		
		if( rhs.right->is_leaf() ) right = new Leaf( *static_cast<Leaf*>(rhs.right) );
		else 			   right = new Fork( *static_cast<Fork*>(rhs.right) );
		right->parent = this;
		
		return *this;
	}
	
	template<dim_type D>
	Fern<D>::Fork::~Fork() {
		delete left;
		left = nullptr;

		delete right;
		right = nullptr;
	}
	
	template<dim_type D>
	bin_type Fern<D>::Fork::query(const array<num_type, D> point) const {
		if(point[value.dimension-1] < boundary) {
			if(left->leaf) return static_cast<Leaf*>(left)->query();
			else return static_cast<Fork*>(left)->query(point);
		} else {
			if(right->leaf) return static_cast<Leaf*>(right)->query();
			else return static_cast<Fork*>(right)->query(point);
		}
	}
	
	template<dim_type D>
	void Fern<D>::Fork::update_boundary(const std::array<Interval, D> bounds) {
		num_type ratio = 2.0/(1.0 + sqrt(5));
		Interval interval = bounds[value.dimension-1];
		
		if(value.bit) boundary = interval.lower + ratio*(interval.upper - interval.lower);
		else boundary = interval.lower + (1-ratio)*(interval.upper - interval.lower);
		
		if(!left->leaf) {
			std::array<Interval, D> left_bounds = bounds;
			left_bounds[value.dimension-1].upper = boundary;
			static_cast<Fork*>(left)->update_boundary(left_bounds);
		}
		
		if(!right->leaf) {
			std::array<Interval, D> right_bounds = bounds;
			right_bounds[value.dimension-1].lower = boundary;
			static_cast<Fork*>(right)->update_boundary(right_bounds);
		}
	}

	//==================== Fern::node_handle methods ============
	template<dim_type D>
	void Fern<D>::node_handle::mutate() {}
	
	template<dim_type D>
	void Fern<D>::node_handle::splice(const node_handle& other) {}
	
	template<dim_type D>
	bool Fern<D>::node_handle::split_leaf(const Division new_value) {
		//returns false for forks or if leaf is a ghost
		if( current->leaf ) {
			
			Leaf* leaf_ptr = static_cast<Leaf*>(current);
			bin_type kept_bin = leaf_ptr->bin; //same for both new leaves
			up();
			Fork* parent_ptr = static_cast<Fork*>(current);
			if( parent_ptr->left == leaf_ptr ) {
			
				delete leaf_ptr;
				parent_ptr->left = new Fork(parent_ptr, new_value, 
							    kept_bin, kept_bin);
				left();
				return true;
			
			} else if ( parent_ptr->right == leaf_ptr ) {
			
				delete leaf_ptr;
				parent_ptr->right = new Fork(parent_ptr, new_value, 
							     kept_bin, kept_bin);
				right();
				return true;
			
			} else {
				current = leaf_ptr;
				return false; //leaf is a ghost
			}
			
		} else return false; //current points to a fork
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::set_leaf_bin(const bin_type new_bin) {
		//returns false for forks or if new_bin is out-of-range
		if( current->leaf && (new_bin <= fern->max_bin) ) {
		
			static_cast<Leaf*>(current)->bin = new_bin;
			return true;
			
		} else return false;
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::merge_fork() {
		//returns false for leaves, if both children are not leaves,
		// or if fork is a ghost or root
		if( !current->leaf && !is_root() ) {
			Fork* fork_ptr = static_cast<Fork*>(current);
			if(fork_ptr->left->leaf && fork_ptr->right->leaf) {
			
				//either keep bin of larger interval or left leaf 
				bin_type kept_bin = static_cast<Leaf*>(fork_ptr->left)->bin;
				up();
				Fork* parent_ptr = static_cast<Fork*>(current); 
				if( parent_ptr->left == fork_ptr ) {
				
					delete fork_ptr;
					parent_ptr->left = new Leaf(parent_ptr, kept_bin);
					left();
					return true;
					
				} else if( parent_ptr->right == fork_ptr) {
				
					delete fork_ptr;
					parent_ptr->right = new Leaf(parent_ptr, kept_bin);
					right();
					return true;
					
				} else {
					current = fork_ptr; //return to original node
					return false; //fork is a ghost
				}
				
			} else return false; //both children are not leaves
		} else return false; //current points to a leaf or root
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::set_fork_dimension(const dim_type new_dimension) {
		//returns false for leaves or if new_dimension is out-of-range
		if( !current->leaf && (new_dimension <= D) && (new_dimension > 0) ) {
		
			static_cast<Fork*>(current)->value.dimension = new_dimension;
			fern->root->update_boundary(root_region);
			return true;
			
		} else return false;
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::set_fork_bit(const bool new_bit) {
		//returns false for leaves
		if( !current->leaf ) {
		
			static_cast<Fork*>(current)->value.bit = new_bit;
			fern->root->update_boundary(root_region);
			return true;
			
		} else return false;
	}
	
} //namespace clau

#endif


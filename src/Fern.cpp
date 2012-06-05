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
	template<dim_type D>
	Fern<D>::Fern() : max_bin(0) {
		
		Interval null_interval = {0.0, 0.0};
		root_region.fill(null_interval);
		
		Division root_division = {false, 1};
		root = new Fork(nullptr, root_division, 0, 0);
		
		std::random_device device;
		generator.seed( device() );
	}
	
	template<dim_type D>
	Fern<D>::Fern(const std::array<Interval, D> bounds, const bin_type num_bins) 
		: root_region(bounds), max_bin(num_bins-1) {
		
		Division root_division = {false, 1};
		root = new Fork(nullptr, root_division, 0, 0);
		
		root->update_boundary(root_region);
		
		std::random_device device;
		generator.seed( device() );
	}
	
	template<dim_type D>
	Fern<D>::Fern(const Fern<D>& rhs) 
		: root( new Fork(*rhs.root) ), root_region(rhs.root_region), 
		  max_bin(rhs.max_bin) {
		  
		std::random_device device;
		generator.seed( device() );
	}
	
	template<dim_type D>
	Fern<D>& Fern<D>::operator=(const Fern<D>& rhs) { 
		if(this != &rhs) {
			root_region = rhs.root_region;
			max_bin = rhs.max_bin;
			delete root;
			root = new Fork(*rhs.root);
		}
		return *this;
	}
	
	template<dim_type D>
	Fern<D>::~Fern() {
		delete root;
	}
	
	template<dim_type D>
	void Fern<D>::set_bounds(const std::array<Interval, D> bounds) { 
		root_region = bounds;
		root->update_boundary(bounds);
	}
	
	template<dim_type D>
	void Fern<D>::mutate() { 
		auto locus = begin();
		locus.random_node();
		std::bernoulli_distribution  mutation_type_chance(0.5);

		if( mutation_type_chance(generator) ) 
			if( !locus.mutate_structure() ) locus.mutate_value();
		else locus.mutate_value();
	}
	
	template<dim_type D>
	void Fern<D>::crossover(const Fern& other) { 
		auto target = begin();
		auto source = other.begin();
		random_analagous(target, source);
		target.splice(source);
	}
	
	/*
	std::ostream& operator<<(std::ostream& out, const Fern& fern) {
		std::vector<bool> branch_stack;
		auto node = fern.begin();
		
		out << "Interval: [" << fern.lower_bound << ", " << fern.upper_bound << "]" << std::endl; //not valid
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
	Fern<D>::Fork::Fork(const Fork& rhs) 
		: Node(rhs.parent, false), value(rhs.value), boundary(rhs.boundary) {
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
	typename Fern<D>::Fork&  Fern<D>::Fork::operator=(const Fork& rhs) {
	
		if( this != &rhs ) {
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
		}
		return *this;
	}
	
	template<dim_type D>
	Fern<D>::Fork::~Fork() noexcept {
		delete left;
		left = nullptr;

		delete right;
		right = nullptr;
	}
	
	template<dim_type D>
	bin_type Fern<D>::Fork::query(const std::array<num_type, D> point) const {
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
	typename Fern<D>::node_handle&  Fern<D>::node_handle::random_node() {
		auto start = current; //save current position
		while( !is_root() ) up(); //go to root
		auto choice = current; 
		
		std::vector<bool> branch_stack; //stack recording traversal choices: left=false, right=true
		unsigned int n=1; //number of nodes traversed so far
		while(true) { 
			//test current node
			if( (1.0/n) >= std::generate_canonical<num_type,16>(fern->generator) ) 
				choice = current;
			++n;
			
			//find new node
			if( is_leaf() ) {
			
				up();
				while(branch_stack.back()) { //until we reach an unexplored right branch
					up();
					branch_stack.pop_back();
					if(branch_stack.size() == 0) { //no more unexplored branches
						current = choice;
						return *this;
					}
				}
				
				right();
				branch_stack.back() = true;
	
			} else {
				left();
				branch_stack.push_back(false);
			}
		}
	}
	
	template<dim_type D>
	void Fern<D>::node_handle::mutate_value() {
		if( is_leaf() ) {
			
			std::uniform_int_distribution<bin_type> random_int(0, fern->max_bin);
			set_leaf_bin( random_int(fern->generator) );
			
		} else {
			
			std::bernoulli_distribution random_bit(0.5);
			if( random_bit(fern->generator) ) {
				std::uniform_int_distribution<dim_type> random_int(1, D);
				set_fork_dimension( random_int(fern->generator) );
			} else set_fork_bit( random_bit(fern->generator) );
		}
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::mutate_structure() {
		//returns false for ghosts, root or forks that don't have two leaf-children
		if( is_leaf() ) {
		
			std::bernoulli_distribution random_bit(0.5);
			std::uniform_int_distribution<dim_type> random_int(1, D);
			Division new_division = { random_bit(fern->generator), 
						  random_int(fern->generator) };
			return split_leaf(new_division); //performs ghost check
			
		} else return merge_fork(); //performs root, ghost and child checks
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::splice(const node_handle& other) {
		//returns false if current points to a ghost or root
		if( !is_root() ) {
			auto target_ptr = current;
			up();
			auto parent_ptr = static_cast<Fork*>(current);
			if( parent_ptr->left == target_ptr ) {
			
				delete target_ptr;
				target_ptr = nullptr;
				if( other.is_leaf() ) 
					parent_ptr->left = new Leaf( *static_cast<Leaf*>(other.current) );
				else 	parent_ptr->left = new Fork( *static_cast<Fork*>(other.current) );
				return true;
				
			} else if( parent_ptr->right == target_ptr ) {
			
				delete target_ptr;
				target_ptr = nullptr;
				if( other.is_leaf() ) 
					parent_ptr->right = new Leaf( *static_cast<Leaf*>(other.current) );
				else 	parent_ptr->right = new Fork( *static_cast<Fork*>(other.current) );
				return true;
			
			} else {
				current = target_ptr;
				return false; //current points to a ghost
			}
		} else return false; //current points to root
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::split_leaf(const Division new_value) {
		//returns false for forks or if leaf is a ghost
		if( current->leaf ) {
			
			auto leaf_ptr = static_cast<Leaf*>(current);
			bin_type kept_bin = leaf_ptr->bin; //same for both new leaves
			up();
			auto parent_ptr = static_cast<Fork*>(current);
			if( parent_ptr->left == leaf_ptr ) {
			
				delete leaf_ptr;
				leaf_ptr = nullptr;
				parent_ptr->left = new Fork(parent_ptr, new_value, 
							    kept_bin, kept_bin);
				left();
				return true;
			
			} else if ( parent_ptr->right == leaf_ptr ) {
			
				delete leaf_ptr;
				leaf_ptr = nullptr;
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
			auto fork_ptr = static_cast<Fork*>(current);
			if(fork_ptr->left->leaf && fork_ptr->right->leaf) {
			
				//either keep bin of larger interval or left leaf 
				bin_type kept_bin = static_cast<Leaf*>(fork_ptr->left)->bin;
				up();
				auto parent_ptr = static_cast<Fork*>(current); 
				if( parent_ptr->left == fork_ptr ) {
				
					delete fork_ptr;
					fork_ptr = nullptr;
					parent_ptr->left = new Leaf(parent_ptr, kept_bin);
					left();
					return true;
					
				} else if( parent_ptr->right == fork_ptr) {
				
					delete fork_ptr;
					fork_ptr = nullptr;
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
	
	template<dim_type D>
	bool Fern<D>::node_handle::is_ghost() const {
		
		if( is_root() ) return false;
		auto target_ptr = current;
		auto parent_ptr = static_cast<Fork*>(current->parent);
		if( (parent_ptr->left == target_ptr) || (parent_ptr->right == target_ptr) ) 
			return true;
		else return false;
	}
	
	template<dim_type D>
	bool Fern<D>::random_analagous(Fern<D>::node_handle one, Fern<D>::node_handle two) {
		//returns false if ferns have different numbers of bins
		auto original_one = one;
		auto original_two = two;
		while( !one.is_root() ) one.up();
		while( !two.is_root() ) two.up();
		if( one == two ) { //if both handles are from the same fern, just pick random node
		
			one.random_node;
			two = one;
			return true;
			
		} else if( one.fern->max_bin != two.fern->max_bin) return false; 
		  else {
			auto choice_one = one;
			auto choice_two = two;
			
			std::vector<bool> branch_stack; //stack recording traversal choices: left=false, right=true
			unsigned int n=1; //number of nodes traversed so far
			while(true) {
				if(one.is_leaf() || two.is_leaf()) { 
				
					one.up(); two.up();
					while(branch_stack.back()) { //until we reach an unexplored right branch
						one.up(); two.up();
						branch_stack.pop_back();
						if(branch_stack.size() == 0) { //no more unexplored branches
							one = choice_one;
							two = choice_two;
							return true;
						}
					}
				
					one.right(); two.right();
					branch_stack.back() = true;
					
				} else {
					//test current node
					if( (1.0/n) >= std::generate_canonical<num_type,16>(generator) ) {
						choice_one = one;
						choice_two = two;
					}
					++n;
				
					//next left
					one.left(); two.left();
					branch_stack.push_back(false);
				}
			}
		}
	}
	
} //namespace clau

#endif


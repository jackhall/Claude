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
	Fern<D>::Fern() : Fern(1) {}
	
	template<dim_type D>
	Fern<D>::Fern(const bin_type numBins) : root_region(), max_bin(numBins-1),
						node_type_chance(0.6),
						mutation_type_chance_leaf(0.25), 
						mutation_type_chance_fork(0.15) {
		
		Division<D> root_division = {false, 1};
		root = new Fork(nullptr, root_division, 0, 0);
		
		std::random_device device;
		generator.seed( device() );
	}
	
	template<dim_type D>
	Fern<D>::Fern(const Region<D> bounds, const bin_type numBins) 
		: root_region(bounds), max_bin(numBins-1), node_type_chance(0.6),
		  mutation_type_chance_leaf(0.25), mutation_type_chance_fork(0.15) {
		
		Division<D> root_division = {false, 1};
		root = new Fork(nullptr, root_division, 0, 0);
		
		root->update_boundary(root_region);
		
		std::random_device device;
		generator.seed( device() );
	}
	
	template<dim_type D>
	Fern<D>::Fern(const Fern<D>& rhs) 
		: root( new Fork(*rhs.root) ), root_region(rhs.root_region), 
		  max_bin(rhs.max_bin), node_type_chance(rhs.node_type_chance),
		  mutation_type_chance_leaf(rhs.mutation_type_chance_leaf),
		  mutation_type_chance_fork(rhs.mutation_type_chance_fork) {
		  
		std::random_device device;
		generator.seed( device() );
	}
	
	template<dim_type D>
	Fern<D>& Fern<D>::operator=(const Fern<D>& rhs) { 
		if(this != &rhs) {
			root_region = rhs.root_region;
			max_bin = rhs.max_bin;
			node_type_chance = rhs.node_type_chance;
			mutation_type_chance_leaf = rhs.mutation_type_chance_leaf;
			mutation_type_chance_fork = rhs.mutation_type_chance_fork;
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
	bool Fern<D>::set_node_type_chance(const float chance) {
		//returns false if chance is out of range
		if(chance>=0.0 && chance<=1.0) {
			node_type_chance = chance;
			return true;
		} else return false;
	}
	
	template<dim_type D>
	bool Fern<D>::set_mutation_type_chance( const float chance_fork, 
						const float chance_leaf) {
		if(chance_fork >= 0.0 && chance_leaf>= 0.0 && 
		   chance_fork <= 1.0 && chance_leaf>= 0.0) {
		   	mutation_type_chance_fork = chance_fork;
		   	mutation_type_chance_leaf = chance_leaf;
		   	return true;
		} else return false;
	}
	
	template<dim_type D>
	void Fern<D>::set_bounds(const Region<D> bounds) { 
		root_region = bounds;
		root->update_boundary(bounds);
	}
	
	template<dim_type D>
	void Fern<D>::randomize(const unsigned int mutations) {
		auto locus = begin();
		for(int i=mutations; i>0; i--) {
			while( !locus.is_leaf() ) locus.random();
			std::bernoulli_distribution mutation_type_gen(mutation_type_chance_leaf);
			if( mutation_type_gen(generator) ) { //if mutation is structural
				if( !locus.mutate_structure() ) locus.mutate_value();
			} else locus.mutate_value();
		}
	}
	
	template<dim_type D>
	void Fern<D>::mutate() { 
		auto locus = begin();
		std::bernoulli_distribution  node_type_gen(node_type_chance);
		bool node_type = node_type_gen(generator);
		if( node_type ) { //if locus is a leaf
			while( !locus.is_leaf() ) locus.random();
		} else { 
			do locus.random(); 
			while( locus.is_leaf() );
		}
		
		std::bernoulli_distribution  mutation_type_gen( node_type ? 
								mutation_type_chance_leaf :
								mutation_type_chance_fork);
		if( mutation_type_gen(generator) ) { //if mutation is structural
			if( !locus.mutate_structure() ) locus.mutate_value();
		} else locus.mutate_value();
	}
	
	template<dim_type D>
	void Fern<D>::crossover(const Fern& other) { 
		auto target = begin(); 
		auto source = const_cast<Fern&>(other).begin(); 
		target.random_analagous(source); 
		target.splice(source);
	}
	
	template<dim_type T>
	std::ostream& operator<<(std::ostream& out, const Fern<T>& fern) {
		
		using namespace std;
		
		out << "Interval:" << endl;
		for(int i=1; i<=T; ++i)
			out << "\t[" << fern.root_region(i).lower << ", " << fern.root_region(i).upper << "]" << endl; 
		out << "Number of bins: " << fern.max_bin+1 << endl;
		out << "Fern structure:" << endl;
		fern.root->print(out, 0);
		
		return out;
	}
	
	//==================== Fern::Fork methods ===================
	template<dim_type D>
	Fern<D>::Fork::Fork(Fork* pParent, const Division<D> cValue, 
			    const bin_type left_bin, const bin_type right_bin) 
		: Node(pParent, false), value(cValue) {
		//does not set boundary! This should be done by node_handle from the root node
		left  = new Leaf(this, left_bin);
		right = new Leaf(this, right_bin);
	}
	
	template<dim_type D>
	Fern<D>::Fork::Fork(Fork* pParent, const Division<D> cValue) 
		: Node(pParent, false), value(cValue), left(nullptr), right(nullptr) {}
	
	template<dim_type D>
	Fern<D>::Fork::Fork(const Fork& rhs) 
		: Node(rhs.parent, false), value(rhs.value), boundary(rhs.boundary) {
		//copy left subtree
		if( rhs.left->leaf ) left = new Leaf( *static_cast<Leaf*>(rhs.left) );
		else 		     left = new Fork( *static_cast<Fork*>(rhs.left) );
		left->parent = this;
		
		//copy right subtree
		if( rhs.right->leaf ) right = new Leaf( *static_cast<Leaf*>(rhs.right) );
		else 		      right = new Fork( *static_cast<Fork*>(rhs.right) );
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
	
			if( rhs.left->leaf ) left = new Leaf( *static_cast<Leaf*>(rhs.left) );
			else 		     left = new Fork( *static_cast<Fork*>(rhs.left) );
			left->parent = this;
		
			//replace right subtree
			delete right;
			right = nullptr;
		
			if( rhs.right->leaf ) right = new Leaf( *static_cast<Leaf*>(rhs.right) );
			else 		      right = new Fork( *static_cast<Fork*>(rhs.right) );
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
	void Fern<D>::Fork::print(std::ostream& out, unsigned int depth) const {
		using namespace std;
		for(int i=depth; i>0; --i) out << "    ";
		out << "{" << value.bit << ", D" << value.dimension << "}" << endl;
		left->print(out, depth+1);
		right->print(out, depth+1);
	}
	
	template<dim_type D>
	bin_type Fern<D>::Fork::query(const Point<D> point) const {
		if(point(value.dimension) < boundary) {
			if(left->leaf) return static_cast<Leaf*>(left)->query();
			else return static_cast<Fork*>(left)->query(point);
		} else {
			if(right->leaf) return static_cast<Leaf*>(right)->query();
			else return static_cast<Fork*>(right)->query(point);
		}
	}
	
	template<dim_type D>
	void Fern<D>::Fork::update_boundary(const Region<D> bounds) {
		num_type ratio = 2.0/(1.0 + sqrt(5));
		Interval interval = bounds(value.dimension);
		
		if(value.bit) boundary = interval.lower + ratio*(interval.upper - interval.lower);
		else boundary = interval.lower + (1-ratio)*(interval.upper - interval.lower);
		
		if( !(left->leaf) ) { 
			Region<D> left_bounds = bounds;
			left_bounds(value.dimension).upper = boundary;
			static_cast<Fork*>(left)->update_boundary(left_bounds);
		}
		
		if( !(right->leaf) ) {
			Region<D> right_bounds = bounds;
			right_bounds(value.dimension).lower = boundary;
			static_cast<Fork*>(right)->update_boundary(right_bounds);
		}
	}

	//==================== Fern::Leaf methods
	template<dim_type D>
	void Fern<D>::Leaf::print(std::ostream& out, unsigned int depth) const {
		using namespace std;
		for(int i=depth; i>0; --i) out << "    ";
		out << "{B" << bin << "}" << endl;
	}

	//==================== Fern::node_handle methods ============
	template<dim_type D>
	typename Fern<D>::node_handle&  Fern<D>::node_handle::random() {
		auto it = fern->sbegin(); //depth-first search iterator
		auto choice = it; 
	
		unsigned int n=1; //number of nodes traversed so far
		while( !it.is_null() ) { 
			if( (1.0/n) >= std::generate_canonical<num_type,16>(fern->generator) ) 
				choice = it;
			++n; ++it;
		}
		*this = choice.get_handle();
		return *this;
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::random_analagous(Fern<D>::node_handle& other) { //random analagous
		//returns false if owning ferns have different numbers of bins
		auto original_one = *this;
		auto original_two = other;
		root(); other.root();
		if( get_max_bin() != other.get_max_bin() ) return false; 
		else {
			auto choice_one = *this;
			auto choice_two = other;
			
			std::vector<bool> branch_stack; //stack recording traversal choices: left=false, right=true
			unsigned int n=1; //number of nodes traversed so far
			while(true) {
				if(is_leaf() || other.is_leaf()) { 
				
					up(); other.up();
					while(branch_stack.back()) { //until we reach an unexplored right branch
						up(); other.up();
						branch_stack.pop_back();
						if(branch_stack.size() == 0) { //no more unexplored branches
							*this = choice_one;
							other = choice_two;
							return true;
						}
					}
				
					right(); other.right();
					branch_stack.back() = true;
					
				} else {
					//test current node
					if( (1.0/n) >= std::generate_canonical<num_type,16>(fern->generator) ) {
						choice_one = *this;
						choice_two = other;
					}
					++n;
				
					//next left
					left(); other.left();
					branch_stack.push_back(false);
				}
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
			Division<D> new_division = { random_bit(fern->generator), 
						     random_int(fern->generator) };
			return split_leaf(new_division); //performs ghost check
			
		} else {
		
			std::uniform_int_distribution<bin_type> random_int(0, fern->max_bin);
			//performs root, ghost and child checks: //update: no child check
			return merge_fork( random_int(fern->generator) ); 
		
		}
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
				left();
				current->parent = parent_ptr;
				fern->update_boundary();
				return true;
				
			} else if( parent_ptr->right == target_ptr ) {
			
				delete target_ptr;
				target_ptr = nullptr;
				if( other.is_leaf() ) 
					parent_ptr->right = new Leaf( *static_cast<Leaf*>(other.current) );
				else 	parent_ptr->right = new Fork( *static_cast<Fork*>(other.current) );
				right();
				current->parent = parent_ptr;
				fern->update_boundary();
				return true;
			
			} else {
				current = target_ptr;
				return false; //current points to a ghost
			}
		} else return false; //current points to root
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::split_leaf(const Division<D> new_value) {
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
				fern->update_boundary();
				return true;
			
			} else if ( parent_ptr->right == leaf_ptr ) {
			
				delete leaf_ptr;
				leaf_ptr = nullptr;
				parent_ptr->right = new Fork(parent_ptr, new_value, 
							     kept_bin, kept_bin);
				right();
				fern->update_boundary();
				return true;
			
			} else {
				current = leaf_ptr;
				return false; //leaf is a ghost
			}
			
		} else return false; //current points to a fork
	}
	
	template<dim_type D>
	bin_type Fern<D>::node_handle::get_leaf_bin() const {
		if( !is_leaf() ) return 0;
		else return static_cast<Leaf*>(current)->bin;
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
	bool Fern<D>::node_handle::merge_fork(const bin_type kept_bin) {
		//returns false for leaves, if both children are not leaves,
		// or if fork is a ghost or root
		if( !current->leaf && !is_root() ) {
			auto fork_ptr = static_cast<Fork*>(current);
			//if(fork_ptr->left->leaf && fork_ptr->right->leaf) {
			
			//either keep bin of _larger interval_ or left leaf 
			//bin_type kept_bin;
			//if(fork_ptr->value.bit) kept_bin = static_cast<Leaf*>(fork_ptr->left)->bin;
			//else kept_bin = static_cast<Leaf*>(fork_ptr->right)->bin;
			
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
				
			//} else return false; //both children are not leaves
		} else return false; //current points to a leaf or root
	}
	
	template<dim_type D>
	num_type Fern<D>::node_handle::get_fork_boundary() const {
		if( is_leaf() ) return 0.0;
		else return static_cast<Fork*>(current)->boundary;
	}
	
	template<dim_type D>
	dim_type Fern<D>::node_handle::get_fork_dimension() const {
		if( is_leaf() ) return 0;
		else return static_cast<Fork*>(current)->value.dimension;
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::get_fork_bit() const {
		if( is_leaf() ) return false;
		else return static_cast<Fork*>(current)->value.bit;
	}
	
	template<dim_type D>
	Division<D> Fern<D>::node_handle::get_fork_division() const {
		if( is_leaf() ) return false;
		else return static_cast<Fork*>(current)->value;
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::set_fork_dimension(const dim_type new_dimension) {
		//returns false for leaves or if new_dimension is out-of-range
		if( !is_leaf() && (new_dimension <= D) && (new_dimension > 0) ) {
		
			static_cast<Fork*>(current)->value.dimension = new_dimension;
			fern->update_boundary(); 
			return true;
			
		} else return false;
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::set_fork_bit(const bool new_bit) {
		//returns false for leaves
		if( !is_leaf() ) {
		
			static_cast<Fork*>(current)->value.bit = new_bit;
			fern->update_boundary(); //doesn't run properly
			return true;
			
		} else return false;
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::set_fork_division(const Division<D> division) {
		if( !is_leaf() ) {
		
			static_cast<Fork*>(current)->value = division;
			fern->update_boundary(); //doesn't run properly
			return true;
			
		} else return false;
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::is_ghost() const {
	
		if( is_root() ) return false;
		auto target_ptr = current;
		auto parent_ptr = static_cast<Fork*>(current->parent);
		if( (parent_ptr->left == target_ptr) || (parent_ptr->right == target_ptr) ) 
			return false;
		else return true;
	}
	
	template<dim_type D>
	bool Fern<D>::node_handle::belongs_to(const Fern& owner) { 
		if( fern==nullptr ) return false;
		return fern==&owner; 
	}
	
	//=================== Fern::dfs_iterator methods ==================
	template<dim_type D>
	typename Fern<D>::dfs_iterator&  Fern<D>::dfs_iterator::operator++() {
		if( node.is_leaf() ) {
		
			node.up();
			while(branch_stack.back()) { //until we reach an unexplored right branch
				node.up();
				branch_stack.pop_back();
				if(branch_stack.size() == 0) { //no more unexplored branches
					node = node_handle();
					return *this;
				}
			}
			
			node.right();
			branch_stack.back() = true;

		} else {
			node.left();
			branch_stack.push_back(false);
		}
		
		return *this;
	}
	
	template<dim_type D>
	typename Fern<D>::dfs_iterator  Fern<D>::dfs_iterator::operator++(int) {
		auto temp = *this;
		operator++();
		return temp;
	}

} //namespace clau

#endif


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

//need to take another look at Leaf copying and creation

	Fork::Fork() 
		: parent(NULL), left(NULL), right(NULL), 
		  boundary(0.0), value(false) {}
	
	Fork::Fork(Fork* pParent, const bool bValue) 
		: Node(pParent), left(new Leaf()), right(new Leaf()), 
		  value(bValue), boundary(0.0) {}
	
	Fork::Fork(const Fork& rhs, Fork* pParent)
		: Node(pParent), left(NULL), right(NULL), 
		  boundary(rhs.boundary), value(rhs.value) {
		//The parent should be specified because branches are copied to be
		//analagous, not duplicates. Keeping the same parent would create
		//a "ghost" tree alongside the original one, instead of a new copy
		//in a new place. 
		if(rhs.left != NULL) {
			if( rhs.left.is_leaf() ) left = new Leaf(*rhs.left, this);
			else left = new Fork(*rhs.left, this);
		}
		
		if(rhs.right != NULL) {
			if( rhs.right.is_leaf() ) right = new Leaf(*rhs.right, this);
			else right = new Fork(*rhs.right, this);
		}
	}
	
	Fork& Fork::operator=(const Fork& rhs) {
		//keeps same parent, but deletes child branches and copies new ones from rhs
		if(this != &rhs) {
			if(left != NULL) { 
				delete left;
				left = NULL;
			}
			
			if(right != NULL) {
				delete right;
				right = NULL;
			}
			
			value = rhs.value;
			boundary = rhs.boundary;
			
			if(rhs.left != NULL) {
				if( rhs.left.is_leaf() ) left = new Leaf(*rhs.left, this);
				else left = new Fork(*rhs.left, this);
			}
			
			if(rhs.right != NULL) {
				if( rhs.right.is_leaf() ) right = new Leaf(*rhs.right, this);
				else right = new Fork(*rhs.right, this);
			}
		}
		
		return *this;
	}
	
	Fork::~Fork() {
		if(left!=NULL) {
			delete left;
			left = NULL;
		}
		
		if(right!=NULL) {
			delete right;
			right = NULL;
		}
	}
	
	void Fork::update_boundary(const num_type lower_bound, const num_type upper_bound) {
		//update_boundary calls its child nodes recursively
		num_type ratio = 2.0/(1.0 + sqrt(5));
		if(value) boundary = lower_bound + ratio*(upper_bound - lower_bound);
		else boundary = lower_bound + (1-ratio)*(upper_bound - lower_bound);
		
		if(child_zero!=NULL && child_one!=NULL) {
			left->update_boundary(lower_bound, boundary);
			right->update_boundary(boundary, upper_bound);
		}
	}
	
	void Fork::update_max_bin(const bin_type nMaxBin) {
		left->update_max_bin(nMaxBin);
		right->update_max_bin(nMaxBin);
	}
	
	bin_type Fork::query(const num_type number) const {
		//calls the proper child node recursively
		if(number < boundary) return left->query(number);
		else return right->query(number);
	}

	Node* Fork::up() {
		if(parent != NULL) return parent;
		else return this;
	}

	void Fork::mutate(const num_type random) {
		//flip value/convert to leaf
	}
	
} //namespace clau


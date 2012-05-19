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

	Fern::Fern()
		: root( new Fork(NULL, false) ), upper_bound(0.0),
		  lower_bound(0.0), max_bin(0) {}
	
	Fern::Fern(const num_type upperBound, const num_type lowerBound, 
		   const bin_type numBins)
		: root( new Fork(NULL, true) ), upper_bound(upperBound),
		  lower_bound(lowerBound), max_bin(num_bins-1) {
		
		root->update_boundary(lower_bound, upper_bound);
		root->update_max_bin(max_bin);
	}
	
	Fern::Fern(const Fern& rhs)
		: root( new Fork(rhs.root) ), upper_bound(rhs.upper_bound), 
		  lower_bound(rhs.lower_bound), max_bin(rhs.max_bin) {}
	
	Fern& Fern::operator=(const Fern& rhs) {
		if(this != &rhs) {
			delete root;
			root = new Fork(rhs.root);
			upper_bound = rhs.upper_bound;
			lower_bound = rhs.lower_bound;
			max_bin = rhs.max_bin;
		}
		return *this;
	}
	
	Fern::~Fern() {
		delete root;
	}
	
	void Fern::mutate() {
		Node::iterator itn(root); //new node
		itn.left(); //excluding the root node from the search
		Node::iterator locus(itn); //current choice
		
		vector<bool> branch_stack(); //stack recording traversal choices: left=false, right=true
		branch_stack.push_back(false); //starting from left child of root
		
		unsigned int n=1; //number of nodes traversed so far
		
		std::random_device rd;
		std::mt19937 gen(rd());
		
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
	
	void Fern::crossover(const Fern& other) {
		Node::iterator itn1(root); //new node, this tree
		itn1.left(); //excluding the root node from the search
		Node::iterator itn2(other.root); //new node, other tree (need const_iterator?)
		itn2.left();
		
		Node::iterator locus1(itn1); //current choices
		Node::iterator locus2(itn2); 
		
		vector<bool> branch_stack(); //stack recording traversal choices: left=false, right=true
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
		*locus1 = *locus2;
		root->update_boundary(lower_bound, upper_bound);
		root->update_max_bin(max_bin);
	}

} //namespace clau


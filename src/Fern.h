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

#include "Node.h"
#include "Fork.h"
#include "Leaf.h"

namespace clau {
	
	class Fern {
	private:
		Fork* root;
		num_type upper_bound, lower_bound;
		bin_type max_bin;
		
	public:
		Fern();
		Fern(const num_type lowerBound, const num_type upperBound, 
		     const bin_type numBins);
		Fern(const Fern& rhs);
		Fern& operator=(const Fern& rhs);
		~Fern();
	
		void mutate();
		void crossover(const Fern& other);
	
		class iterator {
		private:
			Node* current;
			iterator(Node* start);
			friend Fern::root();
			
		public:
			iterator() : current(NULL) {}
			iterator(const iterator& rhs) : current(rhs.current) {}
			Node& operator=(const iterator& rhs);
			~iterator() = default;
			
			iterator& up();
			iterator& left();
			iterator& right();
			
			Node& operator*() { return *current; }
			Node* operator->() { return current; }
			
		}; //class iterator
		
		iterator root() { return iterator(root); }
		
	}; //class Fern
	
} //namespace clau

#endif


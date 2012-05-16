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

namespace clau {
	
	class Fern {
	private:
		Node* pRoot;
		float upper_bound, lower_bound;
		
	public:
		class node_iterator {
		private:
			Node* current;
			
			iterator(Node* start);
			friend 
			
		public:
			iterator();
			iterator(Node* start);
			iterator(const iterator& rhs);
			Node& operator=(const iterator& rhs);
			~iterator() = default;
			
			iterator& up();
			iterator& left();
			iterator& right();
			
			Node& operator*() { return *current; }
			Node* operator->() { return current; }
			
		}; //class iterator
		
		node_iterator root();
		bin_iterator begin();
		bin_iterator end();
		
	}; //class Fern
	
} //namespace clau

#endif


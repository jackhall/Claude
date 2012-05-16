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
		Node* root;
		std::vector<Bin> bins;
		float upper_bound, lower_bound;
		
	public:
		Fern();
		Fern(const Fern& rhs);
		Fern& operator=(const Fern& rhs);
		~Fern();
	
		class node_iterator {
		private:
			Node* current;
			
		public:
			node_iterator();
			node_iterator(Node* start);
			node_iterator(const node_iterator& rhs);
			Node& operator=(const node_iterator& rhs);
			~node_iterator() = default;
			
			node_iterator& up();
			node_iterator& left();
			node_iterator& right();
			
			Node& operator*() { return *current; }
			Node* operator->() { return current; }
			
		}; //class node_iterator
		
		class bin_iterator {
		private:
			Bin* current;
		
		public:
			bin_iterator();
			bin_iterator(Bin* start);
			bin_iterator(const bin_iterator& rhs);
			Bin& operator=(const bin_iterator& rhs);
			~bin_iterator() = default;
			
			bin_iterator& operator++();
			bin_iterator operator++(int);
			bin_iterator& operator--();
			bin_iterator operator--(int);
			
			Bin& operator*() { return *current; }
			Bin* operator->() { return current; }
		
		}; //class bin_iterator
		
		node_iterator root();
		bin_iterator begin();
		bin_iterator end();
		
	}; //class Fern
	
} //namespace clau

#endif


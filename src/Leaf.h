#ifndef Leaf_h
#define Leaf_h

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

	class Leaf : public Node {
	/*
		A Leaf uses the Node interface, adding a pointer to a discrete Bin. Its version
		of update_boundary does nothing, as it has no boundary and Bins require no 
		updating. Its version of query returns the index of the bin it points to. 
	*/
	private:
		Bin* bin;
		
	public:
		Leaf();
		Leaf(Fork* pParent, Bin* pBin);
		Leaf(const Leaf& rhs);
		Leaf& operator=(const Leaf& rhs);
		~Leaf();
		
		bool is_leaf() const { return true; }
		void update_boundary(const num_type lower_bound, const num_type upper_bound) {}
		unsigned short query(const num_type number) const { return bin->get_index(); }
		
	}; //class Bin

} //namespace clau

#endif

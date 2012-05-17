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
	
	Leaf::Leaf() : Node(NULL), bin(NULL) {}
	
	Leaf::Leaf(Fork* pParent, Bin* pBin) 
		: Node(pParent), bin(pBin) {
		bin->insert(this);
	}
	
	Leaf::Leaf(const Leaf& rhs, Fork* pParent) 
		: {
		//both parent and bin need new values; does it make sense to copy a leaf?
	}
	
	Leaf& Leaf::operator=(const Leaf& rhs) {
		//both parent and bin need new values; does it make sense to copy a leaf?
	}
	
	Leaf::~Leaf() {
		bin->remove(this);
	}
	
} //namespace clau

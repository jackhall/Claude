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
	
	Bin::Bin()
	
	Bin::Bin(const unsigned short number)
	
	Bin::Bin(const Bin& rhs)
	
	Bin& Bin::operator=(const Bin& rhs)
	
	Bin::~Bin()
	
	void Bin::insert(Leaf* new_leaf)
	
	void Bin::remove(Leaf* old_leaf)
	
	void Bin::move_left()
	
	void Bin::move_right()
	
	bool Bin::can_move_left() const
	
	bool Bin::can_move_right() const
	
} //namespace clau
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
	
	//Leaf::Leaf() : Node(NULL), max_bin(0), bin(0) {}
	
	Leaf::Leaf(const Leaf& rhs, Fork* pParent) 
		: Node(pParent), max_bin(rhs.max_bin), bin(rhs.bin) {}
	
	Leaf& Leaf::operator=(const Leaf& rhs) {
		if(this != &rhs) {
			max_bin = rhs.max_bin;
			if(rhs.bin > max_bin) {
				bin = max_bin; //set randomly?
			} else bin = rhs.bin;
		}
	}
	
	Leaf::~Leaf() {}
	
	void Leaf::update_max_bin(const bin_type nMaxBin) {
		max_bin = nMaxBin;
		if(bin > max_bin) bin = max_bin; //maybe decide on a new bin number randomly?
	}
	
	void Leaf::mutate(rng_type& gen) {
		std::bernoulli_distribution random_bits(0.5);
		std::uniform_int_distribution<> random_ints(0, max_bin);
		
		if(random_bits(gen)) split( random_bits(gen) );
		else bin = random_ints(gen);
	}
	
	void Leaf::split(const bool bValue) {
	//deletes this object! invalidates all pointers, references, iterators to it
		Fork* parentFork = static_cast<Fork*>(parent);
		if( parentFork->is_left(this) ) {
			parentFork->replace_left( new Fork(parentFork, bValue) ); //deletes this instance
		} else {
			parentFork->replace_right( new Fork(parentFork, bValue) ); //deletes this instance
		}
	}
	
} //namespace clau

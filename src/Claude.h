#ifndef Claude_h
#define Claude_h

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

	/*
		Claude is an algorithm that uses a binary tree to partition
		real-number spaces. The partitioning is designed to be
		optimal in an information-theoretic sense, extracting only
		the information necessary for whatever decision is made
		with the data. 
	
		The current version of Claude does not include interval 
		extensions and asks the programmer to manually set the 
		number of discrete bins. It also is not generalized for
		use in more than one dimension. 
	*/
	
	typedef num_type float;
	typedef bin_type unsigned short;
	
	#include "Node.h"
	#include "Fork.h"
	#include "Leaf.h"
	#include "Fern.h"

} //namespace clau

#endif

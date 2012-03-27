/*
    Alexander: a neural networks library
    Copyright (C) 2011-2012  Jack Hall

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

#include "ValueMatrix.h"

namespace alex {

	PDF* ValueMatrix::add(const unsigned int address);
	void ValueMatrix::remove(const unsigned int address);
	void ValueMatrix::add_data(const unsigned int address, const data_type data_point);
	void ValueMatrix::update();
	
	bool ValueMatrix::ready() const;
		
	info_type ValueMatrix::get_mutual_info(	const unsigned int address1, 
						const unsigned int address2) const;

} //namespace alex


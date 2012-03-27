#ifndef PDF_h
#define PDF_h

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

namespace alex {
	
	template<unsigned int D> //number of dimensions
	class PDF { 
	private:
	
	public:
		PDF();
		PDF(const PDF& rhs) = delete; //for now
		PDF& operator=(const PDF& rhs) = delete;
		~PDF() = default;
		
		info_type query(const data_type point); //how to write these?
		void update(const data_type point);
	}; //class PDF
	
} //namespace alex

#endif

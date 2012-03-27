#ifndef Value_h
#define Value_h

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

#include <map>

namespace alex {
	
	struct AddressPair {
		unsigned int one, two;
		AddressPair() : one(0), two(0) {}
		AddressPair(const unsigned int first, const unsigned int second) 
			: one(first), two(second) {}
		AddressPair(const AddressPair& rhs) = default;
		AddressPair& operator=(const AddressPair& rhs) = default;
		~AddressPair() = default;
		
		inline bool operator<(const AddressPair& rhs) const {
			if(one == rhs.one) return two < rhs.two;
			else return one < rhs.one;
		}
	}; //struct AddressPair


	class Value {
	private:
		std::map< AddressPair, PDF<2> > joint_pdfs;
		std::map< unsigned int, PDF<1> > single_pdfs; //different pdf_type
		std::map<AddressPair, info_type> mutual_info;
		std::map<unsigned int, data_type> new_data;
		
	public:
		Value() = default;
		Value(const Value& rhs) = delete;
		Value& operator=(const Value& rhs) = delete;
		~Value() = default;
		
		PDF* add(const unsigned int address); //correct return value?
		void remove(const unsigned int address);
		void add_data(const unsigned int address, const data_type data_point);
		void update();
		
		bool ready() const;
		
		info_type get_mutual_info(const unsigned int address1, 
					  const unsigned int address2) const;
					  
	}; //class Value

} //namespace alex

#endif


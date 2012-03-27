#ifndef InputCell_h
#define InputCell_h

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

	class InputCell : public Neuron_base {	
	public:
		InputCell() = delete;
		InputCell(forward_index_type& fIndex, 
			  backprop_index_type& bIndex,
			  information_index_type& iIndex);
		InputCell(const InputCell& rhs) = default;
		InputCell(InputCell&& rhs);
		InputCell& operator=(const InputCell& rhs) = default;
		InputCell& operator=(InputCell&& rhs) = delete;
		~InputCell() = default;
		
		gradient_type take_gradient()
			{ return collect_errors(); }
		void feed_signal(const data_type signal)
			{ output = signal;	distribute_signals(); }
	}; //class InputCell
	
	InputCell::InputCell(forward_index_type& fIndex, backprop_index_type& bIndex) 
		: Neuron_base(fIndex, bIndex, iIndex, 0.0, false) {}
	
	InputCell::InputCell(InputCell&& rhs)
		: Neuron_base(std::move(rhs.Neuron_base)) {}
	
} //namespace alex

#endif

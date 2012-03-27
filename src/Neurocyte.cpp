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

#include "Alexander.h"
#include <cmath>

namespace alex {
	
	void Neurocyte::fire() {
		//collect input signals
		auto ip = forward_node.input_begin();
		auto ipe = forward_node.input_begin();
		while(ip != ipe) {
			ip >> signal;
			//add to pdf
			state += ip->weight * signal;
			++ip;
		}
		
		//update joint distributions and output distribution (self-information)
		
		output = f(state);
		distribute_signals(output);
	}
	
	void Neurocyte::train() {
		//topology training
		value = collect_value();
		distribute_value();
		
		//weight and bias training
		auto input_sum = collect_errors();
		gradient_type gradient = input_sum * df(state);
		backprop_node.bias = -learning_rate*gradient + momentum*backprop_node.bias;
		distribute_errors(gradient);
	}
	
} //namespace alex


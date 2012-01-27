#ifndef InfoNet_h
#define InfoNet_h

/*
	Claude is an algorithm for modeling dynamic systems with a living network
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
	
	jackwhall7@gmail.com
*/

#include "Benoit.h"

namespace claude {
	typedef float real_type;

	class InfoNet {
	private:
		Index<real_type, real_type> info_index;
		Index<real_type, real_type> signal_index;
		std::list<InfoNeuron> neurons;
	
	public:
		InfoNet() = default;
		InfoNet(const InfoNet& rhs) = delete;
		InfoNet(InfoNet&& rhs) = default;
		InfoNet& operator=(const InfoNet& rhs) = delete;
		InfoNet& operator=(InfoNet&& rhs) = default;
		~InfoNet() = default;
	
		//unsigned int add_tanh(const double bias, const bool trainable=true);
		//unsigned int add_tanh(const std::pair<double,bool> bias);
		//unsigned int add_sigmoid(const double bias, const bool trainable=true);
		//unsigned int add_sigmoid(const std::pair<double,bool> bias);
		//unsigned int add_linear(const double bias, const bool trainable=true);
		//unsigned int add_linear(const std::pair<double,bool> bias);
		//unsigned int add_gaussian(const double bias, const bool trainable=true);
		//unsigned int add_gaussian(const std::pair<double,bool> bias);
	
		void connect(const unsigned int origin, 
			     const unsigned int target,
			     const real_type weight);
		
		void run();
	}; //class InfoNet
} //namespace claude

#endif

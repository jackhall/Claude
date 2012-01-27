#ifndef InfoNeuron_h
#define InfoNeuron_h

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

	class InfoNeuron {
	protected:
		Node<real_type, real_type> forward; //FIELD
		Node<real_type, real_type> backward; //FIELD
		std::deque<real_type> state; //FIELD
		
	public:
		const char neuron_type; //FIELD

		InfoNeuron() = delete;
		InfoNeuron(const char chNeuron_type,
			   Index<real_type, real_type>& fIndex, 
			   Index<real_type, real_type>& bIndex, 
			   const real_type tBias); //not written yet
		InfoNeuron(const InfoNeuron& rhs); //not written yet
		InfoNeuron(InfoNeuron&& rhs) = default;
		InfoNeuron& operator=(const InfoNeuron& rhs); //not written yet
		InfoNeuron& operator=(InfoNeuron&& rhs) = delete;
		virtual ~InfoNeuron(); //not written yet
	
		virtual void fire() = 0;
	
		void add_input(const unsigned int address, const real_type tWeight);
		void remove_input(const unsigned int address); 
		unsigned int ID() const { return forward.ID; }

	}; //class InfoNeuron
} //namespace claude
#endif

#ifndef Organism_h
#define Organism_h

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

#include <list>
#include <vector>
#include "Alexander.h"

namespace alex {
	
	class Organism {
	/*
		Organism manages and contains a living network. It has a ben::Index object
		for each of the three overlaid networks and a Fitness object to mediate 
		between Genotypes. It has separate storage for input cells (static), output
		cells (static), and hidden cells (dynamic). 
	*/
	private:
		//cells
		std::list<InputCell> inputs;
		std::list<Neurocyte> cells;
		std::list<Neurocyte> outputs;
	
	public:
		//Indicies, public for Neurocyte construction
		ben::Index<data_type, data_type> forward;
		ben::Index<delta_type, gradient_type> backprop; //reverse of forward
		ben::Index<pdf_type, info_type> information; //full set of possible connections
	
		Value distributions; //centralized pdf and information matrix
		Fitness<352> fitness; //centralized evolutionary data
		
		Organism() = default;
		Organism(const Organism& rhs) = delete;
		//Organism(Organism&& rhs);
		Organism& operator=(const Organism& rhs) = delete;
		//Organism& operator=(Organism&& rhs);
		~Organism() = default;
		
		unsigned int add_neurocyte(const unsigned int layer);
		unsigned int add_input();
		unsigned int add_output();
		
		void connect(const unsigned int origin, 
			     const unsigned int target,
			     const double weight,
			     const bool trainable=true);
		
		void run();
		void train();
	}; //class Organism
	
} //namespace alex

#endif


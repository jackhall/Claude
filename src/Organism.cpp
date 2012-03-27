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

	/* add copying later; not necessary now
	Organism::Organism(Organism&& rhs) 
		: forward(std::move(rhs.forward)), 
		  backprop(std::move(rhs.backprop)),
		  information(std::move(rhs.information)),
		  inputs(std::move(rhs.inputs)),
		  cells(std::move(rhs.cells)),
		  outputs(std::move(rhs.outputs)) {}
	
	Organism& Organism::operator=(Organism&& rhs) {
		forward = std::move(rhs.forward);
		backprop = std::move(rhs.backprop);
		information = std::move(rhs.information);
		inputs = std::move(rhs.inputs);
		cells = std::move(rhs.cells);
		outputs = std::move(rhs.outputs); 
	}
	*/ 
	
	unsigned int Organism::add_neurocyte(const unsigned int layer);
	unsigned int Organism::add_input();
	unsigned int Organism::add_output();
	
	void Organism::connect( const unsigned int origin, 
				const unsigned int target,
				const double weight,
				const bool trainable=true);
	
	void Organism::run();
	void Organism::train();
		
} //namespace alex


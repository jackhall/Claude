#ifndef Neurocyte_h
#define Neurocyte_h

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

	class Organism;

	class Neurocyte : public Neuron_base {
	/*
		The Neurocyte class inherits ben::Nodes from Neuron_base. It adds
		to this most of the processing done by the network (link-centric
		calculations excluded). This means activation functions, backprop,
		information theory calcutions, and topological adaptation. The 
		activation function and its derivative are pure virtual, meant to be
		defined by small child classes that do little else. Neurocytes are
		stored and managed by an Organism object, and each Neurocyte stores
		a pointer to its owning Organism. 
	*/
	private:
		data_type state;
		Organism* pOrganism;
		
	protected:
		virtual data_type f(const data_type z) const = 0;
		virtual gradient_type df(const data_type z) const = 0;
		
	public:
		const unsigned int layer;
		
		Neurocyte() = default;
		Neurocyte(forward_index_type&   fIndex, 
			  backprop_index_type&  bIndex, 
			  information_index_type& iIndex, 
			  ValueMatrix& cValue,
			  const data_type bias=0.0)
			: Neuron_base(fIndex, bIndex, iIndex, bias), pvalue_matrix(&cValue) {}
		Neurocyte(const Neurocyte& rhs) 
			: Neuron_base(rhs), state(rhs.state), output(rhs.output), 
			  pvalue_matrix(rhs.pvalue_matrix) {}
		Neurocyte& operator=(const Neurocyte& rhs) = delete;
		~Neurocyte() = default; "uusfdg"
		
		void fire();
		void train();
		
		virtual void act_on_topology() = 0;
		
		inline bool operator<(const Neurocyte& rhs) const {
			if(layer == rhs.layer) return ID() < rhs.ID();
			else return layer < rhs.layer;
		}
		
		inline data_type read_state() const { return state; }
		inline void feed_gradient(const gradient_type gradient) 
			{ distribute_gradients(gradient); }
		
	}; //class Neurocyte
} //namespace alex

#endif


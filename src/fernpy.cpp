/*
    Claude: a real-to-discrete coding scheme based on spiraling trees
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
    
    e-mail: jackwhall7@gmail.com
*/

#include <boost/python.hpp>
#include "Fern.h"

//set dimension of Fern here:
#define DIM 1 

//test function and class (diagnosis of C++ -> Python translation)
/*
char const* greet_func() {
	return "Hello, world!";
}

class greet_class {
public:
	std::string greet() { return "Greetings, world!"; }
};
*/

//boost.python module
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(fern_member_overloads, get_bounds, 0, 1)

BOOST_PYTHON_MODULE(fernpy) {
	using namespace boost::python;
	using namespace clau;

	//def("greet_func", greet_func);
	
	//class_<greet_class>("greet_class")
	//	.def("greet", &greet_class::greet);
	
	class_<Interval>("interval")
		.def( init<num_type, num_type>() )
		.def_readwrite("lower", &Interval::lower)
		.def_readwrite("upper", &Interval::upper)
		.def( self == self )
		.def( self != self )
		.def( self_ns::str(self) );
	
	class_< Region<DIM> >("region")
		.def("set_uniform", &Region<DIM>::set_uniform)
		.def( self == self )
		.def( self != self )
		.def( self_ns::str(self) );
		//.def( "__getitem__", &Region<DIM>::operator(), arg( "index" ), return_internal_reference<>() );
		
	class_< Point<DIM> >("point");
		//.def( "__getitem__", &Point<DIM>::operator(), arg( "index" ), return_internal_reference<>() );
		
	class_< Fern<DIM> >("fern", init<bin_type>())
		.def( init<Region<DIM>, bin_type>() )
		.def("set_bounds", &Fern<DIM>::set_bounds)
		//.def("get_bounds", &Fern<DIM>::get_bounds, fern_member_overloads());
		.def("get_num_bins", &Fern<DIM>::get_num_bins)
		.def("mutate", &Fern<DIM>::mutate)
		.def("crossover", &Fern<DIM>::crossover)
		.def("query", &Fern<DIM>::query)
		.def( self_ns::str(self) )
		.def("begin", &Fern<DIM>::begin);
		//.def("random_analagous", &Fern<DIM>::random_analagous);
	
	class_< Fern<DIM>::Division >("division")
		.def( init<bool, dim_type>() )
		.def_readwrite("bit", &Fern<DIM>::Division::bit)
		.def_readwrite("dimension", &Fern<DIM>::Division::dimension);
	/*
	class_< Fern<DIM>::node_handle >("node_handle")
		.def( self == self )
		.def( self != self )
		.def("up", &Fern<DIM>::node_handle::up)
		.def("left", &Fern<DIM>::node_handle::left)
		.def("right", &Fern<DIM>::node_handle::right)
		.def("random", &Fern<DIM>::node_handle::random)
		.def("root", &Fern<DIM>::node_handle::root)
		.def("mutate_value", &Fern<DIM>::node_handle::mutate_value)
		.def("mutate_structrure", &Fern<DIM>::node_handle::mutate_structure)
		.def("splice", &Fern<DIM>::node_handle::splice)
		.def("split_leaf", &Fern<DIM>::node_handle::split_leaf)
		.def("get_leaf_bin", &Fern<DIM>::node_handle::get_leaf_bin)
		.def("set_leaf_bin", &Fern<DIM>::node_handle::set_leaf_bin)
		.def("merge_fork", &Fern<DIM>::node_handle::merge_fork)
		.def("get_fork_boundary", &Fern<DIM>::node_handle::get_fork_boundary)
		.def("get_fork_dimension", &Fern<DIM>::node_handle::get_fork_dimension)
		.def("set_fork_dimension", &Fern<DIM>::node_handle::set_fork_dimension)
		.def("get_fork_bit", &Fern<DIM>::node_handle::get_fork_bit)
		.def("set_fork_bit", &Fern<DIM>::node_handle::set_fork_bit)
		.def("is_leaf", &Fern<DIM>::node_handle::is_leaf)
		.def("is_root", &Fern<DIM>::node_handle::is_root)
		.def("is_ghost", &Fern<DIM>::node_handle::is_ghost)
		.def("belongs_to", &Fern<DIM>::node_handle::belongs_to); */
}

/* ctypes
extern "C" {
	
	using namespace clau;
	
	//Fern interface
	//constructors
	Fern<DIM>* Fern_new(const bin_type numBins=1.0) 
	    	{ return new Fern<DIM>(numBins); }
	Fern<DIM>* Fern_new(const Region<DIM> bounds, 
    			    const bin_type numBins=1.0) 
    		{ return new clau::Fern<DIM>(bounds, numBins); }
	Fern<DIM>* Fern_new(const Fern<DIM>& rhs) 
	    	{ return new Fern<DIM>(rhs); }
	    	
	Fern<DIM>& operator=(const Fern<DIM>& rhs);
	~Fern();
	
	//other methods
	void set_bounds(const Region<D> bounds);
	Region<D> get_bounds() const { return root_region; }
	Interval get_bounds(const dim_type dimension) const 	
	bin_type get_num_bins() const { return max_bin+1; }	
	void mutate();
	void crossover(const Fern& other);
	bin_type query(const Point<D> point) const { return root->query(point); }
	
	//Fern::node_handle interface
	//constructor
	    
	void Foo_bar(Foo* foo){ foo->bar(); }
}
*/


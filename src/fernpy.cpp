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

/*
#define PYTHON_ERROR(TYPE, REASON) \
{ \
    PyErr_SetString(TYPE, REASON); \
    throw bp::error_already_set(); \
} */

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
void IndexError() { PyErr_SetString(PyExc_IndexError, "Index out of range"); }

template<class T>
struct std_item { //helper class for Point and Region
	typedef typename T::value_type V;
	
	static V get(const T& x, int i) {
		if( i<0 ) i += x.size();
		if( i>=0 && i<x.size() ) return x(i+1);
		IndexError();
	}
	
	static void set(T& x, int i, const V v) {
		if( i<0 ) i += x.size();
		if( i>=0 && i<x.size() ) x(i+1) = v;
		else IndexError();
    	}
};
/*
template<class T>
inline PyObject * managingPyObject(T *p) {
	return typename boost::python::manage_new_object::apply<T *>::type()(p);
}

template<class Copyable>
boost::python::object std_copy(boost::python::object copyable) {
	using namespace boost::python;
	Copyable *newCopyable = new Copyable(extract<const Copyable&>(copyable));
	object result(detail::new_reference(managingPyObject(newCopyable)));

	extract<dict>(result.attr("__dict__"))().update(copyable.attr("__dict__"));

	return result;
}
/*
template<class Copyable>
bp::object std_deepcopy(bp::object copyable, bp::dict memo) {
	bp::object copyMod = bp::import("copy");
	bp::object deepcopy = copyMod.attr("deepcopy");

	Copyable *newCopyable(new Copyable(bp::extract<const Copyable&>(copyable)));
	bp::object result(bp::detail::new_reference(managingPyObject(newCopyable)));

	// HACK: copyableId shall be the same as the result of id(copyable) in Python -
	// please tell me that there is a better way! (and which ;-p)
	long copyableId = (long)(copyable.ptr());
	memo[copyableId] = result;

	bp::extract<bp::dict>(result.attr("__dict__"))().update(
        deepcopy(bp::extract<bp::dict>(copyable.attr("__dict__"))(), memo));

	return result;
}*/

//class_<foo>(foo)
//	.def("__copy__", &generic__copy__< foo >)
//	.def("__deepcopy__", &generic__deepcopy__< foo >)
//	.def(init< const foo & >())

BOOST_PYTHON_MODULE(fernpy) {
	using namespace boost::python;
	using namespace clau;

	//def("greet_func", greet_func);
	
	//class_<greet_class>("greet_class")
	//	.def("greet", &greet_class::greet);
	
	class_<Interval>("interval")
		.def( init<num_type, num_type>() )
		.def( init<const Interval&>() )
		//.def("__copy__", &std_copy<Interval>)
		//.def("__deepcopy", &std_deepcopy<Interval>)
		.def_readwrite("lower", &Interval::lower)
		.def_readwrite("upper", &Interval::upper)
		.def( self == self )
		.def( self != self )
		.def( self_ns::str(self) );
	
	//////////////////////////////////////////////////////////////////////////
	
	class_< Region<1> >("region1")
		.def( init<const Region<1>&>() )
		//.def("__copy__", &std_copy< Region<DIM> >)
		//.def("__deepcopy__", &std_deepcopy< Region<DIM> >)
		.def("set_uniform", &Region<1>::set_uniform)
		.def( self == self )
		.def( self != self )
		.def( self_ns::str(self) )
		.def("__getitem__", &std_item< Region<1> >::get)
		.def("__setitem__", &std_item< Region<1> >::set);
	
	class_< Point<1> >("point1")
		.def( init<const Point<1>&>() )
		//.def("__copy__", &std_copy< Point<DIM> >)
		//.def("__deepcopy__", &std_deepcopy< Point<DIM> >)
		.def( self == self )
		.def( self != self )
		.def( self_ns::str(self) )
		.def("__getitem__", &std_item< Point<1> >::get)
		.def("__setitem__", &std_item< Point<1> >::set);
	
	class_< Fern<1> >("fern1", init<bin_type>())
		.def( init<Region<1>, bin_type>() )
		.def( init<const Fern<1>&>() )
		//.def("__copy__", &std_copy< Fern<DIM> >)
		//.def("__deepcopy__", &std_deepcopy< Fern<DIM> >)
		.def("set_node_type_chance", &Fern<1>::set_node_type_chance)
		.def("get_node_type_chance", &Fern<1>::get_node_type_chance)
		.def("set_mutation_type_chance", &Fern<1>::set_mutation_type_chance) 
		.def("get_mutation_type_chance_fork", &Fern<1>::get_mutation_type_chance_fork) 
		.def("get_mutation_type_chance_leaf", &Fern<1>::get_mutation_type_chance_leaf) 
		.def("set_bounds", &Fern<1>::set_bounds)
		.def("get_bounds", &Fern<1>::get_bounds)
		.def("get_region", &Fern<1>::get_region)
		.def("get_num_bins", &Fern<1>::get_num_bins)
		.def("mutate", &Fern<1>::mutate)
		.def("crossover", &Fern<1>::crossover)
		.def("query", &Fern<1>::query)
		.def( self_ns::str(self) )
		.def("begin", &Fern<1>::begin);
	
	class_< Fern<1>::Division >("division1")
		.def( init<const Fern<1>::Division&>() )
		//.def("__copy__", &std_copy< Fern<DIM>::Division >)
		//.def("__deepcopy__", &std_deepcopy< Fern<DIM>::Division >)
		.def( init<bool, dim_type>() )
		.def_readwrite("bit", &Fern<1>::Division::bit)
		.def_readwrite("dimension", &Fern<1>::Division::dimension);
	
	class_< Fern<1>::node_handle >("node_handle1") 
		.def( init<const Fern<1>::node_handle&>() )
		//.def("__copy__", &std_copy< Fern<DIM>::node_handle >)
		//.def("__deepcopy__", &std_deepcopy< Fern<DIM>::node_handle >)
		.def( self == self )
		.def( self != self )
		.def("up", &Fern<1>::node_handle::up, return_internal_reference<>())
		.def("left", &Fern<1>::node_handle::left, return_internal_reference<>())
		.def("right", &Fern<1>::node_handle::right, return_internal_reference<>())
		.def("random", &Fern<1>::node_handle::random, return_internal_reference<>())
		.def("random_analagous", &Fern<1>::node_handle::random_analagous)
		.def("root", &Fern<1>::node_handle::root, return_internal_reference<>())
		.def("mutate_value", &Fern<1>::node_handle::mutate_value)
		.def("mutate_structrure", &Fern<1>::node_handle::mutate_structure)
		.def("splice", &Fern<1>::node_handle::splice)
		.def("split_leaf", &Fern<1>::node_handle::split_leaf)
		.def("get_leaf_bin", &Fern<1>::node_handle::get_leaf_bin)
		.def("set_leaf_bin", &Fern<1>::node_handle::set_leaf_bin)
		.def("get_max_bin", &Fern<1>::node_handle::get_max_bin)
		.def("merge_fork", &Fern<1>::node_handle::merge_fork)
		.def("get_fork_boundary", &Fern<1>::node_handle::get_fork_boundary)
		.def("get_fork_dimension", &Fern<1>::node_handle::get_fork_dimension)
		.def("set_fork_dimension", &Fern<1>::node_handle::set_fork_dimension)
		.def("get_fork_bit", &Fern<1>::node_handle::get_fork_bit)
		.def("set_fork_bit", &Fern<1>::node_handle::set_fork_bit)
		.def("is_leaf", &Fern<1>::node_handle::is_leaf)
		.def("is_root", &Fern<1>::node_handle::is_root)
		.def("is_ghost", &Fern<1>::node_handle::is_ghost)
		.def("belongs_to", &Fern<1>::node_handle::belongs_to); 
	
	///////////////////////////////////////////////////////////////////////
	
	class_< Region<2> >("region2")
		.def( init<const Region<2>&>() )
		//.def("__copy__", &std_copy< Region<DIM> >)
		//.def("__deepcopy__", &std_deepcopy< Region<DIM> >)
		.def("set_uniform", &Region<2>::set_uniform)
		.def( self == self )
		.def( self != self )
		.def( self_ns::str(self) )
		.def("__getitem__", &std_item< Region<2> >::get)
		.def("__setitem__", &std_item< Region<2> >::set);
	
	class_< Point<2> >("point2")
		.def( init<const Point<2>&>() )
		//.def("__copy__", &std_copy< Point<DIM> >)
		//.def("__deepcopy__", &std_deepcopy< Point<DIM> >)
		.def( self == self )
		.def( self != self )
		.def( self_ns::str(self) )
		.def("__getitem__", &std_item< Point<2> >::get)
		.def("__setitem__", &std_item< Point<2> >::set);
	
	class_< Fern<2> >("fern2", init<bin_type>())
		.def( init<Region<2>, bin_type>() )
		.def( init<const Fern<2>&>() )
		//.def("__copy__", &std_copy< Fern<DIM> >)
		//.def("__deepcopy__", &std_deepcopy< Fern<DIM> >)
		.def("set_node_type_chance", &Fern<2>::set_node_type_chance)
		.def("get_node_type_chance", &Fern<2>::get_node_type_chance)
		.def("set_mutation_type_chance", &Fern<2>::set_mutation_type_chance) 
		.def("get_mutation_type_chance_fork", &Fern<2>::get_mutation_type_chance_fork) 
		.def("get_mutation_type_chance_leaf", &Fern<2>::get_mutation_type_chance_leaf) 
		.def("set_bounds", &Fern<2>::set_bounds)
		.def("get_bounds", &Fern<2>::get_bounds)
		.def("get_region", &Fern<2>::get_region)
		.def("get_num_bins", &Fern<2>::get_num_bins)
		.def("mutate", &Fern<2>::mutate)
		.def("crossover", &Fern<2>::crossover)
		.def("query", &Fern<2>::query)
		.def( self_ns::str(self) )
		.def("begin", &Fern<2>::begin);
	
	class_< Fern<2>::Division >("division2")
		.def( init<const Fern<2>::Division&>() )
		//.def("__copy__", &std_copy< Fern<DIM>::Division >)
		//.def("__deepcopy__", &std_deepcopy< Fern<DIM>::Division >)
		.def( init<bool, dim_type>() )
		.def_readwrite("bit", &Fern<2>::Division::bit)
		.def_readwrite("dimension", &Fern<2>::Division::dimension);
	
	class_< Fern<2>::node_handle >("node_handle2") 
		.def( init<const Fern<2>::node_handle&>() )
		//.def("__copy__", &std_copy< Fern<DIM>::node_handle >)
		//.def("__deepcopy__", &std_deepcopy< Fern<DIM>::node_handle >)
		.def( self == self )
		.def( self != self )
		.def("up", &Fern<2>::node_handle::up, return_internal_reference<>())
		.def("left", &Fern<2>::node_handle::left, return_internal_reference<>())
		.def("right", &Fern<2>::node_handle::right, return_internal_reference<>())
		.def("random", &Fern<2>::node_handle::random, return_internal_reference<>())
		.def("random_analagous", &Fern<2>::node_handle::random_analagous)
		.def("root", &Fern<2>::node_handle::root, return_internal_reference<>())
		.def("mutate_value", &Fern<2>::node_handle::mutate_value)
		.def("mutate_structrure", &Fern<2>::node_handle::mutate_structure)
		.def("splice", &Fern<2>::node_handle::splice)
		.def("split_leaf", &Fern<2>::node_handle::split_leaf)
		.def("get_leaf_bin", &Fern<2>::node_handle::get_leaf_bin)
		.def("set_leaf_bin", &Fern<2>::node_handle::set_leaf_bin)
		.def("get_max_bin", &Fern<2>::node_handle::get_max_bin)
		.def("merge_fork", &Fern<2>::node_handle::merge_fork)
		.def("get_fork_boundary", &Fern<2>::node_handle::get_fork_boundary)
		.def("get_fork_dimension", &Fern<2>::node_handle::get_fork_dimension)
		.def("set_fork_dimension", &Fern<2>::node_handle::set_fork_dimension)
		.def("get_fork_bit", &Fern<2>::node_handle::get_fork_bit)
		.def("set_fork_bit", &Fern<2>::node_handle::set_fork_bit)
		.def("is_leaf", &Fern<2>::node_handle::is_leaf)
		.def("is_root", &Fern<2>::node_handle::is_root)
		.def("is_ghost", &Fern<2>::node_handle::is_ghost)
		.def("belongs_to", &Fern<2>::node_handle::belongs_to); 
}


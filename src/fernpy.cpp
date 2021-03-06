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
#include <string>
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
struct pyInterval : public Interval, boost::python::pickle_suite { 

	static boost::python::tuple getinitargs(const pyInterval& x) {
		return boost::python::make_tuple
	}

	//static boost::python::tuple getstate(const pyInterval& x) {
		//in save() method: encode
	//}

	//static void setstate(pyInterval& x, boost::python::tuple state) {
		//translate data from tuple
		//in load() method: 
	//}
};
*/
template<class T>
struct std_pickle : boost::python::pickle_suite { //helper class for pickling
	static boost::python::tuple getinitargs(const T& x) {
		return boost::python::make_tuple(); //all classes must be default-constructible
	}

	static boost::python::tuple getstate(const T& x) {
		//in save() method: encode all data in a string
		return boost::python::make_tuple(x.save());
	}

	static void setstate(T& x, boost::python::tuple state) {
		//extract data from tuple (only one element, which is a string)
		std::string data = boost::python::extract<std::string>(state[0]);
		x.load(data); //reconstruct object from string
	}
};

template<clau::dim_type D>
struct fern_pickle : boost::python::pickle_suite {
	static boost::python::tuple savenode(typename clau::Fern<D>::Fork* fork_ptr) { //clau::Fern<D>::Fork not recognized as type?
		using namespace clau;
		namespace bp = boost::python; //needed to tell between std::tuple and bp::tuple
		
		//save division in this node
		auto divisionstr = fork_ptr->value.save();
		
		//save left subtree
		bp::tuple lefttuple;
		if( fork_ptr->left->leaf )
			lefttuple = bp::make_tuple(true, static_cast<typename Fern<D>::Leaf*>(fork_ptr->left)->bin);
		else 	lefttuple = savenode( static_cast<typename Fern<D>::Fork*>(fork_ptr->left) );
		
		//save right subtree
		bp::tuple righttuple;
		if( fork_ptr->right->leaf )
			righttuple = bp::make_tuple(true, static_cast<typename Fern<D>::Leaf*>(fork_ptr->right)->bin);
		else	righttuple = savenode( static_cast<typename Fern<D>::Fork*>(fork_ptr->right) );
			
		return bp::make_tuple(false, divisionstr, lefttuple, righttuple);
	}
	
	static typename clau::Fern<D>::Fork* constructnode(typename clau::Fern<D>::Fork* parent_ptr, boost::python::tuple state) {
		using namespace clau;
		using namespace boost::python;
		
		//load division for this node
		Division<D> division;
		std::string divisionstr = extract<std::string>(state[1]);
		division.load(divisionstr);
		
		auto fork_ptr = new typename Fern<D>::Fork(parent_ptr, division);
		
		//reconstruct left subtree
		tuple lefttuple = extract<tuple>(state[2]);
		if( extract<bool>(lefttuple[0]) ) {
			bin_type leftbin = extract<bin_type>(lefttuple[1]);
			fork_ptr->left = new typename Fern<D>::Leaf(fork_ptr, leftbin);
		} else fork_ptr->left = constructnode(fork_ptr, lefttuple);
		
		//reconstruct right subtree
		tuple righttuple = extract<tuple>(state[3]);
		if( extract<bool>(righttuple[0]) ) {
			bin_type rightbin = extract<bin_type>(righttuple[1]);
			fork_ptr->right = new typename Fern<D>::Leaf(fork_ptr, rightbin);
		} else fork_ptr->right = constructnode(fork_ptr, righttuple);
		
		return fork_ptr;
	}
	
	static boost::python::tuple getinitargs(const clau::Fern<D>& x) {
		return boost::python::make_tuple();
	}
	
	static boost::python::tuple getstate(const clau::Fern<D>& x) {
		auto regionstr = x.root_region.save();
		auto binnum = x.max_bin;
		auto nchance = x.node_type_chance;
		auto mchancef = x.mutation_type_chance_fork;
		auto mchancel = x.mutation_type_chance_leaf;
		auto roottuple = savenode(x.root);

		return boost::python::make_tuple(regionstr, binnum, nchance, 
						 mchancef, mchancel, roottuple);
	}
	
	static void setstate(clau::Fern<D>& x, boost::python::tuple state) {
		using namespace boost::python;
		
		std::string regionstr = extract<std::string>(state[0]);
		x.root_region.load(regionstr);
		x.max_bin = extract<clau::bin_type>(state[1]);
		x.node_type_chance = extract<float>(state[2]);
		x.mutation_type_chance_fork = extract<float>(state[3]);
		x.mutation_type_chance_leaf = extract<float>(state[4]);
		
		delete x.root;
		tuple roottuple = extract<tuple>(state[5]);
		x.root = constructnode(nullptr, roottuple);
		x.update_boundary();
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
		.def( init<const num_type, const num_type>() )
		.def( init<const Interval&>() )
		//.def("__copy__", &std_copy<Interval>)
		//.def("__deepcopy", &std_deepcopy<Interval>)
		.def_readwrite("lower", &Interval::lower)
		.def_readwrite("upper", &Interval::upper)
		.def("span", &Interval::span)
		.def( self == self )
		.def( self != self )
		.def( self_ns::str(self) )
		.def_pickle(std_pickle<Interval>());
	
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
		.def("__setitem__", &std_item< Region<1> >::set)
		.def("split", &Region<1>::split)
		.def("expand", &Region<1>::expand)
		.def_pickle(std_pickle< Region<1> >());
	
	class_< Point<1> >("point1")
		.def( init<const Point<1>&>() )
		//.def("__copy__", &std_copy< Point<DIM> >)
		//.def("__deepcopy__", &std_deepcopy< Point<DIM> >)
		.def( self == self )
		.def( self != self )
		.def( self_ns::str(self) )
		.def("__getitem__", &std_item< Point<1> >::get)
		.def("__setitem__", &std_item< Point<1> >::set)
		.def_pickle(std_pickle< Point<1> >());
	
	class_< Fern<1> >("fern1")
		.def( init<const bin_type>() )
		.def( init<const Region<1>, const bin_type>() )
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
		.def("randomize", &Fern<1>::randomize)
		.def("mutate", &Fern<1>::mutate)
		.def("crossover", &Fern<1>::crossover)
		.def("query", &Fern<1>::query)
		.def( self_ns::str(self) )
		.def("begin", &Fern<1>::begin)
		.def_pickle(fern_pickle<1>());
	
	class_< Division<1> >("division1")
		.def( init<const Division<1>&>() )
		//.def("__copy__", &std_copy< Fern<DIM>::Division >)
		//.def("__deepcopy__", &std_deepcopy< Fern<DIM>::Division >)
		.def( init<bool, dim_type>() )
		.def_readwrite("bit", &Division<1>::bit)
		.def_readwrite("dimension", &Division<1>::dimension)
		.def_pickle(std_pickle< Division<1> >());
	
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
		.def("get_fork_division", &Fern<1>::node_handle::get_fork_division)
		.def("set_fork_division", &Fern<1>::node_handle::set_fork_division)
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
		.def("__setitem__", &std_item< Region<2> >::set)
		.def("split", &Region<2>::split)
		.def("expand", &Region<2>::expand)
		.def_pickle(std_pickle< Region<2> >());
	
	class_< Point<2> >("point2")
		.def( init<const Point<2>&>() )
		//.def("__copy__", &std_copy< Point<DIM> >)
		//.def("__deepcopy__", &std_deepcopy< Point<DIM> >)
		.def( self == self )
		.def( self != self )
		.def( self_ns::str(self) )
		.def("__getitem__", &std_item< Point<2> >::get)
		.def("__setitem__", &std_item< Point<2> >::set)
		.def_pickle(std_pickle< Point<2> >());
	
	class_< Fern<2> >("fern2")
		.def( init<const bin_type>() )
		.def( init<const Region<2>, const bin_type>() )
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
		.def("randomize", &Fern<2>::randomize)
		.def("mutate", &Fern<2>::mutate)
		.def("crossover", &Fern<2>::crossover)
		.def("query", &Fern<2>::query)
		.def( self_ns::str(self) )
		.def("begin", &Fern<2>::begin)
		.def_pickle(fern_pickle<2>());
	
	class_< Division<2> >("division2")
		.def( init<const Division<2>&>() )
		//.def("__copy__", &std_copy< Fern<DIM>::Division >)
		//.def("__deepcopy__", &std_deepcopy< Fern<DIM>::Division >)
		.def( init<bool, dim_type>() )
		.def_readwrite("bit", &Division<2>::bit)
		.def_readwrite("dimension", &Division<2>::dimension)
		.def_pickle(std_pickle< Division<2> >());
	
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
		.def("get_fork_division", &Fern<2>::node_handle::get_fork_division)
		.def("set_fork_division", &Fern<2>::node_handle::set_fork_division)
		.def("is_leaf", &Fern<2>::node_handle::is_leaf)
		.def("is_root", &Fern<2>::node_handle::is_root)
		.def("is_ghost", &Fern<2>::node_handle::is_ghost)
		.def("belongs_to", &Fern<2>::node_handle::belongs_to); 
}



#ifndef PER_MODULE_NAME_HPP
#define PER_MODULE_NAME_HPP

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "per/per.h"
#include "per/sum_tree.h"

PYBIND11_MODULE(pyper, m)
{
   py::class_< SumTree > sumtree(m, "SumTree");
   sumtree.def(py::init< size_t >());
   sumtree.def("__len__", &SumTree::len);
   sumtree.def("__getitem__", &SumTree::priority, py::arg("index"));
   sumtree.def("__str__", &SumTree::as_str);
   sumtree.def_property_readonly("total", &SumTree::total);
   sumtree.def("insert", &SumTree::insert, py::arg("value"), py::arg("priority"));
   sumtree.def(
      "update",
      py::overload_cast< size_t, double >(&SumTree::update),
      py::arg("index"),
      py::arg("priority"));
   sumtree.def(
      "update",
      py::overload_cast< const std::vector< size_t >&, const std::vector< double >& >(
         &SumTree::update),
      py::arg("index"),
      py::arg("priority"));
   sumtree.def("get", &SumTree::get, py::arg("priority"), py::arg("percentage") = true);
   sumtree.def("priorities", &SumTree::priorities_all);

   py::class_< PriorityExperience > pe(m, "PriorityExperience");
   pe.def(
      py::init< size_t, std::mt19937_64::result_type >(),
      py::arg("capacity"),
      py::arg("seed") = std::random_device{}());
   pe.def("push", py::overload_cast< py::object >(&PriorityExperience::push), py::arg("value"));
   pe.def(
      "push",
      py::overload_cast< const std::vector< py::object >& >(&PriorityExperience::push),
      py::arg("value"));
   pe.def("update", &PriorityExperience::update, py::arg("indices"), py::arg("priorities"));
   pe.def("sample", &PriorityExperience::sample, py::arg("n"), py::arg("beta"));
   pe.def_property(
      "alpha",
      py::overload_cast<>(&PriorityExperience::alpha, py::const_),
      py::overload_cast< double >(&PriorityExperience::alpha));
   pe.def_property_readonly("capacity", &PriorityExperience::capacity);
}

#endif  // PER_MODULE_NAME_HPP

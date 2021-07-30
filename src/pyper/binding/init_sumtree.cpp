#include <pybind11/pybind11.h>

#include "per/sum_tree.h"

namespace py = pybind11;

void init_sumtree(py::module_& m)
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

   sumtree.def("priority_iter", [](const SumTree& tree) {
      return py::make_iterator(tree.priority_begin(), tree.priority_end());
   });

   sumtree.def("value_iter", [](const SumTree& tree) {
      return py::make_iterator(tree.value_begin(), tree.value_end());
   });
}
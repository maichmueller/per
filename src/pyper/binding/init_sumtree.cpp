#include <pybind11/pybind11.h>

#include "per/sum_tree.hpp"

namespace py = pybind11;

void init_sumtree(py::module_& m)
{
   using PySumTree = SumTree< py::object >;
   py::class_< PySumTree > sumtree(m, "SumTree");
   sumtree.def(py::init< size_t >());

   sumtree.def("__len__", &PySumTree::size);

   sumtree.def("__getitem__", &PySumTree::priority, py::arg("index"));

   sumtree.def("__str__", &PySumTree::as_str);

   sumtree.def_property_readonly("total", &PySumTree::total);

   sumtree.def("insert", &PySumTree::insert, py::arg("value"), py::arg("priority"));

   sumtree.def(
      "update",
      py::overload_cast< size_t, double, std::optional< PySumTree::value_type > >(
         &PySumTree::update),
      py::arg("index"),
      py::arg("priority"),
      py::arg("value") = py::none());

   sumtree.def(
      "update",
      py::overload_cast<
         const std::vector< size_t >&,
         const std::vector< double >&,
         const std::optional< std::vector< std::optional< PySumTree::value_type > > >& >(
         &PySumTree::update),
      py::arg("index"),
      py::arg("priority"),
      py::arg("value") = py::none());

   sumtree.def("get", &PySumTree::get, py::arg("priority"), py::arg("percentage") = true);

   sumtree.def("priority_iter", [](const PySumTree& tree) {
      return py::make_iterator(tree.priority_begin(), tree.priority_end());
   });

   sumtree.def("value_iter", [](const PySumTree& tree) {
      return py::make_iterator(tree.value_begin(), tree.value_end());
   });
}
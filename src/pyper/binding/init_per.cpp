#include <pybind11/pybind11.h>
#include "per/per.h"

namespace py = pybind11;

void init_per(py::module_ &m) {
   py::class_< PrioritizedExperience > pe(m, "PrioritizedExperience");

   pe.def(
      py::init< size_t, std::mt19937_64::result_type >(),
      py::arg("capacity"),
      py::arg("seed") = std::random_device{}());

   pe.def("push", py::overload_cast< py::object >(&PrioritizedExperience::push), py::arg("value"));

   pe.def(
      "push",
      py::overload_cast< const std::vector< py::object >& >(&PrioritizedExperience::push),
      py::arg("value"));
   pe.def("update", &PrioritizedExperience::update, py::arg("indices"), py::arg("priorities"));

   pe.def("sample", &PrioritizedExperience::sample, py::arg("n"), py::arg("beta"));

   pe.def_property(
      "alpha",
      py::overload_cast<>(&PrioritizedExperience::alpha, py::const_),
      py::overload_cast< double >(&PrioritizedExperience::alpha));

   pe.def_property_readonly("capacity", &PrioritizedExperience::capacity);
}
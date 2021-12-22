#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "per/per.hpp"

namespace py = pybind11;

void init_experience_replay(py::module_& m)
{
   using PyPrioritizedExperience = per::PrioritizedExperience< py::object >;

   py::class_< PyPrioritizedExperience > pe(m, "PrioritizedExperience");

   pe.def(
      py::init< size_t, double, double, std::mt19937_64::result_type >(),
      py::arg("capacity"),
      py::arg("alpha") = 1.,
      py::arg("beta") = 1.,
      py::arg("seed") = std::random_device{}());

   pe.def(
      "push",
      py::overload_cast< PyPrioritizedExperience::value_type >(&PyPrioritizedExperience::push),
      py::arg("value"));

   pe.def(
      "push",
      py::overload_cast< const PyPrioritizedExperience::ValueVec& >(&PyPrioritizedExperience::push),
      py::arg("value"));

   pe.def("update", &PyPrioritizedExperience::update, py::arg("indices"), py::arg("priorities"));

   pe.def("sample", &PyPrioritizedExperience::sample, py::arg("n"));

   pe.def_property(
      "alpha",
      py::overload_cast<>(&PyPrioritizedExperience::alpha, py::const_),
      py::overload_cast< double >(&PyPrioritizedExperience::alpha));

   pe.def_property_readonly("capacity", &PyPrioritizedExperience::capacity);
}

#ifndef PER_MODULE_NAME_HPP
#define PER_MODULE_NAME_HPP

#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_per(py::module_ &);
void init_sumtree(py::module_ &);

PYBIND11_MODULE(_pyper, m)
{
   init_sumtree(m);
   init_per(m);
}

#endif  // PER_MODULE_NAME_HPP

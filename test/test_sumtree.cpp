
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include "gtest/gtest.h"
#include "per/per.h"

TEST(SumTree, All)
{
   // enable usage of py::object subtypes
   py::scoped_interpreter guard{};

   std::map< int, std::tuple< size_t, py::object, double > > expected;
   expected[10] = {5, py::int_(15), 30.};
   expected[20] = {11, py::int_(31), 62.};
   expected[30] = {17, py::int_(47), 94.};
   expected[50] = {29, py::int_(79), 158.};
   expected[100] = {58, py::int_(158), 316.};
   expected[200] = {116, py::int_(316), 632.};
   expected[500] = {290, py::int_(790), 1580.};
   expected[1000] = {581, py::int_(1581), 3162.};

   for(auto [n, results] : expected) {
      SumTree<py::object> tree(n);
      for(int i = 0; i < 2 * n; i++) {
         tree.insert(py::int_(i), 2 * i);
      }
      auto return_value = tree.get(0.5);
      ASSERT_EQ(std::get< 0 >(return_value), std::get< 0 >(results));
      ASSERT_EQ(std::get< 1 >(return_value).cast<int>(), std::get< 1 >(results).cast<int>());
      ASSERT_NEAR(std::get< 2 >(return_value), std::get< 2 >(results), 1e-16);
   }
}

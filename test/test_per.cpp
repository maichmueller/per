

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include "gtest/gtest.h"
#include "per/per.h"

TEST(PrioritizedExperience, reproduciblity)
{
   // enable usage of py::object subtypes
   py::scoped_interpreter guard{};

   std::vector< py::object > values{
      py::object{py::cast(new bool(0))},
      py::object{py::cast(new int(1))},
      py::object{py::cast(new int(2))},
      py::object{py::cast(new int(3))},
      py::object{py::cast(new int(4))},
      py::object{py::cast(std::string("5"))},
      py::object{py::cast(std::string("6"))},
      py::object{py::cast(std::string("7"))},
      py::object{py::cast(std::string("8"))},
      py::object{py::cast(std::string("9"))},
   };

   auto [per1, per2, per3] = std::tuple{
      PrioritizedExperience(10, 0), PrioritizedExperience(10, 0), PrioritizedExperience(10, 1)};

   for(auto& v : values) {
      per1.push(v);
      per2.push(v);
      per3.push(v);
   }
   auto sample1 = per1.sample(5, 1);
   auto sample2 = per2.sample(5, 1);
   auto sample3 = per3.sample(5, 1);

   ASSERT_EQ(sample1, sample2);
   ASSERT_NE(sample1, sample3);
}

TEST(PrioritizedExperience, sample)
{
   // enable usage of py::object subtypes
   py::scoped_interpreter guard{};

   auto per = PrioritizedExperience(10, 0);
   size_t n = 10;
   std::vector< size_t > values;
   std::vector< double > prios(n, 0.);
   for(auto v = 0; v < n; v++) {
      values.emplace_back(v);
      per.push(py::int_(v));
   }
   per.update(values, prios);  // set all priorities to 0 so they are ignored for sampling;
   per.update({5}, {1.});  // set 5th entry to 1 to enable only its sampling

   auto [sample_vs, sample_ps, sample_is] = per.sample(5, 1);

   // the sampling logic sets an already sampled entry's priority to 0 after its draw, so only the
   // first sample should be the prioritized index.
   ASSERT_EQ(py::cast< size_t >(*sample_vs.begin()), 5);
   ASSERT_EQ(*(sample_is.begin()), 5);
   // now remove the sample and check the remaining entries simply always pick the first entry
   sample_vs.erase(sample_vs.begin());
   sample_is.erase(sample_is.begin());
   ASSERT_TRUE(std::all_of(
      sample_vs.begin(), sample_vs.end(), [](auto v) { return py::cast< size_t >(v) == 0; }));
   ASSERT_TRUE(std::all_of(sample_is.begin(), sample_is.end(), [](auto v) { return v == 0; }));
}



#ifndef PER_PER_HPP
#define PER_PER_HPP

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <random>
#include <vector>

#include "sum_tree.h"

class PrioritizedExperience {
  public:
   using ValueVec = std::vector< py::object >;
   using WeightVec = std::vector< double >;
   using IndexVec = std::vector< size_t >;

   PrioritizedExperience(
      size_t capacity,
      std::mt19937_64::result_type seed = std::random_device{}());

   void push(py::object value);
   void push(const std::vector< py::object >& values);
   void update(const std::vector< size_t >& indices, const std::vector< double >& priorities);

   std::tuple< ValueVec, WeightVec, IndexVec > sample(size_t n, double beta);

   void alpha(double alpha);
   [[nodiscard]] double alpha() const { return m_alpha; }
   [[nodiscard]] auto capacity() const { return m_capacity; }

  private:
   size_t m_capacity;
   double m_alpha;
   std::mt19937_64 m_rng;

   SumTree m_sumtree;

   double m_max_priority = 1.;

   void _recompute_max_priority(double  triggering_prio);
   void _recompute_max_priority();
};

#endif  // PER_PER_HPP

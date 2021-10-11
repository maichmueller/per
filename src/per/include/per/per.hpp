

#ifndef PER_PER_HPP
#define PER_PER_HPP

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <random>
#include <vector>

#include "sum_tree.hpp"

class PrioritizedExperience {
  public:
   using SumTreeType = SumTree< std::pair< py::object, double > >;
   using value_type = typename SumTreeType::value_type;
   using ValueVec = std::vector< py::object >;
   using WeightVec = std::vector< double >;
   using IndexVec = std::vector< size_t >;

   PrioritizedExperience(
      size_t capacity,
      double alpha = 1.,
      double beta = 1.,
      std::mt19937_64::result_type seed = std::random_device{}());

   void push(py::object value);
   void push(const std::vector< py::object >& values);
   void update(const std::vector< size_t >& indices, const std::vector< double >& priorities);

   std::tuple< ValueVec, WeightVec, IndexVec > sample(size_t n);

   void beta(double beta);
   void alpha(double alpha);
   [[nodiscard]] double alpha() const { return m_alpha; }
   [[nodiscard]] double beta() const { return m_beta; }
   [[nodiscard]] auto capacity() const { return m_capacity; }

  private:
   size_t m_capacity;
   double m_alpha;
   double m_beta = 1.;
   std::mt19937_64 m_rng;

   double m_max_priority = 1.;
   double m_max_weight = 1.;

   SumTreeType m_sumtree;

   void _recompute_max_priority(std::optional< double > triggering_prio = std::nullopt);
   void _recompute_max_weight(std::optional< double > triggering_wegith = std::nullopt);
};

#endif  // PER_PER_HPP


#ifndef PER_EXPERIENCE_REPLAY_HPP
#define PER_EXPERIENCE_REPLAY_HPP

#include <pybind11/pybind11.h>

#include <random>
#include <vector>

#include "per/macro.hpp"
#include "per/sum_tree.hpp"

/**
 * Prioritized Experience Algorithm Buffer as defined in \cite{per}.
 *
 * This class uses a Sum Tree structure to store data with an associated priority and allows to
 * sample the data according to the distribution:
 *
 *      \f$ \mathbb{P}(i) = \frac{\text{prio}_i^\alpha}{\sum_k \text{prio}_k^\alpha} \f$
 *
 * The parameter \f$\alpha\f$ controls the degree of uniformity within the distribution and can be
 * set in the class's constructor. It can be changed later on as well at a computational cost. If
 * more data than the buffer's capacity is emplaced, the buffer will overwrite the oldest entries
 * with the new data points.
 */
class PER_API PrioritizedExperience {
  public:
   using SumTreeType = SumTree< std::pair< py::object, double > >;
   using value_type = typename SumTreeType::value_type;
   using ValueVec = std::vector< py::object >;
   using WeightVec = std::vector< double >;
   using IndexVec = std::vector< size_t >;

   /**
    * The constructor of a PER buffer.
    *
    * @param capacity the maximum numbers of samples to be held at any point in time.
    * @param alpha
    * @param beta
    * @param seed
    */
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

#endif  // PER_EXPERIENCE_REPLAY_HPP

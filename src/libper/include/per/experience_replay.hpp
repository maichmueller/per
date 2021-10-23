
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
 * This class uses a Sum Tree structure to store samples with an associated priority and allows to
 * draw the samples according to the distribution:
 *
 *      \f$ \mathbb{P}(i) = \frac{\text{prio}_i^\alpha}{\sum_k \text{prio}_k^\alpha} \f$
 *
 * The parameter \f$\alpha\f$ controls the degree of uniformity within the distribution and can be
 * set in the class's constructor. It can be changed later on as well at a computational cost.
 * Each sample \f$ i \f$ holds an associated weight \f$ w_i \f$ of the form
 *
 *      \f$ w_i = ( N \cdot \mathbb{P}(i) )^{-\beta}
 *
 * with \f$ N \f$ being the capacity. \f$ \beta \f$ behaves in the same way for the weights as \f$
 * \alpha \f$ does for the priorities. The weights should be scaled by \f$ w_\max \f$ when used in
 * update rules.
 * The buffer will overwrite the oldest entries to make room for new data points if the capacity is
 * exhausted.
 */
class PER_API PrioritizedExperience {
  public:
   /// the sum tree is supposed to hold data entries (py::object, double) \f$= (d_i, w_i) \f$ with
   /// \f$ d_i \f$ being the python object and \f$ w_i \f$ being the associated weight
   using SumTreeType = SumTree< std::pair< py::object, double > >;
   using value_type = typename SumTreeType::value_type;
   using ValueVec = std::vector< py::object >;
   using WeightVec = std::vector< double >;
   using IndexVec = std::vector< size_t >;

   /**
    * The constructor of a PER buffer.
    *
    * @param capacity the maximum numbers of samples to be held at any point in time.
    * @param alpha the degree of uniformity in the distribution \f$ p_i^\alpha \f$.
    * @param beta the 'temperature' paramter for the weights.
    * @param seed the random seed for sampling.
    */
   PrioritizedExperience(
      size_t capacity,
      double alpha = 1.,
      double beta = 1.,
      std::mt19937_64::result_type seed = std::random_device{}());

   /**
    * Add a sample to the buffer.
    * @param value the sample to add.
    */
   void push(py::object value);
   /**
    * Add a collection of samples to the buffer.
    * @param values the vector of samples to add.
    */
   void push(const std::vector< py::object >& values);
   /**
    * Update the given sample indices with new priorties.
    *
    * This is needed within the sampling rule itself, in order to mask already drawn samples. But it
    * may also be used externally to overwrite previous priorities (the effect of this would result
    * in priorities mathematically not in line with the PER method).
    * @param indices the vector of indices to address.
    * @param priorities the vector of priorities to emplace.
    * The entries of @p indices and @p priorities are paired.
    */
   void update(const std::vector< size_t >& indices, const std::vector< double >& priorities);

   /**
    * Sample @p n samples from the buffer according to the PER method.
    * @param n the nubmer ofsamples to draw.
    * @return a tuple of 3 vectors holding the values, weights, and indices respectively (in this
    * order). The entries of these return vectors are linked, i.e. for drawn sample \f$ i \f$ the
    * respective value, weight, and index is found in v[i], w[i], ind[i].
    */
   std::tuple< ValueVec, WeightVec, IndexVec > sample(size_t n);

   /**
    * Setter for \f$ \beta \f$.
    * @param beta the new value.
    */
   void beta(double beta);
   /**
    * Setter for \f$ \alpha \f$
    * @param alpha the new value.
    */
   void alpha(double alpha);
   /**
    * Getter for \f$ \alpha \f$
    * @return \f$ \alpha \f$.
    */
   [[nodiscard]] double alpha() const { return m_alpha; }
   /**
    * Getter for \f$ \beta \f$.
    * @return \f$ \beta \f$.
    */
   [[nodiscard]] double beta() const { return m_beta; }
   /**
    * Getter for the capacity.
    * @return the capacity.
    */
   [[nodiscard]] auto capacity() const { return m_capacity; }

  private:
   /// the buffer maximum number of samples to hold
   size_t m_capacity;
   /// the temperature parameter for the priority probabilities
   double m_alpha;
   /// the temperature parameter for the weights
   double m_beta = 1.;
   /// the random number generator for sampling
   std::mt19937_64 m_rng;
   /// the current max priority stored
   double m_max_priority = 1.;
   /// the current max weight stored
   double m_max_weight = 1.;
   /// the sum tree structure holing the samples with associated priority and updating them
   /// accordingly. This is computationally faster than a simple array of (samples, priorites).
   SumTreeType m_sumtree;

   void _recompute_max_priority(std::optional< double > triggering_prio = std::nullopt);
   void _recompute_max_weight(std::optional< double > triggering_wegith = std::nullopt);
};

#endif  // PER_EXPERIENCE_REPLAY_HPP

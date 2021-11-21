
#ifndef PER_EXPERIENCE_REPLAY_HPP
#define PER_EXPERIENCE_REPLAY_HPP

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
template < typename ValueType >
class PER_API PrioritizedExperience {
  public:
   /// the sum tree is supposed to hold data entries (value_type, double) \f$= (d_i, w_i) \f$ with
   /// \f$ d_i \f$ being the data object and \f$ w_i \f$ being the associated weight
   using SumTreeType = SumTree< std::pair< ValueType, double > >;
   using value_type = ValueType;
   using tree_value_type = typename SumTreeType::value_type;
   using ValueVec = std::vector< value_type >;
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
   void push(value_type value);
   /**
    * Add a collection of samples to the buffer.
    * @param values the vector of samples to add.
    */
   void push(const std::vector< value_type > &values);
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
   void update(const std::vector< size_t > &indices, const std::vector< double > &priorities);

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

template < typename ValueType >
void PrioritizedExperience< ValueType >::_recompute_max_weight(
   std::optional< double > triggering_weight)
{
   if(not triggering_weight.has_value()
      or std::abs(triggering_weight.value() - m_max_weight) >= 1e-16) {
      return;
   }
   m_max_weight = std::max_element(
                     m_sumtree.value_begin(),
                     m_sumtree.value_end(),
                     [](const tree_value_type &value1, const tree_value_type &value2) {
                        return value1.second < value2.second;
                     })
                     ->second;
}

template < typename ValueType >
void PrioritizedExperience< ValueType >::_recompute_max_priority(
   std::optional< double > triggering_prio)
{
   if(not triggering_prio.has_value()
      or std::abs(triggering_prio.value() - m_max_priority) >= 1e-16) {
      return;
   }
   m_max_weight = *std::max_element(m_sumtree.priority_begin(), m_sumtree.priority_end());
}

template < typename ValueType >
void PrioritizedExperience< ValueType >::push(PrioritizedExperience::value_type value)
{
   auto deleted_entry = m_sumtree.insert(
      tree_value_type{
         /*value=*/std::move(value),
         /*weight=*/std::pow(
            m_max_priority / m_sumtree.total() * static_cast< double >(m_capacity), m_beta)},
      m_max_priority);

   if(deleted_entry.has_value()) {
      auto &deleted_entry_value = deleted_entry.value();
      _recompute_max_weight(std::get< 0 >(deleted_entry_value).second);
      _recompute_max_priority(std::get< 1 >(deleted_entry_value));
   }
}

template < typename ValueType >
void PrioritizedExperience< ValueType >::push(
   const std::vector< PrioritizedExperience::value_type > &values)
{
   for(const auto &value : values) {
      push(value);
   }
}

template < typename ValueType >
void PrioritizedExperience< ValueType >::update(
   const std::vector< size_t > &indices,
   const std::vector< double > &priorities)
{
   for(size_t i = 0; i < indices.size(); i++) {
      if(i >= m_capacity) {
         throw std::out_of_range(
            "Index '" + std::to_string(i) + "' out of bounds for replay capacity "
            + std::to_string(m_capacity));
      }
      m_sumtree.update(indices[i], std::pow(std::abs(priorities[i]), m_alpha));
   }
}

template < typename ValueType >
std::tuple<
   typename PrioritizedExperience< ValueType >::ValueVec,
   typename PrioritizedExperience< ValueType >::WeightVec,
   typename PrioritizedExperience< ValueType >::IndexVec >
PrioritizedExperience< ValueType >::sample(size_t n)
{
   ValueVec values;
   WeightVec weights;
   IndexVec indices;

   // backup container for the priorities of already sampled elements
   // (sample without replacement)
   std::vector< double > priorities;

   auto n_samples = std::min(n, m_sumtree.size());
   values.reserve(n_samples);
   weights.reserve(n_samples);
   indices.reserve(n_samples);
   priorities.reserve(n_samples);

   std::uniform_real_distribution< double > dist(0, 1);

   for(size_t i = 0; i < n_samples; i++) {
      auto r = dist(m_rng);
      auto [index, value, priority] = m_sumtree.get(r);
      priorities.emplace_back(priority);
      weights.emplace_back(value.second);
      indices.emplace_back(index);
      values.emplace_back(value.first);
      // mask the already sampled elements
      m_sumtree.update(index, 0);
   }
   // restore the priorities
   m_sumtree.update(indices, priorities);

   return {values, weights, indices};
}
template < typename ValueType >
void PrioritizedExperience< ValueType >::alpha(double alpha)
{
   double old_alpha = m_alpha;
   m_alpha = alpha;
   for(size_t i = 0; i < m_sumtree.size(); i++) {
      // we have always stored priority^alpha. So in order to change the exponent to the new
      // alpha we need to exponentiate the stored priority by the fraction of new/old alpha:
      //    (p^(a_1))^(a_2 / a_1) = p^(a_2)
      m_sumtree.update(i, std::pow(m_sumtree.priority(i), alpha / old_alpha));
   }
}
template < typename ValueType >
void PrioritizedExperience< ValueType >::beta(double beta)
{
   double old_beta = m_beta;
   m_beta = beta;
   for(size_t i = 0; i < m_sumtree.size(); i++) {
      auto &[value, weight] = m_sumtree[i];
      m_sumtree[i] = {
         value, 1. / (std::pow(weight * m_max_weight, m_beta / old_beta) * m_max_weight)};
   }
}
template < typename ValueType >
PrioritizedExperience< ValueType >::PrioritizedExperience(
   size_t capacity,
   double alpha,
   double beta,
   std::mt19937_64::result_type seed)
    : m_capacity(capacity), m_alpha(alpha), m_beta(beta), m_rng(seed), m_sumtree(capacity)
{
}

#endif  // PER_EXPERIENCE_REPLAY_HPP

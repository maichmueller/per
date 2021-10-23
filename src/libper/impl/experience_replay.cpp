
#include <utility>

#include "per/per.hpp"

void PrioritizedExperience::_recompute_max_weight(std::optional< double > triggering_weight)
{
   if(not triggering_weight.has_value()
      or std::abs(triggering_weight.value() - m_max_weight) >= 1e-16) {
      return;
   }
   m_max_weight = std::max_element(
                     m_sumtree.value_begin(),
                     m_sumtree.value_end(),
                     [](const value_type &value1, const value_type &value2) {
                        return value1.second < value2.second;
                     })
                     ->second;
}

void PrioritizedExperience::_recompute_max_priority(std::optional< double > triggering_prio)
{
   if(not triggering_prio.has_value()
      or std::abs(triggering_prio.value() - m_max_priority) >= 1e-16) {
      return;
   }
   m_max_weight = *std::max_element(m_sumtree.priority_begin(), m_sumtree.priority_end());
}

void PrioritizedExperience::push(py::object value)
{
   auto deleted_entry = m_sumtree.insert(
      value_type{
         std::move(value),
         std::pow(m_max_priority / m_sumtree.total() * static_cast< double >(m_capacity), m_beta)},
      m_max_priority);

   if(deleted_entry.has_value()) {
      auto &deleted_entry_value = deleted_entry.value();
      _recompute_max_weight(std::get< 0 >(deleted_entry_value).second);
      _recompute_max_priority(std::get< 1 >(deleted_entry_value));
   }
}

void PrioritizedExperience::push(const std::vector< py::object > &values)
{
   for(const auto &value : values) {
      push(value);
   }
}

void PrioritizedExperience::update(
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

std::tuple<
   PrioritizedExperience::ValueVec,
   PrioritizedExperience::WeightVec,
   PrioritizedExperience::IndexVec >
PrioritizedExperience::sample(size_t n)
{
   ValueVec values;
   WeightVec weights;
   IndexVec indices;

   // backup container for the priorities of already sampled elements
   // (sample without replacement)
   std::vector< double > priorities;

   auto n_samples = std::min(n, m_sumtree.size());

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

void PrioritizedExperience::alpha(double alpha)
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

void PrioritizedExperience::beta(double beta)
{
   double old_beta = m_beta;
   m_beta = beta;
   for(size_t i = 0; i < m_sumtree.size(); i++) {
      auto &[value, weight] = m_sumtree[i];
      m_sumtree[i] = {
         value, 1. / (std::pow(weight * m_max_weight, m_beta / old_beta) * m_max_weight)};
   }
}

PrioritizedExperience::PrioritizedExperience(
   size_t capacity,
   double alpha,
   double beta,
   std::mt19937_64::result_type seed)
    : m_capacity(capacity), m_alpha(alpha), m_beta(beta), m_rng(seed), m_sumtree(capacity)
{
}

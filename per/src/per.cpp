
#include "per/per.h"

void PriorityExperience::push(py::object value)
{
   m_sumtree.insert(value, std::numeric_limits< double >::infinity());
}

void PriorityExperience::push(const std::vector< py::object > &values)
{
   for(size_t i = 0; i < values.size(); i++) {
      push(values[i]);
   }
}

void PriorityExperience::update(
   const std::vector< size_t > &indices,
   const std::vector< double > &priorities)
{
   for(size_t i = 0; i < indices.size(); i++) {
      m_sumtree.update(indices[i], std::pow(priorities[i], m_alpha));
   }
}

std::tuple<
   PriorityExperience::ValueVec,
   PriorityExperience::WeightVec,
   PriorityExperience::IndexVec >
PriorityExperience::sample(size_t n, double beta)
{
   ValueVec values;
   WeightVec weights;
   IndexVec indices;

   // backup container for the priorities of already sampled elements (sample without replacement)
   std::vector< double > priorities;

   auto n_samples = std::min(n, m_sumtree.size());

   std::uniform_real_distribution< double > dist(0, 1);

   double total_prio = m_sumtree.total();
   double weight_max = 0;
   auto size = m_sumtree.size();
   for(size_t i = 0; i < n_samples; i++) {
      auto r = dist(m_rng);
      auto [index, value, priority] = m_sumtree.get(r);
      priorities.emplace_back(priority);
      double weight = priority > 1e-16 ? std::pow(1. / (size * priority / total_prio), beta) : 0;
      weight_max = weight_max < weight ? weight : weight_max;
      weights.emplace_back(weight);
      indices.emplace_back(index);
      values.emplace_back(value);
      // mask the already sampled elements
      m_sumtree.update(index, 0);
   }
   // restore the priorities
   m_sumtree.update(indices, priorities);
   for(auto &w : weights) {
      w /= weight_max;
   }

   return {values, weights, indices};
}

void PriorityExperience::alpha(double alpha)
{
   double old_alpha = m_alpha;
   m_alpha = alpha;
   for(size_t i = 0; i < m_sumtree.size(); i++) {
      // we have always stored priority^alpha. So in order to change the exponent to the new alpha
      // we need to exponentiate the stored priority by the fraction of new/old alpha:
      //    (p^(a_1))^(a_2 / a_1) = p^(a_2)
      m_sumtree.update(i, std::pow(m_sumtree.priority(i), alpha / old_alpha));
   }
}
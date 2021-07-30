
#include "per/sum_tree.h"

#include <cmath>
#include <utility>

SumTree::SumTree(size_t capacity)
    : m_capacity(capacity),
      m_leaf_level(std::ceil(std::log2(capacity) + 1)),
      m_prioritree(std::exp2(m_leaf_level) - 1, 0),
      m_values(capacity, py::none{})
{
}

std::optional< std::tuple< py::object, double > > SumTree::insert(py::object value, double priority)
{
   std::optional< std::tuple< py::object, double > > old_pair = std::nullopt;
   if(m_size == m_capacity) {
      old_pair = {m_values[m_leaf_pos], *(priority_begin() + m_leaf_pos)};
   }
   m_values[m_leaf_pos] = std::move(value);
   update(m_leaf_pos, priority);

   m_leaf_pos = (m_leaf_pos + 1) % m_capacity;
   m_size = std::min(m_size + 1, m_capacity);
   return old_pair;
}

void SumTree::update(size_t index, double priority)
{
   index += _first_index_at_level(m_leaf_level);
   double delta = priority - m_prioritree[index];
   m_prioritree[index] = priority;
   while(index > 0) {
      index = (index - 1) / 2;
      m_prioritree[index] += delta;
   }
}

void SumTree::update(const std::vector< size_t >& index, const std::vector< double >& priority)
{
   _assert_length_eq(index, priority);
   for(size_t i = 0; i < index.size(); i++) {
      if(i < 0 || i >= m_size) {
         throw std::out_of_range("Index '" + std::to_string(i) + "' out of bounds.");
      }
      update(index[i], priority[i]);
   }
};

double SumTree::priority(size_t index)
{
   return m_prioritree[std::exp2(m_leaf_level - 1) - 1 + index];
}

auto SumTree::get(double priority, bool percentage) -> std::tuple< size_t, py::object, double >
{
   if(percentage) {
      priority *= m_prioritree[0];
   }
   size_t index = 0;
   size_t breaking_index = std::exp2(m_leaf_level - 1) - 1;
   while(true) {
      size_t left_idx = 2 * index + 1;
      if(priority <= m_prioritree[left_idx]) {
         index = left_idx;
      } else {
         index = left_idx + 1;  // the right child's index
         priority -= m_prioritree[left_idx];
      }
      // breaking condition
      if(long long value_idx = static_cast< long long >(index)
                               - static_cast< long long >(breaking_index);
         value_idx >= 0) {
         return {value_idx, m_values[value_idx], m_prioritree[index]};
      }
   }
}

std::string SumTree::as_str()
{
   std::vector< double > prios;
   // stores the indices at which to break for the next level segment. Essentially stores the 2-dim
   // info of the prio vector. Each element is the index at which the counter counts up one
   // dimension up, e.g. from [3, 21] to [4,0] if the 3rd element in `shape_vec` is 22
   std::vector< size_t > shape_vec;
   prios.reserve(m_capacity);
   size_t level = 1;
   size_t curr_elems = std::exp2(level - 1);
   for(size_t i = 0; i < m_prioritree.size(); i++) {
      prios.emplace_back(m_prioritree[i]);
      if(i + 1 == curr_elems) {
         shape_vec.emplace_back(prios.size());
         level++;
         curr_elems = std::exp2(level - 1);
      }
   }

   auto curr_break = shape_vec.begin();
   size_t idx = 0;
   std::stringstream ss;
   for(auto p : prios) {
      ss << std::to_string(p);
      if(idx + 1 == *curr_break) {
         // next element belongs to a new row, so add a break
         ss << "\n";
         curr_break++;
      } else {
         ss << " ";
      }
   }
   return ss.str();
}

std::vector< double >::const_iterator SumTree::priority_begin() const
{
   return m_prioritree.begin() + _first_index_at_level(m_leaf_level);
}

std::vector< double >::const_iterator SumTree::priority_end() const
{
   return m_prioritree.begin() + _first_index_at_level(m_leaf_level) + m_size;
}


#ifndef PER_SUM_TREE_HPP
#define PER_SUM_TREE_HPP

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <optional>

#include "per/macro.hpp"

namespace py = pybind11;

template < typename ValueType >
class PER_PUBLIC SumTree {
  public:
   using value_type = ValueType;

   SumTree(size_t capacity);

   [[nodiscard]] inline size_t len() const { return m_size; }
   [[nodiscard]] inline double total() const { return m_prioritree[0]; }
   [[nodiscard]] inline size_t size() const { return m_size; }
   [[nodiscard]] inline size_t curr_pos() const { return m_leaf_pos; }

   std::optional< std::tuple< value_type, double > > insert(value_type value, double priority);
   void update(size_t index, double priority, std::optional< value_type > value_opt = std::nullopt);
   void update(
      const std::vector< size_t >& index,
      const std::vector< double >& priority,
      const std::optional< std::vector< std::optional< value_type > > >& value = std::nullopt);

   std::tuple< size_t, value_type, double > get(double priority, bool percentage = true);
   double priority(size_t index);
   value_type& operator[](size_t index) { return m_values[index]; }
   const std::vector< value_type >& values() const { return &m_values; }

   [[nodiscard]] std::vector< double >::const_iterator priority_begin() const;
   [[nodiscard]] std::vector< double >::const_iterator priority_end() const;
   [[nodiscard]] typename std::vector< value_type >::const_iterator value_begin() const
   {
      return m_values.begin();
   }
   [[nodiscard]] typename std::vector< value_type >::const_iterator value_end() const
   {
      return m_values.end();
   }

   std::string as_str();

  private:
   size_t m_capacity;
   size_t m_size = 0;

   size_t m_leaf_pos = 0;
   size_t m_leaf_level;

   std::vector< double > m_prioritree;
   std::vector< value_type > m_values;

   [[nodiscard]] size_t _first_index_at_level(size_t level) const
   {
      return static_cast< size_t >(std::exp2(level - 1)) - 1;
   }

   inline void _assert_value_index(size_t index)
   {
      if(index >= m_size) {
         throw std::out_of_range("Index '" + std::to_string(index) + "' out of bounds.");
      }
   }

   template < typename T1, typename T2, typename Allocator1, typename Allocator2 >
   void _assert_length_eq(
      const std::vector< T1, Allocator1 >& values,
      const std::vector< T2, Allocator2 >& priorities);
};

// IMPLEMENTATION

#include <cmath>
#include <utility>

template < typename ValueType >
template < typename T1, typename T2, typename Allocator1, typename Allocator2 >
void SumTree< ValueType >::_assert_length_eq(
   const std::vector< T1, Allocator1 >& values,
   const std::vector< T2, Allocator2 >& priorities)
{
   if(values.size() != priorities.size()) {
      throw std::invalid_argument("Value sequence and priority sequence do not match in length.");
   }
}

template < typename ValueType >
SumTree< ValueType >::SumTree(size_t capacity)
    : m_capacity(capacity),
      m_leaf_level(static_cast< size_t >(std::ceil(std::log2(capacity) + 1))),
      m_prioritree(static_cast< size_t >(std::exp2(m_leaf_level)) - 1, 0),
      m_values(capacity)
{
}

template < typename ValueType >
std::optional< std::tuple< ValueType, double > > SumTree< ValueType >::insert(
   ValueType value,
   double priority)
{
   std::optional< std::tuple< ValueType, double > > old_pair = std::nullopt;
   using iter_diff_t = typename decltype(m_prioritree)::difference_type;
   if(m_size == m_capacity) {
      old_pair = {
         m_values[m_leaf_pos], *(priority_begin() + static_cast< iter_diff_t >(m_leaf_pos))};
   }
   m_size = std::min(m_size + 1, m_capacity);
   update(m_leaf_pos, priority, std::move(value));

   m_leaf_pos = (m_leaf_pos + 1) % m_capacity;
   return old_pair;
}

template < typename ValueType >
void SumTree< ValueType >::update(
   size_t index,
   double priority,
   std::optional< ValueType > value_opt)
{
   _assert_value_index(index);
   if(value_opt.has_value()) {
      m_values[index] = std::move(value_opt.value());
   }
   index += _first_index_at_level(m_leaf_level);
   double delta = priority - m_prioritree[index];
   m_prioritree[index] = priority;
   while(index > 0) {
      index = (index - 1) / 2;
      m_prioritree[index] += delta;
   }
}

template < typename ValueType >
void SumTree< ValueType >::update(
   const std::vector< size_t >& index,
   const std::vector< double >& priority,
   const std::optional< std::vector< std::optional< ValueType > > >& value)
{
   _assert_length_eq(index, priority);
   std::function value_getter = [&](size_t /*index*/) -> std::optional< ValueType > {
      return std::nullopt;
   };
   if(value.has_value()) {
      _assert_length_eq(index, value.value());
      value_getter = [value_vec = value.value()](size_t idx) { return value_vec[idx]; };
   }
   for(size_t i = 0; i < index.size(); i++) {
      _assert_value_index(i);
      update(index[i], priority[i], value_getter(i));
   }
}

template < typename ValueType >
double SumTree< ValueType >::priority(size_t index)
{
   _assert_value_index(index);
   return m_prioritree[_first_index_at_level(m_leaf_level) + index];
}

template < typename ValueType >
auto SumTree< ValueType >::get(double priority, bool percentage)
   -> std::tuple< size_t, ValueType, double >
{
   if(percentage) {
      priority *= m_prioritree[0];
   }
   size_t index = 0;
   auto breaking_index = static_cast< size_t >(std::exp2(m_leaf_level - 1) - 1);
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
         return {value_idx, m_values[static_cast< size_t >(value_idx)], m_prioritree[index]};
      }
   }
}

template < typename ValueType >
std::string SumTree< ValueType >::as_str()
{
   std::vector< double > prios;
   // stores the indices at which to break for the next level segment. Essentially stores the 2-dim
   // info of the prio vector. Each element is the index at which the counter counts up one
   // dimension up, e.g. from [3, 21] to [4,0] if the 3rd element in `shape_vec` is 22
   std::vector< size_t > shape_vec;
   prios.reserve(m_capacity);
   size_t level = 1;
   auto curr_elems = static_cast< size_t >(std::exp2(level - 1));
   for(size_t i = 0; i < m_prioritree.size(); i++) {
      prios.emplace_back(m_prioritree[i]);
      if(i + 1 == curr_elems) {
         shape_vec.emplace_back(prios.size());
         level++;
         curr_elems = static_cast< size_t >(std::exp2(level - 1));
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

template < typename ValueType >
std::vector< double >::const_iterator SumTree< ValueType >::priority_begin() const
{
   using iter_diff_t = typename decltype(m_prioritree)::difference_type;
   return m_prioritree.begin() + static_cast< iter_diff_t >(_first_index_at_level(m_leaf_level));
}

template < typename ValueType >
std::vector< double >::const_iterator SumTree< ValueType >::priority_end() const
{
   using iter_diff_t = typename decltype(m_prioritree)::difference_type;
   return m_prioritree.begin()
          + static_cast< iter_diff_t >(_first_index_at_level(m_leaf_level) + m_size);
}

#endif  // PER_SUM_TREE_HPP

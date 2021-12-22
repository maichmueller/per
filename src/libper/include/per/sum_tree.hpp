
#ifndef PER_SUM_TREE_HPP
#define PER_SUM_TREE_HPP

#include <optional>
#include <sstream>

#include "per/macro.hpp"

namespace per {

/**
 * Class providing a sum-tree structure for data of differing priority.
 *
 * Each element is essentially stored in the leaves of the tree.
 * From there the tree spans its upper nodes from the leaves by summing up the priorities of two
 * adjacent nodes \f$ S = p_i + p_{i+1} \f$  exclusively (i.e. nodes only combine with left OR right
 * adjacent neighbour) and forms a new node of priority \f$ S \f$ connecting to the two leaves.
 * Repeating until the root spans the tree.
 *
 * @tparam ValueType the data type to hold. The ValueType must be movable.
 */
template < typename ValueType >
class SumTree {
  public:
   // Every sample entered into the buffer is copied (as is done for e.g. std::vector). Within the
   // buffer the sample may be moved (e.g. when updated).
   static_assert(
      ::std::is_copy_constructible_v< ValueType > and ::std::is_move_assignable_v< ValueType >,
      "The ValueType of the SumTree must be copy constructible and move assignable.");

   using value_type = ValueType;

   /**
    * The constructor.
    *
    * Defaults all values but the capacity which is provided.
    * @param capacity the maximum nr of samples to hold.
    */
   SumTree(size_t capacity);

   /**
    * Getter for the total sum priority.
    * @return the root's priority.
    */
   [[nodiscard]] inline double total() const { return m_prioritree[0]; }

   /**
    * Getter for the number of currently contained elements.
    * @return the size of the tree.
    */
   [[nodiscard]] inline size_t size() const { return m_size; }

   /**
    * Insert an element into the tree together with its priority.
    * @param value the element to emplace.
    * @param priority the element's associated priority.
    * @return the optional element that had to be overwritten if the capacity was exceeded.
    * Otherwise returns an empty optinal.
    */
   ::std::optional< ::std::tuple< value_type, double > > insert(value_type value, double priority);
   /**
    * Update the value at the given index with the provided priority.
    *
    * If a new value is provided as well, then the value itself is overwritten this new value.
    * @param index the index of the value to update.
    * @param priority the priority to update.
    * @param value_opt an optional new value to place at this index.
    */
   void update(size_t index, double priority, ::std::optional< value_type > value_opt = ::std::nullopt);
   /**
    * Update a collection of values at given indices with the provided priorities.
    *
    * The method calls the single value function is iteration.
    * @param index vector of value indices to update.
    * @param priority vector of priorities to update with .
    * @param value optional vector of optional new values to emplace at these indices.
    */
   void update(
      const ::std::vector< size_t >& index,
      const ::std::vector< double >& priority,
      const ::std::optional< ::std::vector< ::std::optional< value_type > > >& value = ::std::nullopt);

   /**
    * Get the tuple (element's leaf index, element, priority) pertaining to a given priority.
    *
    * This method traverses the tree from root to an appropriate leaf element. The algorithm always
    * checks if the current available priority is less than the left child's priority and enters
    * that subtree. If it isn't it subtracts the left child's priority from the remaining priority
    * and continues the search in the right subtree. The search ends once a leaf was found.
    *
    * @param priority the starting priority to serch for.
    * @param percentage boolean switch to indicate whether the priority is relative to the total
    * priority or absolute.
    * @return a tuple of leaf index, element, element's priority.
    */
   ::std::tuple< size_t, value_type, double > get(double priority, bool percentage = true);
   double priority(size_t index);

   /**
    * Access the leaf element at the given index.
    *
    * This method only counts the leaf indices, thereby leaving out all nodes in a higher level of
    * the tree. The index 0 hence refers to the first leaf node.
    * @param index the index of the leaf.
    * @return the value present at this index.
    */
   value_type& operator[](size_t index) { return m_values[index]; }
   /**
    * Getter of the entire value vector.
    * @return a const reference to the values collection.
    */
   const ::std::vector< value_type >& values() const { return &m_values; }
   /**
    * Begin iterator for the priorities collection.
    * @return the iterator pointing at the start of the priorities.
    */
   [[nodiscard]] ::std::vector< double >::const_iterator priority_begin() const;
   /**
    * End iterator for the priorities collection.
    * @return the iterator pointing at the end of the priorities.
    */
   [[nodiscard]] ::std::vector< double >::const_iterator priority_end() const;
   /**
    * Begin iterator for the values collection.
    * @return the iterator pointing at the start of the values.
    */
   [[nodiscard]] typename ::std::vector< value_type >::const_iterator value_begin() const
   {
      return m_values.begin();
   }
   /**
    * End iterator for the values collection.
    * @return the iterator pointing at the end of the values.
    */
   [[nodiscard]] typename ::std::vector< value_type >::const_iterator value_end() const
   {
      return m_values.end();
   }
   /**
    * Print the priority tree as a string representation.
    * @return the tree as string
    */
   [[nodiscard]] ::std::string as_str() const;

  private:
   /// the maximum number of elements to store at any time.
   size_t m_capacity;
   /// the current number of elements stored in the tree.
   size_t m_size = 0;
   /// the current position of the cursor on leaf level
   size_t m_leaf_pos = 0;
   /// the level at which the leaves are (starts counting from 1)
   size_t m_leaf_level;
   /// the priority tree collection
   ::std::vector< double > m_prioritree;
   /// the value collection
   ::std::vector< value_type > m_values;

   /**
    * Get the first index pertaining to a given level.
    * @param level the level in question (starts counting from 1).
    * @return the first index of this level
    */
   [[nodiscard]] size_t _first_index_at_level(size_t level) const
   {
      return static_cast< size_t >(::std::exp2(level - 1)) - 1;
   }
   /**
    * Check if the index lies within the bounds of the values collection.
    * @param index the index to check.
    * @throw ::std::out_of_range exception if the index is not whithin value collection bounds
    */
   inline void _assert_index_in_range(size_t index)
   {
      if(index >= m_size) {
         throw ::std::out_of_range("Index '" + ::std::to_string(index) + "' out of bounds.");
      }
   }
   /**
    * Get the current position of the leaf cursor.
    * @return the leaf cursor position.
    */
   [[nodiscard]] inline size_t _curr_pos() const { return m_leaf_pos; }

   /**
    * Check if two vectors of potentially differing types are of the same length.
    *
    * Intended for a check of value and priority collection.
    * @tparam T1 the type of the first vector.
    * @tparam T2 the type of the second vector.
    * @tparam Allocator1 allocator type of the first vector.
    * @tparam Allocator2 allocator type of the second vector.
    * @param values the value collection.
    * @param priorities the priority collection.
    */
   template < typename T1, typename T2, typename Allocator1, typename Allocator2 >
   void _assert_length_eq(
      const ::std::vector< T1, Allocator1 >& values,
      const ::std::vector< T2, Allocator2 >& priorities);
};

// IMPLEMENTATION

#include <cmath>
#include <sstream>
#include <utility>

template < typename ValueType >
template < typename T1, typename T2, typename Allocator1, typename Allocator2 >
void SumTree< ValueType >::_assert_length_eq(
   const ::std::vector< T1, Allocator1 >& values,
   const ::std::vector< T2, Allocator2 >& priorities)
{
   if(values.size() != priorities.size()) {
      throw ::std::invalid_argument("Value sequence and priority sequence do not match in length.");
   }
}

template < typename ValueType >
SumTree< ValueType >::SumTree(size_t capacity)
    : m_capacity(capacity),
      m_leaf_level(static_cast< size_t >(::std::ceil(::std::log2(capacity) + 1))),
      m_prioritree(static_cast< size_t >(::std::exp2(m_leaf_level)) - 1, 0),
      m_values(capacity)
{
}

template < typename ValueType >
::std::optional< ::std::tuple< ValueType, double > > SumTree< ValueType >::insert(
   ValueType value,
   double priority)
{
   ::std::optional< ::std::tuple< ValueType, double > > old_pair = ::std::nullopt;
   using iter_diff_t = typename decltype(m_prioritree)::difference_type;
   if(m_size == m_capacity) {
      old_pair = {
         m_values[m_leaf_pos], *(priority_begin() + static_cast< iter_diff_t >(m_leaf_pos))};
   }
   m_size = ::std::min(m_size + 1, m_capacity);
   update(m_leaf_pos, priority, ::std::move(value));

   m_leaf_pos = (m_leaf_pos + 1) % m_capacity;
   return old_pair;
}

template < typename ValueType >
void SumTree< ValueType >::update(
   size_t index,
   double priority,
   ::std::optional< ValueType > value_opt)
{
   _assert_index_in_range(index);
   if(value_opt.has_value()) {
      m_values[index] = ::std::move(value_opt.value());
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
   const ::std::vector< size_t >& index,
   const ::std::vector< double >& priority,
   const ::std::optional< ::std::vector< ::std::optional< ValueType > > >& value)
{
   _assert_length_eq(index, priority);
   ::std::function value_getter = [&](size_t /*index*/) -> ::std::optional< ValueType > {
      return ::std::nullopt;
   };
   if(value.has_value()) {
      _assert_length_eq(index, value.value());
      value_getter = [value_vec = ::std::move(value.value())](size_t idx) { return value_vec[idx]; };
   }
   for(size_t i = 0; i < index.size(); i++) {
      _assert_index_in_range(i);
      update(index[i], priority[i], value_getter(i));
   }
}

template < typename ValueType >
double SumTree< ValueType >::priority(size_t index)
{
   _assert_index_in_range(index);
   return m_prioritree[_first_index_at_level(m_leaf_level) + index];
}

template < typename ValueType >
auto SumTree< ValueType >::get(double priority, bool percentage)
   -> ::std::tuple< size_t, ValueType, double >
{
   if(percentage) {
      priority *= m_prioritree[0];
   }
   size_t index = 0;
   // the breaking index is the FIRST leaf level index. The tree enumerates elements by starting
   // with 0 at the root, yet the levels starting from 1. This means that, in order to not
   // overcount, the leaf_level's first index is at 2^(m_max_level - 1) with -1 since starting from
   // 0. And then one subtracts -1 to reach the first index. An example tree with leaf level 4 is:
   // 0
   // 1 2
   // 3 4 5 6
   // 7 8 9 10 11 12 13 14
   // To reach the first leaf index 7, one computes 2**(4-1) - 1 = 2**3 -1 = 8 - 1 = 7
   auto breaking_index = static_cast< size_t >(::std::exp2(m_leaf_level - 1) - 1);
   while(true) {
      size_t left_idx = 2 * index + 1;
      if(priority <= m_prioritree[left_idx]) {
         index = left_idx;  // the left child's index
      } else {
         index = left_idx + 1;  // the right child's index
         // subtract this priority since the sum that lead here is composed of the left priority +
         // right priority. But the search can only continue in a subtree whose priority is less
         // than that subtree's root, otherwise the logic would never finish.
         priority -= m_prioritree[left_idx];
      }
      // once we have reached a leaf index, we have found the corresponding element.
      if(long long value_idx = static_cast< long long >(index)
                               - static_cast< long long >(breaking_index);
         value_idx >= 0) {
         return {value_idx, m_values[static_cast< size_t >(value_idx)], m_prioritree[index]};
      }
   }
}

template < typename ValueType >
::std::string SumTree< ValueType >::as_str() const
{
   ::std::vector< double > prios;
   prios.reserve(m_capacity);

   // stores the indices at which to break for the next level segment. Essentially stores the 2-dim
   // info of the prio vector. Each element is the index at which the counter counts up one
   // dimension up, e.g. from [3, 21] to [4,0] if the 3rd element in `shape_vec` is 22
   ::std::vector< size_t > shape_vec;

   size_t level = 1;
   auto curr_elems = static_cast< size_t >(::std::exp2(level - 1));
   for(size_t i = 0; i < m_prioritree.size(); i++) {
      prios.emplace_back(m_prioritree[i]);
      if(i + 1 == curr_elems) {
         shape_vec.emplace_back(prios.size());
         level++;
         curr_elems = static_cast< size_t >(::std::exp2(level - 1));
      }
   }

   auto curr_break = shape_vec.begin();
   size_t idx = 0;
   ::std::stringstream ss;
   for(auto p : prios) {
      ss << ::std::to_string(p);
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
::std::vector< double >::const_iterator SumTree< ValueType >::priority_begin() const
{
   using iter_diff_t = typename decltype(m_prioritree)::difference_type;
   return m_prioritree.begin() + static_cast< iter_diff_t >(_first_index_at_level(m_leaf_level));
}

template < typename ValueType >
::std::vector< double >::const_iterator SumTree< ValueType >::priority_end() const
{
   using iter_diff_t = typename decltype(m_prioritree)::difference_type;
   return m_prioritree.begin()
          + static_cast< iter_diff_t >(_first_index_at_level(m_leaf_level) + m_size);
}

}  // namespace per

#endif  // PER_SUM_TREE_HPP


#ifndef PER_SUM_TREE_H
#define PER_SUM_TREE_H

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <optional>

namespace py = pybind11;

class SumTree {
  public:
   SumTree(size_t capacity);

   [[nodiscard]] inline size_t len() const { return m_size; }
   [[nodiscard]] inline double total() const { return m_prioritree[0]; }
   [[nodiscard]] inline size_t size() const { return m_size; }

   void insert(py::object value, double priority);
   void update(size_t index, double priority);
   void update(const std::vector< size_t >& index, const std::vector< double >& priority);

   std::tuple< size_t, py::object, double > get(double priority, bool percentage = true);
   double priority(size_t index);
   [[nodiscard]] std::vector< double > priorities_all() const;
   std::string as_str();

  private:
   size_t m_capacity;
   size_t m_size = 0;

   size_t m_leaf_pos = 0;
   size_t m_leaf_level;

   std::vector< double > m_prioritree;
   std::vector< py::object > m_values;

   template < typename T1, typename T2, typename Allocator1, typename Allocator2 >
   void _assert_length_eq(
      const std::vector< T1, Allocator1 >& values,
      const std::vector< T2, Allocator2 >& priorities);
};

template < typename T1, typename T2, typename Allocator1, typename Allocator2 >
void SumTree::_assert_length_eq(
   const std::vector< T1, Allocator1 >& values,
   const std::vector< T2, Allocator2 >& priorities)
{
   if(values.size() != priorities.size()) {
      throw std::invalid_argument("Value sequence and priority sequence do not match in length.");
   }
}

#endif  // PER_SUM_TREE_H

#ifndef SCAN_H
#define SCAN_H
#include <vector.h>

/**
    exclusive scan apply sum of operator result, not include corresponding input operand in partial operator result
  */
template<typename scalar>
void exclusive_scan(const int first, const int last, const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out);

/**
  inclusive scan apply sum of operator reslut, includes corresponding input operand in partial operator result.
  this version just sum;
  */
template<typename scalar>
void inclusive_scan( const int first, const int last,
                     const Vector<scalar, CPU>& in,
                     Vector<scalar, CPU>& out);

/**
  inclusive scan apply sum of operator reslut, includes corresponding input operand in partial operator result.
  In my code i need MAX functor. in that case operator gives = max(x,y);
  */
template<typename scalar>
void inclusive_scan_max( const int first, const int last,
                     const Vector<scalar, CPU>& in,
                     Vector<scalar, CPU>& out);

/**
  Inclusive scan by key performs partial inclusive sums of sub arrays pointed by the indices.
  subarray is pointed with the same index; here uses eq binary predicate and sum as binary operator;
  */

template<typename scalar>
void inclusive_scan_by_key(const int first, const int last,
                           const Vector<scalar, CPU>& in,
                           const Vector<int, CPU>& keys,
                           Vector<scalar, CPU>& out);

#endif // SCAN_H

#ifndef SCATTER_H
#define SCATTER_H

#include <vector.h>

/**
  scatter copy in to result according to map. Map points the order of elements that will appear in the result vector
  */
template<typename scalar>
void scatter (int first, int last, const Vector<scalar, CPU>& in, const Vector<int, CPU>& map, Vector<scalar, CPU>& result);

/**
  scatter_permuted copy permuted in to result according to map. Map points the order of elements that will apear in the result vector.
  The permutation vector points the order of elements in input.
  scatter_permuted is the scatter function where maps is assigned to input and result vectors.
  */
template<typename scalar>
void scatter_permuted(int first, int last, const Vector<scalar, CPU>& in, const Vector<int, CPU>& p, const Vector<int, CPU>& map, Vector<scalar, CPU>& result);


#endif // SCATTER_H

#ifndef SCATTER_H
#define SCATTER_H

#include <vector.h>

/**
  scatter copy in to result according to map. Map points the order of elements that will appear in the result vector
  scatter applies map to output
  gather applies map to input
  */
template<typename scalar>
void scatter (const int first, const int last, const Vector<scalar, CPU>& in,
              const Vector<int, CPU>& map,
                    Vector<scalar, CPU>& out);

/**
  scatter_permuted copy permuted in to result according to map. Map points the order of elements that will apear in the result vector.
  The permutation vector points the order of elements in input.
  scatter_permuted is the scatter function where maps is assigned to input and result vectors.
  */
template<typename scalar>
void scatter_permuted( const int first, const int last, const Vector<scalar, CPU>& in,
                      const Vector<int, CPU>& p, const Vector<int, CPU>& map,
                       Vector<scalar, CPU>& out);

/**
  Scatter if will copy the in to out according to map if stencil[i] = true;
  */
template<typename scalar>
void scatter_if(const int first, const int last, const Vector<scalar, CPU>& in,
                const Vector<int, CPU>& map, const Vector<int, CPU>& stencil,
                Vector<scalar, CPU>& out, const int shift = 0);

/**
  Scatter if permuted copy the in to put according to map if stencil[i] = true
  in addition the in vector is permuted according to p vector pointing to indices i in vector
  */

template<typename scalar>
void scatter_if_permuted(const int first, const int last,
                          const Vector<scalar, CPU>& in, const Vector<int, CPU>& p,
                          const Vector<int, CPU>& map, const Vector<int, CPU>& stencil,
                          Vector<scalar, CPU>& out, const int shift = 0);

#endif // SCATTER_H

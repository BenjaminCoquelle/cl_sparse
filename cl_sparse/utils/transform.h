#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vector.h>

/**
  Specific Transformation required for SPMM
  V[i] = A[Ap[i] * B[Bp[i]]
  V - result vector;
  A - input, Ap - permutation vector for A
  B - input, Bp - permutation vector for B;
  */
template<typename scalar>
void transform (const int first, const int last,
                const Vector<scalar, CPU>& A, const Vector<int, CPU>& Ap,
                const Vector<scalar, CPU>& B, const Vector<int, CPU>& Bp,
                Vector<scalar, CPU>& V);

#endif // TRANSFORM_H

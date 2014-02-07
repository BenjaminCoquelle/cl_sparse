#ifndef REDUCE_H
#define REDUCE_H
#include <vector.h>
#include <matrix/matrixcoo.h>

template<typename scalar>
void reduce_by_key(const Vector<int, CPU>& I,
                   const Vector<int, CPU>& J,
                   const Vector<scalar, CPU>& V,
                   MatrixCOO<scalar, CPU>& C);


#endif // REDUCE_H

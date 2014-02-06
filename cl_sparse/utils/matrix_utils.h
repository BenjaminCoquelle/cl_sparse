#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H
#include <matrix/matrixcoo.h>

template<typename scalar>
void calculate_offsets(const MatrixCOO<scalar, CPU>& matrix,
                                Vector<int, CPU>& row_offsets);

void calculate_row_lenghts(const Vector<int, CPU>& row_offsets,
                                 Vector<int, CPU>& row_lenghts);

template<typename scalar>
void sort_by_row_and_column(Vector<int, CPU>& I,
                            Vector<int, CPU>& J,
                            Vector<scalar, CPU>& V);

template<typename scalar>
void calculate_NNZ(const Vector<int, CPU>& I,
                   const Vector<int, CPU>& J,
                   const Vector<scalar, CPU>& V,
                   int& NNZ);

#endif // MATRIX_UTILS_H

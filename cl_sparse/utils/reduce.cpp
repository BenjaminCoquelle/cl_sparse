#include "reduce.h"
#include <thrust/reduce.h>
#include <thrust/iterator/zip_iterator.h>
template<typename scalar>
void reduce_by_key(const Vector<int, CPU>& I,
                   const Vector<int, CPU>& J,
                   const Vector<scalar, CPU>& V,
                   MatrixCOO<scalar, CPU>& C)
{

    thrust::reduce_by_key
            (thrust::make_zip_iterator(thrust::make_tuple(&I[0], &J[0])),
            thrust::make_zip_iterator(thrust::make_tuple(&I[I.get_csize() - 1], &J[J.get_csize() - 1])),
            &V[0],
            thrust::make_zip_iterator(thrust::make_tuple(C.get_rowPtr(), C.get_colPtr())),
            C.get_valPtr(),
            thrust::equal_to< thrust::tuple<int, int> >(),
            thrust::plus<scalar>());




}





template
void reduce_by_key( const Vector<int, CPU>& I,
                    const Vector<int, CPU>& J,
                    const Vector<int, CPU>& V,
                    MatrixCOO<int, CPU>& C);


template
void reduce_by_key( const Vector<int, CPU>& I,
                    const Vector<int, CPU>& J,
                    const Vector<float, CPU>& V,
                    MatrixCOO<float, CPU>& C);

template
void reduce_by_key( const Vector<int, CPU>& I,
                    const Vector<int, CPU>& J,
                    const Vector<double, CPU>& V,
                    MatrixCOO<double, CPU>& C);

#include "matrix_utils.h"
#include "utils/gather.h"
//#include "utils"
//#include <algorithm>
#include "vector.h"
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/inner_product.h>
#include <thrust/iterator/zip_iterator.h>

#include <boost/iterator/zip_iterator.hpp>

#include <fstream>

//row_offsets are the same as in CSR format;
//if they are calculated correctly we can call row_offsets[i+1]
void calculate_row_lenghts(const Vector<int, CPU>& row_offsets, Vector<int, CPU>& row_lenghts)
{
    for( int i = 0; i < row_lenghts.get_size(); i++)
        row_lenghts[i] = row_offsets[i+1] - row_offsets[i];
}


//lower_bound algorithm should be used
template<typename scalar>
void calculate_offsets(const MatrixCOO<scalar, CPU>& matrix, Vector<int, CPU>& row_offsets)
{
    assert(matrix.get_ncol() > 0);
    assert(matrix.get_nrow() > 0);
    assert(matrix.get_nnz() > 0);

    //from this point nnz, ncol, nrow are allocated and good;
    //allocate(matrix.get_nnz(), matrix.get_nrow(), matrix.get_ncol());


    int* coo_row = matrix.get_rowPtr();
    int* coo_col = matrix.get_colPtr();
    scalar* coo_val = matrix.get_valPtr();
    int nnz = matrix.get_nnz();
    int nrow = matrix.get_nrow();

    //calculate nnz entries per CSR row
    for ( int i = 0; i < nnz; i++ )
        row_offsets[ coo_row[i] ] = row_offsets[ coo_row[i] ] + 1;

    // sum the entries per row to get row_offsets[]
    int sum = 0;
    for (int i = 0; i < nrow; i++)
    {
        int temp = row_offsets[i];
        row_offsets[i] = sum;
    //    printf("offsets[%d]: %d\n", i, offsets[i]);
        sum += temp;
    }
    //last entry
    row_offsets[nrow] = sum;
    //printf("offsets[%d]: %d\n", nrow, offsets[nrow]);

    for (int i = 0; i < nnz; i++)
    {
        int row = coo_row[i];
        row_offsets[row] = row_offsets[row] + 1;
    }

    int last = 0;
    for (int i = 0; i <= nrow; i++)
    {
        int temp = row_offsets[i];
        row_offsets[i] = last;
        last = temp;
    }

}


template<typename scalar>
void sort_by_row_and_column(Vector<int, CPU>& I,
                            Vector<int, CPU>& J,
                            Vector<scalar, CPU>& V)
{
    int N = I.get_size();

    Vector<int, CPU> permutation(0, N, N); //create sequence vector from 0 to N;
    printf("Permutation: \n");
    for(int i = 0; i < 5; i++)
        printf("p[%d] = %d\n", i, permutation[i] );

    //compute permutation and sort by (I, J)
    {
        Vector<int, CPU> temp(J);
        //calculate some order of permutation vector
        //thrust::host_vector<int>::iterator t = thrust::raw_pointer_cast(&temp[0]);
//        printf("%d\n", t[0]);
//        printf("%d\n", t[1]);
//        printf("%d\n", t[2]);

//        printf("\n%d\n", temp[0]);
//        printf("%d\n", temp[1]);
//        printf("%d\n", temp[2]);
        thrust::stable_sort_by_key(&temp[0], &temp[temp.get_size() - 1], &permutation[0]);

        memcpy(temp.get_data(), I.get_cdata(), I.get_size()*sizeof(int));
        gather(0, permutation.get_size(), temp, permutation, I);
        thrust::stable_sort_by_key(&I[0], &I[I.get_size() - 1], &permutation[0]);

        memcpy(temp.get_data(), J.get_cdata(), J.get_size()*sizeof(int));
        gather(0, permutation.get_size(), temp, permutation, J);

    }

    //use permutation to reorder values;
    {
        Vector<scalar, CPU> temp(V);
        gather(0, permutation.get_size(), temp, permutation, V);
    }
}


//make it inline;
bool is_not_equal ( const int px1, const int py1,
                 const int px2, const int py2)
{
//    std::cout << "A: " << px1 << " " << py1 << ", B" << px2 << " " << py2;
//    bool r = (px1 != px2) || (py1 != py2);
//    std::cout << " res: " << r << std::endl;
//    return r;
    return (px1 != px2) || (py1 != py2);
}

template<typename scalar>
void calculate_NNZ(const Vector<int, CPU>& I,
                   const Vector<int, CPU>& J,
                   const Vector<scalar, CPU>& V,
                   int &NNZ)
{
//    printf("size of I: %d\n", I.get_csize());
//    printf("size of J: %d\n", J.get_csize());
//    printf("size of V: %d\n", V.get_csize());

//    std::ofstream fI; fI.open("I.txt");
//    std::ofstream fJ; fJ.open("J.txt");
//    std::ofstream fV; fV.open("V.txt");


//    //save
//    for(int i = 0; i < I.get_csize(); i++)
//    {
//        fI << i << "\t" << I[i] << std::endl;
//        fJ << i << "\t" << J[i] << std::endl;
//        fV << i << "\t" << V[i] << std::endl;
//    }
//    //close
//    fI.close();
//    fJ.close();
//    fV.close();

//    // typedef these iterators for shorthand
//    typedef thrust::host_vector<int>::iterator   IntIterator;
//    // typedef a tuple of these iterators
//    typedef thrust::tuple<IntIterator, IntIterator> IteratorTuple;
//    // typedef the zip_iterator of this tuple
//    typedef thrust::zip_iterator<IteratorTuple> ZipIterator;
//    // finally, create the zip_iterator
//    //ZipIterator iter = thrust::make_zip_iterator(thrust::make_tuple(&I[0], &J[0]));
//    ZipIterator it = thrust::make_zip_iterator(thrust::make_tuple(I.get_cdata(), J.get_cdata()));
//    //IteratorTuple it = thrust::make_tuple(I.get_cdata(), J.get_cdata());

//    std::ofstream iter_file; iter_file.open("begin_zip_iter.txt");
//    for(int i = 0; i < I.get_csize(); i++)
//    {
//        iter_file << i << "\t" << thrust::get<0>(it[i]) << "\t" << thrust::get<1>(it[i]) << std::endl;
//    }
//    iter_file.close();

//    it = thrust::make_zip_iterator(thrust::make_tuple
//                                   (I.get_cdata()+I.get_csize()-1,
//                                    J.get_cdata()+I.get_csize()-1)
//                                   );
//    std::cout << "last: " << thrust::get<0>(*it) << "\t" << thrust::get<1>(*it) << std::endl;

//    it = thrust::make_zip_iterator(thrust::make_tuple
//                                   (I.get_cdata()+I.get_csize()-1,
//                                    J.get_cdata()+I.get_csize()-1)
//                                   )-1;
//    std::cout << "last-1: " << thrust::get<0>(*it) << "\t" << thrust::get<1>(*it) << std::endl;

//    it = thrust::make_zip_iterator(thrust::make_tuple
//                                   (I.get_cdata(),
//                                    J.get_cdata())
//                                   )+1;
//    std::cout << "first+1: " << thrust::get<0>(*it) << "\t" << thrust::get<1>(*it) << std::endl;

    // I J V na pewno mają ten sam rozmiar = workspace_size;

    //first wskazuje na elementy od 0, do size - 2;
    Vector<int, CPU> firstI;
    Vector<int, CPU> firstJ;

    firstI.set_pointerData(I.get_cdata(), I.get_csize()-1);
    firstJ.set_pointerData(J.get_cdata(), J.get_csize()-1);


    //last wskazuje na elementy od 1 do size -1;
    Vector<int, CPU> lastI;
    Vector<int, CPU> lastJ;

    lastI.set_pointerData(I.get_cdata()+1, I.get_csize());
    lastJ.set_pointerData(J.get_cdata()+1, J.get_csize());

    int suma = 0;
    for (int i = 0; i < I.get_csize()-1; i++)
    {

        suma += is_not_equal(firstI[i], firstJ[i], lastI[i], lastJ[i]);
    }

    std::cout << "Distance: " << I.get_csize();
    std::cout << "suma : " << suma << std::endl;

    firstI.release_pointerData();
    firstJ.release_pointerData();
    lastI.release_pointerData();
    lastJ.release_pointerData();
//    NNZ =
//       thrust::inner_product(thrust::make_zip_iterator(
//                             thrust::make_tuple(&I[0], &J[0])),
//                             thrust::make_zip_iterator(
//                                    thrust::make_tuple(&I[I.get_csize()-1],  &J[J.get_csize()-1])),
//                            thrust::make_zip_iterator(thrust::make_tuple(&I[0], &J[0])) + 1,
//                                  0,
//                            thrust::plus<int>(),
//                            thrust::not_equal_to< thrust::tuple<int, int> >())+1;
//   printf("inner_product = %d\n", NNZ);
    NNZ = suma+1;

}

//template
//void calculate_offsets(const MatrixCOO<int, CPU>& matrix, int* offsets);
template
void calculate_offsets(const MatrixCOO<float, CPU>& matrix, Vector<int, CPU>& row_offsets);
template
void calculate_offsets(const MatrixCOO<double, CPU>& matrix, Vector<int, CPU>& row_offsets);

template
void sort_by_row_and_column(Vector<int, CPU>& I,
                            Vector<int, CPU>& J,
                            Vector<int, CPU>& V);

template
void sort_by_row_and_column(Vector<int, CPU>& I,
                            Vector<int, CPU>& J,
                            Vector<float, CPU>& V);
template
void sort_by_row_and_column(Vector<int, CPU>& I,
                            Vector<int, CPU>& J,
                            Vector<double, CPU>& V);

template
void calculate_NNZ(const Vector<int, CPU>& I,
                   const Vector<int, CPU>& J,
                   const Vector<int, CPU>& V,
                   int& NNZ);

template
void calculate_NNZ(const Vector<int, CPU>& I,
                   const Vector<int, CPU>& J,
                   const Vector<float, CPU>& V,
                   int& NNZ);

template
void calculate_NNZ(const Vector<int, CPU>& I,
                   const Vector<int, CPU>& J,
                   const Vector<double, CPU>& V,
                   int& NNZ);

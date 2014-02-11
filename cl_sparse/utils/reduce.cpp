#include "reduce.h"
#include <thrust/reduce.h>
#include <thrust/iterator/zip_iterator.h>

#include <fstream>

inline bool is_equal_keys(const int ix, const int iy,
                          const int jx, const int jy)
{
    return (ix == jx) && (iy == jy);
}

template<typename scalar>
void reduce_by_key(const Vector<int, CPU>& I,
                   const Vector<int, CPU>& J,
                   const Vector<scalar, CPU>& V,
                   MatrixCOO<scalar, CPU>& C)
{

    //I, J, V mają tą samą długość zawsze!
    int num_elements = I.get_csize();

    //result containters
    scalar* v_out = C.get_valPtr();
    int*    i_out = C.get_rowPtr();
    int*    j_out = C.get_colPtr();

    int out_iterator = 0;
    v_out[0] = V[0];
    i_out[0] = I[0];
    j_out[0] = J[0];

    int count = 1;
    int value_iterator = 1;
    for (int k = 1; k < num_elements; k++)
    {
        //load keys
        int current_key_i = I[k];
        int current_key_j = J[k];

        int previous_key_i = I[k-1];
        int previous_key_j = J[k-1];
        //load values
        scalar current_value = V[value_iterator];
        scalar previous_value = v_out[out_iterator];

        //if the same values accumulate
        if (is_equal_keys(current_key_i, current_key_j,
                          previous_key_i, previous_key_j))
        {
            scalar r = current_value + previous_value;
            v_out[out_iterator] = r;
            i_out[out_iterator] = current_key_i;
            j_out[out_iterator] = current_key_j;
        }
        //else increment to the next key
        else
        {
            out_iterator += 1;

            v_out[out_iterator] = current_value;
            i_out[out_iterator] = current_key_i;
            j_out[out_iterator] = current_key_j;
            count++; //To count the number of elements in the output array
        }
        //inc values first
        value_iterator += 1;
    }
}





//    std::ofstream fIJV; fIJV.open("redIJV.txt");

//    //save
//    for(int i = 0; i < I.get_csize(); i++)
//    {
//        fIJV << i << "\t" << I[i]<< "\t" << J[i] << "\t" << V[i] << std::endl;
//    }

//    //close
//    fIJV.close();


//    int* r;
//    int* c;
//    scalar* v;
//    r = C.get_rowPtr();
//    c = C.get_colPtr();
//    v = C.get_valPtr();

//    thrust::reduce_by_key
//            (thrust::make_zip_iterator(thrust::make_tuple(&I[0], &J[0])),
//             thrust::make_zip_iterator(thrust::make_tuple(&I[I.get_csize()-1], &J[J.get_csize()-1]))+1, //very important + 1
//             &V[0],
//            thrust::make_zip_iterator(thrust::make_tuple(&r[0], &c[0])),
//             &v[0],
//             thrust::equal_to< thrust::tuple<int, int> >(),
//             thrust::plus<scalar>());



//    for(int i = 0; i < C.get_nnz(); i++)
//    {
//        std::cout << i << ": " <<r[i] << ", " << c[i] << ", " << v[i] << std::endl;

//    }


//}





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

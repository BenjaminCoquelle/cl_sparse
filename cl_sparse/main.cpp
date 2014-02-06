#include <QtCore/QCoreApplication>

#include "timer.h"
#include "vector.h"
#include "matrix.h"

#include "utils/scatter.h"
#include "utils/gather.h"
#include "utils/matrix_utils.h"
#include "utils/scan.h"
#include "utils/transform.h"

template<typename scalar>
void vector_tests();

template<typename scalar>
bool compare_scalars(const scalar reference, const scalar data, const scalar epsilon)
{

    scalar diff = reference - data;
    bool comp = ((diff <= epsilon) && (diff >= -epsilon));

    return comp;
}
template<typename scalar>
bool compare_array(const scalar* reference, const scalar* data, const int len, const scalar epsilon)
{
    bool result = true;
    unsigned int error_count = 0;

    for (int i = 0; i < len; i++)
    {
        scalar diff = reference[i] - data[i];
        bool comp = ((diff <= epsilon) && (diff >= -epsilon));

        result &= comp;
        error_count += !comp;
    }

    return result;
}

template<typename scalar, typename IndexType>
void coo_spmm_helper(int workspace_size,
                     int begin_row,
                     int end_row,
                     int begin_segment,
                     int end_segment,
                     const MatrixCOO<scalar, CPU>& A,
                     const MatrixCOO<scalar, CPU>& B,
                           MatrixCOO<scalar, CPU>& C,
                     const Vector<IndexType, CPU>& B_row_offsets,
                     const Vector<IndexType, CPU>& segment_lengths,
                     const Vector<IndexType, CPU>& output_ptr,
                           Vector<IndexType, CPU>& A_gather_locations,
                           Vector<IndexType, CPU>& B_gather_locations,
                           Vector<IndexType, CPU>& I,
                           Vector<IndexType, CPU>& J,
                           Vector<scalar, CPU>& V)
{

    A_gather_locations.resize(workspace_size);
    B_gather_locations.resize(workspace_size);
    I.resize(workspace_size);
    J.resize(workspace_size);
    V.resize(workspace_size);

    if (workspace_size == 0)
    {
        //CRITICAL POINT!
        C.resize(A.get_nrow(), B.get_ncol(), 0);
        return;
    }

    A_gather_locations.set(0.0);

    //input is range between begin and end segment.
    Vector<IndexType, CPU> in_segment(begin_segment, end_segment, end_segment - begin_segment);

    //WARNING: THIS WORKS BECAUSE BEGIN SEGMENT IS 0 and out_ptr[begin_segment] is also 0; ;(
    //printf("out_ptr[0]: %d\t begin_segment = %d\n", output_ptr[begin_segment], begin_segment);

    //AD WARNING: TO jednak może działać ponieważ przekazujemy begin_segmet!!!!!
//    thrust::scatter_if(thrust::counting_iterator<IndexType>(begin_segment),
//                       thrust::counting_iterator<IndexType>(end_segment),
//                       output_ptr.begin() + begin_segment,
//                       segment_lengths.begin() + begin_segment,
//                       A_gather_locations.begin() - output_ptr[begin_segment]);
    scatter_if(begin_segment, end_segment, in_segment, output_ptr, segment_lengths, A_gather_locations, output_ptr[begin_segment]);
    printf("A_gather_locations after scatter if\n");
    for (IndexType i = A_gather_locations.get_size()-10; i < A_gather_locations.get_size(); i++)
        printf("\ti:%d = %d\n", i, A_gather_locations[i]);



    inclusive_scan_max(0, A_gather_locations.get_csize(), A_gather_locations, A_gather_locations);
    printf("A_gather_locations after inclusive scan\n");
    for (int i = A_gather_locations.get_size() - 10; i < A_gather_locations.get_size(); i++)
        printf("\ti:%d = %d\n", i, A_gather_locations[i]);

    B_gather_locations.set(1);
    Vector<IndexType, CPU> A_column_indices;
    A_column_indices.set_pointerData(A.get_colPtr(), A.get_nnz());
    scatter_if_permuted(begin_segment, end_segment,
                        B_row_offsets, A_column_indices,
                        output_ptr,segment_lengths,
                        B_gather_locations, output_ptr[begin_segment]); //tutaj jest problem bo shift powinien być przeunięty o output_ptr[begin_segment]
    A_column_indices.release_pointerData();
    printf("B_gather_locations after scatter if permutted \n");
    for( IndexType i = B_gather_locations.get_size() - 10; i < B_gather_locations.get_size(); i++)
        printf("\ti:%d = %d\n", i, B_gather_locations[i]);

    //inplace
    inclusive_scan_by_key(0, A_gather_locations.get_csize(),
                          B_gather_locations,//input
                          A_gather_locations,//map
                          B_gather_locations);//output

    printf("B_gather_locations after inclusive scan by key\n");
    for( IndexType i = B_gather_locations.get_size() - 10; i < B_gather_locations.get_size(); i++)
        printf("\ti:%d = %d\n", i, B_gather_locations[i]);


    //should be A_gather_location.begin(), is it eq to 0 in it's index?
    //input = row_indices; map = gather_locations. output = I.begin
    Vector<IndexType, CPU> A_row_indices;
    A_row_indices.set_pointerData(A.get_rowPtr(), A.get_nnz());
    gather(0, A_gather_locations.get_csize(),
           A_row_indices,
           A_gather_locations,
           I);
    printf("I: \n");
    for( IndexType i = I.get_size() - 10; i < I.get_size(); i++)
        printf("\ti:%d = %d\n", i, I[i]);
    A_row_indices.release_pointerData();

    Vector<IndexType, CPU> B_column_indices;
    B_column_indices.set_pointerData(B.get_colPtr(), B.get_nnz());
    gather(0, B_gather_locations.get_csize(),
           B_column_indices,
           B_gather_locations,
           J);
    printf("J:\n");
    for( IndexType i = J.get_size() - 10; i < J.get_size(); i++)
        printf("\ti:%d = %d\n", i, J[i]);
    B_column_indices.release_pointerData();

    Vector<scalar, CPU> Avalues;
    Avalues.set_pointerData(A.get_valPtr(), A.get_nnz());

    Vector<scalar, CPU> Bvalues;
    Bvalues.set_pointerData(B.get_valPtr(), B.get_nnz());

    transform(0, A_gather_locations.get_csize(),
              Avalues, A_gather_locations,
              Bvalues, B_gather_locations,
              V);

    printf("V:\n");

    for( IndexType i = V.get_size() - 10; i < V.get_size(); i++)
        printf("\ti:%d = %f\n", i, V[i]);

    Avalues.release_pointerData();
    Bvalues.release_pointerData();

    sort_by_row_and_column(I, J, V);

    for (int i = V.get_size() - 25; i < V.get_size(); i++)
        printf ("%d: %d, %d, %f\n", i, I[i], J[i], V[i]);

    IndexType NNZ = 0;
    calculate_NNZ(I, J, V, NNZ);

    printf("NNZ: %d\n", NNZ );
    prntf("NNZ SIĘ NIE ZGADZA! Porównać wszystko od początku!");
}



int main(int argc, char *argv[])
{

    // -1 = best device with most compute units
    // =>0 - device_id usually same as nvidia_smi ids.
    typedef float scalar;
    typedef int IndexType;
    OpenCL ocl(0);

    std::string file = "/media/Storage/matrices/mm/orani678.mtx";

    MatrixCOO<scalar, CPU> A;
    MatrixCOO<scalar, CPU> B;//write copy constructor!
    MatrixCOO<scalar, CPU> C;
    A.load(file);
    B.load(file);

    //printf("space: %d\n", coo.get_nrow() + 1);

    //może zmienić na Vector<int>
    //need to be zeroed!!
    Vector<IndexType, CPU> B_row_offsets(B.get_nrow() + 1, 0);// = new int [coo.get_nrow() + 1];
    //need to zero the input
//    for(int i = 0; i < B.get_nrow() + 1; i++)
//        offsets[i] = 0;

    calculate_offsets(B, B_row_offsets);
//    for (int i = 0; i < B.get_nrow() + 1; i++)
//        printf("\t%d\n", B_row_offsets[i]);

    Vector<IndexType, CPU> B_row_lenghts(B.get_nrow());

    calculate_row_lenghts(B_row_offsets, B_row_lenghts);
//    for (int i = 0; i < coo.get_nrow(); i++)
//        printf("\ti:%d %d\n", i, B_row_lenghts[i]);
//    printf("nrow %d\n", coo.get_nrow());
    Vector<IndexType, CPU> segment_lengths(A.get_nnz());

    Vector<IndexType, CPU> map;
    map.set_pointerData(A.get_colPtr(), A.get_nnz()); //czy nnz?
    gather(0, A.get_nnz(), B_row_lenghts, map, segment_lengths);
//    for (int i = 0; i < A.get_nnz(); i++)
//            printf("\ti:%d %d\n", i, segment_lenghts[i]);
    map.release_pointerData();

    Vector<IndexType, CPU> output_ptr(A.get_nnz() + 1);
    exclusive_scan(0, segment_lengths.get_size(), segment_lengths, output_ptr);
    //due to different sizes between out_ptr and segments we need to apply last item which not included in scan
    output_ptr[A.get_nnz()]  = output_ptr[A.get_nnz() - 1 ] + segment_lengths[A.get_nnz() - 1];
//    for (int i = 0; i < A.get_nnz()+1; i++)
//            printf("\ti:%d %d\n", i, output_ptr[i]);

    int coo_num_nonzeros = output_ptr[A.get_nnz()];
    int workspace_capacity = std::min(coo_num_nonzeros, 16 << 20);

    {
        /**
          Calculate workspace_capacity; should be third of the available workspace;
          */

//        size_t free, total;
//        cudaMemGetInfo(&free, &total);

//        // divide free bytes by the size of each workspace unit
//        size_t max_workspace_capacity = free / (4 * sizeof(IndexType) + sizeof(ValueType));

//        // use at most one third of the remaining capacity
//        workspace_capacity = thrust::min<size_t>(max_workspace_capacity / 3, workspace_capacity);
    }


    Vector<IndexType, CPU> A_gather_locations;
    Vector<IndexType, CPU> B_gather_locations;
    Vector<IndexType, CPU> I;
    Vector<IndexType, CPU> J;
    Vector<scalar,    CPU> V;

    if (coo_num_nonzeros <= workspace_capacity)
    {
        printf("ONE HIT SPMM\n");

        int begin_row       = 0;
        int end_row         = A.get_nrow();
        int begin_segment   = 0;
        int end_segment      = A.get_nnz();
        int workspace_size  = coo_num_nonzeros;

        printf(" %d %d %d %d %d\n", begin_row, end_row, begin_segment, end_segment, workspace_size);

        coo_spmm_helper(workspace_size,
                        begin_row, end_row,
                        begin_segment, end_segment,
                        A, B, C,
                        B_row_offsets,
                        segment_lengths, output_ptr,
                        A_gather_locations, B_gather_locations,
                        I, J, V);
    }
    else
    {
        printf("SLICING\n");
    }

    ocl.shutdown ();

}


    /**
    //inclusive_scan_by_key
    int values[10]   = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int keys[10]     = {0, 0, 0, 1, 1, 2, 3, 3, 3, 3};
    Vector<int, CPU> v; v.set_pointerData(&values[0], 10);
    Vector<int, CPU> k; k.set_pointerData(&keys[0], 10);
    v.print(10);
    inclusive_scan_by_key(0, 10, v, k, v); //inplace
    for( int i = 0; i < v.get_size(); i++)
        printf("\ti:%d = %d\n", i, v[i]);
    //result should be {1, 2, 3, 1, 2, 1, 1, 2, 3, 4};
    v.release_pointerData();
    k.release_pointerData();

      //permutation test
//    scalar* values = new scalar[8];
//    values[0] = 10.0f;
//    values[1] = 20.0f;
//    values[2] = 30.0f;
//    values[3] = 40.0f;
//    values[4] = 50.0f;
//    values[5] = 60.0f;
//    values[6] = 70.0f;
//    values[7] = 80.0f;

//    int* indices = new int [4];
//    indices[0] = 2;
//    indices[1] = 6;
//    indices[2] = 1;
//    indices[3] = 3;

//    for (int i = 0; i < 4; i++)
//        printf("%f\n", values[indices[i]]);


    //inclusive scan
    {int values[6]   = {1, -1, -2, 2, 1, 3};

    Vector<int, CPU> v; v.set_pointerData(&values[0], 6);
    v.print(6);
    inclusive_scan(0, 6, v, v, true); //inplace
    v.print(6);
    v.release_pointerData();
    }

    // ScatterIF test
    int values[8]   = {10, 20, 30, 40, 50, 60, 70, 80};
    int map[8]      = {0, 5, 1, 6, 2, 7, 3, 4};
    int st[8]         = {1, 0, 1, 0, 1, 0, 1, 0};
    int d_output[8]   = {0, 0, 0, 0, 0, 0, 0, 0};

    Vector<int, CPU> v; v.set_pointerData(&values[0], 8);
    Vector<int, CPU> m; m.set_pointerData(&map[0], 8);
    Vector<int, CPU> s; s.set_pointerData(&st[0], 8);
    Vector<int, CPU> r; r.set_pointerData(&d_output[0], 8);

    scatter_if(0, 8, v, m, s, r);
    r.print(10);
    v.release_pointerData();
    m.release_pointerData();
    r.release_pointerData();
    s.release_pointerData();

    //  Exclusive scan
    {int values[6]   = {1, 0, 2, 2, 1, 3};

    Vector<int, CPU> v; v.set_pointerData(&values[0], 6);
    v.print(6);
    exclusive_scan(0, 6, v, v); //inplace
    v.print(6);
    v.release_pointerData();
    }
    //gather test
    {int values[10]   = {2 , 0, 4, 0, 6, 0, 8, 0, 10, 0};
    int map[10]      = {0 , 2, 4, 6, 8, 1, 3, 5, 7, 9};
    int d_output[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

    Vector<int, CPU> v; v.set_pointerData(&values[0], 10);
    Vector<int, CPU> m; m.set_pointerData(&map[0], 10);
    Vector<int, CPU> r; r.set_pointerData(&d_output[0], 10);

    gather(0, 10, v, m, r);
    r.print(10);
    v.release_pointerData();
    m.release_pointerData();
    r.release_pointerData();
    }

  //TEST MATRIX SPMV put that inside ocl
    QList<std::string> matrices;

    matrices.append("/home/kuba/External/UFget/MM/mtx/cant.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/consph.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/rma10.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/pwtk.mtx");
    //matrices.append("/home/kuba/External/UFget/MM/mtx/rail4284.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/pdb1HYS.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/raefsky3.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/mc2depi.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/scircuit.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/tsopf_rs_2383.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/shipsec1.mtx");


    foreach (std::string fmtx, matrices)
    {
        fprintf(stdout, "%s\n", fmtx.c_str());
        MatrixCSR<scalar, GPU> csr;
        csr.load(fmtx);

        Vector<scalar, GPU> x(csr.get_ncol(), 1.0);
        Vector<scalar, GPU> b(csr.get_nrow(), 0.0);

        Timer t;


        //WITH THIS PIECE OF CODE WE WANT TO USE TEXTURE MEMORY

        int WIDTH = 8192;
        int HEIGHT= 1 + ((x.get_csize() - 1) >> 15);
        fprintf(stdout, "len: %d, rowDim: %d\n", x.get_csize(), HEIGHT);

        scalar* input = new scalar [x.get_csize()];
        OpenCL::copy(input, x.get_cdata(), x.get_csize()*sizeof(scalar));
        scalar* image2dVec = new scalar [WIDTH*HEIGHT*4];

        memset(image2dVec, 0, sizeof(float)*WIDTH*HEIGHT*4);
        for(int i = 0; i<x.get_csize(); i++)
        {
            image2dVec[i] = input[i];
        }

        size_t origin[] = {0, 0, 0};
        size_t vectorSize[] = {WIDTH, HEIGHT, 1};

        const cl_image_format floatFormat =
        {
            //CL_R works for vector kernel. CL_RGBA działa na scalar
                CL_R,
                CL_FLOAT,
        };
        cl_int error;
        cl_mem devTexVec = clCreateImage2D(OpenCL::get_context(), CL_MEM_READ_ONLY, &floatFormat, WIDTH, HEIGHT, 0, NULL, &error );
        //printf("Create devTexVec: %s\n", OpenCL::getError(error).toStdString().c_str());
        error = clEnqueueWriteImage(OpenCL::get_queue(), devTexVec, CL_TRUE, origin, vectorSize, 0, 0, image2dVec, 0, NULL, NULL);
        clFinish(OpenCL::get_queue());
        //printf("Write to devTexVec: %s\n", OpenCL::getError(error).toStdString().c_str());


        b.set(0.0);
        t.reset();
        for (int i = 0; i < 50; i++)
            csr.multiply(x, b, true);
        printf("Time scalar: %gms\n", (1000.0*t.elapsed_seconds())/50);
        printf("csr b norm %f\n", b.norm());

        b.set(0.0);
        t.reset();
        for (int i = 0; i < 50; i++)
            csr.multiply(x, b, false);
        printf("Time vector: %gms\n", (1000.0*t.elapsed_seconds())/50);
        printf("csr b norm %f\n", b.norm());


//        b.set(0.0);
//        t.reset();
//        for (int i = 0; i < 50; i++)
//            csr.test1(x,b, devTexVec);
//        printf("Time scalar TEX: %gms\n", (1000.0*t.elapsed_seconds())/50);
//        printf("csr scalar TEX b norm %f\n", b.norm());


        b.set(0.0);
         t.reset();
         for (int i = 0; i < 50; i++)
             csr.test2(x, b, devTexVec);
         printf("Time vector TEX: %gms\n",(1000.0*t.elapsed_seconds())/50);
         printf("test b norm %f\n", b.norm());

         printf("\n\n");

             MatrixELL<scalar, GPU> ell;
             ell.load(fmtx);

             b.set(0.0);
             t.reset();
             for (int i = 0; i < 50; i++)
                 ell.multiply(x, b);
             printf("T: %g\n",(1000* t.elapsed_seconds())/50);
             printf("ell b norm %f\n", b.norm());
             b.save("b_ell_result.txt");
    }




//    MatrixELL<scalar, GPU> ell;
//    ell.load(fmtx);

//    b.set(0.0);
//    t.reset();
//    for (int i = 0; i < 50; i++)
//        ell.multiply(x, b);
//    printf("T: %g\n", t.elapsed_seconds()/50);
//    printf("ell b norm %f\n", b.norm());
//    b.save("b_ell_result.txt");

//    MatrixHYB<scalar, GPU> hyb;
//    hyb.load(fmtx);

//    b.set(0.0);
//    t.reset();
//    for (int i = 0; i < 50; i++)
//        hyb.multiply(x, b);
//    printf("T: %g\n", t.elapsed_seconds()/50);
//    printf("hyb b norm %f\n", b.norm());
//    b.save("b_hyb_result.txt");

  */

#include "matrixcoo.h"
#include "io/mmio.h"
#include <algorithm>




template<typename scalar>
MatrixCOO<scalar, GPU>::MatrixCOO()
{
    this->mat.row = NULL;
    this->mat.col = NULL;
    this->mat.val = NULL;

    this->nnz = 0;
    this->nrow = 0;
    this->ncol = 0;
}


template<typename scalar>
MatrixCOO<scalar, GPU>::MatrixCOO(const int* row, const int* col, const scalar* val,
                                  const int nnz, const int nrow, const int ncol)
{

    assert( nnz   >= 0);
    assert( ncol  >= 0);
    assert( nrow  >= 0);


    allocate(nnz, nrow, ncol);


    // lenght of the tables is the same! = nnz!
    OpenCL::copy (this->mat.row, row, nnz*sizeof(int));
    OpenCL::copy (this->mat.col, col, nnz*sizeof(int));
    OpenCL::copy (this->mat.val, val, nnz*sizeof(scalar));


}

template<typename scalar>
MatrixCOO<scalar, GPU>::MatrixCOO(const MatrixCOO<scalar, CPU>& other)
{
    allocate(other.get_nnz(), other.get_nrow(), other.get_ncol());

    OpenCL::copy(this->mat.row, other.get_rowPtr(), nnz*sizeof(int));
    OpenCL::copy(this->mat.col, other.get_colPtr(), nnz*sizeof(int));
    OpenCL::copy(this->mat.val, other.get_valPtr(), nnz*sizeof(scalar));
}

template<typename scalar>
MatrixCOO<scalar, GPU>::~MatrixCOO()
{
    clear();
}

template<typename scalar>
void MatrixCOO<scalar, GPU>::allocate(const int nnz, const int nrow, const int ncol)
{

    assert( nnz   >= 0);
    assert( ncol  >= 0);
    assert( nrow  >= 0);


    this->nnz = nnz;
    this->ncol = ncol;
    this->nrow = nrow;

     // lenght of the tables is the same! = nnz!
    OpenCL::allocate (&this->mat.row, nnz * sizeof(int));
    OpenCL::allocate (&this->mat.col, nnz * sizeof(int));
    OpenCL::allocate (&this->mat.val, nnz * sizeof(scalar));


}

template<typename scalar>
void MatrixCOO<scalar, GPU>::clear()
{
    if(this->get_nnz() > 0)
    {
        OpenCL::free(&this->mat.row);
        OpenCL::free(&this->mat.col);
        OpenCL::free(&this->mat.val);

        this->mat.row = 0;
        this->mat.col = 0;
        this->mat.val = 0;

        this->nrow = 0;
        this->ncol = 0;
        this->nnz = 0;
    }
}

template<typename scalar>
const int MatrixCOO<scalar, GPU>::get_nnz() const
{
    return this->nnz;
}

template<typename scalar>
const int MatrixCOO<scalar, GPU>::get_nrow() const
{
    return this->nrow;
}

template<typename scalar>
const int MatrixCOO<scalar, GPU>::get_ncol() const
{
    return this->ncol;
}

template<typename scalar>
cl_mem const MatrixCOO<scalar, GPU>::get_rowPtr() const
{
    return this->mat.row;
}

template<typename scalar>
cl_mem const MatrixCOO<scalar, GPU>::get_colPtr() const
{
    return this->mat.col;
}

template<typename scalar>
cl_mem const MatrixCOO<scalar, GPU>::get_valPtr() const
{
    return this->mat.val;
}

template<typename scalar>
void MatrixCOO<scalar, GPU>::info()
{
    printf("\n\tGPU Matrix, rows: %d, cols: %d, vals: %d\n", get_nrow(), get_ncol(), get_nnz());
}

template<typename scalar>
void MatrixCOO<scalar, GPU>::load(const std::string& fname_mtx)
{
    MatrixCOO<scalar, CPU> m;
    m.load(fname_mtx);

    clear();
    allocate(m.get_nnz(), m.get_nrow(), m.get_ncol());

    OpenCL::copy(this->mat.row, m.get_rowPtr(), nnz*sizeof(int));
    OpenCL::copy(this->mat.col, m.get_colPtr(), nnz*sizeof(int));
    OpenCL::copy(this->mat.val, m.get_valPtr(), nnz*sizeof(scalar));

}

template<typename scalar>
void MatrixCOO<scalar, GPU>::save(const std::string& fname_mtx)
{
    MatrixCOO<scalar, CPU> m (*this);
    m.save(fname_mtx);

}

template<typename scalar>
double MatrixCOO<scalar, GPU>::get_troughput(const double& time)
{
    double result = 0.0;
    //vals
    result += 1.0e-9 * ((double)nnz*sizeof(scalar))/time;
    //rows
    result += 1.0e-9 * ((double)nnz*sizeof(int))/time;
    //cols
    result += 1.0e-9 * ((double)nnz*sizeof(int))/time;

    return result;
}


template<typename scalar>
void MatrixCOO<scalar, GPU>::multiply(const Vector<scalar, GPU>& in, Vector<scalar, GPU>& out)
{
    if (this->get_nnz() > 0)
    {
        assert(in.get_csize() >= 0);
        assert(out.get_size()>= 0);
        assert(in.get_csize() == this->get_ncol());
        assert(out.get_size()== this->get_nrow());

        //qDebug() << "Matrix NNZ: " << this->get_nnz() << "\n";

        /** TODO:: DO I have to zero it */
        out.set(0.f);
        //qDebug() << " out in multip: "; out.print();

        // do not support super small matrices
        const int OPENCL_WARPSIZE = 32;
        assert(this->get_nnz() > OPENCL_WARPSIZE);

        //Max work-group size: 1024
        const unsigned int BLOCK_SIZE = 128; //this->local_backend_.OCL_max_work_group_size;

        const unsigned int MAX_BLOCKS = 32; //  cusp::detail::device::arch::max_active_blocks(spmv_coo_flat_kernel<IndexType, ValueType, BLOCK_SIZE, UseCache>, BLOCK_SIZE, (size_t) 0);

        const unsigned int WARPS_PER_BLOCK = BLOCK_SIZE / OPENCL_WARPSIZE;

        const unsigned int num_units  = this->get_nnz() / OPENCL_WARPSIZE;
        const unsigned int num_warps  = std::min(num_units, WARPS_PER_BLOCK * MAX_BLOCKS);
        const unsigned int num_blocks = (num_warps + (WARPS_PER_BLOCK-1)) / WARPS_PER_BLOCK; // (N + (granularity - 1)) / granularity
        const unsigned int num_iters  = (num_units +  (num_warps-1)) / num_warps;

        const unsigned int interval_size = OPENCL_WARPSIZE * num_iters;

        // do the last few nonzeros separately (fewer than this->local_backend_.GPU_wrap elements)
        // remaining elements = nnz() - tail.
        const int tail = num_units * OPENCL_WARPSIZE; // do the last few nonzeros separately (fewer than this->local_backend_.GPU_wrap elements)

        const unsigned int active_warps = (interval_size == 0) ? 0 : ((tail + (interval_size-1))/interval_size);

        cl_mem temp_rows = NULL;
        cl_mem temp_vals = NULL;

        OpenCL::allocate (&temp_rows, active_warps * sizeof(int));
        OpenCL::allocate (&temp_vals, active_warps * sizeof(scalar));

//        qDebug() <<"WARP_SIZE =" << OPENCL_WARPSIZE << "\n";
//        qDebug() <<"active_warps = " << active_warps;
//        qDebug() <<"tail =" << tail;
//        qDebug() <<"interval_size =" << interval_size;
//        qDebug() <<"num_iters =" << num_iters;
//        qDebug() <<"num_blocks =" << num_blocks;
//        qDebug() <<"num_warps =" << num_warps;
//        qDebug() <<"num_units =" << num_units;
//        qDebug() <<"WARPS_PER_BLOCK =" << WARPS_PER_BLOCK;
//        qDebug() <<"MAX_BLOCKS =" << MAX_BLOCKS;
//        qDebug() <<"BLOCK_SIZE =" << BLOCK_SIZE;

        cl_int    err;
        cl_event  ocl_event;
        size_t    localWorkSize[1];
        size_t    globalWorkSize[1];

        scalar s = 1.0;

//        int * i = new int [this->nnz];
//        OpenCL::copy(i, this->mat.row, this->nnz*sizeof(int));
//        int * j = new int [this->nnz];
//        OpenCL::copy(j, this->mat.col, this->nnz*sizeof(int));
//        scalar* v = new scalar [this->nnz];
//        OpenCL::copy(v, this->mat.val, this->nnz*sizeof(int));

//        for(int w = 0; w < this->nnz; w++)
//            printf("%d, %d, %f\n", i[w], j[w], v[w]);


        // Set arguments for kernel call
        Kernel k;
        if(sizeof(scalar) == 4)
            k = OpenCL::kernels["s_kernel_coo_spmv_flat"];
        else
            k = OpenCL::kernels["d_kernel_coo_spmv_flat"];
        k.set_int     (0, tail);
        k.set_int     (1, interval_size);
        k.set_buffer  (2, this->mat.row);
        k.set_buffer  (3, this->mat.col);
        k.set_buffer  (4, this->mat.val);
        if (sizeof(scalar) == 4)
            k.set_float (5, s);
        else
            k.set_double (5, s);

        k.set_buffer (6, in.get_cdata());
        k.set_buffer (7, out.get_data());
        k.set_buffer (8, temp_rows);
        k.set_buffer (9, temp_vals);

        // Determine local work size for kernel call
        localWorkSize[0]  = BLOCK_SIZE;
        // Determine global work size for kernel call
        globalWorkSize[0] = num_blocks * localWorkSize[0];
        // Start kernel run
        k.execute (localWorkSize[0], globalWorkSize[0], &ocl_event);
        // Wait for kernel run to finish
        err = clWaitForEvents( 1, &ocl_event );
        // printf("flat kernel wait event: %s\n", OpenCL::getError (err).toStdString().c_str());
        // Release event when kernel run finished
        err = clReleaseEvent( ocl_event );
        // printf("flat kernel release event: %s\n", OpenCL::getError (err).toStdString().c_str ());

        /** Check Temporary rows and values */
//        int* h_temp_rows = new int [active_warps];
//        scalar* h_temp_vals = new scalar [active_warps];
//        OpenCL::copy(h_temp_rows, temp_rows, active_warps*sizeof(int));
//        OpenCL::copy(h_temp_vals, temp_vals, active_warps*sizeof(scalar));

//        for(int i = 0; i < active_warps; i++)
//        {
//          printf("temp[%d]: %d\t%f\n", i, h_temp_rows[i], h_temp_vals[i]);
//        }
//        delete [] h_temp_rows;
//        delete [] h_temp_vals;

//        /** Checke out vector after first kernel*/
//        qDebug() << "After first kernel! \n\n";
//        out.print(out.get_csize());
//        qDebug() << "\n\n";



        // Set arguments for kernel call
        if (sizeof(scalar) == 4)
            k = OpenCL::kernels["s_kernel_coo_spmv_reduce_update"];
        else
            k = OpenCL::kernels["d_kernel_coo_spmv_reduce_update"];
        k.set_int     (0, active_warps);
        k.set_buffer  (1, temp_rows);
        k.set_buffer  (2, temp_vals);
        k.set_buffer  (3, out.get_data());

        // Determine global work size for kernel call
        globalWorkSize[0] = localWorkSize[0];

        // Start kernel run
        k.execute (localWorkSize[0], globalWorkSize[0], &ocl_event);
        err = clWaitForEvents( 1, &ocl_event );
        // printf("coo spmv reduce kernel wait event: %s\n", OpenCL::getError (err).toStdString().c_str());
        // Release event when kernel run finished
        err = clReleaseEvent( ocl_event );
        // printf("coo spmv reduce kernel release event: %s\n", OpenCL::getError (err).toStdString().c_str ());

        //qDebug() << "After second kernel! \n\n";
        //out->print(out->get_size());
        //qDebug() << "\n\n";


        //int nnz = this->get_nnz();

        // Set arguments for kernel call
        if (sizeof(scalar) == 4)
            k = OpenCL::kernels["s_kernel_coo_spmv_serial"];
        else
            k = OpenCL::kernels["d_kernel_coo_spmv_serial"];
        k.set_int(0, nnz);
        k.set_buffer(1, this->mat.row);
        k.set_buffer(2, this->mat.col);
        k.set_buffer(3, this->mat.val);
        if (sizeof(scalar) == 4)
          k.set_float (4, s);
        else
         k.set_double (4, s);
        k.set_buffer (5, in.get_cdata());
        k.set_buffer (6, out.get_data());
        k.set_int(7, tail);

        localWorkSize[0] = 1;
        globalWorkSize[0] = 1;

        k.execute(localWorkSize[0], globalWorkSize[0], &ocl_event);

        err = clWaitForEvents( 1, &ocl_event );
        //printf("coo spmv serial kernel wait event: %s\n", OpenCL::getError (err).toStdString().c_str());
        // Release event when kernel run finished
        err = clReleaseEvent( ocl_event );
        // printf("coo spmv serial kernel release event: %s\n", OpenCL::getError (err).toStdString().c_str ());

       // OpenCL::free(&temp_rows);
       // OpenCL::free(&temp_vals);

    }
    else
    {
        qCritical() << "nnz of the matrix : " << this->get_nnz();
    }


}

template class MatrixCOO<float, GPU>;
template class MatrixCOO<double, GPU>;

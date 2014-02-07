#include "matrixhyb.h"

template<typename scalar>
MatrixHYB<scalar, GPU>::MatrixHYB()
{
    this->mat.ELL.val = NULL;
    this->mat.ELL.col = NULL;
    this->mat.ELL.max_row = 0;

    this->mat.COO.row = NULL;
    this->mat.COO.col = NULL;
    this->mat.COO.val = NULL;

    this->ncol = 0;
    this->nrow = 0;
    this->nnz = 0;

    this->ell_nnz = 0;
    this->coo_nnz = 0;
}


template<typename scalar>
MatrixHYB<scalar, GPU>::MatrixHYB(const MatrixHYB<scalar, CPU>& other)
{
    allocate(other.get_ell_nnz(), other.get_coo_nnz(), other.get_ell_max_row(), other.get_nrow(), other.get_ncol());

    OpenCL::copy(this->mat.COO.val, other.get_cooValPtr(), coo_nnz*sizeof(scalar));
    OpenCL::copy(this->mat.COO.col, other.get_cooColPtr(), coo_nnz*sizeof(int));
    OpenCL::copy(this->mat.COO.row, other.get_cooRowPtr(), coo_nnz*sizeof(int));

    OpenCL::copy(this->mat.ELL.val, other.get_ellValPtr(), ell_nnz*sizeof(scalar));
    OpenCL::copy(this->mat.ELL.col, other.get_ellColPtr(), ell_nnz*sizeof(int));
}

template<typename scalar>
MatrixHYB<scalar, GPU>::~MatrixHYB()
{
    clear();
}

template<typename scalar>
void MatrixHYB<scalar, GPU>::clear()
{
    if (this->get_nnz() > 0)
    {
        OpenCL::free(&this->mat.ELL.val);
        OpenCL::free(&this->mat.ELL.col);

        OpenCL::free(&this->mat.COO.col);
        OpenCL::free(&this->mat.COO.row);
        OpenCL::free(&this->mat.COO.val);

        this->mat.ELL.val = NULL;
        this->mat.ELL.col = NULL;

        this->mat.COO.val = NULL;
        this->mat.COO.col = NULL;
        this->mat.COO.row = NULL;

        this->ell_nnz = 0;
        this->coo_nnz = 0;
        this->nnz = 0;

        this->nrow = 0;
        this->ncol = 0;

    }
}

template<typename scalar>
void MatrixHYB<scalar, GPU>::allocate(const int ell_nnz, const int coo_nnz, const int ell_max_row, const int nrow, const int ncol)
{
    assert( ell_nnz   >= 0);
    assert( coo_nnz   >= 0);
    assert( ell_max_row >= 0);

    assert( ncol  >= 0);
    assert( nrow  >= 0);


    if (ell_nnz + coo_nnz > 0)
    {

      // ELL
      assert(ell_nnz == ell_max_row*nrow);

      OpenCL::allocate(&this->mat.ELL.val, ell_nnz*sizeof(scalar));
      OpenCL::allocate(&this->mat.ELL.col, ell_nnz*sizeof(int));

      this->mat.ELL.max_row = ell_max_row;
      this->ell_nnz = ell_nnz;

      // COO
      OpenCL::allocate(&this->mat.COO.row, coo_nnz*sizeof(int));
      OpenCL::allocate(&this->mat.COO.col, coo_nnz*sizeof(int));
      OpenCL::allocate(&this->mat.COO.val, coo_nnz*sizeof(scalar));

      this->coo_nnz = coo_nnz;

      this->nrow = nrow;
      this->ncol = ncol;
      this->nnz  = ell_nnz + coo_nnz;
    }
}

template<typename scalar>
void MatrixHYB<scalar, GPU>::load(const std::string &fname_mtx)
{
    MatrixCSR<scalar, CPU> matrix;
    matrix.load(fname_mtx);
//    matrix.save("internal_csr.mat");

    //fprintf(stdout, "nnz: %d, nrow: %d, ncol: %d\n", matrix.get_nnz(), matrix.get_nrow(), matrix.get_ncol());

    MatrixHYB<scalar, CPU>  hyb;
    hyb.get_data(matrix);
//    hyb.save("internal_hyb.mat");
//    fprintf(stdout, "nnz: %d, ell_nnz: %d, coo_nnz: %d, max_row: %d, nrow: %d, ncol: %d\n",
//            hyb.get_nnz(),
//            hyb.get_ell_nnz(),
//            hyb.get_coo_nnz(),
//            hyb.get_ell_max_row(),
//            hyb.get_nrow(),
//            hyb.get_ncol());

    allocate(hyb.get_ell_nnz(), hyb.get_coo_nnz(), hyb.get_ell_max_row(), hyb.get_nrow(), hyb.get_ncol());

//    fprintf(stdout, "nnz: %d, ell_nnz: %d, coo_nnz: %d, max_row: %d, nrow: %d, ncol: %d\n",
//            get_nnz(),
//            get_ell_nnz(),
//            get_coo_nnz(),
//            get_ell_max_row(),
//            get_nrow(),
//            get_ncol());

    OpenCL::copy(this->mat.COO.val, hyb.get_cooValPtr(), coo_nnz*sizeof(scalar));
    OpenCL::copy(this->mat.COO.col, hyb.get_cooColPtr(), coo_nnz*sizeof(int));
    OpenCL::copy(this->mat.COO.row, hyb.get_cooRowPtr(), coo_nnz*sizeof(int));

    OpenCL::copy(this->mat.ELL.val, hyb.get_ellValPtr(), ell_nnz*sizeof(scalar));
    OpenCL::copy(this->mat.ELL.col, hyb.get_ellColPtr(), ell_nnz*sizeof(int));


}

template<typename scalar>
void MatrixHYB<scalar, GPU>::save(const std::string &fname_mtx)
{
    MatrixHYB<scalar, CPU> hyb(*this);
    MatrixCSR<scalar, CPU> matrix;
    matrix.get_data(hyb);
    matrix.save(fname_mtx);

}


template<typename scalar>
void MatrixHYB<scalar, GPU>::multiply(const Vector<scalar, GPU> &in, Vector<scalar, GPU> &out)
{
    if (get_nnz() > 0)
    {
        assert(in.get_csize() >= 0);
        assert(out.get_size() >= 0);
        assert(in.get_csize() == this->get_ncol());
        assert(out.get_size() == this->get_nrow());


        cl_int    err;
        cl_event  ocl_event;
        size_t    localWorkSize[1];
        size_t    globalWorkSize[1];
        Kernel    k;


        //multiply ELL
        if (get_ell_nnz() > 0)
        {
            localWorkSize[0] = 512;
            globalWorkSize[0] = ( size_t( this->nrow / localWorkSize[0] ) + 1 ) * localWorkSize[0];

            if (sizeof(scalar) == 4)
                k = OpenCL::kernels["s_kernel_ell_spmv"];
            else
                k = OpenCL::kernels["d_kernel_ell_spmv"];
            k.set_int(0, this->nrow);
            k.set_int(1, this->ncol);
            k.set_int(2, this->mat.ELL.max_row);
            k.set_buffer(3, this->mat.ELL.col);
            k.set_buffer(4, this->mat.ELL.val);
            k.set_buffer(5, in.get_cdata());
            k.set_buffer(6, out.get_data());

            k.execute(localWorkSize[0], globalWorkSize[0], &ocl_event);

            err = clWaitForEvents( 1, &ocl_event );
            //printf("ell kernel wait event: %s\n", OpenCL::getError (err).toStdString().c_str());
            //out.print(out.get_size());

            err = clReleaseEvent( ocl_event );

        }
        //multiply COO
       if (get_coo_nnz() > 0)
        {

            const int OPENCL_WARPSIZE = 32;

            assert(this->get_coo_nnz() > OPENCL_WARPSIZE);

            //Max work-group size: 1024
            const unsigned int BLOCK_SIZE = 128; //this->local_backend_.OCL_max_work_group_size;

            const unsigned int MAX_BLOCKS = 32; //  cusp::detail::device::arch::max_active_blocks(spmv_coo_flat_kernel<IndexType, ValueType, BLOCK_SIZE, UseCache>, BLOCK_SIZE, (size_t) 0);

            const unsigned int WARPS_PER_BLOCK = BLOCK_SIZE / OPENCL_WARPSIZE;

            const unsigned int num_units  = this->get_coo_nnz() / OPENCL_WARPSIZE;
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

             scalar s = 1.0;

//            int * i = new int [this->coo_nnz];
//            OpenCL::copy(i, this->mat.COO.row, this->coo_nnz*sizeof(int));
//            int * j = new int [this->coo_nnz];
//            OpenCL::copy(j, this->mat.COO.col, this->coo_nnz*sizeof(int));
//            scalar* v = new scalar [this->coo_nnz];
//            OpenCL::copy(v, this->mat.COO.val, this->coo_nnz*sizeof(scalar));

//            for(int w = 0; w < this->coo_nnz; w++)
//                printf("%d, %d, %f\n", i[w], j[w], v[w]);


            // Set arguments for kernel call
           // Kernel k;
            if(sizeof(scalar) == 4)
                k = OpenCL::kernels["s_kernel_coo_spmv_flat"];
            else
                k = OpenCL::kernels["d_kernel_coo_spmv_flat"];
            k.set_int     (0, tail);
            k.set_int     (1, interval_size);
            k.set_buffer  (2, this->mat.COO.row);
            k.set_buffer  (3, this->mat.COO.col);
            k.set_buffer  (4, this->mat.COO.val);
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

//            /** Check Temporary rows and values */
//            int* h_temp_rows = new int [active_warps];
//            scalar* h_temp_vals = new scalar [active_warps];
//            OpenCL::copy(h_temp_rows, temp_rows, active_warps*sizeof(int));
//            OpenCL::copy(h_temp_vals, temp_vals, active_warps*sizeof(scalar));

//            for(int i = 0; i < active_warps; i++)
//            {
//              printf("temp[%d]: %d\t%f\n", i, h_temp_rows[i], h_temp_vals[i]);
//            }
//            delete [] h_temp_rows;
//            delete [] h_temp_vals;

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

//            qDebug() << "After second kernel! \n\n";
//            out.print(out.get_size());
//            qDebug() << "\n\n";




            // Set arguments for kernel call
            if (sizeof(scalar) == 4)
                k = OpenCL::kernels["s_kernel_coo_spmv_serial"];
            else
                k = OpenCL::kernels["d_kernel_coo_spmv_serial"];
            k.set_int(0, get_coo_nnz());
            k.set_buffer(1, this->mat.COO.row);
            k.set_buffer(2, this->mat.COO.col);
            k.set_buffer(3, this->mat.COO.val);
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
            //printf("coo spmv serial kernel release event: %s\n", OpenCL::getError (err).toStdString().c_str ());

            OpenCL::free(&temp_rows);
            OpenCL::free(&temp_vals);

        }

    }
}

template<typename scalar>
int MatrixHYB<scalar, GPU>::get_nnz() const
{
    return this->nnz;
}

template<typename scalar>
int MatrixHYB<scalar, GPU>::get_ncol() const
{
    return this->ncol;
}

template<typename scalar>
int MatrixHYB<scalar, GPU>::get_nrow() const
{
    return this->nrow;
}

template<typename scalar>
int MatrixHYB<scalar,GPU>::get_ell_max_row() const
{
    return this->mat.ELL.max_row;
}

template<typename scalar>
int MatrixHYB<scalar, GPU>::get_ell_nnz() const
{
    return this->ell_nnz;
}

template <typename scalar>
int MatrixHYB<scalar, GPU>::get_coo_nnz() const
{
    return this->coo_nnz;
}


template<typename scalar>
cl_mem MatrixHYB<scalar, GPU>::get_ellColPtr() const
{
    return this->mat.ELL.col;
}

template<typename scalar>
cl_mem MatrixHYB<scalar, GPU>::get_ellValPtr() const
{
    return this->mat.ELL.val;
}

template<typename scalar>
cl_mem MatrixHYB<scalar, GPU>::get_cooColPtr() const
{
    return this->mat.COO.col;
}

template<typename scalar>
cl_mem MatrixHYB<scalar, GPU>::get_cooRowPtr() const
{
    return this->mat.COO.row;
}

template<typename scalar>
cl_mem MatrixHYB<scalar, GPU>::get_cooValPtr() const
{
    return this->mat.COO.val;
}

template class MatrixHYB<int, GPU>;
template class MatrixHYB<float, GPU>;
template class MatrixHYB<double, GPU>;

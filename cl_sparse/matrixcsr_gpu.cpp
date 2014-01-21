#include "matrixcsr.h"

template<typename scalar>
MatrixCSR<scalar, GPU>::MatrixCSR()
{
    this->mat.row_offset = NULL;
    this->mat.col = NULL;
    this->mat.val = NULL;

    this->nnz = 0;
    this->ncol = 0;
    this->nrow = 0;

}


template<typename scalar>
MatrixCSR<scalar, GPU>::MatrixCSR(const int nnz, const int nrow, const int ncol)
{
    assert( nnz   >= 0);
    assert( ncol  >= 0);
    assert( nrow  >= 0);

    //clear();

    if (nnz > 0)
        allocate(nnz, nrow, ncol);

}

template<typename scalar>
MatrixCSR<scalar, GPU>::MatrixCSR(const MatrixCSR<scalar, CPU>& other)
{

    allocate(other.get_nnz(), other.get_nrow(), other.get_ncol());

    OpenCL::copy(this->mat.val, other.get_valPtr(), nnz*sizeof(scalar));
    OpenCL::copy(this->mat.col, other.get_colPtr(), nnz*sizeof(int));
    OpenCL::copy(this->mat.row_offset, other.get_rowPtr(), (nrow+1)*sizeof(int));
}

template<typename scalar>
MatrixCSR<scalar, GPU>::~MatrixCSR()
{
    clear();
}


template<typename scalar>
void MatrixCSR<scalar, GPU>::clear()
{
    if (this->get_nnz())
    {
        OpenCL::free(&this->mat.val);
        OpenCL::free(&this->mat.col);
        OpenCL::free(&this->mat.row_offset);

        this->mat.val = NULL;
        this->mat.col = NULL;
        this->mat.row_offset = NULL;

        this->ncol = 0;
        this->nrow = 0;
        this->nnz = 0;
    }
}

template<typename scalar>
void MatrixCSR<scalar, GPU>::allocate(const int nnz, const int nrow, const int ncol)
{
    assert(nnz >= 0);
    assert(ncol >= 0);
    assert(nrow >= 0);

    //clear();

    if (nnz > 0)
    {
        OpenCL::allocate(&this->mat.val, nnz*sizeof(scalar));
        OpenCL::allocate(&this->mat.col, nnz*sizeof(int));
        OpenCL::allocate(&this->mat.row_offset, (nrow+1)*sizeof(int));

        this->nrow = nrow;
        this->ncol = ncol;
        this->nnz  = nnz;
    }

}

template<typename scalar>
int const MatrixCSR<scalar, GPU>::get_nnz() const
{
    return this->nnz;
}



template<typename scalar>
int const MatrixCSR<scalar, GPU>::get_nrow() const
{
    return this->nrow;
}


template<typename scalar>
int const MatrixCSR<scalar, GPU>::get_ncol() const
{
    return this->ncol;
}


template<typename scalar>
cl_mem const MatrixCSR<scalar, GPU>::get_valPtr() const
{
    return this->mat.val;
}

template<typename scalar>
cl_mem const MatrixCSR<scalar, GPU>::get_rowPtr() const
{
    return this->mat.row_offset;
}

template<typename scalar>
cl_mem const MatrixCSR<scalar, GPU>::get_colPtr() const
{
    return this->mat.col;
}


template<typename scalar>
void MatrixCSR<scalar, GPU>::load(const std::string &fname_mtx)
{
    MatrixCSR<scalar, CPU> m;
    m.load(fname_mtx);

    clear();
    allocate(m.get_nnz(), m.get_nrow(), m.get_ncol());

    OpenCL::copy(this->mat.val, m.get_valPtr(), nnz*sizeof(scalar));
    OpenCL::copy(this->mat.col, m.get_colPtr(), nnz*sizeof(int));
    OpenCL::copy(this->mat.row_offset, m.get_rowPtr(), (nrow+1)*sizeof(int));

}

template<typename scalar>
void MatrixCSR<scalar, GPU>::save(const std::string &fname_mtx)
{
    MatrixCSR<scalar, CPU> m(*this);
    m.save(fname_mtx);

}


template<typename scalar>
void MatrixCSR<scalar,GPU>::multiply(const Vector<scalar, GPU>& in, Vector<scalar, GPU>& out)
{

    assert(in.get_csize() >= 0);
    assert(out.get_size()>= 0);
    assert(in.get_csize() == this->get_ncol());
    assert(out.get_size()== this->get_nrow());

    //const int nnz_per_row = get_nnz() / get_nrow();
    //printf("nnz_per_row: %d\n", nnz_per_row);

    cl_int    err;
    cl_event  ocl_event;
    size_t    localWorkSize[1];
    size_t    globalWorkSize[1];

    //TODO: Tune up those parameters!
    const int WARP_SIZE = 32;
    localWorkSize[0] = WARP_SIZE * 8;
    globalWorkSize[0] = localWorkSize[0] * 128;

    Kernel k;
    if (sizeof(scalar) == 4)
        k = OpenCL::kernels["s_kernel_csr_spmv_vector"];
    else
        k = OpenCL::kernels["d_kernel_csr_spmv_vector"];
    k.set_int(0, this->nrow);
    k.set_buffer(1, this->mat.row_offset);
    k.set_buffer(2, this->mat.col);
    k.set_buffer(3, this->mat.val);
    k.set_buffer(4, in.get_cdata());
    k.set_buffer(5, out.get_data());
    k.set_local(6, (localWorkSize[0]+16)*sizeof(scalar) );

    k.execute(localWorkSize[0], globalWorkSize[0], &ocl_event);

    err = clWaitForEvents( 1, &ocl_event );
    //printf("csr kernel wait event: %s\n", OpenCL::getError (err).toStdString().c_str());

    err = clReleaseEvent( ocl_event );

}


template class MatrixCSR<float, GPU>;
template class MatrixCSR<double, GPU>;













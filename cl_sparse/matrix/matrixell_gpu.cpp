#include "matrixell.h"

template<typename scalar>
MatrixELL<scalar, GPU>::MatrixELL()
{
    this->mat.max_row   = 0;
    this->mat.col       = NULL;
    this->mat.val       = NULL;

    this->nnz  = 0;
    this->ncol = 0;
    this->nrow = 0;
}


template<typename scalar>
MatrixELL<scalar, GPU>::MatrixELL(const MatrixELL<scalar, CPU>& other)
{
    allocate(other.get_nnz(), other.get_nrow(), other.get_ncol(), other.get_max_row());

    OpenCL::copy(this->mat.col, other.get_colPtr(), nnz * sizeof(int));
    OpenCL::copy(this->mat.val, other.get_valPtr(), nnz * sizeof(scalar));

}

template<typename scalar>
MatrixELL<scalar, GPU>::~MatrixELL()
{
    clear();
}

template<typename scalar>
void MatrixELL<scalar, GPU>::clear()
{
    if (this->get_nnz() > 0)
    {
        OpenCL::free(&this->mat.col);
        OpenCL::free(&this->mat.val);

        this->mat.col = NULL;
        this->mat.val = NULL;
        this->mat.max_row = 0;

        this->ncol = 0;
        this->nnz  = 0;
        this->nrow = 0;
    }
}

template<typename scalar>
void MatrixELL<scalar, GPU>::allocate(const int nnz, const int nrow, const int ncol, const int max_row)
{
    assert( nnz   >= 0);
    assert( ncol  >= 0);
    assert( nrow  >= 0);
    assert( max_row >= 0);
    assert( nnz == max_row*nrow);

    OpenCL::allocate(&this->mat.val, nnz * sizeof(scalar));
    OpenCL::allocate(&this->mat.col, nnz * sizeof(int));

    this->mat.max_row = max_row;

    this->nnz  = nnz;
    this->nrow = nrow;
    this->ncol = ncol;

}


template <typename scalar>
int MatrixELL<scalar, GPU>::get_nnz() const
{
    return this->nnz;
}

template <typename scalar>
int MatrixELL<scalar, GPU>::get_nrow() const
{
    return this->nrow;
}

template <typename scalar>
int MatrixELL<scalar, GPU>::get_ncol() const
{
    return this->ncol;
}

template <typename scalar>
int MatrixELL<scalar, GPU>::get_max_row() const
{
    return this->mat.max_row;
}

template <typename scalar>
cl_mem MatrixELL<scalar, GPU>::get_colPtr() const
{
    return this->mat.col;
}

template <typename scalar>
cl_mem MatrixELL<scalar, GPU>::get_valPtr() const
{
    return this->mat.val;
}


template<typename scalar>
void MatrixELL<scalar, GPU>::load (const std::string &fname_mtx)
{

    MatrixCSR<scalar, CPU> matrix;
    matrix.load(fname_mtx);

    MatrixELL<scalar, CPU> ell;
    ell.get_data(matrix);

    clear();
    allocate(ell.get_nnz(), ell.get_nrow(), ell.get_ncol(), ell.get_max_row());
    OpenCL::copy(this->mat.col, ell.get_colPtr(), nnz * sizeof(int));
    OpenCL::copy(this->mat.val, ell.get_valPtr(), nnz * sizeof(scalar));

}

template<typename scalar>
void MatrixELL<scalar, GPU>::save (const std::string &fname_mtx)
{

    MatrixELL<scalar, CPU> ell(*this);
    MatrixCSR<scalar, CPU> matrix;
    //ell-csr
    matrix.get_data(ell);
    matrix.save(fname_mtx);
}

template<typename scalar>
void MatrixELL<scalar, GPU>::multiply(const Vector<scalar, GPU> &in, Vector<scalar, GPU> &out)
{
    assert(in.get_csize() >= 0);
    assert(out.get_size() >= 0);
    assert(in.get_csize() == this->get_ncol());
    assert(out.get_size() == this->get_nrow());


    cl_int    err;
    cl_event  ocl_event;
    size_t    localWorkSize[1];
    size_t    globalWorkSize[1];


    localWorkSize[0] = 512;
    globalWorkSize[0] = ( size_t( nrow / localWorkSize[0] ) + 1 ) * localWorkSize[0];

    Kernel k;
    if (sizeof(scalar) == 4)
        k = OpenCL::kernels["s_kernel_ell_spmv"];
    else
        k = OpenCL::kernels["d_kernel_ell_spmv"];
    k.set_int(0, this->nrow);
    k.set_int(1, this->ncol);
    k.set_int(2, this->mat.max_row);
    k.set_buffer(3, this->mat.col);
    k.set_buffer(4, this->mat.val);
    k.set_buffer(5, in.get_cdata());
    k.set_buffer(6, out.get_data());

    k.execute(localWorkSize[0], globalWorkSize[0], &ocl_event);

    err = clWaitForEvents( 1, &ocl_event );
    //printf("ell kernel wait event: %s\n", OpenCL::getError (err).toStdString().c_str());

    err = clReleaseEvent( ocl_event );
}

template class MatrixELL<float, GPU>;
template class MatrixELL<double, GPU>;

#include "matrixcsr.h"


template<typename scalar>
MatrixCSR<scalar, CPU>::MatrixCSR()
{
    this->mat.row_offset = NULL;
    this->mat.col = NULL;
    this->mat.val = NULL;

    this->nnz = 0;
    this->ncol = 0;
    this->nrow = 0;
}





template<typename scalar>
MatrixCSR<scalar, CPU>::MatrixCSR(const int nnz, const int nrow, const int ncol)
{
    assert( nnz   >= 0);
    assert( ncol  >= 0);
    assert( nrow  >= 0);

    clear();

    if (nnz > 0)
        allocate(nnz, nrow, ncol);

}

template<typename scalar>
MatrixCSR<scalar, CPU>::MatrixCSR(const MatrixCSR<scalar, GPU>& other)
{
    allocate(other.get_nnz(), other.get_nrow(), other.get_ncol());

    OpenCL::copy(this->mat.val, other.get_valPtr(), nnz*sizeof(scalar));
    OpenCL::copy(this->mat.col, other.get_colPtr(), nnz*sizeof(int));
    OpenCL::copy(this->mat.row_offset, other.get_rowPtr(), (nrow+1)*sizeof(int));

}


template<typename scalar>
MatrixCSR<scalar, CPU>::~MatrixCSR()
{
    clear();
}



template<typename scalar>
void MatrixCSR<scalar, CPU>::clear()
{
    if (this->get_nnz())
    {
        delete [] this->mat.val;
        delete [] this->mat.col;
        delete [] this->mat.row_offset;

        this->mat.val = NULL;
        this->mat.col = NULL;
        this->mat.row_offset = NULL;

        this->ncol = 0;
        this->nrow = 0;
        this->nnz = 0;
    }
}



template<typename scalar>
void MatrixCSR<scalar, CPU>::allocate(const int nnz, const int nrow, const int ncol)
{
    assert(nnz >= 0);
    assert(ncol >= 0);
    assert(nrow >= 0);

    clear();

    if (nnz > 0)
    {
        this->mat.val        =  new scalar [nnz];
        this->mat.col        =  new int    [nnz];
        this->mat.row_offset =  new int    [nrow+1];

        this->nrow = nrow;
        this->ncol = ncol;
        this->nnz  = nnz;
    }

}


template<typename scalar>
void MatrixCSR<scalar, CPU>::load(const std::string &fname_mtx)
{
    MatrixCOO<scalar, CPU> matrix;
    matrix.load(fname_mtx);
    get_data(matrix);

}


template<typename scalar>
void MatrixCSR<scalar, CPU>::save(const std::string &fname_mtx)
{
    MatrixCOO<scalar, CPU> matrix;
    matrix.get_data(*this);
    matrix.save(fname_mtx);
}


template<typename scalar>
int const MatrixCSR<scalar, CPU>::get_nnz() const
{
    return this->nnz;
}



template<typename scalar>
int const MatrixCSR<scalar, CPU>::get_nrow() const
{
    return this->nrow;
}


template<typename scalar>
int const MatrixCSR<scalar, CPU>::get_ncol() const
{
    return this->ncol;
}


template<typename scalar>
scalar* const MatrixCSR<scalar, CPU>::get_valPtr() const
{
    return this->mat.val;
}

template<typename scalar>
int* const MatrixCSR<scalar, CPU>::get_rowPtr() const
{
    return this->mat.row_offset;
}

template<typename scalar>
int* const MatrixCSR<scalar, CPU>::get_colPtr() const
{
    return this->mat.col;
}


template<typename scalar>
void MatrixCSR<scalar, CPU>::get_data(const MatrixCOO<scalar, CPU> &matrix)
{
    assert(matrix.get_ncol() > 0);
    assert(matrix.get_nrow() > 0);
    assert(matrix.get_nnz() > 0);

    //from this point nnz, ncol, nrow are allocated and good;
    allocate(matrix.get_nnz(), matrix.get_nrow(), matrix.get_ncol());


    int* coo_row = matrix.get_rowPtr();
    int* coo_col = matrix.get_colPtr();
    scalar* coo_val = matrix.get_valPtr();


    //calculate nnz entries per CSR row
    for ( int i = 0; i < nnz; i++ )
        this->mat.row_offset[ coo_row[i] ] = this->mat.row_offset[ coo_row[i] ] + 1;

    // sum the entries per row to get row_offsets[]
    int sum = 0;
    for (int i = 0; i < nrow; i++)
    {
        int temp = this->mat.row_offset[i];
        this->mat.row_offset[i] = sum;
        sum += temp;
    }
    //last entry
    this->mat.row_offset[nrow] = sum;

    //write cols and values
    for (int i = 0; i < nnz; i++)
    {
        int row = coo_row[i];
        int dest = this->mat.row_offset[row];

        this->mat.col[dest] = coo_col[i];
        this->mat.val[dest] = coo_val[i];
        this->mat.row_offset[row] = this->mat.row_offset[row] + 1;
    }

    int last = 0;
    for (int i = 0; i <= nrow; i++)
    {
        int temp = this->mat.row_offset[i];
        this->mat.row_offset[i] = last;
        last = temp;
    }

}

template<typename scalar>
void MatrixCSR<scalar, CPU>::multiply(const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out)
{
    assert(in.get_size()  >= 0);
    assert(out.get_size() >= 0);
    assert(in.get_size()  == this->get_ncol());
    assert(out.get_size() == this->get_nrow());

    out.set(0);

    const int rows = this->get_nrow();

    scalar* out_data = out.get_data();

    assert(mat.row_offset[rows] == this->get_nnz());
    assert(rows > 0);

    for (int i = 0; i < rows; i++)
    {
        scalar sum = 0;
        for (int j = mat.row_offset[i]; j < mat.row_offset[i+1]; j++)
            sum += mat.val[j] * in[mat.col[j]];

        out_data[i] = sum;

    }

}


template class MatrixCSR<float, CPU>;
template class MatrixCSR<double, CPU>;
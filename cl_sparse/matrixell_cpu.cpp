#include "matrixell.h"

template<typename scalar>
MatrixELL<scalar, CPU>::MatrixELL()
{
    this->mat.val = NULL;
    this->mat.col = NULL;

    this->mat.max_row = 0;
}

template<typename scalar>
MatrixELL<scalar, CPU>::MatrixELL(const int nnz, const int nrow, const int ncol, const int max_row)
{
    assert( nnz   >= 0);
    assert( ncol  >= 0);
    assert( nrow  >= 0);
    assert( max_row >= 0);

    clear();

    if (nnz > 0)
    {
        assert(nnz == max_row * nrow);
        allocate(nnz, nrow, ncol, max_row);
    }
}


template<typename scalar>
MatrixELL<scalar, CPU>::~MatrixELL()
{
    clear();
}

template<typename scalar>
void MatrixELL<scalar, CPU>::clear()
{
    if (this->get_nnz() > 0)
    {
        delete [] this->mat.col;
        delete [] this->mat.val;

        this->mat.col = NULL;
        this->mat.val = NULL;
        this->mat.max_row = 0;

        this->ncol = 0;
        this->nnz  = 0;
        this->nrow = 0;
    }
}

template<typename scalar>
void MatrixELL<scalar, CPU>::allocate(const int nnz, const int nrow, const int ncol, const int max_row)
{
    assert( nnz   >= 0);
    assert( ncol  >= 0);
    assert( nrow  >= 0);
    assert( max_row >= 0);

    clear();

    this->mat.val = new scalar [nnz];
    this->mat.col = new int [nnz];

    this->mat.max_row = max_row;

    this->nnz = nnz;
    this->nrow = nrow;
    this->ncol = ncol;
}

template<typename scalar>
void MatrixELL<scalar, CPU>::load (const std::string &fname_mtx)
{

    MatrixCOO<scalar, CPU> coo;
    coo.load(fname_mtx);
    set_data(coo);
}

template<typename scalar>
void MatrixELL<scalar, CPU>::save (const std::string &fname_mtx)
{

    MatrixCOO<scalar, CPU> coo;
    get_data(coo);
    coo.save(fname_mtx);
}


template<typename scalar>
void MatrixELL<scalar, CPU>::set_data(const MatrixCSR<scalar, CPU> &matrix)
{

}


template<typename scalar>
void MatrixELL<scalar, CPU>::get_data(MatrixCSR<scalar, CPU> &matrix)
{

}

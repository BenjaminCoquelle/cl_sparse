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
    assert( nnz == max_row*nrow);
    clear();

    this->mat.val = new scalar [nnz];
    this->mat.col = new int [nnz];

    this->mat.max_row = max_row;

    this->nnz = nnz;
    this->nrow = nrow;
    this->ncol = ncol;
}

template <typename scalar>
int const MatrixELL<scalar, CPU>::get_nnz() const
{
    return this->nnz;
}

template <typename scalar>
int const MatrixELL<scalar, CPU>::get_nrow() const
{
    return this->nrow;
}

template <typename scalar>
int const MatrixELL<scalar, CPU>::get_ncol() const
{
    return this->ncol;
}

template <typename scalar>
int const MatrixELL<scalar, CPU>::get_max_row() const
{
    return this->mat.max_row;
}

template <typename scalar>
int* const MatrixELL<scalar, CPU>::get_colPtr() const
{
    return this->mat.col;
}

template <typename scalar>
scalar* const MatrixELL<scalar, CPU>::get_valPtr() const
{
    return this->mat.val;
}



template<typename scalar>
void MatrixELL<scalar, CPU>::load (const std::string &fname_mtx)
{

    MatrixCSR<scalar, CPU> matrix;
    matrix.load(fname_mtx);
    get_data(matrix);
}

template<typename scalar>
void MatrixELL<scalar, CPU>::save (const std::string &fname_mtx)
{

    MatrixCSR<scalar, CPU> matrix;
    //ell-csr
    matrix.get_data(*this);
    matrix.save(fname_mtx);
}



//csr -> ell
template<typename scalar>
void MatrixELL<scalar, CPU>::get_data(MatrixCSR<scalar, CPU> &matrix)
{
    assert ( matrix.get_nrow() > 0);
    assert ( matrix.get_ncol() > 0);
    assert ( matrix.get_nnz()  > 0);

    //CSR data pointers
    int*    row_offset  = matrix.get_rowPtr();
    int*    col         = matrix.get_colPtr();
    scalar* val         = matrix.get_valPtr();

    //calculate max row lenght;
    this->mat.max_row = 0;
    for (int i = 0; i < matrix.get_nrow(); i++)
    {
        int max_row = row_offset[i+1] - row_offset[i];
        if (max_row > this->mat.max_row)
            this->mat.max_row = max_row;
    }

    //entries are much more than just nnz;
    int nnz_ell = this->mat.max_row * matrix.get_nrow();

    allocate(nnz_ell, matrix.get_nrow(), matrix.get_ncol(), this->mat.max_row);

    for (int i = 0; i < nrow; i++)
    {
        int n = 0;

        for (int j = row_offset[i]; j < row_offset[i+1]; j++)
        {
            int index = ELL_IND (i, n, nrow, this->mat.max_row);

            this->mat.val[index] = val[j];
            this->mat.col[index] = col[j];
            n++;
        }

        //there can be zeros
        for (int j = row_offset[i+1] - row_offset[i]; j < this->mat.max_row; j++)
        {
            int index = ELL_IND (i, n, nrow, this->mat.max_row);
            this->mat.val[index] = scalar(0.0);
            this->mat.col[index] = int(-999);
            n++;

        }
    }
}

template<typename scalar>
void MatrixELL<scalar, CPU>::multiply(const Vector<scalar, CPU> &in, Vector<scalar, CPU> &out)
{
    assert(in.get_size()  >= 0);
    assert(out.get_size() >= 0);
    assert(in.get_size()  == this->get_ncol());
    assert(out.get_size() == this->get_nrow());

    scalar* out_data = out.get_data();
    //out.set(0);
    for (int ai = 0; ai < this->get_nrow(); ai++)
    {
        out_data[ai] = scalar(0.0);

        for (int n = 0; n < this->get_max_row(); n++)
        {
            int aj = ELL_IND(ai, n , this->get_nrow(), this->get_max_row());
            if ((this->mat.col[aj] >= 0) && (this->mat.col[aj] < this->get_ncol()))
            {
                out_data[ai] += this->mat.val[aj] * in[this->mat.col[aj]];
            }
        }
    }
}


template class MatrixELL<float, CPU>;
template class MatrixELL<double, CPU>;

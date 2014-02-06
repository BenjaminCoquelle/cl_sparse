#include "matrixhyb.h"

template<typename scalar>
MatrixHYB<scalar, CPU>::MatrixHYB()
{
    this->mat.ELL.val = NULL;
    this->mat.ELL.col = NULL;
    this->mat.ELL.max_row = 0;

    this->mat.COO.row = NULL;
    this->mat.COO.col = NULL;
    this->mat.COO.val = NULL;

    this->nrow = 0;
    this->ncol = 0;
    this->nnz = 0;

    this->ell_nnz = 0;
    this->coo_nnz = 0;
}


template<typename scalar>
MatrixHYB<scalar, CPU>::MatrixHYB(const MatrixHYB<scalar, GPU>& other)
{
    fprintf(stdout, "from GPU ctr nnz: %d\n", other.get_nnz());
    allocate(other.get_ell_nnz(), other.get_coo_nnz(), other.get_ell_max_row(), other.get_nrow(), other.get_ncol());

    OpenCL::copy(this->mat.COO.val, other.get_cooValPtr(), coo_nnz*sizeof(scalar));
    OpenCL::copy(this->mat.COO.col, other.get_cooColPtr(), coo_nnz*sizeof(int));
    OpenCL::copy(this->mat.COO.row, other.get_cooRowPtr(), coo_nnz*sizeof(int));

    OpenCL::copy(this->mat.ELL.val, other.get_ellValPtr(), ell_nnz*sizeof(scalar));
    OpenCL::copy(this->mat.ELL.col, other.get_ellValPtr(), ell_nnz*sizeof(int));
}

template<typename scalar>
MatrixHYB<scalar, CPU>::~MatrixHYB()
{
    clear();
}

template<typename scalar>
void MatrixHYB<scalar, CPU>::clear()
{
    if(this->get_nnz())
    {
        delete [] this->mat.ELL.val;
        delete [] this->mat.ELL.col;

        delete [] this->mat.COO.val;
        delete [] this->mat.COO.col;
        delete [] this->mat.COO.row;

        this->ell_nnz = 0;
        this->coo_nnz = 0;
        this->nnz = 0;
        this->ncol = 0;
        this->nrow = 0;


    }
}

template<typename scalar>
void MatrixHYB<scalar, CPU>::allocate(const int ell_nnz, const int coo_nnz, const int ell_max_row, const int nrow, const int ncol)
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

      this->mat.ELL.val = new scalar [ell_nnz];
      this->mat.ELL.col = new int [ell_nnz];

      this->mat.ELL.max_row = ell_max_row;
      this->ell_nnz = ell_nnz;

      // COO
      this->mat.COO.row = new int [coo_nnz];
      this->mat.COO.col = new int [coo_nnz];
      this->mat.COO.val = new scalar [coo_nnz];

      this->coo_nnz = coo_nnz;

      this->nrow = nrow;
      this->ncol = ncol;
      this->nnz  = ell_nnz + coo_nnz;
    }

}

template<typename scalar>
void MatrixHYB<scalar, CPU>::load(const std::string &fname_mtx)
{

    MatrixCSR<scalar, CPU> matrix;
    matrix.load(fname_mtx);
    get_data(matrix);

}

template<typename scalar>
void MatrixHYB<scalar, CPU>::save(const std::string &fname_mtx)
{

    MatrixCSR<scalar, CPU> matrix;
    matrix.get_data(*this);
    matrix.save(fname_mtx);

}


template<typename scalar>
void MatrixHYB<scalar, CPU>::get_data(const MatrixCSR<scalar, CPU>& matrix)
{
    assert(matrix.get_nnz()  > 0);
    assert(matrix.get_ncol() > 0);
    assert(matrix.get_nrow() > 0);

    this->ncol = matrix.get_ncol();
    this->nrow = matrix.get_nrow();

    int* row_offset = matrix.get_rowPtr();
    int* col        = matrix.get_colPtr();
    scalar* val     = matrix.get_valPtr();

    if (this->mat.ELL.max_row == 0)
        this->mat.ELL.max_row = matrix.get_nnz() / matrix.get_nrow();

    this->coo_nnz = 0;
    for (int i = 0; i < matrix.get_nrow(); i++)
    {
        int nnz_per_row = row_offset[i+1] - row_offset[i];

        if (nnz_per_row > this->mat.ELL.max_row)
            this->coo_nnz += nnz_per_row - this->mat.ELL.max_row;
    }

    this->ell_nnz = this->mat.ELL.max_row * matrix.get_nrow();
    this->nnz = coo_nnz + ell_nnz;

    assert(nnz > 0);
    assert(ell_nnz > 0);
    assert(coo_nnz > 0);

    //ELL
    this->mat.ELL.val = new scalar [ell_nnz];
    this->mat.ELL.col = new int    [ell_nnz];

    //TODO: Check if that is necessary
    for(int i = 0; i < ell_nnz; i++)
    {
        this->mat.ELL.val[i] = scalar(0.0);
        this->mat.ELL.col[i] = int(-1);
    }

    //COO
    this->mat.COO.val = new scalar [coo_nnz];
    this->mat.COO.col = new int    [coo_nnz];
    this->mat.COO.row = new int    [coo_nnz];

    int* nnzcoo = new int [nrow];
    int* nnzell = new int [nrow];

    //ELL will store num_cols_per_row
    for (int i = 0; i < nrow; i++)
    {
        int n = 0;
        for(int j = row_offset[i]; j < row_offset[i+1]; j++)
        {
            if (n >= this->mat.ELL.max_row)
            {
                nnzcoo[i] = row_offset[i+1] - j;
                break;
            }

            nnzcoo[i] = 0;
            int index = ELL_IND(i, n, nrow, this->mat.ELL.max_row);
            this->mat.ELL.col[index] = col[j];
            this->mat.ELL.val[index] = val[j];
            n++;
        }

        nnzell[i] = n;
    }

    for (int i = 1; i < nrow; i++)
        nnzell[i] += nnzell[i-1];

    // copy rest values in row into coo;
    for (int i = 0; i < nrow; i++)
        for (int j = row_offset[i+1] - nnzcoo[i]; j < row_offset[i+1]; j++)
        {
            this->mat.COO.row[j - nnzell[i]] = i;
            this->mat.COO.col[j - nnzell[i]] = col[j];
            this->mat.COO.val[j - nnzell[i]] = val[j];
        }
    delete [] nnzell;
    delete [] nnzcoo;
}

template<typename scalar>
void MatrixHYB<scalar, CPU>::multiply(const Vector<scalar, CPU> &in, Vector<scalar, CPU> &out)
{
    if (get_nnz() > 0)
    {
//         fprintf(stdout, "\nMATRIX HYB CPU\n");
//        for(int w = 0; w < this->ell_nnz; w++)
//            printf("%d, %f\n", this->mat.ELL.col[w], this->mat.ELL.val[w]);
//        fprintf(stdout, "\n\n");


        assert(in.get_csize() >= 0);
        assert(out.get_size() >= 0);
        assert(in.get_csize() == get_ncol());
        assert(out.get_size() == get_nrow());

        scalar* out_data = out.get_data();
        const scalar* in_data  = in.get_cdata();
        //ell
        if (get_ell_nnz() > 0)
        {
            for (int ai = 0; ai < this->get_nrow(); ai++)
            {
                out_data[ai] = scalar(0.0);

                for (int n = 0; n < this->get_ell_max_row(); n++)
                {
                    int aj = ELL_IND(ai, n , this->get_nrow(), this->get_ell_max_row());
                    if ((this->mat.ELL.col[aj] >= 0) && (this->mat.ELL.col[aj] < this->get_ncol()))
                    {
                        out_data[ai] += this->mat.ELL.val[aj] * in_data[this->mat.ELL.col[aj]];
                    }
                }
            }
        }

        //coo
        if (get_coo_nnz() > 0)
        for (int i = 0; i < get_coo_nnz(); ++i)
            out_data[this->mat.COO.row[i]] += this->mat.COO.val[i] * in_data[ this->mat.COO.col[i] ];

    }
}

template<typename scalar>
int MatrixHYB<scalar, CPU>::get_nnz() const
{
    return this->nnz;
}

template<typename scalar>
int MatrixHYB<scalar, CPU>::get_ncol() const
{
    return this->ncol;
}

template<typename scalar>
int MatrixHYB<scalar, CPU>::get_nrow() const
{
    return this->nrow;
}

template<typename scalar>
int MatrixHYB<scalar,CPU>::get_ell_max_row() const
{
    return this->mat.ELL.max_row;
}

template<typename scalar>
int MatrixHYB<scalar, CPU>::get_ell_nnz() const
{
    return this->ell_nnz;
}

template <typename scalar>
int MatrixHYB<scalar, CPU>::get_coo_nnz() const
{
    return this->coo_nnz;
}

template<typename scalar>
int* MatrixHYB<scalar, CPU>::get_ellColPtr() const
{
    return this->mat.ELL.col;
}

template<typename scalar>
scalar* MatrixHYB<scalar, CPU>::get_ellValPtr() const
{
    return this->mat.ELL.val;
}

template<typename scalar>
int * MatrixHYB<scalar, CPU>::get_cooColPtr() const
{
    return this->mat.COO.col;
}

template<typename scalar>
int* MatrixHYB<scalar, CPU>::get_cooRowPtr() const
{
    return this->mat.COO.row;
}

template<typename scalar>
scalar* MatrixHYB<scalar, CPU>::get_cooValPtr() const
{
    return this->mat.COO.val;
}


template class MatrixHYB<float, CPU>;
template class MatrixHYB<double, CPU>;

#include "matrixcoo.h"
#include "mmio.h"



/**
  Helper class for reading matrix from MM format
  Keeps only one row from file.
*/
template <typename scalar>
class coo_element
{
public:
    scalar val;
    int row;
    int col;

    coo_element()
    {
        _valid = false;
    }

    coo_element(int r, int c, scalar v)
    {
        row = r;
        col = c;
        val = v;
        _valid = true;
    }
private:
    bool _valid;
};


/**
    Sorter to convert matrix from COO to CSR format
*/
template <typename scalar>
class coo_sort_ascending
{
public:
    inline
    bool operator()(coo_element<scalar> const & e1,
                    coo_element<scalar> const & e2)
    {
        if (e1.row == e2.row)
        {
            return (e1.col < e2.col);
        }
        else
        {
            return (e1.row < e2.row);
        }
    }
};

/**
    To get rid of the warinings proper read format have to be used.
  */
template < typename scalar >
int readline(FILE* f, int& row, int& column, scalar& v) { return -1; }

template<>
int readline<double>(FILE* f, int&row, int& column, double& v)
{
    return fscanf(f, "%d %d %lg\n", &row, &column, &v);
}

template<>
int readline<float>(FILE* f, int&row, int& column, float& v)
{
    return fscanf(f, "%d %d %g\n", &row, &column, &v);
}

template<>
int readline<int>(FILE *f, int &row, int &column,  int &v)
{
    return fscanf(f, "%d %d %d\n", &row, &column, &v);
}


template<typename scalar>
MatrixCOO<scalar, CPU>::MatrixCOO()
{
    this->mat.row = 0;
    this->mat.col = 0;
    this->mat.val = 0;

    this->nnz = 0;
    this->nrow = 0;
    this->ncol = 0;
}

template<typename scalar>
MatrixCOO<scalar, CPU>::MatrixCOO(const int* row, const int* col, const scalar* val, const int nnz, const int nrow, const int ncol)
{
    assert( nnz   >= 0);
    assert( ncol  >= 0);
    assert( nrow  >= 0);

    this->nnz = nnz;
    this->nrow = nrow;
    this->ncol = ncol;

    allocate(nnz, nrow, ncol);

    memcpy(this->mat.row, row, this->nnz*sizeof(int));
    memcpy(this->mat.col, col, this->nnz*sizeof(int));
    memcpy(this->mat.val, val, this->nnz*sizeof(scalar));


}

template<typename scalar>
MatrixCOO<scalar, CPU>::MatrixCOO(const MatrixCOO<scalar, GPU>& other)
{
    //clear();
    allocate(other.get_nnz(), other.get_nrow(), other.get_ncol());

    OpenCL::copy(this->mat.row, other.get_rowPtr(), nnz*sizeof(int));
    OpenCL::copy(this->mat.col, other.get_colPtr(), nnz*sizeof(int));
    OpenCL::copy(this->mat.val, other.get_valPtr(), nnz*sizeof(scalar));
}

template<typename scalar>
MatrixCOO<scalar, CPU>::~MatrixCOO()
{
    clear();
}

template<typename scalar>
void MatrixCOO<scalar, CPU>::allocate(const int nnz, const int nrow, const int ncol)
{
    assert( nnz   >= 0);
    assert( ncol  >= 0);
    assert( nrow  >= 0);

    this->mat.row = new int [nnz];
    this->mat.col = new int [nnz];
    this->mat.val = new scalar [nnz];

    this->nnz = nnz;
    this->ncol = ncol;
    this->nrow = nrow;
}

template<typename scalar>
void MatrixCOO<scalar, CPU>::clear()
{
    if(this->get_nnz() > 0)
    {
        free(mat.row);
        free(mat.col);
        free(mat.val);

        this->mat.row = NULL;
        this->mat.col = NULL;
        this->mat.val = NULL;

        this->nrow = 0;
        this->ncol = 0;
        this->nnz = 0;

    }
}


template<typename scalar>
int const MatrixCOO<scalar, CPU>::get_nnz() const
{
    return this->nnz;
}

template<typename scalar>
int const MatrixCOO<scalar, CPU>::get_nrow() const
{
    return this->nrow;
}

template<typename scalar>
int const MatrixCOO<scalar, CPU>::get_ncol() const
{
    return this->ncol;
}

template<typename scalar>
int* const MatrixCOO<scalar, CPU>::get_rowPtr() const
{
    return this->mat.row;
}

template<typename scalar>
int* const MatrixCOO<scalar, CPU>::get_colPtr() const
{
    return this->mat.col;
}

template<typename scalar>
scalar* const MatrixCOO<scalar, CPU>::get_valPtr() const
{
    return this->mat.val;
}

template<typename scalar>
void MatrixCOO<scalar, CPU>::info()
{
    printf("\n\tCPU Matrix, rows: %d, cols: %d, vals: %d\n", get_nrow(), get_ncol(), get_nnz());
}

template<typename scalar>
void MatrixCOO<scalar, CPU>::load(const std::string& fname_mtx)
{
    MM_typecode matcode;
    FILE* f;
    bool is_integer = false;


    if ((f = fopen(fname_mtx.c_str(), "r")) == NULL)
    {
        printf("Error (load (CPU)): Can't open file %s.\n", fname_mtx.c_str());
        return;
    }

    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Error (load (CPU)): Could not process matrix market banner!\n");
        return;
    }

    //we support only sparse, matrix in coordinate format
    if (mm_is_matrix(matcode) && mm_is_sparse(matcode) && mm_is_coordinate(matcode) && (mm_is_general(matcode)
        || mm_is_symmetric(matcode)) && !mm_is_pattern(matcode))
    {
        if (mm_is_integer(matcode))
        {

            is_integer = true;
        }

    }
    else
    {
        printf("Error (load (CPU)): Matrix banner: %s is unsupported!\n", mm_typecode_to_str(matcode));
        return;
    }

    /* find out size of sparse matrix .... */
    int ret_code = mm_read_mtx_crd_size(f, &nrow, &ncol, &nnz);
    if ( ret_code != 0)
    {
        printf("Error (load (CPU)): Can not obtain matrix size");
    }

    //printf("INFO: %s, [%s]: rows: %d, cols: %d, nnz: %d.\n", fname_mtx.c_str(), mm_typecode_to_str(matcode), nrow, ncol, nnz);

    scalar v;
    int row, column;

    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"      */
    /*       specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*       (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)           */
    std::vector<coo_element<scalar> > coo_matrix(nnz);

    //read matrix in coo format
    for (int i = 0; i < nnz; i++)
    {
        if (is_integer)
        {
            //will convert to scalar
            int iv = 0;
            ret_code = readline<int>(f, row, column, iv);
            v = scalar(iv);
        }
        else
        {
            ret_code = readline<scalar>(f, row, column, v);
        }
        if (ret_code == 0)
        {
            printf("Warning (load CPU): Problem with reading file %s at row %d (%d).\n", fname_mtx.c_str(), i, ret_code);
            exit(i);
        }

        //zero based index
        row--;
        column--;
        coo_matrix[i] = coo_element<scalar>(row, column, v);
    }

    fclose(f);

    //Expand symmetrix matrix if necessary
    if (mm_is_symmetric(matcode))
    {
        for (int i = 0; i < nnz; i++)
        {
            if (coo_matrix[i].row != coo_matrix[i].col)
            {
                int r = coo_matrix[i].col;
                int c = coo_matrix[i].row;
                coo_matrix.push_back(coo_element<scalar>(r, c, coo_matrix[i].val));
            }
        }
    }

    std::sort (coo_matrix.begin(), coo_matrix.end(), coo_sort_ascending<scalar>());
    allocate(nnz, nrow, ncol);

    for(int i = 0; i < nnz; i++)
    {
        this->mat.row[i] = coo_matrix[i].row;
        this->mat.col[i] = coo_matrix[i].col;
        this->mat.val[i] = coo_matrix[i].val;

        //printf("row: %d, col: %d, val: %f\n", hr[i], hc[i], hv[i]);
    }


    //printf("loading of filename= %s. Done\n", fname_mtx.c_str());
}

template<typename scalar>
void MatrixCOO<scalar, CPU>::save(const std::string& fname_mtx)
{
    MM_typecode matcode;
    FILE *f;

    printf("save matrix to filename= %s\n", fname_mtx.c_str());

    if ((f = fopen(fname_mtx.c_str(), "w")) == NULL) {
      printf("WriteFileMTX cannot open file %s\n", fname_mtx.c_str());
      return;
    }

    mm_initialize_typecode(&matcode);
    mm_set_matrix(&matcode);
    mm_set_coordinate(&matcode);
    mm_set_real(&matcode);

    mm_write_banner(f, matcode);

    //  mm_write_mtx_crd_size(f, this->get_ncol(), this->get_nrow(), this->get_nnz());
    mm_write_mtx_crd_size(f, this->get_nrow(), this->get_ncol(), this->get_nnz());

    /* NOTE: matrix market files use 1-based indices, i.e. first element
       of a vector has index 1, not 0.  */
    int nnz = this->get_nnz();
    for (int i=0; i < nnz; i++)
    {

          fprintf(f, "%d %d %f\n",
                  this->mat.row[i]+1,
                  this->mat.col[i]+1,
                  this->mat.val[i]);
    }

    printf("saving of filename=%s completed\n", fname_mtx.c_str());

    fclose(f);
}

template<typename scalar>
double MatrixCOO<scalar, CPU>::get_troughput(const double& time)
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
void MatrixCOO<scalar, CPU>::multiply(const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out)
{
    assert(in.get_size()  >= 0);
    assert(out.get_size() >= 0);
    assert(in.get_size()  == this->get_ncol());
    assert(out.get_size() == this->get_nrow());

    int nnz = this->get_nnz();

    //zero out vector;
    out.set(0);

    scalar* out_data = out.get_data();
    const scalar* in_data  = in.get_cdata();
    for (int i = 0; i < nnz; ++i)
        out_data[this->mat.row[i]] += this->mat.val[i] * in_data[ this->mat.col[i] ];

    //printf("%f\t%f\n",this->mat.val[nnz-2] * in_data[ this->mat.col[nnz-2] ], out->get_data()[this->mat.row[nnz-2]] );

}

template<typename scalar>
void MatrixCOO<scalar, CPU>::get_data(const MatrixCSR<scalar, CPU> &matrix)
{
    assert ( matrix.get_nrow() > 0);
    assert ( matrix.get_ncol() > 0);
    assert ( matrix.get_nnz()  > 0);

    allocate(matrix.get_nnz(), matrix.get_nrow(), matrix.get_ncol());

    int* row_offset = matrix.get_rowPtr();
    int* col = matrix.get_colPtr();
    scalar* val = matrix.get_valPtr();

    for (int i = 0; i < nrow; i++)
        for (int j = row_offset[i]; j < row_offset[i+1]; j++)
            this->mat.row[j] = i;

    for (int i = 0; i < nnz; i++)
        this->mat.col[i] = col[i];

    for (int i = 0; i < nnz; i++)
        this->mat.val[i] = val[i];
}

template class MatrixCOO<float, CPU>;
template class MatrixCOO<double, CPU>;

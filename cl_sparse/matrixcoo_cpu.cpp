#include "matrixcoo.h"
#include "mmio.h"

template<typename scalar>
MatrixCOO<scalar, CPU>::MatrixCOO()
{
    this->mat.row = NULL;
    this->mat.col = NULL;
    this->mat.val = NULL;

    this->nnz = 0;
    this->nrow = 0;
    this->ncol = 0;
}

template<typename scalar>
MatrixCOO<scalar, CPU>::MatrixCOO(const int* row, const int* col, const scalar* val, int nnz, int nrow, int ncol)
{
    this->nnz = nnz;
    this->nrow = nrow;
    this->ncol = ncol;

    //clear is called in allocate
    allocate(nnz, nrow, ncol);

    memcpy(this->mat.row, row, this->nnz*sizeof(int));
    memcpy(this->mat.col, col, this->nnz*sizeof(int));
    memcpy(this->mat.val, val, this->nnz*sizeof(scalar));


}

template<typename scalar>
MatrixCOO<scalar, CPU>::~MatrixCOO()
{
    clear();
}

template<typename scalar>
void MatrixCOO<scalar, CPU>::allocate(int nnz, int nrow, int ncol)
{
    clear();
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
        delete [] this->mat.row;
        delete [] this->mat.col;
        delete [] this->mat.val;

        this->mat.row = 0;
        this->mat.col = 0;
        this->mat.val = 0;

        this->nrow = 0;
        this->ncol = 0;
        this->nnz = 0;
    }
}


template<typename scalar>
int MatrixCOO<scalar, CPU>::get_nnz()
{
    return this->nnz;
}

template<typename scalar>
int MatrixCOO<scalar, CPU>::get_nrow()
{
    return this->nrow;
}

template<typename scalar>
int MatrixCOO<scalar, CPU>::get_ncol()
{
    return this->ncol;
}

template<typename scalar>
void MatrixCOO<scalar, CPU>::info()
{
    printf("\n\tCPU Matrix, rows: %d, cols: %d, vals: %d\n", get_nrow(), get_ncol(), get_nnz());
}

template<typename scalar>
void MatrixCOO<scalar, CPU>::load(const std::string& fname_mtx)
{
    // Follow example_read.c (from Matrix Market web site)
    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N;
    int fnz, nnz;  // file nnz, real nnz
    bool sym = false;

    printf("loading %s\n", fname_mtx.c_str());

    if ((f = fopen(fname_mtx.c_str(), "r")) == NULL) {
      printf("ReadFileMTX cannot open file:  %s\n", fname_mtx.c_str());
      return;
    }

    if (mm_read_banner(f, &matcode) != 0)
      {
        printf("ReadFileMTX could not process Matrix Market banner.\n");
        return;
      }


    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) &&
        mm_is_sparse(matcode) )
      {
        printf("ReadFileMTX does not support Market Market type: %s\n", mm_typecode_to_str(matcode));
        return;
      }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &fnz)) !=0) {
      printf("ReadFileMTX matrix size error\n");
      return;
    }

    nnz = fnz ;

    /* reseve memory for matrices */
    if(mm_is_symmetric(matcode)) {

      if (N != M) {
        printf("ReadFileMTX non-squared symmetric matrices are not supported\n");
        printf("What is symmetric and non-squared matrix? e-mail me\n");
        return;
      }

      nnz = 2*(nnz - N) + N;
      sym = true ;
    }

    this->allocate(nnz,M,N);

    int ii=0;
    int col, row;
    scalar val;
    int ret;
    for (int i=0; i<fnz; ++i) {
        if(sizeof(val) == 8)
            ret = fscanf(f, "%d %d %lg\n", &row, &col, &val);
        else
            ret = fscanf(f, "%d %d %f\n", &row, &col, &val);

      if (!ret)
      {
          printf("Problem with reading %s at row %d\n", fname_mtx.c_str(), row);
          return;
      }


      row--; /* adjust from 1-based to 0-based */
      col--;

      assert (ret == 3);

      //    LOG_INFO(row << " " << col << " " << val);

      this->mat.row[ii] = row;
      this->mat.col[ii] = col;
      this->mat.val[ii] = val;

      if (sym && (row!=col)) {
        ++ii;
        this->mat.row[ii] = col;
        this->mat.col[ii] = row;
        this->mat.val[ii] = val;
      }

      ++ii;
    }

    printf("ReadFileMTX: filename= %s. Done\n", fname_mtx.c_str());

    fclose(f);
}

template<typename scalar>
void MatrixCOO<scalar, CPU>::save(const std::string& fname_mtx)
{
    MM_typecode matcode;
    FILE *f;

    printf("WriteFileMTX: filename= %s; writing...\n", fname_mtx.c_str());

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
      fprintf(f, "%d %d %f\n",
              this->mat.row[i]+1,
              this->mat.col[i]+1,
              this->mat.val[i]);

    printf("WriteFileMTX: filename=%s; done\n", fname_mtx.c_str());

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
void MatrixCOO<scalar, CPU>::multiply(Vector<scalar, CPU>& in, Vector<scalar, CPU>* out)
{
    assert(in.  get_size() >= 0);
    assert(out->get_size() >= 0);
    assert(in.  get_size() == this->get_ncol());
    assert(out->get_size() == this->get_nrow());

    int nnz = this->get_nnz();

    //zero out vector;
    out->set(0);

    scalar* out_data = out->get_data();
    scalar* in_data  = in.get_data();
    for (int i = 0; i < nnz; ++i)
        out_data[this->mat.row[i]] += this->mat.val[i] * in_data[ this->mat.col[i] ];

    //printf("%f\t%f\n",this->mat.val[nnz-2] * in_data[ this->mat.col[nnz-2] ], out->get_data()[this->mat.row[nnz-2]] );

}


template class MatrixCOO<float, CPU>;
template class MatrixCOO<double, CPU>;

#include "matrixcoo.h"
#include "mmio.h"
#include <algorithm>

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
                                  int nnz, int nrow, int ncol)
{
    //clear is called in allocate
    allocate(nnz, nrow, ncol);


    // lenght of the tables is the same! = nnz!
    OpenCL::copy (this->mat.row, row, nnz*sizeof(int));
    OpenCL::copy (this->mat.col, col, nnz*sizeof(int));
    OpenCL::copy (this->mat.val, val, nnz*sizeof(scalar));


}

template<typename scalar>
MatrixCOO<scalar, GPU>::~MatrixCOO()
{
    clear();
}

template<typename scalar>
void MatrixCOO<scalar, GPU>::allocate(int nnz, int nrow, int ncol)
{
    clear();
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
int MatrixCOO<scalar, GPU>::get_nnz()
{
    return this->nnz;
}

template<typename scalar>
int MatrixCOO<scalar, GPU>::get_nrow()
{
    return this->nrow;
}

template<typename scalar>
int MatrixCOO<scalar, GPU>::get_ncol()
{
    return this->ncol;
}

template<typename scalar>
void MatrixCOO<scalar, GPU>::info()
{
    printf("\n\tGPU Matrix, rows: %d, cols: %d, vals: %d\n", get_nrow(), get_ncol(), get_nnz());
}

template<typename scalar>
void MatrixCOO<scalar, GPU>::load(const std::string& fname_mtx)
{
    MM_typecode matcode;
    FILE* f;

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
        || mm_is_symmetric(matcode)) && !mm_is_pattern(matcode))  { }
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

    printf("INFO: %s, [%s]: rows: %d, cols: %d, nnz: %d.\n", fname_mtx.c_str(), mm_typecode_to_str(matcode), nrow, ncol, nnz);

    scalar v;
    int row, column;

    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"      */
    /*       specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*       (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)           */
    std::vector<coo_element<scalar> > coo_matrix(nnz);

    //read matrix in coo format
    for (int i = 0; i < nnz; i++)
    {
        ret_code = readline<scalar>(f, row, column, v);
        if (ret_code == 0) printf("Warning (load CPU): Problem with reading file at row %d.\n", i);
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
    //COPY Host matrix to device matrix;
    int *hr = new int [nnz];
    int *hc = new int [nnz];
    scalar *hv = new scalar [nnz];

    for(int i = 0; i < nnz; i++)
    {
        hr[i] = coo_matrix[i].row;
        hc[i] = coo_matrix[i].col;
        hv[i] = coo_matrix[i].val;

        printf("row: %d, col: %d, val: %f\n", hr[i], hc[i], hv[i]);
    }

    // nnz, nrow, ncol is filled with proper numbers therefore clear() in allocate will throws an error
    //this->allocate(nnz, nrow, ncol);
    // we will call allocate functions explicitly
    OpenCL::allocate (&this->mat.row, nnz * sizeof(int));
    OpenCL::allocate (&this->mat.col, nnz * sizeof(int));
    OpenCL::allocate (&this->mat.val, nnz * sizeof(scalar));




    OpenCL::copy (this->mat.row, hr, this->nnz * sizeof(int));
    OpenCL::copy (this->mat.col, hc, this->nnz * sizeof(int));
    OpenCL::copy (this->mat.val, hv, this->nnz * sizeof(scalar));

    delete [] hr;
    delete [] hc;
    delete [] hv;
    info();

    printf("ReadFileMTX: filename= %s. Done\n", fname_mtx.c_str());

// Follow example_read.c (from Matrix Market web site)
//    int ret_code;
//    MM_typecode matcode;
//    FILE *f;
//    int M, N;
//    int fnz, nnz;  // file nnz, real nnz
//    bool sym = false;

//    printf("ReadFileMTX: filename= %s. reading...\n", fname_mtx.c_str());

//    if ((f = fopen(fname_mtx.c_str(), "r")) == NULL) {
//      printf("ReadFileMTX cannot open file:  %s\n", fname_mtx.c_str());
//      return;
//    }

//    if (mm_read_banner(f, &matcode) != 0)
//      {
//        printf("ReadFileMTX could not process Matrix Market banner.\n");
//        return;
//      }


//    /*  This is how one can screen matrix types if their application */
//    /*  only supports a subset of the Matrix Market data types.      */

//    if (mm_is_complex(matcode) && mm_is_matrix(matcode) &&
//        mm_is_sparse(matcode) )
//      {
//        printf("ReadFileMTX does not support Market Market type: %s\n", mm_typecode_to_str(matcode));
//        return;
//      }

//    /* find out size of sparse matrix .... */

//    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &fnz)) !=0) {
//      printf("ReadFileMTX matrix size error\n");
//      return;
//    }

//    nnz = fnz ;

//    /* reseve memory for matrices */
//    if(mm_is_symmetric(matcode)) {

//      if (N != M) {
//        printf("ReadFileMTX non-squared symmetric matrices are not supported\n");
//        printf("What is symmetric and non-squared matrix? e-mail me\n");
//        return;
//      }

//      nnz = 2*(nnz - N) + N;
//      sym = true ;
//    }

//    //create Host COO Matrix
//    MatrixCOO<scalar, CPU>* m = new MatrixCOO<scalar, CPU>();
//    m->allocate(nnz,M,N);

//    m->info();

//    int ii=0;
//    int col, row;
//    scalar val;
//    int ret;
//    for (int i=0; i<fnz; ++i) {
//        if(sizeof(val) == 8)
//            ret = fscanf(f, "%d %d %lg\n", &row, &col, &val);
//        else
//            ret = fscanf(f, "%d %d %f\n", &row, &col, &val);

//      if (!ret)
//      {
//          printf("Problem with reading %s at row %d\n", fname_mtx.c_str(), row);
//          return;
//      }


//      row--; /* adjust from 1-based to 0-based */
//      col--;

//      assert (ret == 3);

//      //    LOG_INFO(row << " " << col << " " << val);
//      //printf("row: %d, col: %d, val: %f\n", row, col, val);
//      m->mat.row[ii] = row;
//      m->mat.col[ii] = col;
//      m->mat.val[ii] = val;

//      if (sym && (row!=col)) {
//        ++ii;
//        m->mat.row[ii] = col;
//        m->mat.col[ii] = row;
//        m->mat.val[ii] = val;
//      }

//      ++ii;
//    }

//    //COPY Host matrix to device matrix;
//    this->allocate(nnz, M, N);
//    OpenCL::copy (this->mat.row, m->mat.row, this->nnz * sizeof(int));
//    OpenCL::copy (this->mat.col, m->mat.col, this->nnz * sizeof(int));
//    OpenCL::copy (this->mat.val, m->mat.val, this->nnz * sizeof(scalar));

//    info();

//    delete m;
//    printf("ReadFileMTX: filename= %s. Done\n", fname_mtx.c_str());

//    fclose(f);
}

template<typename scalar>
void MatrixCOO<scalar, GPU>::save(const std::string& fname_mtx)
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

    int* row = new int [this->nnz];
    int* col = new int [this->nnz];
    scalar* val = new scalar [this->nnz];

    //for( int i = 0; i < this->get_nrow(); i++)
    //    printf("%d, row[i]: %d, \t col[i]: %d\n", i, row[i], col[i]);

    OpenCL::copy(row, this->mat.row, this->nnz * sizeof(int));
    OpenCL::copy(col, this->mat.col, this->nnz * sizeof(int));
    OpenCL::copy(val, this->mat.val, this->nnz * sizeof(scalar));

   // for( int i = 0; i < this->get_nrow(); i++)
    //    printf("%d, row[i]: %d, \t col[i]: %d\n", i, row[i], col[i]);

    /* NOTE: matrix market files use 1-based indices, i.e. first element
       of a vector has index 1, not 0.  */
    int nnz = this->get_nnz();
    for (int i=0; i < nnz; i++)
      fprintf(f, "%d %d %f\n",
              row[i]+1,
              col[i]+1,
              val[i]);

    printf("WriteFileMTX: filename=%s; done\n", fname_mtx.c_str());

    delete [] row;
    delete [] col;
    delete [] val;

    fclose(f);
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
void MatrixCOO<scalar, GPU>::multiply(Vector<scalar, GPU> &in, Vector<scalar, GPU> *out)
{
    if (this->get_nnz() > 0)
    {
        assert(in.  get_size() >= 0);
        assert(out->get_size() >= 0);
        assert(in.  get_size() == this->get_ncol());
        assert(out->get_size() == this->get_nrow());

        qDebug() << "Matrix NNZ: " << this->get_nnz() << "\n";

        /** TODO:: DO I have to zero it */
        out->set(0.f);
        qDebug() << " out in multip: "; out->print();

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

        qDebug() <<"WARP_SIZE =" << OPENCL_WARPSIZE << "\n";
        qDebug() <<"active_warps = " << active_warps;
        qDebug() <<"tail =" << tail;
        qDebug() <<"interval_size =" << interval_size;
        qDebug() <<"num_iters =" << num_iters;
        qDebug() <<"num_blocks =" << num_blocks;
        qDebug() <<"num_warps =" << num_warps;
        qDebug() <<"num_units =" << num_units;
        qDebug() <<"WARPS_PER_BLOCK =" << WARPS_PER_BLOCK;
        qDebug() <<"MAX_BLOCKS =" << MAX_BLOCKS;
        qDebug() <<"BLOCK_SIZE =" << BLOCK_SIZE;

        cl_int    err;
        cl_event  ocl_event;
        size_t    localWorkSize[1];
        size_t    globalWorkSize[1];

        scalar s = 1.0;

        int * i = new int [this->nnz];
        OpenCL::copy(i, this->mat.row, this->nnz*sizeof(int));
        int * j = new int [this->nnz];
        OpenCL::copy(j, this->mat.col, this->nnz*sizeof(int));
        scalar* v = new scalar [this->nnz];
        OpenCL::copy(v, this->mat.val, this->nnz*sizeof(int));

        for(int w = 0; w < this->nnz; w++)
            printf("%d, %d, %f\n", i[w], j[w], v[w]);


        // Set arguments for kernel call
        Kernel k = OpenCL::kernels["s_kernel_coo_spmv_flat"];
        k.set_int     (0, tail);
        k.set_int     (1, interval_size);
        k.set_buffer  (2, this->mat.row);
        k.set_buffer  (3, this->mat.col);
        k.set_buffer  (4, this->mat.val);
        if (sizeof(scalar) == 4)
        {
            printf("\nSetting float\n");
            k.set_float (5, s);
        }
          else
        {
            printf("\nSetting double\n");
            k.set_double (5, s);
        }
        k.set_buffer (6, in.get_data());
        k.set_buffer (7, out->get_data());
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
        int* h_temp_rows = new int [active_warps];
        scalar* h_temp_vals = new scalar [active_warps];
        OpenCL::copy(h_temp_rows, temp_rows, active_warps*sizeof(int));
        OpenCL::copy(h_temp_vals, temp_vals, active_warps*sizeof(scalar));

        for(int i = 0; i < active_warps; i++)
        {
          printf("temp[%d]: %d\t%f\n", i, h_temp_rows[i], h_temp_vals[i]);
        }
        delete [] h_temp_rows;
        delete [] h_temp_vals;

        /** Checke out vector after first kernel*/
        qDebug() << "After first kernel! \n\n";
        out->print(out->get_size());
        qDebug() << "\n\n";



        // Set arguments for kernel call
        k = OpenCL::kernels["s_kernel_coo_spmv_reduce_update"];
        k.set_int     (0, active_warps);
        k.set_buffer  (1, temp_rows);
        k.set_buffer  (2, temp_vals);
        k.set_buffer  (3, out->get_data());

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


        int nnz = this->get_nnz();

        // Set arguments for kernel call
        k = OpenCL::kernels["s_kernel_coo_spmv_serial"];
        k.set_int(0, nnz);
        k.set_buffer(1, this->mat.row);
        k.set_buffer(2, this->mat.col);
        k.set_buffer(3, this->mat.val);
        if (sizeof(scalar) == 4)
          k.set_float (4, s);
        else
         k.set_double (4, s);
        k.set_buffer (5, in.get_data());
        k.set_buffer (6, out->get_data());
        k.set_int(7, tail);

        localWorkSize[0] = 1;
        globalWorkSize[0] = 1;

        k.execute(localWorkSize[0], globalWorkSize[0], &ocl_event);

        err = clWaitForEvents( 1, &ocl_event );
        //printf("coo spmv serial kernel wait event: %s\n", OpenCL::getError (err).toStdString().c_str());
        // Release event when kernel run finished
        err = clReleaseEvent( ocl_event );
        // printf("coo spmv serial kernel release event: %s\n", OpenCL::getError (err).toStdString().c_str ());

        OpenCL::free(&temp_rows);
        OpenCL::free(&temp_vals);

    }
    else
    {
        qDebug() << "nnz of the matrix : " << this->get_nnz();
    }


}

template class MatrixCOO<float, GPU>;
template class MatrixCOO<double, GPU>;

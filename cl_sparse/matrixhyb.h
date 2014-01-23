#ifndef MATRIXHYB_H
#define MATRIXHYB_H

#include <ocl.h>
#include <types.h>
#include <matrix_types.h>
#include <matrixell.h>
#include <matrixcoo.h>
#include <matrixcsr.h>
#include <vector.h>
#include <assert.h>

//forward declaration
template<typename scalar, DeviceType device>
class MatrixCOO;
template<typename scalar, DeviceType device>
class MatrixELL;
template<typename scalar, DeviceType device>
class MatrixCSR;

template <typename scalar, DeviceType device>
class MatrixHYB
{
public:
    MatrixHYB();

    ~MatrixHYB();

    void load(const std::string& fname_mtx);
    void save(const std::string& fname_mtx);

    void clear();

    void allocate(const int ell_nnz, const int coo_nnz, const int ell_max_row,
                  const int nrow, const int ncol);

    int const get_nnz() const;
    int const get_nrow() const;
    int const get_ncol() const;

    int const get_ell_max_row() const;
    int const get_ell_nnz() const;
    int const get_coo_nnz() const;



    void multiply(const Vector<scalar, device>& in, Vector<scalar, device>& out );
private:

    int ncol;
    int nrow;
    int nnz;

    int ell_nnz;
    int coo_nnz;


};


template <typename scalar>
class MatrixHYB<scalar, CPU>
{
public:
    MatrixHYB<scalar, CPU>();

    MatrixHYB<scalar, CPU>( const MatrixHYB<scalar,GPU>& other);

    ~MatrixHYB<scalar, CPU> ();

    void load(const std::string& fname_mtx);
    void save(const std::string& fname_mtx);

    void clear();

    void allocate(const int ell_nnz, const int coo_nnz, const int ell_max_row,
                  const int nrow, const int ncol);

    int const get_nnz() const;
    int const get_nrow() const;
    int const get_ncol() const;


    int const get_ell_max_row() const;
    int const get_ell_nnz() const;
    int const get_coo_nnz() const;

    int* const get_ellColPtr() const;
    scalar* const get_ellValPtr() const;

    int* const get_cooColPtr() const;
    int* const get_cooRowPtr() const;
    scalar* const get_cooValPtr() const;

    //from csr -> hyb
    void get_data(const MatrixCSR<scalar, CPU>& matrix);

    void multiply(const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out );
private:

    s_MatrixHYB<scalar, int> mat;

    int ncol;
    int nrow;
    int nnz;

    int ell_nnz;
    int coo_nnz;

};


template <typename scalar>
class MatrixHYB<scalar, GPU>
{
public:
    MatrixHYB<scalar, GPU>();

    MatrixHYB<scalar, GPU>( const MatrixHYB<scalar,CPU>& other);

    ~MatrixHYB<scalar, GPU> ();

    void load(const std::string& fname_mtx);
    void save(const std::string& fname_mtx);

    void clear();

    void allocate(const int ell_nnz, const int coo_nnz, const int ell_max_row,
                  const int nrow, const int ncol);

    int const get_nnz() const;
    int const get_nrow() const;
    int const get_ncol() const;

    int const get_ell_max_row() const;
    int const get_ell_nnz() const;
    int const get_coo_nnz() const;

    cl_mem const get_ellColPtr() const;
    cl_mem const get_ellValPtr() const;

    cl_mem const get_cooColPtr() const;
    cl_mem const get_cooRowPtr() const;
    cl_mem const get_cooValPtr() const;

    void multiply(const Vector<scalar, GPU>& in, Vector<scalar, GPU>& out );
private:

    s_gMatrixHYB<cl_mem, cl_mem> mat;

    int ncol;
    int nrow;
    int nnz;

    int ell_nnz;
    int coo_nnz;

};

#endif // MATRIXHYB_H

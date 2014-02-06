#ifndef MATRIXCSR_H
#define MATRIXCSR_H

#include <ocl.h>
#include <types.h>
#include <matrix_types.h>
#include <matrix/matrixell.h>
#include <matrix/matrixcoo.h>
#include <matrix/matrixhyb.h>
#include <vector.h>
#include <assert.h>

//forward declaration
template<typename scalar, DeviceType device>
class MatrixCOO;
template<typename scalar, DeviceType device>
class MatrixELL;
template<typename scalar, DeviceType device>
class MatrixHYB;



template<typename scalar, DeviceType device>
class MatrixCSR
{
public:
    //default constructor, does nothing
    MatrixCSR();

    //allocate memory for matrix
    MatrixCSR(const int nnz, const int nrow, const int ncol);

    //load from mtx
    void load(const std::string& fname_mtx);

    //save to mtx
    void save(const std::string& fname_mtx);

    ~MatrixCSR();

    int get_nnz()const;
    int get_nrow()const;
    int get_ncol()const;

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol);


    void get_data(const MatrixCOO<scalar, CPU>& matrix);
    void get_data(const MatrixELL<scalar, CPU>& matrix);

private:
    int nnz;
    int nrow;
    int ncol;

};

/**
  CPU implementation of csr matrix
  */

template<typename scalar>
class MatrixCSR<scalar, CPU>
{
public:
    //default constructor, does nothing
    MatrixCSR();

    //allocate memory for matrix
    MatrixCSR(const int nnz, const int nrow, const int ncol);

    MatrixCSR(const MatrixCSR<scalar, GPU>& matrix);

    //load from mtx
    void load(const std::string& fname_mtx);

    //save to mtx
    void save(const std::string& fname_mtx);

    ~MatrixCSR();

    int get_nnz()  const;
    int get_nrow() const;
    int get_ncol() const;

    scalar* get_valPtr() const;
    int* get_rowPtr() const;
    int* get_colPtr() const;

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol);

    void multiply(const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out);

    void get_data(const MatrixCOO<scalar, CPU>& matrix);
    void get_data(const MatrixELL<scalar, CPU>& matrix);
    void get_data(const MatrixHYB<scalar, CPU>& matrix);

private:

    s_MatrixCSR<scalar, int> mat;

    int nnz;
    int nrow;
    int ncol;

};

/**
  GPU implementation of CSR matrix
  */
template<typename scalar>
class MatrixCSR<scalar, GPU>
{
public:
    //default constructor, does nothing
    MatrixCSR();

    //allocate memory for matrix
    MatrixCSR(const int nnz, const int nrow, const int ncol);

    MatrixCSR(const MatrixCSR<scalar, CPU>& other);

    //load from mtx
    void load(const std::string& fname_mtx, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);

    //save to mtx
    void save(const std::string& fname_mtx);

    ~MatrixCSR();

    int get_nnz()  const;
    int get_nrow() const;
    int get_ncol() const;

    cl_mem get_valPtr() const;
    cl_mem get_rowPtr() const;
    cl_mem get_colPtr() const;

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);

    void multiply(const Vector<scalar, GPU>& in, Vector<scalar, GPU>& out, bool use_scalar = false);
    void test1(const Vector<scalar, GPU>& in, Vector<scalar, GPU>& out, cl_mem devTexVec);
    void test2(const Vector<scalar, GPU>& in, Vector<scalar, GPU>& out, cl_mem devTexVec);

private:

    s_gMatrixCSR<cl_mem, cl_mem> mat;

    int nnz;
    int nrow;
    int ncol;

};

#endif // MATRIXCSR_H

#ifndef MATRIXCOO_H
#define MATRIXCOO_H

#include <ocl.h>
#include <types.h>
#include <matrix_types.h>
#include <matrix/matrixcsr.h>
#include <vector.h>
#include <assert.h>

template<typename scalar, DeviceType device>
class MatrixCSR;

//This interface just properly expand code completion
template <typename scalar, DeviceType device>
class MatrixCOO
{
public:
    //default constructor, does nothing
    MatrixCOO();

    //copy data from input to internal str : USELESS!
    MatrixCOO(const int* row, const int* col, const scalar* val, const int nnz, const int nrow, const int ncol);

    ~MatrixCOO();

    int const get_nnz()const;
    int const get_nrow()const;
    int const get_ncol()const;

    void info();

    //load from mtx
    void load(const std::string& fname_mtx);
    //save to mtx
    void save(const std::string& fname_mtx);

    void multiply(const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out);

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol);

    void get_data(const MatrixCSR<scalar, CPU>& matrix);

private:

    int nnz;
    int nrow;
    int ncol;

};



template <typename scalar>
class MatrixCOO<scalar, CPU>
{
public:
    //default constructor, does nothing
    MatrixCOO();
    //copy data from input to internal str : USELESS!
    MatrixCOO(const int* row, const int* col, const scalar* val, const int nnz, const int nrow, const int ncol);

    //UŻYWAĆ Z ROZWAGĄ!
    MatrixCOO(const MatrixCOO<scalar, GPU>& other);

    ~MatrixCOO();

    int const get_nnz()const;
    int const get_nrow()const;
    int const get_ncol()const;

    scalar* const get_valPtr() const;
    int* const get_rowPtr() const;
    int* const get_colPtr() const;

    void info();

    //load from mtx
    void load(const std::string& fname_mtx);
    //save to mtx
    void save(const std::string& fname_mtx);

    void multiply(const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out);

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol);

    void get_data(const MatrixCSR<scalar, CPU>& matrix);

private:
    s_MatrixCOO<scalar, int> mat;
    int nnz;
    int nrow;
    int ncol;

    //friend class MatrixCOO<scalar, GPU>;
};



template <typename scalar>
class MatrixCOO<scalar, GPU>
{
public:
    //default constructor, does nothing
    MatrixCOO();
    //copy data from HOST input to internal GPU str
    MatrixCOO(const int* row, const int* col, const scalar* val, const int nnz, const int nrow, const int ncol);

    MatrixCOO(const MatrixCOO<scalar, CPU>& other);

    ~MatrixCOO();

    int const get_nnz() const;
    int const get_nrow() const;
    int const get_ncol() const;

    cl_mem const get_valPtr() const;
    cl_mem const get_rowPtr() const;
    cl_mem const get_colPtr() const;

    void info();

    //load from mtx
    void load(const std::string& fname_mtx);

    void save(const std::string& fname_mtx);

    void multiply(const Vector<scalar, GPU> &in, Vector<scalar, GPU> &out);

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol);
private:
    s_gMatrixCOO<cl_mem, cl_mem> mat;
    int nnz;
    int nrow;
    int ncol;

};



#endif // MATRIXCOO_H

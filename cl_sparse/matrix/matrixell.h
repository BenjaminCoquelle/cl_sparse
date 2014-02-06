#ifndef MATRIXELL_H
#define MATRIXELL_H


#include <ocl.h>
#include <types.h>
#include <matrix_types.h>
#include <matrix/matrixcsr.h>
#include <vector.h>
#include <assert.h>

//forward declaration
template<typename scalar, DeviceType device>
class MatrixCSR;



template <typename scalar, DeviceType device>
class MatrixELL
{
public:
    //default constructo, does nothing
    MatrixELL();

    //allocate memory for matrix
    //MatrixELL(const int nnz, const int nrow, const int ncol, const int max_row);

    //load from mtx
    void load(const std::string& fname_mtx);
    //save to mtx
    void save(const std::string& fname_mtx);

    ~MatrixELL();

    int get_nnz()  const;
    int get_nrow() const;
    int get_ncol() const;
    int get_max_row() const;



    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol, const int max_row);

    //set data to CSR from this
    void get_data(MatrixCSR<scalar, CPU> &matrix);

    void multiply(const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out);
private:
    int nnz;
    int nrow;
    int ncol;




};


template <typename scalar>
class MatrixELL<scalar, CPU>
{
public:
    //default constructo, does nothing
    MatrixELL();

    //allocate memory for matrix
    //MatrixELL(const int nnz, const int nrow, const int ncol, const int max_row);
    MatrixELL(const MatrixELL<scalar, GPU>& other);
    //load from mtx
    void load(const std::string& fname_mtx);
    //save to mtx
    void save(const std::string& fname_mtx);

    ~MatrixELL();

    int get_nnz() const;
    int get_nrow() const;
    int get_ncol() const;
    int get_max_row() const;

    //get pointers;
    scalar* get_valPtr() const;
    int* get_colPtr() const;

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol, const int max_row);

    void get_data(MatrixCSR<scalar, CPU> &matrix);

    void multiply(const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out);

private:
    s_MatrixELL<scalar, int> mat;
    int nnz;
    int nrow;
    int ncol;

};


/**
  GPU - implementation of ELL matrix
  */

template <typename scalar>
class MatrixELL<scalar, GPU>
{
public:
    //default constructo, does nothing
    MatrixELL();

    //allocate memory for matrix
    //MatrixELL(const int nnz, const int nrow, const int ncol, const int max_row);

    MatrixELL(const MatrixELL<scalar, CPU>& other);

    //load from mtx
    void load(const std::string& fname_mtx);
    //save to mtx
    void save(const std::string& fname_mtx);

    ~MatrixELL();

    int get_nnz() const;
    int get_nrow() const;
    int get_ncol() const;
    int get_max_row() const;

    //get pointers;
    cl_mem get_valPtr() const;
    cl_mem get_colPtr() const;

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol, const int max_row);

    //set data to CSR from this
    //void get_data(MatrixCSR<scalar, CPU> &matrix);

    void multiply(const Vector<scalar, GPU>& in, Vector<scalar, GPU>& out);

private:
    s_gMatrixELL<cl_mem, cl_mem> mat;
    int nnz;
    int nrow;
    int ncol;

};


#endif // MATRIXELL_H

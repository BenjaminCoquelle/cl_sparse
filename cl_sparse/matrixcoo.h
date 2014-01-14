#ifndef MATRIXCOO_H
#define MATRIXCOO_H

#include <ocl.h>
#include <types.h>
#include <matrix_types.h>
#include <vector.h>
#include <assert.h>

template <typename scalar, DeviceType device>
class MatrixCOO;



template <typename scalar>
class MatrixCOO<scalar, CPU>
{
public:
    //default constructor, does nothing
    MatrixCOO();
    //copy data from input to internal str
    MatrixCOO(const int* row, const int* col, const scalar* val, int nnz, int nrow, int ncol);

    ~MatrixCOO();

    int get_nnz();
    int get_nrow();
    int get_ncol();

    void info();

    //load from mtx
    void load(const std::string& fname_mtx);
    //save to mtx
    void save(const std::string& fname_mtx);

    void multiply(Vector<scalar, CPU>& in, Vector<scalar, CPU>* out);

    double get_troughput(const double& time);

    void clear();

    void allocate(int nnz, int nrow, int ncol);

private:
    s_MatrixCOO<scalar, int> mat;
    int nnz;
    int nrow;
    int ncol;

    friend class MatrixCOO<scalar, GPU>;
};



template <typename scalar>
class MatrixCOO<scalar, GPU>
{
public:
    //default constructor, does nothing
    MatrixCOO();
    //copy data from HOST input to internal GPU str
    MatrixCOO(const int* row, const int* col, const scalar* val, int nnz, int nrow, int ncol);

    ~MatrixCOO();

    int get_nnz();
    int get_nrow();
    int get_ncol();

    void info();

    //load from mtx
    void load(const std::string& fname_mtx);

    void save(const std::string& fname_mtx);

    void multiply(Vector<scalar, GPU> &in, Vector<scalar, GPU> *out);

    double get_troughput(const double& time);

    void clear();

    void allocate(int nnz, int nrow, int ncol);
private:
    s_gMatrixCOO<cl_mem, cl_mem> mat;
    int nnz;
    int nrow;
    int ncol;

};


#endif // MATRIXCOO_H

#ifndef MATRIXELL_H
#define MATRIXELL_H


#include <ocl.h>
#include <types.h>
#include <matrix_types.h>
#include <matrixcsr.h>
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
    MatrixELL(const int nnz, const int nrow, const int ncol, const int max_row);

    //load from mtx
    void load(const std::string& fname_mtx);
    //save to mtx
    void save(const std::string& fname_mtx);

    ~MatrixELL();

    int const get_nnz() const;
    int const get_nrow() const;
    int const get_ncol() const;
    int const get_max_row() const;



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
    MatrixELL(const int nnz, const int nrow, const int ncol, const int max_row);

    //load from mtx
    void load(const std::string& fname_mtx);
    //save to mtx
    void save(const std::string& fname_mtx);

    ~MatrixELL();

    int const get_nnz() const;
    int const get_nrow() const;
    int const get_ncol() const;
    int const get_max_row() const;

    //get pointers;
    scalar* const get_valPtr() const;
    int* const get_colPtr() const;

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol, const int max_row);

    //set data to CSR from this
    void get_data(MatrixCSR<scalar, CPU> &matrix);

    void multiply(const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out);

private:
    s_MatrixELL<scalar, int> mat;
    int nnz;
    int nrow;
    int ncol;


    //friend class Matrix
};



#endif // MATRIXELL_H

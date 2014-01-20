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

    int get_nnz();
    int get_nrow();
    int get_ncol();
    int get_max_row();

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol, const int max_row);

    //get data from CSR matrix to this
    void set_data(const MatrixCSR<scalar, CPU> &matrix);

    //set data to CSR from this
    void get_data(MatrixCSR<scalar, CPU> &matrix);


private:
    int nnz;
    int nrow;
    int ncol;




};


template <typename scalar>
class MatrixELL<scalar, CPU>
{
    //default constructo, does nothing
    MatrixELL();

    //allocate memory for matrix
    MatrixELL(const int nnz, const int nrow, const int ncol, const int max_row);

    //load from mtx
    void load(const std::string& fname_mtx);
    //save to mtx
    void save(const std::string& fname_mtx);

    ~MatrixELL();

    int get_nnz();
    int get_nrow();
    int get_ncol();
    int get_max_row();

    double get_troughput(const double& time);

    void clear();

    void allocate(const int nnz, const int nrow, const int ncol, const int max_row);

    //get data from CSR matrix to this
    void set_data(const MatrixCSR<scalar, CPU> &matrix);

    //set data to CSR from this
    void get_data(MatrixCSR<scalar, CPU> &matrix);

private:
    s_MatrixELL<scalar, int> mat;
    int nnz;
    int nrow;
    int ncol;


    //friend class Matrix
};


#endif // MATRIXELL_H

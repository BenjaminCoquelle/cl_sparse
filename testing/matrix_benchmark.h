#ifndef MATRIX_BENCHMARK_H
#define MATRIX_BENCHMARK_H

#include <QObject>
#include "ocl.h"
#include "vector.h"
#include "matrixcoo.h"
#include "matrixcsr.h"
#include "matrixell.h"
#include "matrixhyb.h"

class MatrixBenchmark : public QObject
{
    Q_OBJECT

public:
    MatrixBenchmark();

private:

    OpenCL* ocl;


    MatrixCOO<float, GPU>* coo_float;
    MatrixCSR<float, GPU>* csr_float;
    MatrixELL<float, GPU>* ell_float;
    MatrixHYB<float, GPU>* hyb_float;

    MatrixCOO<double, GPU>* coo_double;
    MatrixCSR<double, GPU>* csr_double;
    MatrixELL<double, GPU>* ell_double;
    MatrixHYB<double, GPU>* hyb_double;

    void generate_data();

    void warmup();

private Q_SLOTS:
    void initTestCase();        //before test suit
    void init();                //before every test case

    void cleanupTestCase();     //after test suit
    void cleanup();             //after every test case

    void bench_Test_COO_float();
    void bench_Test_COO_float_data();

    void bench_Test_CSR_float();
    void bench_Test_CSR_float_data();

    void bench_Test_ELL_float();
    void bench_Test_ELL_float_data();

    void bench_Test_HYB_float();
    void bench_Test_HYB_float_data();

    void bench_Test_COO_double();
    void bench_Test_COO_double_data();

    void bench_Test_CSR_double();
    void bench_Test_CSR_double_data();

    void bench_Test_ELL_double();
    void bench_Test_ELL_double_data();

    void bench_Test_HYB_double();
    void bench_Test_HYB_double_data();

};

#endif // MATRIX_BENCHMARK_H

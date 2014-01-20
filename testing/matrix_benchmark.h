#ifndef MATRIX_BENCHMARK_H
#define MATRIX_BENCHMARK_H

#include <QObject>
#include "ocl.h"
#include "vector.h"
#include "matrixcoo.h"
#include "matrixcsr.h"

class MatrixBenchmark : public QObject
{
    Q_OBJECT

public:
    MatrixBenchmark();

private:

    OpenCL* ocl;

    Vector<float, GPU>* fg;
    MatrixCOO<float, GPU>* coo;
    MatrixCSR<float, GPU>* csr;

    void float_data();

    void warmup();

private Q_SLOTS:
    void initTestCase();        //before test suit
    void init();                //before every test case

    void cleanupTestCase();     //after test suit
    void cleanup();             //after every test case

    void bench_Test_CSR_float();
    void bench_Test_CSR_float_data();

    void bench_Test_COO_float();
    void bench_Test_COO_float_data();


};

#endif // MATRIX_BENCHMARK_H

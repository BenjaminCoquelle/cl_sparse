#ifndef VECTORBENCHMARK_H
#define VECTORBENCHMARK_H

#include <QObject>
#include "ocl.h"
#include "vector.h"


class VectorBenchmark : public QObject
{
    Q_OBJECT

public:
    VectorBenchmark();

private:



    OpenCL* ocl;

    cl_mem fs; //scalar for single precision results
    cl_mem ds; //scalar for double precision results;

    //values to test scale operators
    cl_mem sv;
    cl_mem dv;


    Vector<float, GPU>* fg;
    Vector<double, GPU>* dg;


    Vector<float, CPU>* fc;
    Vector<double, CPU>* dc;


    //common init data
    void float_data();
    void double_data();

    void warmup();


private Q_SLOTS:
    void initTestCase();        //before test suit
    void init();                //before every test case

    void cleanupTestCase();     //after test suit
    void cleanup();             //after every test case
    
    //vec = vec1 + vec2
    void bench_Test_add_cpu_float();
    void bench_Test_add_gpu_float();
    void bench_Test_add_cpu_double();
    void bench_Test_add_gpu_double();

    //vec += vec2
    void bench_Test_apply_cpu_float();
    void bench_Test_apply_gpu_float();
    void bench_Test_apply_cpu_double();
    void bench_Test_apply_gpu_double();


    //tests for dot product
    void bench_Test_dot_cpu_float();
    void bench_Test_dot_gpu_to_cpu_float();
    void bench_Test_dot_gpu_to_gpu_float();


    void bench_Test_dot_cpu_double();
    void bench_Test_dot_gpu_to_cpu_double();
    void bench_Test_dot_gpu_to_gpu_double();

    //tests for norm1
    void bench_Test_norm_cpu_float();
    void bench_Test_norm_gpu_float();
    void bench_Test_norm_gpu_to_gpu_float();

    //tests for norm2
    void bench_Test_norm2_cpu_double();
    void bench_Test_norm2_gpu_double();
    void bench_Test_norm2_gpu_to_gpu_double();

    //tests for scale operator
    void bench_Test_scale_cpu_float();
    void bench_Test_scale_gpu_float();
    void bench_Test_scale_gpu_gpu_float();
    void bench_Test_self_scale_gpu_gpu_float();

    void bench_Test_scale_cpu_double();
    void bench_Test_scale_gpu_double();
    void bench_Test_scale_gpu_gpu_double();
    void bench_Test_self_scale_gpu_gpu_double();


};

#endif // VECTORBENCHMARK_H

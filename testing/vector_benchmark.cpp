#include <QtTest/QtTest>

#include "vector_benchmark.h"

VectorBenchmark::VectorBenchmark() { }


void VectorBenchmark::initTestCase()
{
    ocl = new OpenCL(1);
}

void VectorBenchmark::cleanupTestCase()
{
    ocl->shutdown();
    delete ocl;
}

void VectorBenchmark::warmup()
{
    Vector<double, GPU> w((int)1e7, 1.1);
    //qDebug() << "Warming up!";
    for(int i = 0; i < 25; i++)
        w.dot(w);
}

void VectorBenchmark::init()
{
    OpenCL::allocate(&fs, sizeof(float));
    OpenCL::allocate(&ds, sizeof(double));

    OpenCL::allocate(&sv, sizeof(float));
    OpenCL::allocate(&dv, sizeof(double));

    float two = 2.0;
    double dtwo = 2.0;

    OpenCL::copy(sv, &two, sizeof(float));
    OpenCL::copy(dv, &dtwo, sizeof(double));

    const int _size = int(5e6);
    const float v = 1.0e-4;
    fg = new Vector<float,  GPU>(_size, float (v) );
    dg = new Vector<double, GPU>(_size, double(v) );
    fc = new Vector<float,  CPU>(_size, float (v) );
    dc = new Vector<double, CPU>(_size, double(v) );

    warmup();
}

void VectorBenchmark::cleanup()
{
    delete fg;
    delete dg;
    delete fc;
    delete dc;

    OpenCL::free(&fs);
    OpenCL::free(&ds);
    OpenCL::free(&sv);
    OpenCL::free(&dv);
}

/**
  Tests for operation: vec = vec1 + vec2
  */
void VectorBenchmark::bench_Test_add_cpu_float()
{
    typedef float scalar;
    Vector<scalar, CPU> r;
    QBENCHMARK {
            r = *fc + *fc;
    }
    //r.print();
}

void VectorBenchmark::bench_Test_add_gpu_float()
{
    typedef float scalar;
    Vector<scalar, GPU>  r;
    QBENCHMARK {
        r = *fg + *fg;
    }
    //r.print();
}

void VectorBenchmark::bench_Test_add_cpu_double()
{
    typedef double scalar;
    Vector<scalar, CPU> r;
    QBENCHMARK {
            r = *dc + *dc;
    }
    //r.print();
}

void VectorBenchmark::bench_Test_add_gpu_double()
{
    typedef double scalar;
    Vector<scalar, GPU> r;
    QBENCHMARK {
        r = *dg + *dg;
    }
    //r.print();
}

/**
  Tests for operation: vec += vec1
  */

void VectorBenchmark::bench_Test_apply_cpu_float()
{
    QBENCHMARK{
        *fc += *fc;
    }
    //fc->print();
}

void VectorBenchmark::bench_Test_apply_cpu_double()
{
    QBENCHMARK{
        *dc += *dc;
    }
    //dc->print();
}

void VectorBenchmark::bench_Test_apply_gpu_float()
{
    QBENCHMARK{
        *fg += *fg;
    }
    //fg->print();

}

void VectorBenchmark::bench_Test_apply_gpu_double()
{
    QBENCHMARK{
        *dg += *dg;
    }
    //dg->print();

}

/**
  Tests for dot product
  */
void VectorBenchmark::bench_Test_dot_cpu_float()
{
    float r;
    QBENCHMARK{
        r = fc->dot(*fc);
    }
    //qDebug() << "\tr: " << r;
}

void VectorBenchmark::bench_Test_dot_cpu_double()
{
    double r;
    QBENCHMARK{
        r = dc->dot(*dc);
    }
    //qDebug() << "\tr: " << r;
}

void VectorBenchmark::bench_Test_dot_gpu_to_cpu_float()
{
    float r;
    QBENCHMARK{
        r = fg->dot(*fg);
    }
    //qDebug() << "\tr: " << r;
}

void VectorBenchmark::bench_Test_dot_gpu_to_gpu_float()
{
    QBENCHMARK{
        fg->dot(*fg, &fs);
    }
    //float r;
    //OpenCL::copy(&r, fs, sizeof(float));
    //qDebug() << "\tr: " << r;
}

void VectorBenchmark::bench_Test_dot_gpu_to_cpu_double()
{
    double r;
    QBENCHMARK{
        r = dg->dot(*dg);
    }
   //qDebug() << "\tr: " << r;
}

void VectorBenchmark::bench_Test_dot_gpu_to_gpu_double()
{
    QBENCHMARK{
        dg->dot(*dg, &ds);
    }
    //double r;
    //OpenCL::copy(&r, ds, sizeof(double));
    //qDebug() << "\tr: " << r;
}

//tests for norm1
void VectorBenchmark::bench_Test_norm_cpu_float()
{
    QBENCHMARK{
        fc->norm(1);
    }
}

void VectorBenchmark::bench_Test_norm_gpu_float()
{
    QBENCHMARK{
        fg->norm(1);
    }
}

void VectorBenchmark::bench_Test_norm_gpu_to_gpu_float()
{
    QBENCHMARK{
        fg->norm(&fs, 1);
    }

}

//tests for norm2
void VectorBenchmark::bench_Test_norm2_cpu_double()
{
    QBENCHMARK{
        dc->norm();
    }
}

void VectorBenchmark::bench_Test_norm2_gpu_double()
{
    QBENCHMARK{
        dg->norm();
    }
}

void VectorBenchmark::bench_Test_norm2_gpu_to_gpu_double()
{
    QBENCHMARK{
        dg->norm(&ds);
    }
}


//test scale operator
void VectorBenchmark::bench_Test_scale_cpu_float()
{

    Vector<float, CPU> r;
    QBENCHMARK
    {
        r = *fc * 2.0;
    }
}

void VectorBenchmark::bench_Test_scale_gpu_float()
{

    Vector<float, GPU> r;
    QBENCHMARK
    {
        r = *fg * 2.0;

    }
}


void VectorBenchmark::bench_Test_scale_gpu_gpu_float()
{

    Vector<float, GPU> r;
    QBENCHMARK
    {
        r = *fg * sv;

    }
}

void VectorBenchmark::bench_Test_self_scale_gpu_gpu_float()
{
    QBENCHMARK
    {
        *fg *= sv;
    }
}

void VectorBenchmark::bench_Test_scale_cpu_double()
{
    Vector<double, CPU> r;
    QBENCHMARK
    {
        r = *dc * 2.0;
    }
}

void VectorBenchmark::bench_Test_scale_gpu_double()
{
    Vector<double, GPU> r;
    QBENCHMARK
    {
        r = *dg * 2.0;

    }
}


void VectorBenchmark::bench_Test_scale_gpu_gpu_double()
{
    Vector<double, GPU> r;
    QBENCHMARK
    {
        r = *dg * dv;

    }
}

void VectorBenchmark::bench_Test_self_scale_gpu_gpu_double()
{
    QBENCHMARK
    {
        *dg *= dv;
    }
}

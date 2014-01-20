#include <QtTest/QtTest>
#include "matrix_benchmark.h"
#include <string.h>

MatrixBenchmark::MatrixBenchmark()
{
}

void MatrixBenchmark::initTestCase()
{
    ocl = new OpenCL(1);

}

void MatrixBenchmark::cleanupTestCase()
{
    delete coo;
    delete csr;

    ocl->shutdown();
    delete ocl;
}

void MatrixBenchmark::warmup()
{
    Vector<double, GPU> w((int)1e7, 1.1);
    //qDebug() << "Warming up!";
    for(int i = 0; i < 50; i++)
        w.dot(w);
}

void MatrixBenchmark::float_data()
{
    QTest::addColumn<QString>("matrix_path");

    QTest::newRow("consph") << QString("/media/Storage/matrices/Bell_MTX/consph.mtx");
    QTest::newRow("raefsky3") << QString("/media/Storage/matrices/Bell_MTX/raefsky3.mtx");
    QTest::newRow("pdb1HYS") << QString("/media/Storage/matrices/Bell_MTX/pdb1HYS.mtx");
    QTest::newRow("cant") << QString("/media/Storage/matrices/Bell_MTX/cant.mtx");
    QTest::newRow("webbase-1M") << QString("/media/Storage/matrices/Bell_MTX/webbase-1M.mtx");
    QTest::newRow("shipsec1") << QString("/media/Storage/matrices/Bell_MTX/shipsec1.mtx");
    QTest::newRow("thermal2") << QString("/media/Storage/matrices/Bell_MTX/thermal2.mtx");
//    QTest::newRow("e40r0100") << QString("/media/Storage/matrices/Bell_MTX/e40r0100.mtx");
//    QTest::newRow("cop20k_A") << "/media/Storage/matrices/Bell_MTX/cop20k_A.mtx";
//    QTest::newRow("scircuit") << "/media/Storage/matrices/Bell_MTX/scircuit.mtx";
//    QTest::newRow("mc2depi")  << "/media/Storage/matrices/Bell_MTX/mc2depi.mtx";
}


void MatrixBenchmark::bench_Test_COO_float_data()
{
    float_data();
}

void MatrixBenchmark::bench_Test_CSR_float_data()
{
    float_data();
}

void MatrixBenchmark::init()
{
    warmup();
}

void MatrixBenchmark::cleanup()
{

}

void MatrixBenchmark::bench_Test_COO_float()
{
    typedef float scalar;
    QFETCH(QString, matrix_path);

    coo = new MatrixCOO<float, GPU>();
    coo->load(matrix_path.toStdString());


    Vector<scalar, GPU> x(coo->get_ncol(), 1.0);
    Vector<scalar, GPU> b(coo->get_nrow(), 0.0);

    QBENCHMARK {
        coo->multiply(x,b);
    }

}

void MatrixBenchmark::bench_Test_CSR_float()
{
    typedef float scalar;
    QFETCH(QString, matrix_path);

    csr = new MatrixCSR<float, GPU>();
    csr->load(matrix_path.toStdString());


    Vector<scalar, GPU> x(csr->get_ncol(), 1.0);
    Vector<scalar, GPU> b(csr->get_nrow(), 0.0);

    QBENCHMARK {
        csr->multiply(x,b);
    }
}

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
//    delete csr_float;
//    delete csr_double;
//    delete coo_float;
//    delete coo_double;

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

void MatrixBenchmark::generate_data()
{
    QTest::addColumn<QString>("matrix_path");

    QTest::newRow("consph") << QString("/media/Storage/matrices/Bell_MTX/consph.mtx");
    QTest::newRow("raefsky3") << QString("/media/Storage/matrices/Bell_MTX/raefsky3.mtx");
    QTest::newRow("pdb1HYS") << QString("/media/Storage/matrices/Bell_MTX/pdb1HYS.mtx");
    QTest::newRow("cant") << QString("/media/Storage/matrices/Bell_MTX/cant.mtx");
    QTest::newRow("webbase-1M") << QString("/media/Storage/matrices/Bell_MTX/webbase-1M.mtx");
    QTest::newRow("shipsec1") << QString("/media/Storage/matrices/Bell_MTX/shipsec1.mtx");
    QTest::newRow("thermal2") << QString("/media/Storage/matrices/Bell_MTX/thermal2.mtx");
    QTest::newRow("e40r0100") << QString("/media/Storage/matrices/Bell_MTX/e40r0100.mtx");
    QTest::newRow("cop20k_A") << QString("/media/Storage/matrices/Bell_MTX/cop20k_A.mtx");
    QTest::newRow("scircuit") << QString("/media/Storage/matrices/Bell_MTX/scircuit.mtx");
    QTest::newRow("mc2depi")  << QString("/media/Storage/matrices/Bell_MTX/mc2depi.mtx");
}


void MatrixBenchmark::bench_Test_COO_float_data()
{
    generate_data();
}

void MatrixBenchmark::bench_Test_CSR_float_data()
{
    generate_data();
}

void MatrixBenchmark::bench_Test_COO_double_data()
{
    generate_data();
}

void MatrixBenchmark::bench_Test_CSR_double_data()
{
    generate_data();
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

    coo_float = new MatrixCOO<scalar, GPU>();
    coo_float->load(matrix_path.toStdString());


    Vector<scalar, GPU> x(coo_float->get_ncol(), 1.0);
    Vector<scalar, GPU> b(coo_float->get_nrow(), 0.0);

    QBENCHMARK {
        coo_float->multiply(x,b);
    }
    //qDebug() << b.norm();
    delete coo_float;

}

void MatrixBenchmark::bench_Test_CSR_float()
{
    typedef float scalar;
    QFETCH(QString, matrix_path);

    csr_float = new MatrixCSR<scalar, GPU>();
    csr_float->load(matrix_path.toStdString());


    Vector<scalar, GPU> x(csr_float->get_ncol(), 1.0);
    Vector<scalar, GPU> b(csr_float->get_nrow(), 0.0);

    QBENCHMARK {
        csr_float->multiply(x,b);
    }
    //qDebug() << b.norm();
    delete csr_float;
}


void MatrixBenchmark::bench_Test_COO_double()
{
    typedef double scalar;
    QFETCH(QString, matrix_path);

    coo_double = new MatrixCOO<scalar, GPU>();
    coo_double->load(matrix_path.toStdString());


    Vector<scalar, GPU> x(coo_double->get_ncol(), 1.0);
    Vector<scalar, GPU> b(coo_double->get_nrow(), 0.0);

    QBENCHMARK {
        coo_double->multiply(x,b);
    }
    //qDebug() << b.norm();
    delete coo_double;

}

void MatrixBenchmark::bench_Test_CSR_double()
{
    typedef double scalar;
    QFETCH(QString, matrix_path);

    csr_double = new MatrixCSR<scalar, GPU>();
    csr_double->load(matrix_path.toStdString());


    Vector<scalar, GPU> x(csr_double->get_ncol(), 1.0);
    Vector<scalar, GPU> b(csr_double->get_nrow(), 0.0);

    QBENCHMARK {
        csr_double->multiply(x,b);
    }
   // qDebug() << b.norm();
    delete csr_double;
}



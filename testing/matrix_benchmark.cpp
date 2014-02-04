#include <QtTest/QtTest>
#include "matrix_benchmark.h"
#include <string.h>

MatrixBenchmark::MatrixBenchmark()
{
}

void MatrixBenchmark::initTestCase()
{
    ocl = new OpenCL(-1);

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
     // pattern format is not supported  QTest::newRow("adaptive") << QString("/media/Storage/matrices/Bell_MTX/adaptive.mtx");
    QTest::newRow("mac_econ_fwd500") << QString("/media/Storage/matrices/Bell_MTX/mac_econ_fwd500.mtx");
    // pattern format is not supported QTest::newRow("in-2004") << QString("/media/Storage/matrices/Bell_MTX/in-2004.mtx");
      // unknown error at row 13369448    QTest::newRow("circuit5M_dc") << QString("/media/Storage/matrices/Bell_MTX/circuit5M_dc.mtx");
    //QTest::newRow("relat9") << QString("/media/Storage/matrices/Bell_MTX/relat9.mtx");
    //QTest::newRow("ldoor") << QString("/media/Storage/matrices/Bell_MTX/ldoor.mtx");
//    QTest::newRow("Hook_1498") << QString("/media/Storage/matrices/Bell_MTX/Hook_1498.mtx");
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

void MatrixBenchmark::bench_Test_CSR_float2_data()
{
    generate_data();
}

void MatrixBenchmark::bench_Test_ELL_float_data()
{
    generate_data();
}

void MatrixBenchmark::bench_Test_HYB_float_data()
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

void MatrixBenchmark::bench_Test_ELL_double_data()
{
    generate_data();
}

void MatrixBenchmark::bench_Test_HYB_double_data()
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
    csr_float->load(matrix_path.toStdString(), CL_MEM_READ_WRITE);


    Vector<scalar, GPU> x(csr_float->get_ncol(), 1.0);
    Vector<scalar, GPU> b(csr_float->get_nrow(), 0.0);

    QBENCHMARK {
        csr_float->multiply(x,b);
    }
    qDebug() << b.norm();
    delete csr_float;
}

void MatrixBenchmark::bench_Test_CSR_float2()
{
    typedef float scalar;
    QFETCH(QString, matrix_path);

    csr_float = new MatrixCSR<scalar, GPU>();
    csr_float->load(matrix_path.toStdString(), CL_MEM_READ_ONLY);


    Vector<scalar, GPU> x(csr_float->get_ncol(), 1.0, CL_MEM_READ_ONLY);
    Vector<scalar, GPU> b(csr_float->get_nrow(), 0.0, CL_MEM_WRITE_ONLY);

    QBENCHMARK {
        csr_float->multiply(x,b);
    }
    qDebug() << b.norm();
    delete csr_float;
}

void MatrixBenchmark::bench_Test_ELL_float()
{
    typedef float scalar;
    QFETCH(QString, matrix_path);

    ell_float = new MatrixELL<scalar, GPU>();
    ell_float->load(matrix_path.toStdString());


    Vector<scalar, GPU> x(ell_float->get_ncol(), 1.0);
    Vector<scalar, GPU> b(ell_float->get_nrow(), 0.0);

    QBENCHMARK {
        ell_float->multiply(x,b);
    }
    //qDebug() << b.norm();
    delete ell_float;
}

void MatrixBenchmark::bench_Test_HYB_float()
{
    typedef float scalar;
    QFETCH(QString, matrix_path);

    hyb_float = new MatrixHYB<scalar, GPU>();
    hyb_float->load(matrix_path.toStdString());


    Vector<scalar, GPU> x(hyb_float->get_ncol(), 1.0);
    Vector<scalar, GPU> b(hyb_float->get_nrow(), 0.0);

    QBENCHMARK {
        hyb_float->multiply(x,b);
    }
    //qDebug() << b.norm();
    delete hyb_float;
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
   // //qDebug() << b.norm();
    delete csr_double;
}

void MatrixBenchmark::bench_Test_ELL_double()
{
    typedef double scalar;
    QFETCH(QString, matrix_path);

    ell_double = new MatrixELL<scalar, GPU>();
    ell_double->load(matrix_path.toStdString());


    Vector<scalar, GPU> x(ell_double->get_ncol(), 1.0);
    Vector<scalar, GPU> b(ell_double->get_nrow(), 0.0);

    QBENCHMARK {
        ell_double->multiply(x,b);
    }
    //qDebug() << b.norm();
    delete ell_double;
}

void MatrixBenchmark::bench_Test_HYB_double()
{
    typedef double scalar;
    QFETCH(QString, matrix_path);

    hyb_double = new MatrixHYB<scalar, GPU>();
    hyb_double->load(matrix_path.toStdString());


    Vector<scalar, GPU> x(hyb_double->get_ncol(), 1.0);
    Vector<scalar, GPU> b(hyb_double->get_nrow(), 0.0);

    QBENCHMARK {
        hyb_double->multiply(x,b);
    }
    //qDebug() << b.norm();
    delete hyb_double;
}


#include <QtTest>
#include "vector_test.h"
#include "vector_benchmark.h"
#include "matrix_benchmark.h"


int main(int argc, char** argv)
{

    qDebug() << " Remember to export libs: LD_LIBRARY_PATH=../cl_sparse/:../../clsparse/clBLAS/bin/library/ ";
   // VectorTest vt;
   // VectorBenchmark vb;
    MatrixBenchmark mb;


//    return QTest::qExec(&vt, argc,argv) |
//           QTest::qExec(&vb, argc,argv);

    return QTest::qExec(&mb, argc,argv);
}

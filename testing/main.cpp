#include <QtTest>
#include "vector_test.h"
#include "vector_benchmark.h"
int main(int argc, char** argv)
{




   // VectorTest vt;
    VectorBenchmark vb;

    qCritical()<< "DODAC IMPLEMENTACJE VECTORA scale* alpha!!";

//    return QTest::qExec(&vt, argc,argv) |
//           QTest::qExec(&vb, argc,argv);

    return QTest::qExec(&vb, argc,argv);
}

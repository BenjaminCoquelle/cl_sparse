#include <QtTest>
#include "vector_test.h"

int main(int argc, char** argv)
{


    //true = float, false - double precision;
    VectorTest vt;
    return QTest::qExec(&vt, argc,argv);
}

#include <QtTest>
#include "vector_test.h"

int main(int argc, char** argv)
{




    VectorTest vt;
    qCritical()<< "DODAC IMPLEMENTACJE TESTOW SAVE I LOAD!!";
    return QTest::qExec(&vt, argc,argv);
}

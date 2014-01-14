#include <QtCore/QString>
#include <QtTest/QtTest>

#include "vector_test.h"


VectorTest::VectorTest()
{

}

void VectorTest::initTestCase()
{
    ocl = new OpenCL(1);
}

void VectorTest::cleanupTestCase()
{
    ocl->shutdown();
    delete ocl;
}

void VectorTest::float_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<float>("value");

    QTest::newRow("1e3 - 1.0") << int(1e3) << float(1.0);
    QTest::newRow("1e5 - 2.1") << int(1e5) << float(2.1);
    QTest::newRow("1e7 - 5.5") << int(1e7) << float(5.5);
}

void VectorTest::double_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<double>("value");

    QTest::newRow("1e3 - 1.0") << int(1e3) << double(1.0);
    QTest::newRow("1e5 - 2.1") << int(1e5) << double(2.1);
    QTest::newRow("1e7 - 5.5") << int(1e7) << double(5.5);
}


void VectorTest::Operator_plus_float_data()
{
    float_data();
}

void VectorTest::Operator_plus_double_data()
{
    double_data();
}

void VectorTest::Operator_plus_float()
{
    typedef float scalar;

    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, GPU> gpu_vector1(size, value);
    Vector<scalar, GPU> gpu_vector2(size, value);

    Vector<scalar, CPU> cpu_vector1(size, value);
    Vector<scalar, CPU> cpu_vector2(size, value);

    Vector<scalar, GPU> gpu_result = gpu_vector1 + gpu_vector2;
    //gpu_result.print();
    Vector<scalar, CPU> cpu_result = cpu_vector1 + cpu_vector2;

    bool result = cpu_result == gpu_result;
    QVERIFY(result);

}

void VectorTest::Operator_plus_double()
{
    typedef double scalar;

    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, GPU> gpu_vector1(size, value);
    Vector<scalar, GPU> gpu_vector2(size, value);

    Vector<scalar, CPU> cpu_vector1(size, value);
    Vector<scalar, CPU> cpu_vector2(size, value);

    Vector<scalar, GPU> gpu_result = gpu_vector1 + gpu_vector2;

    Vector<scalar, CPU> cpu_result = cpu_vector1 + cpu_vector2;

    bool result = cpu_result == gpu_result;
    QVERIFY(result);

}


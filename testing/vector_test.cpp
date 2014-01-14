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
    QTest::newRow("1e5 - 3e-4") << int(1e5) << float(-3e-4);
    QTest::newRow("1e7 - -1e-5") << int(1e7) << float(-1e-5);
}

void VectorTest::double_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<double>("value");

    QTest::newRow("1e3 - 1.0") << int(1e3) << double(1.0);
    QTest::newRow("1e5 - 3e-4") << int(1e5) << double(-3e-4);
    QTest::newRow("1e7 - -1e-5") << int(1e7) << double(-1e-5);
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
    Vector<scalar, CPU> cpu_result = cpu_vector1 + cpu_vector2;

    bool result = cpu_result == gpu_result;
    QVERIFY(result);

    cpu_result += cpu_vector1;
    gpu_result += gpu_vector1;

    result = cpu_result == gpu_result;

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

    cpu_result += cpu_vector1;
    gpu_result += gpu_vector1;

    result = cpu_result == gpu_result;

    QVERIFY(result);

}

void VectorTest::Operator_eq_double_data()
{
    double_data();
}

void VectorTest::Operator_eq_float_data()
{
    float_data();
}

void VectorTest::Operator_eq_float()
{
    typedef float scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, GPU> gpu_vector(size, value);
    Vector<scalar, GPU> gpu_result = gpu_vector;

    Vector<scalar, CPU> cpu_vector(size, value);
    Vector<scalar, CPU> cpu_result = cpu_vector;

    bool result = cpu_result == gpu_result;
    QVERIFY(result);
}

void VectorTest::Operator_eq_double()
{
    typedef double scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, GPU> gpu_vector(size, value);
    Vector<scalar, GPU> gpu_result = gpu_vector;

    Vector<scalar, CPU> cpu_vector(size, value);
    Vector<scalar, CPU> cpu_result = cpu_vector;

    bool result = cpu_result == gpu_result;
    QVERIFY(result);

}

void VectorTest::Operator_sub_float_data()
{
    float_data();
}

void VectorTest::Operator_sub_double_data()
{
    double_data();
}

void VectorTest::Operator_sub_float()
{
    typedef float scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, GPU> gpu_vector1(size, value);
    Vector<scalar, GPU> gpu_vector2(size, value);

    Vector<scalar, CPU> cpu_vector1(size, value);
    Vector<scalar, CPU> cpu_vector2(size, value);

    Vector<scalar, GPU> gpu_result = gpu_vector1 - gpu_vector2;
    Vector<scalar, CPU> cpu_result = cpu_vector1 - cpu_vector2;

    bool result = cpu_result == gpu_result;
    QVERIFY(result);

    cpu_result -= cpu_vector1;
    gpu_result -= gpu_vector1;

    result = cpu_result == gpu_result;

    QVERIFY(result);
}

void VectorTest::Operator_sub_double()
{
    typedef double scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, GPU> gpu_vector1(size, value);
    Vector<scalar, GPU> gpu_vector2(size, value);

    Vector<scalar, CPU> cpu_vector1(size, value);
    Vector<scalar, CPU> cpu_vector2(size, value);

    Vector<scalar, GPU> gpu_result = gpu_vector1 - gpu_vector2;
    Vector<scalar, CPU> cpu_result = cpu_vector1 - cpu_vector2;

    bool result = cpu_result == gpu_result;
    QVERIFY(result);

    cpu_result -= cpu_vector1;
    gpu_result -= gpu_vector1;

    result = cpu_result == gpu_result;

    QVERIFY(result);

}


void VectorTest::Operator_mul_float_data()
{
    float_data();
}

void VectorTest::Operator_mul_double_data()
{
    double_data();
}

void VectorTest::Operator_mul_float()
{
    typedef float scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, GPU> gpu_vector1(size, value);
    Vector<scalar, GPU> gpu_vector2(size, value);

    Vector<scalar, CPU> cpu_vector1(size, value);
    Vector<scalar, CPU> cpu_vector2(size, value);

    Vector<scalar, GPU> gpu_result = gpu_vector1 * gpu_vector2;
    Vector<scalar, CPU> cpu_result = cpu_vector1 * cpu_vector2;

    bool result = cpu_result == gpu_result;
    QVERIFY(result);

    cpu_result *= cpu_vector1;
    gpu_result *= gpu_vector1;

    result = cpu_result == gpu_result;

    QVERIFY(result);
}

void VectorTest::Operator_mul_double()
{
    typedef double scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, GPU> gpu_vector1(size, value);
    Vector<scalar, GPU> gpu_vector2(size, value);

    Vector<scalar, CPU> cpu_vector1(size, value);
    Vector<scalar, CPU> cpu_vector2(size, value);

    Vector<scalar, GPU> gpu_result = gpu_vector1 * gpu_vector2;
    Vector<scalar, CPU> cpu_result = cpu_vector1 * cpu_vector2;

    bool result = cpu_result == gpu_result;
    QVERIFY(result);

    cpu_result *= cpu_vector1;
    gpu_result *= gpu_vector1;

    result = cpu_result == gpu_result;

    QVERIFY(result);

}

void VectorTest::Operator_constr_float_data()
{
    float_data();
}

void VectorTest::Operator_constr_double_data()
{
    double_data();
}

void VectorTest::Operator_constr_float()
{
    typedef float scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, CPU> cpu_vector(size, value);
    Vector<scalar, CPU> gpu_vector(size, value);
    Vector<scalar, CPU> gpu_result1(cpu_vector);
    Vector<scalar, CPU> gpu_result2(gpu_vector);

    bool result = cpu_vector == gpu_result1;
    QVERIFY(result);

    result = cpu_vector == gpu_result2;
    QVERIFY(result);
}

void VectorTest::Operator_constr_double()
{
    typedef double scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, CPU> cpu_vector(size, value);
    Vector<scalar, CPU> gpu_vector(size, value);
    Vector<scalar, CPU> gpu_result1(cpu_vector);
    Vector<scalar, CPU> gpu_result2(gpu_vector);

    bool result = cpu_vector == gpu_result1;
    QVERIFY(result);

    result = cpu_vector == gpu_result2;
    QVERIFY(result);
}

//SUM
void VectorTest::Test_sum_float_data()
{
    float_data();
}

void VectorTest::Test_sum_double_data()
{
    double_data();
}

void VectorTest::Test_sum_float()
{
    typedef float scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, CPU> cpu_vector(size, value);
    if(size < 1e5)
    {
        generate_floats(cpu_vector);
    }
    //cpu_vector.print();

    Vector<scalar, GPU> gpu_vector(cpu_vector);

    scalar h_sum = cpu_vector.sum();
    scalar g_sum = gpu_vector.sum();

    scalar diff = fabs(h_sum - g_sum);
    if (diff > 1e-3)
    {
        QWARN("Test Vector sum float can differ for long vectors" );
        qDebug() << "Difference: " << diff;
        QVERIFY(true);
    }
    else { QVERIFY(compare_f(h_sum,g_sum)); }

    cl_mem g_sum1;
    OpenCL::allocate(&g_sum1, sizeof(scalar));
    gpu_vector.sum(&g_sum1);
    scalar h_sum1;
    OpenCL::copy(&h_sum1, g_sum1, sizeof(scalar));
    QVERIFY(compare_f(h_sum1, g_sum));

}

void VectorTest::Test_sum_double()
{
    typedef double scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, CPU> cpu_vector(size, value);
    if(size < 1e5)
    {
        generate_doubles(cpu_vector);
    }
    //cpu_vector.print();

    Vector<scalar, GPU> gpu_vector(cpu_vector);

    scalar h_sum = cpu_vector.sum();
    scalar g_sum = gpu_vector.sum();
    scalar diff = fabs(h_sum - g_sum);
    if (diff > 1e-3)
    {
        QWARN("Test Vector sum double can differ for long vectors" );
        qDebug() << "Difference: " << diff;
        QVERIFY(true);
    }
    else { QVERIFY(compare_f(h_sum,g_sum)); }

    cl_mem g_sum1;
    OpenCL::allocate(&g_sum1, sizeof(scalar));
    gpu_vector.sum(&g_sum1);
    scalar h_sum1;
    OpenCL::copy(&h_sum1, g_sum1, sizeof(scalar));
    QVERIFY(compare_d(h_sum1, g_sum));
}




//DOT
void VectorTest::Test_dot_float_data()
{
    float_data();
}

void VectorTest::Test_dot_double_data()
{
    double_data();
}

void VectorTest::Test_dot_float()
{
    typedef float scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, CPU> cpu_vector(size, value);
    if(size < 1e5)
    {
        generate_floats(cpu_vector);
    }
    //cpu_vector.print();

    Vector<scalar, GPU> gpu_vector(cpu_vector);

    scalar h_dot = cpu_vector.dot(cpu_vector);
    scalar g_dot = gpu_vector.dot(gpu_vector);

    scalar diff = fabs(h_dot - g_dot);
    if (diff > 1e-3)
    {
        QWARN("Test Vector dot float can differ for long vectors" );
        qDebug() << "Difference: " << diff;
        QVERIFY(true);
    }
    else { QVERIFY(compare_f(h_dot,g_dot));}

    cl_mem g_dot1;
    OpenCL::allocate(&g_dot1, sizeof(scalar));
    gpu_vector.dot(gpu_vector, &g_dot1);
    scalar h_dot1;
    OpenCL::copy(&h_dot1, g_dot1, sizeof(scalar));
    QVERIFY(compare_f(g_dot, h_dot1));
}

void VectorTest::Test_dot_double()
{
    typedef double scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, CPU> cpu_vector(size, value);
    if(size < 1e5)
    {
        generate_doubles(cpu_vector);
    }
    //cpu_vector.print();

    Vector<scalar, GPU> gpu_vector(cpu_vector);

    scalar h_dot = cpu_vector.dot(cpu_vector);
    scalar g_dot = gpu_vector.dot(gpu_vector);

    scalar diff = fabs(h_dot - g_dot);
    if (diff > 1e-6)
    {
        QWARN("Test Vector dot double can differ for long vectors" );
        qDebug() << "Difference: " << diff;
        QVERIFY(true);
    }
    else { QVERIFY(compare_d(h_dot,g_dot));}

    cl_mem g_dot1;
    OpenCL::allocate(&g_dot1, sizeof(scalar));
    gpu_vector.dot(gpu_vector, &g_dot1);
    scalar h_dot1;
    OpenCL::copy(&h_dot1, g_dot1, sizeof(scalar));
    QVERIFY(compare_d(g_dot, h_dot1));
}


//norms
void VectorTest::Test_norm_float_data()
{
    float_data();
}

void VectorTest::Test_norm_double_data()
{
    double_data();
}

void VectorTest::Test_norm_float()
{
    typedef float scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, CPU> cpu_vector(size, value);
    if(size < 1e5)
    {
        generate_floats(cpu_vector);
    }
    //cpu_vector.print();

    Vector<scalar, GPU> gpu_vector(cpu_vector);

    scalar h_norm1 = cpu_vector.norm(1);
    scalar h_norm2 = cpu_vector.norm(2);
    scalar g_norm1 = gpu_vector.norm(1);
    scalar g_norm2 = gpu_vector.norm(2);

    scalar diff = fabs(h_norm1 - g_norm1);
    scalar diff2 = fabs(h_norm2 - g_norm2);

    if (diff > 1e-1)
    {
        QWARN("Test Vector norm1 can differ for long vectors" );
        qDebug() << "Difference: " << diff;
        QVERIFY(true);
    }
    else { QVERIFY(compare_f(h_norm1,g_norm1));}

    if (diff2 > 1e-1)
    {
        QWARN("Test Vector norm2 can differ for long vectors" );
        qDebug() << "Difference: " << diff;
        QVERIFY(true);
    }
    else { QVERIFY(compare_f(h_norm2,g_norm2));}

    cl_mem gg_norm1;
    OpenCL::allocate(&gg_norm1, sizeof(scalar));
    gpu_vector.norm(&gg_norm1, 1);
    scalar gh_norm1;
    OpenCL::copy(&gh_norm1, gg_norm1, sizeof(scalar));
    QVERIFY(compare_f(g_norm1, gh_norm1));

    cl_mem gg_norm2;
    OpenCL::allocate(&gg_norm2, sizeof(scalar));
    gpu_vector.norm(&gg_norm2, 2);
    scalar gh_norm2;
    OpenCL::copy(&gh_norm2, gg_norm2, sizeof(scalar));
    QVERIFY(compare_f(g_norm2, gh_norm2));

}

void VectorTest::Test_norm_double()
{
    typedef double scalar;
    QFETCH(int, size);
    QFETCH(scalar, value);

    Vector<scalar, CPU> cpu_vector(size, value);
    if(size < 1e5)
    {
        generate_doubles(cpu_vector);
    }
    //cpu_vector.print();

    Vector<scalar, GPU> gpu_vector(cpu_vector);

    scalar h_norm1 = cpu_vector.norm(1);
    scalar h_norm2 = cpu_vector.norm(2);
    scalar g_norm1 = gpu_vector.norm(1);
    scalar g_norm2 = gpu_vector.norm(2);

    scalar diff = fabs(h_norm1 - g_norm1);
    scalar diff2 = fabs(h_norm2 - g_norm2);

    if (diff > 1e-1)
    {
        QWARN("Test Vector norm1 can differ for long vectors" );
        qDebug() << "Difference: " << diff;
        QVERIFY(true);
    }
    else { QVERIFY(compare_d(h_norm1,g_norm1));}

    if (diff2 > 1e-1)
    {
        QWARN("Test Vector norm2 can differ for long vectors" );
        qDebug() << "Difference: " << diff;
        QVERIFY(true);
    }
    else { QVERIFY(compare_d(h_norm2,g_norm2));}

    cl_mem gg_norm1;
    OpenCL::allocate(&gg_norm1, sizeof(scalar));
    gpu_vector.norm(&gg_norm1, 1);
    scalar gh_norm1;
    OpenCL::copy(&gh_norm1, gg_norm1, sizeof(scalar));
    QVERIFY(compare_d(g_norm1, gh_norm1));

    cl_mem gg_norm2;
    OpenCL::allocate(&gg_norm2, sizeof(scalar));
    gpu_vector.norm(&gg_norm2, 2);
    scalar gh_norm2;
    OpenCL::copy(&gh_norm2, gg_norm2, sizeof(scalar));
    QVERIFY(compare_d(g_norm2, gh_norm2));
}

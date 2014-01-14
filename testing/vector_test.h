#ifndef VECTOR_TEST_H
#define VECTOR_TEST_H

#include "ocl.h"
#include "vector.h"

class VectorTest : public QObject
{
    Q_OBJECT

public:
    VectorTest();

private:

    //requred for initialization of OpenCL library
     OpenCL* ocl;

     //common init data
     void float_data();
     void double_data();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    //test + operator
    void Operator_plus_float();
    void Operator_plus_float_data();

    void Operator_plus_double();
    void Operator_plus_double_data();


};

#endif // VECTOR_TEST_H

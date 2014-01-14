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

     inline float f_rand() { return (float) rand() / (float)RAND_MAX; }
     inline double d_rand() { return (double) rand() / (double)RAND_MAX; }
     inline bool compare_f(float f1, float f2, float eps = 1e-1)
     {
         float diff = f1 - f2;
         //printf("DIFF _float: %f \n",diff);
         bool comp = ((diff <= eps) && (diff >= -eps));
         return comp;
     }

     inline bool compare_d(double f1, double f2, double eps = 1e-5)
     {
         double diff = f1 - f2;
         //printf("DIFF _double: %g \n",diff);
         bool comp = ((diff <= eps) && (diff >= -eps));
         return comp;
     }

     inline void generate_floats(Vector<float, CPU>& v)
     {
         float* data = v.get_data();
         for(int i = 0; i < v.get_size(); i++)
             data[i] = f_rand();

     }

     inline void generate_doubles(Vector<double, CPU>& v)
     {
         double* data = v.get_data();
         for(int i = 0; i < v.get_size(); i++)
             data[i] = d_rand();

     }

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    //operator =
    void Operator_eq_float();
    void Operator_eq_float_data();

    void Operator_eq_double();
    void Operator_eq_double_data();

    //operator - and -=
    void Operator_sub_float();
    void Operator_sub_float_data();

    void Operator_sub_double();
    void Operator_sub_double_data();

    //operator * and *=
    void Operator_mul_float();
    void Operator_mul_float_data();

    void Operator_mul_double();
    void Operator_mul_double_data();

    //operator + and +=
    void Operator_plus_float();
    void Operator_plus_float_data();

    void Operator_plus_double();
    void Operator_plus_double_data();

    //from cpu and gpu constructors
    void Operator_constr_float();
    void Operator_constr_double();

    void Operator_constr_float_data();
    void Operator_constr_double_data();

    //sum operator
    void Test_sum_float_data();
    void Test_sum_double_data();
    void Test_sum_float();
    void Test_sum_double();

    //norm
    void Test_norm_float_data();
    void Test_norm_double_data();
    void Test_norm_float();
    void Test_norm_double();

    //dot product
    void Test_dot_float_data();
    void Test_dot_double_data();
    void Test_dot_float();
    void Test_dot_double();



};

#endif // VECTOR_TEST_H

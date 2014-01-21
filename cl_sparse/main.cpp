#include <QtCore/QCoreApplication>

#include "ocl.h"
#include "vector.h"
#include "matrixcoo.h"
#include "timer.h"

template<typename scalar>
void vector_tests();

template<typename scalar>
bool compare_scalars(const scalar reference, const scalar data, const scalar epsilon)
{

    scalar diff = reference - data;
    bool comp = ((diff <= epsilon) && (diff >= -epsilon));

    return comp;
}
template<typename scalar>
bool compare_array(const scalar* reference, const scalar* data, const int len, const scalar epsilon)
{
    bool result = true;
    unsigned int error_count = 0;

    for (int i = 0; i < len; i++)
    {
        scalar diff = reference[i] - data[i];
        bool comp = ((diff <= epsilon) && (diff >= -epsilon));

        result &= comp;
        error_count += !comp;
    }

    return result;
}



int main(int argc, char *argv[])
{

    // -1 = best device with most compute units
    // =>0 - device_id usually same as nvidia_smi ids.


    std::string fmtx = "/media/Storage/matrices/Bell_MTX/cant.mtx"; //orani678.mtx";
    //std::string fmtx = "/media/Storage/matrices/vampir/pores_1.mtx";
    typedef float scalar;

    OpenCL ocl(1);

    MatrixCOO<scalar, GPU> coo;
    coo.load(fmtx);

    Vector<scalar, GPU> x(coo.get_ncol(), 1.0);
    Vector<scalar, GPU> b(coo.get_nrow(), 0.0);
    for (int i = 0; i < 50; i++)
        coo.multiply(x,b);

    Timer t;
    for (int i = 0; i < 50; i++)
        coo.multiply(x,b);
    printf("T: %g\n", t.elapsed_seconds());
    printf("coo b norm %f\n", b.norm());
    b.save("b_coo_result.txt");



    MatrixCSR<scalar, GPU> csr;
    csr.load(fmtx);

    b.set(0.0);
    t.reset();
    for (int i = 0; i < 50; i++)
        csr.multiply(x, b);
    printf("T: %g\n", t.elapsed_seconds());
    printf("csr b norm %f\n", b.norm());
    b.save("b_csr_result.txt");



    MatrixELL<scalar, GPU> ell;
    ell.load(fmtx);

    b.set(0.0);
    t.reset();
    for (int i = 0; i < 50; i++)
        ell.multiply(x, b);
    printf("T: %g\n", t.elapsed_seconds());
    printf("ell b norm %f\n", b.norm());
    b.save("b_ell_result.txt");

    ocl.shutdown ();

}

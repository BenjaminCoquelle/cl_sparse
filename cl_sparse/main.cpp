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


    std::string fmtx = "/media/Storage/matrices/Bell_MTX/orani678.mtx";
    //std::string fmtx = "/media/Storage/matrices/vampir/pores_1.mtx";
    typedef double scalar;

    OpenCL ocl(1);

    MatrixCOO<scalar, GPU> coo;
    coo.load(fmtx);

    Vector<scalar, GPU> x(coo.get_ncol(), 1.0);
    Vector<scalar, GPU> b(coo.get_nrow(), 0.0);
    coo.multiply(x,b);
    printf("coo b norm %f\n", b.norm());
    b.save("b_coo_result.txt");



    MatrixCSR<scalar, GPU> csr;
    csr.load(fmtx);

    Vector<scalar, GPU> in(csr.get_ncol(), 1.0);
    Vector<scalar, GPU> out(csr.get_nrow(), 0.0);
    csr.multiply(in, out);
    printf("csr b norm %f\n", out.norm());
    out.save("b_csr_result.txt");



    MatrixELL<scalar, CPU> ell;
    ell.load(fmtx);


    Vector<scalar, CPU> a(ell.get_ncol(),1.0);
    Vector<scalar, CPU> c(ell.get_nrow(), 0.0);
    ell.multiply(a,c);
    printf("ell b norm %f\n", c.norm());
    c.save("b_ell_result.txt");

    ocl.shutdown ();

}

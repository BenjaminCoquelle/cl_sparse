#include <QtCore/QCoreApplication>

#include "vector.h"
#include "matrix.h"
#include "utils/scatter.h"
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
    typedef float scalar;
    OpenCL ocl(0);

    int input[10] = {5, 7, 2, 3, 12, 6, 9, 8, 1, 4};//{1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    int m[10] =     {5, 3, 8, 7, 0 , 4, 1, 2, 9, 6};//{0, 5, 1, 6, 2, 7, 3, 8, 4, 9};

    Vector<int, CPU> in (10, input);
    in.print(10);
    Vector<int, CPU> map(10, m);
    map.print(10);
    Vector<int, CPU> out(10);
    out.set(0);
    out.print(10);

    scatter(0, 10, in, map, out);
    qDebug() << "Result\n";
    out.print(10);

    ocl.shutdown ();

}

/**


  //TEST MATRIX SPMV put that inside ocl
    QList<std::string> matrices;

    matrices.append("/home/kuba/External/UFget/MM/mtx/cant.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/consph.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/rma10.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/pwtk.mtx");
    //matrices.append("/home/kuba/External/UFget/MM/mtx/rail4284.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/pdb1HYS.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/raefsky3.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/mc2depi.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/scircuit.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/tsopf_rs_2383.mtx");
    matrices.append("/home/kuba/External/UFget/MM/mtx/shipsec1.mtx");


    foreach (std::string fmtx, matrices)
    {
        fprintf(stdout, "%s\n", fmtx.c_str());
        MatrixCSR<scalar, GPU> csr;
        csr.load(fmtx);

        Vector<scalar, GPU> x(csr.get_ncol(), 1.0);
        Vector<scalar, GPU> b(csr.get_nrow(), 0.0);

        Timer t;


        //WITH THIS PIECE OF CODE WE WANT TO USE TEXTURE MEMORY

        int WIDTH = 8192;
        int HEIGHT= 1 + ((x.get_csize() - 1) >> 15);
        fprintf(stdout, "len: %d, rowDim: %d\n", x.get_csize(), HEIGHT);

        scalar* input = new scalar [x.get_csize()];
        OpenCL::copy(input, x.get_cdata(), x.get_csize()*sizeof(scalar));
        scalar* image2dVec = new scalar [WIDTH*HEIGHT*4];

        memset(image2dVec, 0, sizeof(float)*WIDTH*HEIGHT*4);
        for(int i = 0; i<x.get_csize(); i++)
        {
            image2dVec[i] = input[i];
        }

        size_t origin[] = {0, 0, 0};
        size_t vectorSize[] = {WIDTH, HEIGHT, 1};

        const cl_image_format floatFormat =
        {
            //CL_R works for vector kernel. CL_RGBA działa na scalar
                CL_R,
                CL_FLOAT,
        };
        cl_int error;
        cl_mem devTexVec = clCreateImage2D(OpenCL::get_context(), CL_MEM_READ_ONLY, &floatFormat, WIDTH, HEIGHT, 0, NULL, &error );
        //printf("Create devTexVec: %s\n", OpenCL::getError(error).toStdString().c_str());
        error = clEnqueueWriteImage(OpenCL::get_queue(), devTexVec, CL_TRUE, origin, vectorSize, 0, 0, image2dVec, 0, NULL, NULL);
        clFinish(OpenCL::get_queue());
        //printf("Write to devTexVec: %s\n", OpenCL::getError(error).toStdString().c_str());


        b.set(0.0);
        t.reset();
        for (int i = 0; i < 50; i++)
            csr.multiply(x, b, true);
        printf("Time scalar: %gms\n", (1000.0*t.elapsed_seconds())/50);
        printf("csr b norm %f\n", b.norm());

        b.set(0.0);
        t.reset();
        for (int i = 0; i < 50; i++)
            csr.multiply(x, b, false);
        printf("Time vector: %gms\n", (1000.0*t.elapsed_seconds())/50);
        printf("csr b norm %f\n", b.norm());


//        b.set(0.0);
//        t.reset();
//        for (int i = 0; i < 50; i++)
//            csr.test1(x,b, devTexVec);
//        printf("Time scalar TEX: %gms\n", (1000.0*t.elapsed_seconds())/50);
//        printf("csr scalar TEX b norm %f\n", b.norm());


        b.set(0.0);
         t.reset();
         for (int i = 0; i < 50; i++)
             csr.test2(x, b, devTexVec);
         printf("Time vector TEX: %gms\n",(1000.0*t.elapsed_seconds())/50);
         printf("test b norm %f\n", b.norm());

         printf("\n\n");

             MatrixELL<scalar, GPU> ell;
             ell.load(fmtx);

             b.set(0.0);
             t.reset();
             for (int i = 0; i < 50; i++)
                 ell.multiply(x, b);
             printf("T: %g\n",(1000* t.elapsed_seconds())/50);
             printf("ell b norm %f\n", b.norm());
             b.save("b_ell_result.txt");
    }




//    MatrixELL<scalar, GPU> ell;
//    ell.load(fmtx);

//    b.set(0.0);
//    t.reset();
//    for (int i = 0; i < 50; i++)
//        ell.multiply(x, b);
//    printf("T: %g\n", t.elapsed_seconds()/50);
//    printf("ell b norm %f\n", b.norm());
//    b.save("b_ell_result.txt");

//    MatrixHYB<scalar, GPU> hyb;
//    hyb.load(fmtx);

//    b.set(0.0);
//    t.reset();
//    for (int i = 0; i < 50; i++)
//        hyb.multiply(x, b);
//    printf("T: %g\n", t.elapsed_seconds()/50);
//    printf("hyb b norm %f\n", b.norm());
//    b.save("b_hyb_result.txt");

  */

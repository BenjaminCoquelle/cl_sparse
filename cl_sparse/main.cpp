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

    typedef double scalar;

    OpenCL ocl(1);


    vector_tests<scalar>();




    ocl.shutdown ();

}

template<typename scalar>
void vector_tests()
{
    { //norm to CPU
        Vector<scalar, GPU> g1(1e5, -1.1);
        Vector<scalar, CPU> v1(1e5, -1.1);
        scalar host = v1.norm(1);
        scalar gpu = g1.norm(1);
        printf("host = %f, gpu = %f\n", host, gpu);
        if (!compare_scalars(v1.norm(1), g1.norm(1), scalar(1e-2)))
        {
            printf("GPU norm 1 = %f, CPU norm = %f\n", g1.norm(1), v1.norm(1));
        }
        else
        {
            printf("Norm 1 to CPU OK\n");
        }

    }


    //vector_tests<scalar>();
    {//norm to GPU
        printf("\nTesting norm 1 to GPU\n");
        Vector<scalar, GPU> v1(1e4, -0.125);
        Vector<scalar, CPU> h1(1e4, -0.125);

        cl_mem gn;
        OpenCL::allocate(&gn, sizeof(scalar));
        v1.norm(&gn, 1);

        scalar hgn = 0;
        OpenCL::copy(&hgn, gn, sizeof(scalar));
        printf("Norm from gpu %f\n", hgn);
        if(!compare_scalars(hgn, h1.norm(1), scalar(1e-6)))
        {
            printf("WRONG norm 1 calc: GPU: %f, CPU: %f\n", hgn, h1.norm(1));
        }
        else
        {
            printf("Norm 1 to GPU OK\n");
        }
    }

    { //norm to CPU
        printf("\nTesting norm 2 to CPU \n");
        Vector<scalar, GPU> g1(100e2, -1.);
        Vector<scalar, CPU> v1(100e2, -1.);
        if (!compare_scalars(v1.norm(2), g1.norm(2), scalar(1e-6)))
        {
            printf("GPU norm 2 = %f, CPU norm = %f\n", g1.norm(2), v1.norm(2));
        }
        else
        {
            printf("Norm 2 to CPU OK, %f\t%f\n", v1.norm(2), g1.norm(2));
        }

    }

    {//norm to GPU
        printf("\nTesting norm 2 to GPU \n");
        Vector<scalar, GPU> v1(100e2, -1.);
        Vector<scalar, CPU> h1(100e2, -1.);

        cl_mem gn;
        OpenCL::allocate(&gn, sizeof(scalar));
        v1.norm(&gn, 2);
        scalar hgn = 0;
        OpenCL::copy(&hgn, gn, sizeof(scalar));
        if(!compare_scalars(hgn, h1.norm(2), scalar(1e-6)))
        {
            printf("WRONG norm 2 calc: GPU: %f, CPU: %f\n", hgn, h1.norm(2));
        }
        else
        {
            printf("Norm 2 to GPU OK\n");
        }
    }

    {//norm to GPU
        Vector<scalar, GPU> v1(1e4, -0.125);
        Vector<scalar, CPU> h1(1e4, -0.125);

        cl_mem gn;
        OpenCL::allocate(&gn, sizeof(scalar));
        v1.norm(&gn, 1);
        scalar hgn = 0;
        OpenCL::copy(&hgn, gn, sizeof(scalar));
        if(!compare_scalars(hgn, h1.norm(1), scalar(1e-6)))
        {
            printf("WRONG norm calc: GPU: %f, CPU: %f\n", hgn, h1.norm(1));
        }
    }

    { //norm to CPU
        Vector<scalar, GPU> g1(1e3, -1.235);
        Vector<scalar, CPU> v1(1e3, -1.235);
        if (!compare_scalars(v1.norm(), g1.norm(), scalar(1e-6)))
        {
            printf("GPU norm = %f, CPU norm = %f\n", g1.norm(1), v1.norm(1));
        }

    }

    {
        Vector<scalar, CPU> h1(1e7, 0.125);
        Vector<scalar, CPU> h2 = h1;

        Vector<scalar, GPU> v1 = h1;
        Vector<scalar, GPU> v2 = v1;


        if(compare_scalars(h1.sum(), v1.sum(), scalar(1e-6)))
        {
            printf("SUM OK\n");
        }
        if(compare_scalars(h1.dot(h2), v1.dot(v2), scalar(1e-6)))
        {
            printf("DOT OK\n");
        }

        cl_mem gd;
        OpenCL::allocate(&gd, sizeof(scalar));
        scalar hd = v1.dot(v2);
        v1.dot(v2, &gd);
        scalar hgd = 0;
        OpenCL::copy(&hgd, gd, sizeof(scalar));
        if(!compare_scalars(hgd, hd, scalar(1e-6)))
        {
            printf("WRONG: GPU: %f, CPU: %f\n", hgd, hd);
        }
    }
    /**
        Vector reduction tests - test various sizes - correctness test;
      */

        for (int s = 32768; s < 8388608; s+=2250000) //8388608
        {
            int times = 100;
            Vector<scalar, GPU> g6(s, 1.000f);
            Vector<scalar, CPU> h5(s, 1.000f);

            //host sum
            scalar hs = 0.0;
            //host reduction time
            scalar htr = 0.0;

            //device sum
            scalar ds = 0.0;
            //device reduction time to host
            scalar dtr = 0.0;

            //device sum
            cl_mem ds1; OpenCL::allocate (&ds1, 1*sizeof(scalar));
            //device reduction time to device;
            scalar dtr1 = 0.0;

            Timer t;
            for (int i = 0; i < times; i++)
            {
                hs = h5.sum();
            }
            double ht = t.elapsed_seconds () / times;
            // briefly we are not counting the writing of local chunks
            htr = 1.0e-9 * ((double)s*sizeof(scalar))/ht;

            t.reset ();
            for (int i = 0; i < times; i++)
            {
                ds = g6.sum();
            }
            double dt = t.elapsed_seconds () / times;
            dtr = 1.0e-9 * ((double)s*sizeof(scalar))/dt;

            t.reset ();
            for (int i = 0; i < times; i++)
            {
                g6.sum(&ds1);
            }
            double dt1 = t.elapsed_seconds () / times;
            dtr1 = 1.0e-9 * ((double)s*sizeof(scalar))/dt1;

            scalar hs1 = 0.0;
            OpenCL::copy (&hs1, ds1, sizeof(scalar));

            bool passed = compare_array(&hs, &ds, 1, scalar(1e-6));
            bool passed2 = compare_array(&hs, &hs1, 1, scalar(1e-6));
            if (!passed || !passed2)
            {
                printf("\t\tSum failed for size [%d], CPU=%f, GPU=%f, GPU2=%f\n", s, hs, ds, hs1);
            }
            else
            {
                printf ("%d\tCPU: time = %fs, mem = %f GB/s \t GPU: time = %fs, mem = %fGB/s \t GPU2: time = %fs, mem: %fGB/s\n", s, ht, htr, dt, dtr, dt1, dtr1);
            }
        }

    //Test Vector Operators
    const int size = 1e6;
    unsigned int bytes = size * sizeof(scalar);

    qDebug() << "Test vector kernels for size = " << size;

    Vector<scalar, GPU> g1(size, 1.f);
    Vector<scalar, GPU> g2(size, 2.f);

    Vector<scalar, CPU> h1(size, 1.f);
    Vector<scalar, CPU> h2(size, 2.f);

    g1.print();
    g2.print();


    double time;
    Timer t;
    double troughput;
    int times = 100;


{
    qDebug() << "\nGPU = GPU + GPU";
    t.reset();
    time = t.elapsed_seconds();
    qDebug() << "elapsed: " << time;
    Vector<scalar, GPU> gx;
    for(int i = 0; i < times; i++)
        gx = g1 + g2;

    time = t.elapsed_seconds()/times;
    qDebug() << "Time: "<< time;
    troughput = 1.0e-9 * ((double)bytes)/time;
    gx.print();
    qDebug() << "GPU Vector added and created in " << time << "s " << troughput << " GB/s."  ;

}

{
    qDebug() << "\nGPU += GPU";
    Vector<scalar, GPU> gx(g1.get_size(),0);
    t.reset();
    time = t.elapsed_seconds();
    qDebug() << "elapsed: " <<  time;
    for(int i = 0; i < times; i++)
        gx += g1;
    time = t.elapsed_seconds()/times;
    qDebug() << "Time: "<< time;
    troughput = 1.0e-9 * ((double)bytes)/time;
    gx.print();
    qDebug() << "GPU Vector += " << time << "s " << troughput << " GB/s.";
}


{
    qDebug() << "GPU -= GPU";
    Vector<scalar, GPU> gx(g1.get_size(),10);
    t.reset();
    for(int i = 0; i < times; i++)
        gx -= g1;
    time = t.elapsed_seconds()/times;
    qDebug() << "Time: "<< time;
    troughput = 1.0e-9 * ((double)bytes)/time;
    gx.print();
    qDebug() << "GPU Vector -= " << time << "s " << troughput << " GB/s.";
}


{
    qDebug() << "\nGPU *= GPU";
    Vector<scalar, GPU> gx(g1.get_size(),2);
    t.reset();
    for(int i = 0; i < 4; i++)
        gx *= gx;
    time = t.elapsed_seconds()/4;
    qDebug() << "Time: "<< time;
    troughput = 1.0e-9 * ((double)bytes)/time;
    gx.print();
    qDebug() << "GPU Vector *= " << time << "s " << troughput << " GB/s.";
}


{
    qDebug() << "\nCPU = CPU + CPU";
    t.reset();
    Vector<scalar, CPU> hx;
    for(int i = 0; i < times; i++)
        hx = h1 + h2;

    Vector<scalar, CPU> h3 = hx;
    time = t.elapsed_seconds()/times;
    qDebug() << "Time: "<< time;
    troughput = 1.0e-9 * ((double)bytes)/time;
    h3.print();
    qDebug() << "CPU Vector added and created in " << time << "s " << troughput << " GB/s.";
}
}




//    MatrixCOO<scalar, CPU> hm;
//    hm.load("/media/Storage/matrices/vampir/Hamrle1.mtx");
//    hm.save("cpu_matrix.txt");
//    Vector<scalar, CPU> hx(hm.get_nrow());
//    Vector<scalar, CPU> hy(hm.get_nrow());

//    hx.set(1.f);
//    hy.set(0.f);

//    hm.multiply(hx,&hy);
//    hy.save("cpu.txt");
//    printf("\nNorm: %f\n",hy.norm2());

//    MatrixCOO<scalar, GPU> gm;
//    gm.load("/media/Storage/matrices/vampir/Hamrle1.mtx");
//    gm.save("gpu_matrix.txt");
//    Vector<scalar, GPU> gx(gm.get_nrow());
//    Vector<scalar, GPU> gy(gm.get_nrow());

//    gx.set(1.f);
//    gx.print(hm.get_nrow());
//    gy.set(0.f);
//    gy.print(hm.get_nrow());


//    gm.multiply(gx,&gy);
//    gy.save("gpu.txt");

    /**
      MATRIX TESTS
      */


//    const int times = 1;

//    MatrixCOO<scalar, CPU>* host_matrix = new MatrixCOO<scalar, CPU>();
//    MatrixCOO<scalar, GPU>* device_matrix = new MatrixCOO<scalar, GPU>();

//    //load correctly handle the memory release and free;
//    host_matrix->load("/home/kuba/OpenFOAM/TestMatricesForAMG/tet/ICA_tet_236k/p_equation_matrix.mtx");     //("../mat65k.mtx");
//    device_matrix->load("/home/kuba/OpenFOAM/TestMatricesForAMG/tet/ICA_tet_236k/p_equation_matrix.mtx");   //("../mat65k.mtx");

//    Vector<scalar, CPU>* host_in = new Vector<scalar, CPU>(host_matrix->get_ncol(), 1.0f);
//    Vector<scalar, GPU>* device_in = new Vector<scalar, GPU>(host_matrix->get_ncol(), 1.0f);

//    Vector<scalar, CPU>* host_out = new Vector<scalar, CPU>(host_matrix->get_nrow(), 0.0f);
//    Vector<scalar, GPU>* device_out = new Vector<scalar, GPU>(host_matrix->get_nrow(), 0.0f);

//    double host_time = 0.0;
//    double device_time = 0.0;

//    Timer t;
//    for (int i = 0; i < times; i++)
//    {
//        host_matrix->multiply(*host_in, host_out);
//    }
//    host_time = t.elapsed_seconds () / times;
//    printf("CPU: %f GB/s\t", host_matrix->get_troughput(host_time));

//    t.reset ();
//    for (int i = 0; i < times; i++)
//    {
//        device_matrix->multiply(*device_in, device_out);
//    }
//    device_time = t.elapsed_seconds () / times;
//    printf("GPU: %f GB/s\n", device_matrix->get_troughput(device_time));

//    /** TODO: check the results with out; */

//    scalar* host_device_result = new scalar [device_out->get_size()];
//    OpenCL::copy(host_device_result, device_out->get_data(), device_out->get_size() * sizeof(scalar));

//    bool equal = compare_scalars(host_out->get_data(), host_device_result, device_out->get_size(), 1.0e-14 );
//    if(!equal)
//        printf("\n\tSomething is wrong!!!\n");

//    host_out->save("host.txt");
//    device_out->save("device.txt");

//    delete [] host_device_result;
//    delete host_in;
//    delete device_in;
//    delete host_out;
//    delete device_out;

//    delete host_matrix;
//    delete device_matrix;
/**
    //Test Vector kernels
    const int size = 8388608;
    unsigned int bytes = size * sizeof(scalar);


    qDebug() << "Test vector kernels for size = " << size;

    Vector<scalar, GPU> g1(size, 1.25f);
    Vector<scalar, GPU> g2(size, 1.25f);

    Vector<scalar, CPU> h1(size, 1.25f);
    Vector<scalar, CPU> h2(size, 1.25f);

    g1.print();
    g2.print();

    double time;
    Timer t;
    double troughput;

    Vector<scalar, GPU> g3 = g1 + g2;

    time = t.elapsed_seconds();
    troughput = 1.0e-9 * ((double)bytes)/time;

    g3.print();
    qDebug() << "GPU Vector added and created in " << time << "s " << troughput << " GB/s."  ;

    t.reset();
    Vector<scalar, CPU> h3 = h1 + h2;
    time = t.elapsed_seconds();
    troughput = 1.0e-9 * ((double)bytes)/time;

    h3.print();
    qDebug() << "CPU Vector added and created in " << time << "s " << troughput << " GB/s.";



    //sum of vector elements;
    Vector<scalar, GPU> g4(size, 1.000f);
    Vector<scalar, GPU> g5(size, 1.000f);
    Vector<scalar, CPU> h4(size, 1.000f);

    int times = 32;
    scalar suma = 0.0;
    t.reset();
    for (int i = 0; i < times ; i++)
        suma = g4.sum();

    time = t.elapsed_seconds()/(double)times;
    troughput = 1.0e-9 * ((double)bytes)/time;
    printf("GPU Vector reduced: %f in %fs (%f GB/s).\n", suma, time, troughput);

    t.reset();
    for (int i = 0; i < times ; i++)
        suma = g5.sum2();

    time = t.elapsed_seconds()/(double)times;
    troughput = 1.0e-9 * ((double)bytes)/time;
    printf("GPU Vector reduced: %f in %fs (%f GB/s).\n", suma, time, troughput);

    t.reset();
    for (int i = 0; i < times; i++)
        suma = h4.sum();
    time = t.elapsed_seconds()/(double)times;
    troughput = 1.0e-9 * ((double)bytes)/time;
    printf("CPU Vector reduced: %f in %fs (%f GB/s).\n", suma, time, troughput );
  */




/*******************************
  SOME DEBUG STUFF

  ******************************/
//    Vector<double, CPU>* d = new Vector<double, CPU>(200, 12);
//    d->print(3);
//    d->set(14);
//    d->print(7);
//    delete d;

//    qDebug() << "Add 2 vectors";

//    Vector<scalar, CPU> a1(3,-3);
//    Vector<scalar, CPU> a2(3,2);

//    a1.print();
//    a2.print();

//    Vector<scalar, CPU> a3 = a1 + a2;
//    a3.print();

//    Vector<double, CPU> ones(10, 1);

//    qDebug() << ones.dot(ones);
//    qDebug() << ones.norm1();
//    qDebug() << ones.norm2();
//    qDebug() << ones.sum();


//    qDebug() << "Testing memory";
//    cl_mem buff1;
//    cl_mem buff2;
//    cl_mem buff3;
//    const int rozmiar = 10000;
//    scalar host_buff1[rozmiar];
//    scalar host_buff2[rozmiar];

//    Vector<scalar,CPU> two(rozmiar, 2);

//    std::fill(host_buff1, host_buff1+rozmiar, 3.1);
//    std::fill(host_buff2, host_buff2+rozmiar, -9999.9);

//    //allocate + copy at once
//    OpenCL::allocate(&buff1, rozmiar*sizeof(scalar), CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, host_buff1);
//    //just allocate
//    OpenCL::allocate(&buff2, rozmiar*sizeof(scalar));
//    OpenCL::allocate(&buff3, rozmiar*sizeof(scalar));

//    cl_event readEvent;
//    OpenCL::copy(buff2, host_buff2, rozmiar*sizeof(scalar), CL_FALSE, &readEvent);
//    OpenCL::copy(host_buff1, buff2, rozmiar*sizeof(scalar), CL_FALSE, &readEvent);

//    qDebug() << "Wyniki operacji na pamięci";

//    //vital part of openCL, mem is ready after the completion of scheduled commands in the queue;
//    //this line is unsure sometimes will be 3.1 but sometimes -9999.9
//    qDebug() << host_buff1[0] << ", " << host_buff1[1]<< ", " <<  host_buff1[2];

//    //after this call host_buff1 will be always good. There is clWaitForAnyEvent
//    clWaitForEvents(1, &readEvent);
//    //there is cl_flush() which executes queue and go further
//    qDebug() << host_buff1[0] << ", " << host_buff1[1]<< ", " <<  host_buff1[2];

//    cl_event copyEvent;
//    OpenCL::copy(buff3, buff2, rozmiar*sizeof(scalar), CL_FALSE, &copyEvent);
//    clWaitForEvents(1, &copyEvent);

//    OpenCL::copy(host_buff2, buff3, rozmiar*sizeof(scalar), CL_TRUE, &copyEvent);
//    clWaitForEvents(1, &copyEvent);
//    qDebug()<< "gpuGPU: " << host_buff2[0] << ", " << host_buff2[1]<< ", " <<  host_buff2[2];


//    qDebug() << "Testing memory END";

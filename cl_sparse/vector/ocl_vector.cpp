#include "ocl.h"
#include "vector.h"
#include "ocl_vector.h"

#ifdef CL_BLAS
    #include "clBLAS.h"
#endif

#include <math.h>
#include <QFile>

/*basically what you would do is something like this:
setup buffer N & N + 1
enqueue buffer N
enqueue kernel N
setup buffer N + 2
enqueue Buffer N + 1
enqueue kernel N + 1
readback buffer N
setup buffer N + 3
enqueue Buffer N + 2
enqueue kernel N + 2
readback buffer N + 1
etc...*/

template<typename scalar>
size_t Vector<scalar, GPU>::roundUp(int group_size, int global_size)
{
    int r = global_size % group_size;
    if(r == 0)
    {
        return global_size;
    } else
    {
        return global_size + group_size - r;
    }
}

template<typename scalar>
Vector<scalar, GPU>::Vector()
{
    this->size = 0;
    this->data = NULL;
}

template<typename scalar>
Vector<scalar, GPU>::Vector(int size)
{
    this->size = size;
    OpenCL::allocate(&data, size*sizeof(scalar));
}

template<typename scalar>
Vector<scalar, GPU>::Vector(int size, scalar value, cl_mem_flags mem_flags)
{
    this->size = size;
    OpenCL::allocate(&data, size*sizeof(scalar), mem_flags);

    set(value);
}

//TODO: THERE ARE VARIOUS MEM FLAG COMBINATIONS
// would be nice to test performance when using host_ptr (CPU) and map buffer options
template<typename scalar>
Vector<scalar, GPU>::Vector(const Vector<scalar, GPU>& other)
{
    this->size = other.size;
    OpenCL::allocate(&data, size*sizeof(scalar));
    OpenCL::copy(data, other.data, size*sizeof(scalar));
}

template<typename scalar>
Vector<scalar, GPU>::Vector(const Vector<scalar,CPU>& other)
{
    this->size = other.size;
    OpenCL::allocate(&data, size*sizeof(scalar));
    OpenCL::copy(data, other.data, size*sizeof(scalar));
}

template<typename scalar>
Vector<scalar, GPU>& Vector<scalar,GPU>::operator= (const Vector<scalar, GPU>& other)
{
    if (!equal(other))
    {
        if (data != NULL)
        {
            qDebug() << "DELETING DATA";
            OpenCL::free(&data);
        }
        this->size = other.size;
        OpenCL::allocate(&data, size*sizeof(scalar));
    }
    OpenCL::copy(data, other.data, size*sizeof(scalar));
    return *this;
}

template<typename scalar>
Vector<scalar, GPU>& Vector<scalar,GPU>::operator= (const Vector<scalar, CPU>& other)
{
    if (!equal(other))
    {
        if (data != NULL)
        {
            qDebug() << "DELETING DATA";
            OpenCL::free(&data);
        }
        this->size = other.size;
        OpenCL::allocate(&data, size*sizeof(scalar));
    }
    OpenCL::copy(data, other.data, size*sizeof(scalar));
    return *this;
}

//operators
template<typename scalar>
const Vector<scalar, GPU> Vector<scalar,GPU>::operator+ (const Vector& other)
{
    if (!equal(other))
    {
        qCritical() << "Error Vector(GPU) +: Vectors have different sizes";
        return Vector(0);
    }

    Vector<scalar,GPU> result(*this);
#ifdef CL_BLAS
    //\f$ Y \leftarrow \alpha X + Y \f$
    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;

    if(sizeof(scalar) == 4)
        err = clblasSaxpy(this->get_size(), 1.0, other.get_cdata(), 0, 1, result.get_data(), 0, 1, 1, &q , 0, NULL, &event);
    else
        err = clblasDaxpy(this->get_size(), 1.0, other.get_cdata(), 0, 1, result.get_data(), 0, 1, 1, &q , 0, NULL, &event);

    if (err != CL_SUCCESS)
            qCritical() << "Vector op +: clblas axpy() failed with " << err;
    else
         err = clWaitForEvents(1, &event);

#else
    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_add"];
    if(sizeof(scalar) == 8)
        k = OpenCL::kernels["d_vector_add"];

    k.set_buffer(0, this->data);
    k.set_buffer(1, other.data);
    k.set_buffer(2, result.data);
    k.set_int(3, this->size);


    size_t szLocalWorkSize = 256;
    size_t szGlobalWorkSize = roundUp(szLocalWorkSize, this->size);
    k.execute(szLocalWorkSize, szGlobalWorkSize);
#endif
    return result;
}

template<typename scalar>
const Vector<scalar, GPU> Vector<scalar, GPU>::operator- (const Vector& other)
{
    if(!equal(other))
    {
        qCritical() << "Error Vector(GPU) -: Vectors have different sizes";
        return Vector(0);
    }

    Vector result(*this);
#ifdef CL_BLAS
    //\f$ Y \leftarrow \alpha X + Y \f$
    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;

    if(sizeof(scalar) == 4)
        err = clblasSaxpy(this->get_size(), -1.0, other.get_cdata(), 0, 1, result.get_data(), 0, 1, 1, &q , 0, NULL, &event);
    else
        err = clblasDaxpy(this->get_size(), -1.0, other.get_cdata(), 0, 1, result.get_data(), 0, 1, 1, &q , 0, NULL, &event);

    if (err != CL_SUCCESS)
            qCritical() << "Vector op +: clblas axpy() failed with " << err;
    else
         err = clWaitForEvents(1, &event);

#else
    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_sub"];
    if(sizeof(scalar) == 8)
        k = OpenCL::kernels["d_vector_sub"];

    k.set_buffer(0, this->data);
    k.set_buffer(1, other.data);
    k.set_buffer(2, result.data);
    k.set_int(3, this->size);

    size_t szLocalWorkSize = 256;
    size_t szGlobalWorkSize = roundUp(szLocalWorkSize, this->size);
    k.execute(szLocalWorkSize, szGlobalWorkSize);
#endif
    return result;
}

template<typename scalar>
const Vector<scalar, GPU> Vector<scalar, GPU>::operator* (const Vector& other)
{
    if(!equal(other))
    {
        qCritical() << "Error Vector(GPU): Vectors have different sizes;";
        return Vector(0);
    }

    Vector result(this->get_size());

    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_mul"];
    if(sizeof(scalar) == 8)
        k = OpenCL::kernels["d_vector_mul"];

    k.set_buffer(0, this->data);
    k.set_buffer(1, other.data);
    k.set_buffer(2, result.data);
    k.set_int(3, this->size);

    size_t szLocalWorkSize = 256;
    size_t szGlobalWorkSize = roundUp(szLocalWorkSize, this->size);
    k.execute(szLocalWorkSize, szGlobalWorkSize);
    return result;
}

template<typename scalar>
const Vector<scalar, GPU> Vector<scalar, GPU>::operator* (const scalar alpha)
{

    Vector<scalar, GPU> result(*this);
#ifdef CL_BLAS

    //\f$ Y \leftarrow alpha Y \f$
    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;

    if (sizeof(scalar) == 4)
        err = clblasSscal(result.get_size(), cl_float(alpha), result.get_data(), 0, 1, 1, &q, 0, NULL, &event);
    else
        err = clblasDscal(result.get_size(), cl_double(alpha), result.get_data(), 0, 1, 1, &q, 0, NULL, &event);

    if (err != CL_SUCCESS)
        qCritical() << "Vector op *alpha: clblas scal failed with " << err;
    else
        err = clWaitForEvents(1, &event);
#else

    Kernel k;
    if (sizeof(scalar) == 4)
    {
        k = OpenCL::kernels["s_vector_scale_scalar"];
        k.set_float(1, alpha);
    }
    else
    {
        k = OpenCL::kernels["d_vector_scale_scalar"];
        k.set_double(1, alpha);
    }

    k.set_buffer(0, result.get_data());
    k.set_int(2, result.get_size());

    size_t szLocalWorkSize = 256;
    size_t szGlobalWorkSize = roundUp(szLocalWorkSize, this->size);
    k.execute(szLocalWorkSize, szGlobalWorkSize);

#endif
    return result;
}


template<typename scalar>
const Vector<scalar, GPU> Vector<scalar, GPU>::operator*(const cl_mem alpha)
{

    Vector result(*this);

//#ifdef CL_BLAS
//    qCritical() << "clblasXscal uses alpha as type cl_float use different function";
//#else
    Kernel k;
    if (sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_scale_mem"];
    else
        k = OpenCL::kernels["d_vector_scale_mem"];

    k.set_buffer(0, result.get_data());
    k.set_buffer(1, alpha);
    k.set_int(2, result.get_size());

    size_t szLocalWorkSize = 256;
    size_t szGlobalWorkSize = roundUp(szLocalWorkSize, this->size);
    k.execute(szLocalWorkSize, szGlobalWorkSize);

//#endif
    return result;
}

template<typename scalar>
Vector<scalar, GPU>& Vector<scalar, GPU>::operator*= (const scalar alpha)
{
#ifdef CL_BLAS
    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;

    if (sizeof(scalar) == 4)
        err = clblasSscal(this->get_size(), cl_float(alpha), this->get_data(), 0, 1, 1, &q, 0, NULL, &event);
    else
        err = clblasDscal(this->get_size(), cl_double(alpha), this->get_data(), 0, 1, 1, &q, 0, NULL, &event);

    if (err != CL_SUCCESS)
        qCritical() << "Vector op *alpha: clblas scal failed with " << err;
    else
        err = clWaitForEvents(1, &event);
#else
    Kernel k;
    if (sizeof(scalar) == 4)
    {
        k = OpenCL::kernels["s_vector_scale_scalar"];
        k.set_float(1, alpha);
    }
    else
    {
        k = OpenCL::kernels["d_vector_scale_scalar"];
        k.set_double(1, alpha);
    }

    k.set_buffer(0, this->get_data());
    k.set_int(2, this->get_size());

    size_t szLocalWorkSize = 256;
    size_t szGlobalWorkSize = roundUp(szLocalWorkSize, this->size);
    k.execute(szLocalWorkSize, szGlobalWorkSize);

#endif
    return *this;

}

template<typename scalar>
Vector<scalar, GPU>& Vector<scalar, GPU>::operator*= (const cl_mem alpha)
{
//#ifdef CL_BLAS
//    qCritical() << "clblasXscal uses alpha as type cl_float/cl_double use different function";
//#else
    Kernel k;
    if (sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_scale_mem"];
    else
        k = OpenCL::kernels["d_vector_scale_mem"];

    k.set_buffer(0, this->get_data());
    k.set_buffer(1, alpha);
    k.set_int(2, this->get_size());

    size_t szLocalWorkSize = 256;
    size_t szGlobalWorkSize = roundUp(szLocalWorkSize, this->size);
    k.execute(szLocalWorkSize, szGlobalWorkSize);
//#endif

}




template<typename scalar>
Vector<scalar, GPU>& Vector<scalar, GPU>::operator+= (const Vector& other)
{
    if(!equal(other))
    {
        qCritical() << "Vector (GPU) +=: Vector have different sizes";
        return *this;
    }
#ifdef CL_BLAS
    //\f$ Y \leftarrow \alpha X + Y \f$
    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;

    if(sizeof(scalar) == 4)
        err = clblasSaxpy(this->get_size(), 1.0, other.get_cdata(), 0, 1, this->get_data(), 0, 1, 1, &q , 0, NULL, &event);
    else
        err = clblasDaxpy(this->get_size(), 1.0, other.get_cdata(), 0, 1, this->get_data(), 0, 1, 1, &q , 0, NULL, &event);

    if (err != CL_SUCCESS)
           qCritical() << "Vector op +: clblas axpy() failed with " << err;
    else
         err = clWaitForEvents(1, &event);

#else
    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_add"];
    if(sizeof(scalar) == 8)
        k = OpenCL::kernels["d_vector_add"];

    k.set_buffer(0, this->data);
    k.set_buffer(1, other.data);
    k.set_buffer(2, this->data);
    k.set_int(3, this->size);

    size_t szLocalWorkSize = 256;
    size_t szGlobalWorkSize = roundUp(szLocalWorkSize, this->size);
    k.execute(szLocalWorkSize, szGlobalWorkSize);

#endif
    return *this;

}

template<typename scalar>
Vector<scalar, GPU>& Vector<scalar, GPU>::operator-= (const Vector& other)
{
    if(!equal(other))
    {
        qCritical() << "Vector (GPU) -=: Vector have different sizes";
        return *this;
    }
#ifdef CL_BLAS
    //\f$ Y \leftarrow \alpha X + Y \f$
    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;

    if(sizeof(scalar) == 4)
        err = clblasSaxpy(this->get_size(), -1.0, other.get_cdata(), 0, 1, this->get_data(), 0, 1, 1, &q , 0, NULL, &event);
    else
        err = clblasDaxpy(this->get_size(), -1.0, other.get_cdata(), 0, 1, this->get_data(), 0, 1, 1, &q , 0, NULL, &event);

    if (err != CL_SUCCESS)
           qCritical() << "Vector op +: clblas axpy() failed with " << err;
    else
         err = clWaitForEvents(1, &event);

#else
    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_sub"];
    if(sizeof(scalar) == 8)
        k = OpenCL::kernels["d_vector_sub"];

    k.set_buffer(0, this->data);
    k.set_buffer(1, other.data);
    k.set_buffer(2, this->data);
    k.set_int(3, this->size);

    size_t szLocalWorkSize = 256;
    size_t szGlobalWorkSize = roundUp(szLocalWorkSize, this->size);
    k.execute(szLocalWorkSize, szGlobalWorkSize);
#endif
    return *this;
}

template<typename scalar>
Vector<scalar, GPU>& Vector<scalar, GPU>::operator*= (const Vector& other)
{
    if(!equal(other))
    {
        qCritical() << "Vector (GPU) *=: Vector have different sizes";
        return *this;
    }

    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_mul"];
    if(sizeof(scalar) == 8)
        k = OpenCL::kernels["d_vector_mul"];

    k.set_buffer(0, this->data);
    k.set_buffer(1, other.data);
    k.set_buffer(2, this->data);
    k.set_int(3, this->size);

    size_t szLocalWorkSize = 256;
    size_t szGlobalWorkSize = roundUp(szLocalWorkSize, this->size);
    k.execute(szLocalWorkSize, szGlobalWorkSize);
    return *this;

}

template<typename scalar>
void Vector<scalar, GPU>::set(scalar value)
{
    //Should we wait here for data to be copied?
    //TODO: zmierzyć ten czas z i bez.
    cl_event fill_event;
    cl_int error;

#ifdef CL_VERSION_1_2    //NVIDIA OpenCL does not support 1.2 :(
    cl_int error = clEnqueue
            clEnqueueFillBuffer(OpenCL::get_queue(), data,
                                &value,
                                sizeof(scalar),
                                0,
                                size*sizeof(scalar), 0, 0, &fill_event);
    clWaitForEvents(1, &fill_event);
     if(error != CL_SUCCESS)
         qCritical() << "Vector (GPU): set failed, Fill buffer: " << OpenCL::getError(error);

     return
#else
    scalar* host_array = new scalar[size];
    std::fill(host_array, host_array+size, value);

//    for(int i = 0; i < 5; i++)
//        printf("host [%d]=%f",i,host_array[i]);
//    printf("host [%d]=%f",size-1,host_array[size-1]);

    OpenCL::copy(data, host_array, size*sizeof(scalar), CL_TRUE, &fill_event);
    clWaitForEvents(1, &fill_event);

    delete [] host_array;
#endif


}

template<typename scalar>
void Vector<scalar, GPU>::print(int n)
{
    std::ostringstream info;
    info << "Vector (GPU):\n";

    scalar* host_data = NULL;
    if (size <= n)
    {
        host_data = new scalar [size];
        OpenCL::copy(host_data, data, size*sizeof(scalar));

        for (int i = 0; i < size; i++)
            info << "V[ "<< i << " ] = " << host_data[i] << "\n";

        info << "...,\nV[" << size - 1 << "] = " << host_data[size-1] << "\n";
    }
    else
    {
        host_data = new scalar [size];
        OpenCL::copy(host_data, data, size*sizeof(scalar));

        for (int i = 0; i < n-1; i++)
            info << "V[ "<< i << " ] = " << host_data[i] << "\n";

        info << "...,\nV["<< size - 2 << " ] = " << host_data[size-2] << "\n";
        info << "V["<< size - 1 << " ] = " << host_data[size-1] << "\n";
    }
    qDebug() << qPrintable(QString::fromStdString(info.str()));
    delete [] host_data;
}

template<typename scalar>
cl_mem Vector<scalar, GPU>::get_data()
{
    return data;
}

template<typename scalar>
cl_mem Vector<scalar, GPU>::get_cdata() const
{
    return data;
}

template <typename scalar>
int Vector<scalar, GPU>::get_size()
{
    return size;
}

template<typename scalar>
int Vector<scalar, GPU>::get_csize() const
{
    return size;
}

template<typename scalar>
bool Vector<scalar, GPU>::equal(const Vector& other)
{
    bool r = (this->size == other.size);
    return r;
}


template<typename scalar>
scalar Vector<scalar, GPU>::dot(const Vector& other)
{

    scalar result = 0.0;

#ifdef CL_BLAS
    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;
    int size = this->get_size();

    cl_mem g_result;
    cl_mem scratchBuffer;

    OpenCL::allocate(&g_result, sizeof(scalar));
    OpenCL::allocate(&scratchBuffer, size*sizeof(scalar));

    if(sizeof(scalar) == 4)
        err = clblasSdot(size, g_result, 0, this->get_cdata(), 0, 1, other.get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);
     else
        err = clblasDdot(size, g_result, 0, this->get_cdata(), 0, 1, other.get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);

    if (err != CL_SUCCESS)
        qCritical() << "Vector dot: clblas dot() failed with " << err;
    else
    {
         err = clWaitForEvents(1, &event);
         OpenCL::copy(&result, g_result, sizeof(scalar));
         OpenCL::free(&scratchBuffer);
         OpenCL::free(&g_result);
    }

#else
    //Verry simple version but requre to allocate additional size*sizeof(scalar) data.
    //Vector<scalar, GPU> temp = *this * other;
    //return temp.sum();

    //this version uses modified reduce algorithm
    int maxThreads = 256; //same as blockSize in kernel code ssum2
    int maxBlocks  = 64;

    if(this->get_size() < maxThreads)
    {
        qCritical() << "Vector (GPU) dot: size < 256 is not supported";
    }

    //threads = minimum between maxThreads and size/2
    int threads = (this->size < maxThreads*2) ? next_pow2((this->size + 1)/ 2) : maxThreads;
    int blocks = maxBlocks;

    //W: check this, potential error
    //result on the host side, final reduction done on cpu
    scalar* host_out_data = new scalar[blocks];

    //array of reduced input vector
    //compare with OpenCL::allocate();
    cl_mem device_out_data;
    OpenCL::allocate (&device_out_data, blocks*sizeof(scalar));

    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_dot"];
    else
        k = OpenCL::kernels["d_vector_dot"];
    k.set_buffer (0, data);
    k.set_buffer (1, other.get_cdata());
    k.set_buffer (2, device_out_data);
    k.set_int (3, size);
    k.set_local (4, threads * sizeof(scalar));

    size_t global = blocks*threads;
    size_t local = threads;

    k.execute (local, global);

    //final reduction
    OpenCL::copy (host_out_data, device_out_data, blocks*sizeof(scalar), CL_TRUE);
    for (int i = 0; i < blocks; i++ )
    {
        result += host_out_data[i];
    }

    delete [] host_out_data;
    OpenCL::free(&device_out_data);
#endif
    return result;
}

template<typename scalar>
void Vector<scalar, GPU>::dot(const Vector& other, cl_mem* result)
{

#ifdef CL_BLAS
    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;
    int size = this->get_size();

    cl_mem scratchBuffer;
    OpenCL::allocate(&scratchBuffer, size*sizeof(scalar));

    if(sizeof(scalar) == 4)
        err = clblasSdot(size, *result, 0, this->get_cdata(), 0, 1, other.get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);
     else
        err = clblasDdot(size, *result, 0, this->get_cdata(), 0, 1, other.get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);

    if (err != CL_SUCCESS)
        qCritical() << "Vector dot: clblas dot() failed with " << err;
    else
    {
        err = clWaitForEvents(1, &event);
        OpenCL::free(&scratchBuffer);
    }

#else

    int maxThreads = 256; //same as blockSize in kernel code ssum2
    int maxBlocks  = 64;
    if(this->get_size() < maxThreads)
    {
        qCritical() << "Vector (GPU) sum: size < 256 is not supported";
    }

    //threads = minimum between maxThreads and size/2
    int threads = (this->size < maxThreads*2) ? next_pow2((this->size + 1)/ 2) : maxThreads;
    int blocks = maxBlocks;

    //array of reduced input vector
    //compare with OpenCL::allocate();
    cl_mem device_out_data;
    OpenCL::allocate (&device_out_data, blocks*sizeof(scalar));

    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_dot"];
    else
        k = OpenCL::kernels["d_vector_dot"];
    k.set_buffer (0, data);
    k.set_buffer (1, other.get_cdata());
    k.set_buffer (2, device_out_data);
    k.set_int (3, size);
    k.set_local (4, threads * sizeof(scalar));

    size_t global = blocks*threads;
    size_t local = threads;

    cl_event step1;
    k.execute (local, global, &step1);

    //TODO: remember to zer the result!
    scalar zero = 0.0;
    OpenCL::copy (*result, &zero, sizeof(scalar), CL_FALSE);

    //device_out_data have 64 elements. how to reduce them ? 2 blocks x warp = 64.
    //perform final reduction on gpu and return one element cl_mem
    blocks = 1;
    threads = 64;
    global = blocks*threads;

    //final reduction on gpu
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_sum_scalar"];
    else
        k = OpenCL::kernels["d_vector_sum_scalar"];
    k.set_buffer (0, device_out_data);
    k.set_buffer (1, *result);
    k.set_int (2, threads);

    cl_event step2;
    clWaitForEvents (1, &step1);
    k.execute (threads, threads, &step2);

    OpenCL::free(&device_out_data);
    clWaitForEvents (1, &step2);
#endif
}


template<typename scalar>
inline scalar Vector<scalar, GPU>::_norm1()
{
    scalar result = 0.0;

#ifdef CL_BLAS

    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;
    int size = this->get_size();

    cl_mem g_result;
    cl_mem scratchBuffer;

    OpenCL::allocate(&g_result, sizeof(scalar));
    OpenCL::allocate(&scratchBuffer, size*sizeof(scalar));

    if(sizeof(scalar) == 4)
        err = clblasSasum(size, g_result, 0, this->get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);
     else
        err = clblasDasum(size, g_result, 0, this->get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);

    if (err != CL_SUCCESS)
        qCritical() << "Vector norm1: clblas asum() failed with " << err;
    else
    {
         err = clWaitForEvents(1, &event);
         OpenCL::copy(&result, g_result, sizeof(scalar));
         OpenCL::free(&scratchBuffer);
         OpenCL::free(&g_result);
    }


#else
    int maxThreads = 256; //same as blockSize in kernel code ssum2
    int maxBlocks  = 64;

    if(this->get_size() < maxThreads)
    {
        qCritical() << "Vector (GPU) sum: size < 256 is not supported";
    }

    //threads = minimum between maxThreads and size/2
    int threads = (this->size < maxThreads*2) ? next_pow2((this->size + 1)/ 2) : maxThreads;
    int blocks = maxBlocks;

    //W: check this, potential error
    //result on the host side, final reduction done on cpu
    scalar* host_out_data = new scalar[blocks];

    //array of reduced input vector
    //compare with OpenCL::allocate();
    cl_mem device_out_data;
    OpenCL::allocate (&device_out_data, blocks*sizeof(scalar));

    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_norm"];
    else
        k = OpenCL::kernels["d_vector_norm"];
    k.set_buffer (0, data);
    k.set_buffer (1, device_out_data);
    k.set_int (2, size);
    k.set_local (3, threads * sizeof(scalar));

    size_t global = blocks*threads;
    size_t local = threads;

    k.execute (local, global);

    //final reduction
    OpenCL::copy (host_out_data, device_out_data, blocks*sizeof(scalar), CL_TRUE);
    for (int i = 0; i < blocks; i++ )
    {
        result += host_out_data[i];
    }

    delete [] host_out_data;
    OpenCL::free(&device_out_data);

#endif
    return result;

}

template<typename scalar>
inline scalar Vector<scalar, GPU>::_norm2()
{
    scalar result = 0.0;

#ifdef CL_BLAS

    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;
    int size = this->get_size();

    cl_mem g_result;
    cl_mem scratchBuffer;

    OpenCL::allocate(&g_result, sizeof(scalar));
    //here scratchBuffer = 2*N
    OpenCL::allocate(&scratchBuffer, 2*size*sizeof(scalar));

    if(sizeof(scalar) == 4)
        err = clblasSnrm2(size, g_result, 0, this->get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);
     else
        err = clblasDnrm2(size, g_result, 0, this->get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);

    if (err != CL_SUCCESS)
        qCritical() << "Vector norm2: clblas nrm2() failed with " << err;
    else
    {
         err = clWaitForEvents(1, &event);
         OpenCL::copy(&result, g_result, sizeof(scalar));
         OpenCL::free(&scratchBuffer);
         OpenCL::free(&g_result);
    }


#else
    int maxThreads = 256; //same as blockSize in kernel code ssum2
    int maxBlocks  = 64;

    if(this->get_size() < maxThreads)
    {
        qCritical() << "Vector (GPU) sum: size < 256 is not supported";
    }

    //threads = minimum between maxThreads and size/2
    int threads = (this->size < maxThreads*2) ? next_pow2((this->size + 1)/ 2) : maxThreads;
    int blocks = maxBlocks;

    //W: check this, potential error
    //result on the host side, final reduction done on cpu
    scalar* host_out_data = new scalar[blocks];

    //array of reduced input vector
    //compare with OpenCL::allocate();
    cl_mem device_out_data;
    OpenCL::allocate (&device_out_data, blocks*sizeof(scalar));

    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_norm2"];
    else
        k = OpenCL::kernels["d_vector_norm2"];
    k.set_buffer (0, data);
    k.set_buffer (1, device_out_data);
    k.set_int (2, size);
    k.set_local (3, threads * sizeof(scalar));

    size_t global = blocks*threads;
    size_t local = threads;

    k.execute (local, global);

    //final reduction
    OpenCL::copy (host_out_data, device_out_data, blocks*sizeof(scalar), CL_TRUE);
    for (int i = 0; i < blocks; i++ )
    {
        result += host_out_data[i];
    }

    delete [] host_out_data;
    OpenCL::free(&device_out_data);
    result = sqrt(result);

#endif
    return result;
}

template<typename scalar>
inline void Vector<scalar, GPU>::_norm1(cl_mem* result)
{

#ifdef CL_BLAS

    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;
    int size = this->get_size();

    cl_mem scratchBuffer;
    OpenCL::allocate(&scratchBuffer, size*sizeof(scalar));

    if(sizeof(scalar) == 4)
        err = clblasSasum(size, *result, 0, this->get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);
     else
        err = clblasDasum(size, *result, 0, this->get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);

    if (err != CL_SUCCESS)
        qCritical() << "Vector norm1: clblas norm1() failed with " << err;
    else
    {
        err = clWaitForEvents(1, &event);
        OpenCL::free(&scratchBuffer);
    }

#else
    int maxThreads = 256; //same as blockSize in kernel code ssum2
    int maxBlocks  = 64;
    if(this->get_size() < maxThreads)
    {
        qCritical() << "Vector (GPU) sum: size < 256 is not supported";
    }

    //threads = minimum between maxThreads and size/2
    int threads = (this->size < maxThreads*2) ? next_pow2((this->size + 1)/ 2) : maxThreads;
    int blocks = maxBlocks;

    //array of reduced input vector
    //compare with OpenCL::allocate();
    cl_mem device_out_data;
    OpenCL::allocate (&device_out_data, blocks*sizeof(scalar));

    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_norm"];
    else
        k = OpenCL::kernels["d_vector_norm"];

    k.set_buffer (0, data);
    k.set_buffer (1, device_out_data);
    k.set_int (2, size);
    k.set_local (3, threads * sizeof(scalar));

    size_t global = blocks*threads;
    size_t local = threads;

    cl_event step1;
    k.execute (local, global, &step1);

    //TODO: remember to zer the result!
    scalar zero = 0.0;
    OpenCL::copy (*result, &zero, sizeof(scalar), CL_FALSE);

    //device_out_data have 64 elements. how to reduce them ? 2 blocks x warp = 64.
    //perform final reduction on gpu and return one element cl_mem
    blocks = 1;
    threads = 64;
    global = blocks*threads;

    //final reduction on gpu
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_sum_scalar"];
    else
        k = OpenCL::kernels["d_vector_sum_scalar"];
    k.set_buffer (0, device_out_data);
    k.set_buffer (1, *result);
    k.set_int (2, threads);

    cl_event step2;
    clWaitForEvents (1, &step1);
    k.execute (threads, threads, &step2);

    OpenCL::free(&device_out_data);
    clWaitForEvents (1, &step2);
#endif
}

template<typename scalar>
inline void Vector<scalar, GPU>::_norm2(cl_mem* result)
{
#ifdef CL_BLAS

    cl_event event;
    cl_command_queue q = OpenCL::get_queue();
    cl_int err;
    int size = this->get_size();

    cl_mem scratchBuffer;
    //here scratchBuffer = 2*N
    OpenCL::allocate(&scratchBuffer, 2*size*sizeof(scalar));

    if(sizeof(scalar) == 4)
        err = clblasSnrm2(size, *result, 0, this->get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);
     else
        err = clblasDnrm2(size, *result, 0, this->get_cdata(), 0, 1, scratchBuffer, 1, &q, 0, NULL, &event);

    if (err != CL_SUCCESS)
        qCritical() << "Vector norm1: clblas norm1() failed with " << err;
    else
    {
        err = clWaitForEvents(1, &event);
        OpenCL::free(&scratchBuffer);
    }

#else
    int maxThreads = 256; //same as blockSize in kernel code ssum2
    int maxBlocks  = 64;
    if(this->get_size() < maxThreads)
    {
        qCritical() << "Vector (GPU) sum: size < 256 is not supported";
    }

    //threads = minimum between maxThreads and size/2
    int threads = (this->size < maxThreads*2) ? next_pow2((this->size + 1)/ 2) : maxThreads;
    int blocks = maxBlocks;

    //array of reduced input vector
    //compare with OpenCL::allocate();
    cl_mem device_out_data;
    OpenCL::allocate (&device_out_data, blocks*sizeof(scalar));

    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_norm2"];
    else
        k = OpenCL::kernels["d_vector_norm2"];

    k.set_buffer (0, data);
    k.set_buffer (1, device_out_data);
    k.set_int (2, size);
    k.set_local (3, threads * sizeof(scalar));

    size_t global = blocks*threads;
    size_t local = threads;

    cl_event step1;
    k.execute (local, global, &step1);

    //TODO: remember to zer the result!
    scalar zero = 0.0;
    OpenCL::copy (*result, &zero, sizeof(scalar), CL_FALSE);

    //device_out_data have 64 elements. how to reduce them ? 2 blocks x warp = 64.
    //perform final reduction on gpu and return one element cl_mem
    blocks = 1;
    threads = 64;
    global = blocks*threads;

    //final reduction on gpu
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_sum_sqrt"];
    else
        k = OpenCL::kernels["d_vector_sum_sqrt"];
    k.set_buffer (0, device_out_data);
    k.set_buffer (1, *result);
    k.set_int (2, threads);

    cl_event step2;
    clWaitForEvents (1, &step1);
    k.execute (threads, threads, &step2);

    OpenCL::free(&device_out_data);
    clWaitForEvents (1, &step2);
#endif
}

template<typename scalar>
scalar Vector<scalar, GPU>::norm(const int l)
{
    if (l == 1)
        return this->_norm1();
    else if (l == 2)
        return this->_norm2();
    else
    {
        qCritical() << "Error Vector(GPU) norm: Wrong norm given " << l;
    }
}

template<typename scalar>
void Vector<scalar, GPU>::norm(cl_mem* result, const int l)
{
    if (l == 1)
        this->_norm1(result);
    else if (l == 2)
        this->_norm2(result);
    else
    {
        qCritical() << "Error Vector(GPU) norm: Wrong norm given " << l;
    }

}

//Reduce vector (sum) with final CPU reduction
template<typename scalar>
scalar Vector<scalar, GPU>::sum()
{
    int maxThreads = 256; //same as blockSize in kernel code ssum2
    int maxBlocks  = 64;

    if(this->get_size() < maxThreads)
    {
        qCritical() << "Vector (GPU) sum: size < 256 is not supported";
    }
    scalar result = 0.0;
    //threads = minimum between maxThreads and size/2
    int threads = (this->size < maxThreads*2) ? next_pow2((this->size + 1)/ 2) : maxThreads;
    int blocks = maxBlocks;

    //W: check this, potential error
    //result on the host side, final reduction done on cpu
    scalar* host_out_data = new scalar[blocks];

    //array of reduced input vector
    //compare with OpenCL::allocate();
    cl_mem device_out_data;
    OpenCL::allocate (&device_out_data, blocks*sizeof(scalar));

    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_sum"];
    else
        k = OpenCL::kernels["d_vector_sum"];
    k.set_buffer (0, data);
    k.set_buffer (1, device_out_data);
    k.set_int (2, size);
    k.set_local (3, threads * sizeof(scalar));

    size_t global = blocks*threads;
    size_t local = threads;

    k.execute (local, global);

    //final reduction
    OpenCL::copy (host_out_data, device_out_data, blocks*sizeof(scalar), CL_TRUE);
    for (int i = 0; i < blocks; i++ )
    {
        result += host_out_data[i];
    }

    delete [] host_out_data;
    OpenCL::free(&device_out_data);

    return result;

}

//Reduce vector (sum) with final GPU reduction,
template<typename scalar>
void Vector<scalar, GPU>::sum(cl_mem* result)
{

    int maxThreads = 256; //same as blockSize in kernel code ssum2
    int maxBlocks  = 64;
    if(this->get_size() < maxThreads)
    {
        qCritical() << "Vector (GPU) sum: size < 256 is not supported";
    }

    //threads = minimum between maxThreads and size/2
    int threads = (this->size < maxThreads*2) ? next_pow2((this->size + 1)/ 2) : maxThreads;
    int blocks = maxBlocks;

    //array of reduced input vector
    //compare with OpenCL::allocate();
    cl_mem device_out_data;
    OpenCL::allocate (&device_out_data, blocks*sizeof(scalar));

    Kernel k;
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_sum"];
    else
        k = OpenCL::kernels["d_vector_sum"];

    k.set_buffer (0, data);
    k.set_buffer (1, device_out_data);
    k.set_int (2, size);
    k.set_local (3, threads * sizeof(scalar));

    size_t global = blocks*threads;
    size_t local = threads;

    cl_event step1;
    k.execute (local, global, &step1);

    //TODO: remember to zer the result!
    scalar zero = 0.0;
    OpenCL::copy (*result, &zero, sizeof(scalar), CL_FALSE);

    //device_out_data have 64 elements. how to reduce them ? 2 blocks x warp = 64.
    //perform final reduction on gpu and return one element cl_mem
    blocks = 1;
    threads = 64;
    global = blocks*threads;

    //final reduction on gpu
    if(sizeof(scalar) == 4)
        k = OpenCL::kernels["s_vector_sum_scalar"];
    else
        k = OpenCL::kernels["d_vector_sum_scalar"];
    k.set_buffer (0, device_out_data);
    k.set_buffer (1, *result);
    k.set_int (2, threads);

    cl_event step2;
    clWaitForEvents (1, &step1);
    k.execute (threads, threads, &step2);

    OpenCL::free(&device_out_data);
    clWaitForEvents (1, &step2);
}


template<typename scalar>
void Vector<scalar, GPU>::load(std::string fname)
{
    QFile file(fname.c_str());

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Vector (GPU) load: Problem with opening [" << fname.c_str() <<"] to load vector data";
        return;
    }
    QTextStream in(&file);

    QVector<scalar> vector;

    while(!in.atEnd())
    {
        QString line = in.readLine();
        vector.append(scalar(line.toDouble()));
    }

    this->size = vector.size();

    OpenCL::copy(this->get_data(), vector.data(), this->size*sizeof(scalar));
    file.close();
}

template<typename scalar>
void Vector<scalar, GPU>::save(std::string fname)
{
    QFile file(fname.c_str());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
       qCritical() << "Vector (GPU) save: Problem with opening [" << fname.c_str() <<"] to save vector data";
       return;
    }

    scalar* host_data = new scalar [size];
    OpenCL::copy(host_data, data, size * sizeof(scalar));

    QTextStream out (&file);
    out.setRealNumberNotation(QTextStream::ScientificNotation);
    for (int i = 0; i < size; i++)
        out << host_data[i] << "\n" ;

    file.close();
    delete [] host_data;

}

template<typename scalar>
Vector<scalar, GPU>::~Vector()
{
    OpenCL::free (&data);
    this->size = 0;
}


//initialization of templates
template class Vector<int, GPU>;
template class Vector<float, GPU>;
template class Vector<double, GPU>;


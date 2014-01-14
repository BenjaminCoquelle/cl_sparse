#define T float
#define blockSize 256
#define nIsPow2 0
//SIMPLE SUM
__kernel void s_vector_sum_scalar(__global float *g_idata, __global float *g_odata, unsigned int n)
{
    // get index into global data array
    int i = get_local_id(0);
    if (i == 0)
    {
        for( int j = 0; j < n; j++)
            g_odata[0] += g_idata[j];
    }

}

//simple sum sqrt
__kernel void s_vector_sum_sqrt(__global float *g_idata, __global float *g_odata, unsigned int n)
{
    // get index into global data array
    int i = get_local_id(0);
    if (i == 0)
    {
        for( int j = 0; j < n; j++)
            g_odata[0] += g_idata[j];

    }
    if(i == 0)  g_odata[0] = sqrt(g_odata[0]);
}

//SUM vector: optimized version from CUDA openCL
__kernel void s_vector_sum(__global float *g_idata, __global float *g_odata, unsigned int n, __local volatile float* sdata)
{
    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0)*(get_local_size(0)*2) + get_local_id(0);
    unsigned int gridSize = blockSize*2*get_num_groups(0);
    sdata[tid] = 0;

    // we reduce multiple elements per thread.  The number is determined by the
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {
        sdata[tid] += g_idata[i];
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n)
            sdata[tid] += g_idata[i+blockSize];
        i += gridSize;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] += sdata[tid +  64]; } barrier(CLK_LOCAL_MEM_FENCE); }

    if (tid < 32)
    {
        if (blockSize >=  64) { sdata[tid] += sdata[tid + 32]; }
        if (blockSize >=  32) { sdata[tid] += sdata[tid + 16]; }
        if (blockSize >=  16) { sdata[tid] += sdata[tid +  8]; }
        if (blockSize >=   8) { sdata[tid] += sdata[tid +  4]; }
        if (blockSize >=   4) { sdata[tid] += sdata[tid +  2]; }
        if (blockSize >=   2) { sdata[tid] += sdata[tid +  1]; }
    }

    // write result for this block to global mem
    if (tid == 0) g_odata[get_group_id(0)] = sdata[0];
}


//dot product based on reduction algorithm
__kernel void s_vector_dot(__global float *g_idata1, __global float *g_idata2, __global float *g_odata, unsigned int n, __local volatile float* sdata)
{
    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0)*(get_local_size(0)*2) + get_local_id(0);
    unsigned int gridSize = blockSize*2*get_num_groups(0);
    sdata[tid] = 0;

    // we reduce multiple elements per thread.  The number is determined by the
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {
        sdata[tid] += (g_idata1[i] * g_idata2[i]);
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n)
            sdata[tid] += (g_idata1[i+blockSize] * g_idata2[i+blockSize]);
        i += gridSize;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] += sdata[tid +  64]; } barrier(CLK_LOCAL_MEM_FENCE); }

    if (tid < 32)
    {
        if (blockSize >=  64) { sdata[tid] += sdata[tid + 32]; }
        if (blockSize >=  32) { sdata[tid] += sdata[tid + 16]; }
        if (blockSize >=  16) { sdata[tid] += sdata[tid +  8]; }
        if (blockSize >=   8) { sdata[tid] += sdata[tid +  4]; }
        if (blockSize >=   4) { sdata[tid] += sdata[tid +  2]; }
        if (blockSize >=   2) { sdata[tid] += sdata[tid +  1]; }
    }

    // write result for this block to global mem
    if (tid == 0) g_odata[get_group_id(0)] = sdata[0];
}

//norm based on reduction algorithm
__kernel void s_vector_norm(__global float *g_idata, __global float *g_odata, unsigned int n, __local volatile float* sdata)
{
    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0)*(get_local_size(0)*2) + get_local_id(0);
    unsigned int gridSize = blockSize*2*get_num_groups(0);
    sdata[tid] = 0;

    // we reduce multiple elements per thread.  The number is determined by the
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {
        sdata[tid] += fabs(g_idata[i]);
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n)
            sdata[tid] += fabs(g_idata[i+blockSize]);
        i += gridSize;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] += sdata[tid +  64]; } barrier(CLK_LOCAL_MEM_FENCE); }

    if (tid < 32)
    {
        if (blockSize >=  64) { sdata[tid] += sdata[tid + 32]; }
        if (blockSize >=  32) { sdata[tid] += sdata[tid + 16]; }
        if (blockSize >=  16) { sdata[tid] += sdata[tid +  8]; }
        if (blockSize >=   8) { sdata[tid] += sdata[tid +  4]; }
        if (blockSize >=   4) { sdata[tid] += sdata[tid +  2]; }
        if (blockSize >=   2) { sdata[tid] += sdata[tid +  1]; }
    }

    // write result for this block to global mem
    if (tid == 0) g_odata[get_group_id(0)] = sdata[0];
}

//norm 2
__kernel void s_vector_norm2(__global float *g_idata, __global float *g_odata, unsigned int n, __local volatile float* sdata)
{
    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0)*(get_local_size(0)*2) + get_local_id(0);
    unsigned int gridSize = blockSize*2*get_num_groups(0);
    sdata[tid] = 0;

    // we reduce multiple elements per thread.  The number is determined by the
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {
        sdata[tid] += g_idata[i]*g_idata[i];
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n)
            sdata[tid] += g_idata[i+blockSize]*g_idata[i+blockSize];
        i += gridSize;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] += sdata[tid +  64]; } barrier(CLK_LOCAL_MEM_FENCE); }

    if (tid < 32)
    {
        if (blockSize >=  64) { sdata[tid] += sdata[tid + 32]; }
        if (blockSize >=  32) { sdata[tid] += sdata[tid + 16]; }
        if (blockSize >=  16) { sdata[tid] += sdata[tid +  8]; }
        if (blockSize >=   8) { sdata[tid] += sdata[tid +  4]; }
        if (blockSize >=   4) { sdata[tid] += sdata[tid +  2]; }
        if (blockSize >=   2) { sdata[tid] += sdata[tid +  1]; }
    }

    // write result for this block to global mem
    if (tid == 0) g_odata[get_group_id(0)] = sdata[0];
}


// OpenCL Kernel Function for element by element vector multip single precision
__kernel void s_vector_mul(__global const float* a, __global const float* b, __global float* c, int iNumElements)
{
    // get index into global data array
    int iGID = get_global_id(0);

    // bound check (equivalent to the limit on a 'for' loop for standard/serial C code
    if (iGID >= iNumElements)
    {
        return;
    }
    c[iGID] = a[iGID] * b[iGID];
}

// OpenCL Kernel Function for element by element vector subtraction single precision
__kernel void s_vector_sub(__global const float* a, __global const float* b, __global float* c, int iNumElements)
{
    // get index into global data array
    int iGID = get_global_id(0);

    // bound check (equivalent to the limit on a 'for' loop for standard/serial C code
    if (iGID >= iNumElements)
    {
        return;
    }
    c[iGID] = a[iGID] - b[iGID];
}


// OpenCL Kernel Function for element by element vector addition single precision
__kernel void s_vector_add(__global const float* a, __global const float* b, __global float* c, int iNumElements)
{
    int iGID = get_group_id(0)*(get_local_size(0)) + get_local_id(0);//get_global_id(0);

    // bound check (equivalent to the limit on a 'for' loop for standard/serial C code
    if (iGID < 2*iNumElements)
    {
         // add the vector elements
         c[iGID] = a[iGID] + b[iGID];
     }
}


/**
double precision kernels;
*/
// OpenCL Kernel Function for element by element vector addition double precision
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
//SIMPLE SUM
__kernel void d_vector_sum_scalar(__global double *g_idata, __global double *g_odata, unsigned int n)
{
    // get index into global data array
    int i = get_local_id(0);
    if (i == 0)
    {
        for( int j = 0; j < n; j++)
            g_odata[0] += g_idata[j];
    }

}

__kernel void d_vector_sum_sqrt(__global double *g_idata, __global double *g_odata, unsigned int n)
{
    // get index into global data array
    int i = get_local_id(0);
    if (i == 0)
    {
        for( int j = 0; j < n; j++)
            g_odata[0] += g_idata[j];

        g_odata[0] = sqrt(g_odata[0]);
    }
}

//SUM vector: optimized version from CUDA openCL
__kernel void d_vector_sum(__global double *g_idata, __global double *g_odata, unsigned int n, __local volatile double* sdata)
{
    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0)*(get_local_size(0)*2) + get_local_id(0);
    unsigned int gridSize = blockSize*2*get_num_groups(0);
    sdata[tid] = 0;

    // we reduce multiple elements per thread.  The number is determined by the
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {
        sdata[tid] += g_idata[i];
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n)
            sdata[tid] += g_idata[i+blockSize];
        i += gridSize;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] += sdata[tid +  64]; } barrier(CLK_LOCAL_MEM_FENCE); }

    if (tid < 32)
    {
        if (blockSize >=  64) { sdata[tid] += sdata[tid + 32]; }
        if (blockSize >=  32) { sdata[tid] += sdata[tid + 16]; }
        if (blockSize >=  16) { sdata[tid] += sdata[tid +  8]; }
        if (blockSize >=   8) { sdata[tid] += sdata[tid +  4]; }
        if (blockSize >=   4) { sdata[tid] += sdata[tid +  2]; }
        if (blockSize >=   2) { sdata[tid] += sdata[tid +  1]; }
    }

    // write result for this block to global mem
    if (tid == 0) g_odata[get_group_id(0)] = sdata[0];
}


//SUM vector: optimized version from CUDA openCL
__kernel void d_vector_dot(__global double *g_idata1, __global double *g_idata2, __global double *g_odata, unsigned int n, __local volatile double* sdata)
{
    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0)*(get_local_size(0)*2) + get_local_id(0);
    unsigned int gridSize = blockSize*2*get_num_groups(0);
    sdata[tid] = 0;

    // we reduce multiple elements per thread.  The number is determined by the
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {
        sdata[tid] += (g_idata1[i] * g_idata2[i]);
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n)
            sdata[tid] += (g_idata1[i+blockSize] * g_idata2[i+blockSize]);
        i += gridSize;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] += sdata[tid +  64]; } barrier(CLK_LOCAL_MEM_FENCE); }

    if (tid < 32)
    {
        if (blockSize >=  64) { sdata[tid] += sdata[tid + 32]; }
        if (blockSize >=  32) { sdata[tid] += sdata[tid + 16]; }
        if (blockSize >=  16) { sdata[tid] += sdata[tid +  8]; }
        if (blockSize >=   8) { sdata[tid] += sdata[tid +  4]; }
        if (blockSize >=   4) { sdata[tid] += sdata[tid +  2]; }
        if (blockSize >=   2) { sdata[tid] += sdata[tid +  1]; }
    }

    // write result for this block to global mem
    if (tid == 0) g_odata[get_group_id(0)] = sdata[0];
}


//norm product based on reduction algorithm
__kernel void d_vector_norm(__global double *g_idata, __global double *g_odata, unsigned int n, __local volatile double* sdata)
{
    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0)*(get_local_size(0)*2) + get_local_id(0);
    unsigned int gridSize = blockSize*2*get_num_groups(0);
    sdata[tid] = 0;

    // we reduce multiple elements per thread.  The number is determined by the
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {
        sdata[tid] += fabs(g_idata[i]);
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n)
            sdata[tid] += fabs(g_idata[i+blockSize]);
        i += gridSize;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] += sdata[tid +  64]; } barrier(CLK_LOCAL_MEM_FENCE); }

    if (tid < 32)
    {
        if (blockSize >=  64) { sdata[tid] += sdata[tid + 32]; }
        if (blockSize >=  32) { sdata[tid] += sdata[tid + 16]; }
        if (blockSize >=  16) { sdata[tid] += sdata[tid +  8]; }
        if (blockSize >=   8) { sdata[tid] += sdata[tid +  4]; }
        if (blockSize >=   4) { sdata[tid] += sdata[tid +  2]; }
        if (blockSize >=   2) { sdata[tid] += sdata[tid +  1]; }
    }

    // write result for this block to global mem
    if (tid == 0) g_odata[get_group_id(0)] = sdata[0];
}

//norm 2
__kernel void d_vector_norm2(__global double *g_idata, __global double *g_odata, unsigned int n, __local volatile double* sdata)
{
    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = get_local_id(0);
    unsigned int i = get_group_id(0)*(get_local_size(0)*2) + get_local_id(0);
    unsigned int gridSize = blockSize*2*get_num_groups(0);
    sdata[tid] = 0;

    // we reduce multiple elements per thread.  The number is determined by the
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {
        sdata[tid] += g_idata[i]*g_idata[i];
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n)
            sdata[tid] += g_idata[i+blockSize]*g_idata[i+blockSize];
        i += gridSize;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // do reduction in shared mem
    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } barrier(CLK_LOCAL_MEM_FENCE); }
    if (blockSize >= 128) { if (tid <  64) { sdata[tid] += sdata[tid +  64]; } barrier(CLK_LOCAL_MEM_FENCE); }

    if (tid < 32)
    {
        if (blockSize >=  64) { sdata[tid] += sdata[tid + 32]; }
        if (blockSize >=  32) { sdata[tid] += sdata[tid + 16]; }
        if (blockSize >=  16) { sdata[tid] += sdata[tid +  8]; }
        if (blockSize >=   8) { sdata[tid] += sdata[tid +  4]; }
        if (blockSize >=   4) { sdata[tid] += sdata[tid +  2]; }
        if (blockSize >=   2) { sdata[tid] += sdata[tid +  1]; }
    }

    // write result for this block to global mem
    if (tid == 0) g_odata[get_group_id(0)] = sdata[0];
}

// OpenCL Kernel Function for element by element vector multip double precision
__kernel void d_vector_mul(__global const double* a, __global const double* b, __global double* c, int iNumElements)
{
    // get index into global data array
    int iGID = get_global_id(0);

    // bound check (equivalent to the limit on a 'for' loop for standard/serial C code
    if (iGID >= iNumElements)
    {
        return;
    }
    c[iGID] = a[iGID] * b[iGID];
}

// OpenCL Kernel Function for element by element vector subtraction double precision
__kernel void d_vector_sub(__global const double* a, __global const double* b, __global double* c, int iNumElements)
{
    // get index into global data array
    int iGID = get_global_id(0);

    // bound check (equivalent to the limit on a 'for' loop for standard/serial C code
    if (iGID >= iNumElements)
    {
        return;
    }
    c[iGID] = a[iGID] - b[iGID];
}

#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel void d_vector_add(__global const double* a, __global const double* b, __global double* c, int iNumElements)
{
   // get index into global data array
   int iGID = get_global_id(0);

   // bound check (equivalent to the limit on a 'for' loop for standard/serial C code
   if (iGID < iNumElements)
   {
        // add the vector elements
        c[iGID] = a[iGID] + b[iGID];
    }


}

#pragma OPENCL EXTENSION cl_khr_fp64 : disable

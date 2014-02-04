#define WARP_SIZE 32
#define ValueType float


float ReadVecFromImg(int ind, __read_only image2d_t img, const sampler_t smp)
{
  if (ind < 0) return 0;

  int imgPos = ind;// >> 2;
  int2 coords;
  coords.x = imgPos & 0x1fff;
  coords.y = imgPos >> 13;

  float4 temp = read_imagef(img, smp, coords);
  return temp.x;
//  imgPos = ind & 0x0003;
//  if (imgPos < 2)
//  {
//    if (imgPos == 0) return temp.x;
//    else             return temp.y;
//  }
//  else
//  {
//    if (imgPos == 2) return temp.z;
//    else             return temp.w;
//  }
}

__kernel void s_kernel_test(            const int    size,
                                         __global const int* row_offset,
                                         __global const int* col,
                                         __global const ValueType* val,
                                        __read_only image2d_t    x,
                                        __global   ValueType*    y)
{

  const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
  CLK_ADDRESS_CLAMP |
  CLK_FILTER_NEAREST;

    int idx = get_global_id(0);

    if (idx >= size)
        return;


    ValueType suma = 0.0;

    for (int k = row_offset[ idx ]; k < row_offset[idx + 1]; k++)
    {

        //translate from 1D to 2D
        //y = i /WIDTH; ---> y = i >> n if WIDTH = 2^n
        //x = i % (WIDTH -1);---> x = i & 0x1fff  0x1fff =  8191;
//        int2 coords;
//        coords.x = col[k] & 0x1fff;
//        coords.y = col[k] >> 13;  // 8192 = 2^13
//        float4 temp = read_imagef(x, smp, coords);

         suma += val[ k ] * ReadVecFromImg(col[k], x, smp);//temp.x
    }

    y[ idx ] = suma;

}


__kernel void s_kernel_test2(         const int  num_rows,
                             __global const int* row_offset,
                             __global const int* col,
                             __global const ValueType* val,
                             __read_only image2d_t    x,
                             __global   ValueType*    y,
                             __local volatile ValueType* sdata)
{
        const int idx           = get_global_id(0);
        const int warp_id       = idx / WARP_SIZE;
        const int thread_lane   = idx & (WARP_SIZE-1);
        const int num_warps     = get_global_size(0) / WARP_SIZE;

        const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
            CLK_ADDRESS_CLAMP_TO_EDGE | //Clamp to zeros
            CLK_FILTER_NEAREST; //Don't interpolate

        for(int row = warp_id; row < num_rows; row += num_warps)
        {
            const int row_start = row_offset[row];
            const int row_end   = row_offset[row+1];

            ValueType sum = 0;

            for(int j = row_start + thread_lane; j < row_end; j += WARP_SIZE)
                    sum += val[j] * ReadVecFromImg(col[j], x, smp);

            const int i = get_local_id(0);

            sdata[i] = sum;
            sdata[i] = sum += sdata[i + 16];
            sdata[i] = sum += sdata[i + 8];
            sdata[i] = sum += sdata[i + 4];
            sdata[i] = sum += sdata[i + 2];
            sdata[i] = sum += sdata[i + 1];

            // pierwszy watek zapisuje wynik
            if (thread_lane == 0)
             y[row] = sum;
       }

}

__kernel void s_kernel_csr_spmv_vector(const int num_rows,
                                     __global const int * row_offset,
                                     __global const int * col,
                                     __global const ValueType * val,
                                     __global const ValueType * x,
                                     __global ValueType * y,
                                     __local volatile ValueType* sdata
                                     )
{
//    __local volatile ValueType sdata[BLOCK_SIZE+16];

    const int idx           = get_global_id(0);
    const int warp_id       = idx / WARP_SIZE;
    const int thread_lane   = idx & (WARP_SIZE-1);
    const int num_warps     = get_global_size(0) / WARP_SIZE;

    for(int row = warp_id; row < num_rows; row += num_warps)
    {
        const int row_start = row_offset[row];
        const int row_end   = row_offset[row+1];

        ValueType sum = 0;

        for(int j = row_start + thread_lane; j < row_end; j += WARP_SIZE)
            sum += val[j] * x[col[j]];

        const int i = get_local_id(0);

        sdata[i] = sum;
        sdata[i] = sum += sdata[i + 16];
        sdata[i] = sum += sdata[i + 8];
        sdata[i] = sum += sdata[i + 4];
        sdata[i] = sum += sdata[i + 2];
        sdata[i] = sum += sdata[i + 1];

        // pierwszy watek zapisuje wynik
        if (thread_lane == 0)
            y[row] = sum;

    }
}



__kernel void s_kernel_csr_spmv_scalar ( const int num_rows,
                                         __global const int* row_offset,
                                         __global const int* col,
                                         __global const ValueType* val,
                                         __global const ValueType* x,
                                         __global ValueType* y
                                         )
{

    int idx = get_global_id(0); //get thread idx

    if (idx >= num_rows)
        return;

    ValueType suma = 0.0;

    for (int k = row_offset[ idx ]; k < row_offset[idx + 1]; k++)
        suma += val[ k ] * x [ col[k] ];

    y[ idx ] = suma;
}



/**
    Double precision kernels
*/
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#define ValueType double
__kernel void d_kernel_csr_spmv_vector(const int num_rows,
                                     __global const int * row_offset,
                                     __global const int * col,
                                     __global const ValueType * val,
                                     __global const ValueType * x,
                                     __global ValueType * y,
                                     __local volatile ValueType* sdata
                                     )
{
//    __local volatile ValueType sdata[BLOCK_SIZE+16];

    const int idx           = get_global_id(0);
    const int warp_id       = idx / WARP_SIZE;
    const int thread_lane   = idx & (WARP_SIZE-1);
    const int num_warps     = get_global_size(0) / WARP_SIZE;

    for(int row = warp_id; row < num_rows; row += num_warps)
    {
        const int row_start = row_offset[row];
        const int row_end   = row_offset[row+1];

        ValueType sum = 0;

        for(int j = row_start + thread_lane; j < row_end; j += WARP_SIZE)
            sum += val[j] * x[col[j]];

        const int i = get_local_id(0);

        sdata[i] = sum;
        sdata[i] = sum += sdata[i + 16];
        sdata[i] = sum += sdata[i + 8];
        sdata[i] = sum += sdata[i + 4];
        sdata[i] = sum += sdata[i + 2];
        sdata[i] = sum += sdata[i + 1];

        // pierwszy watek zapisuje wynik
        if (thread_lane == 0)
            y[row] = sum;

    }
}

__kernel void d_kernel_csr_spmv_scalar ( const int num_rows,
                                         __global const int* row_offset,
                                         __global const int* col,
                                         __global const ValueType* val,
                                         __global const ValueType* x,
                                         __global ValueType* y
                                         )
{

    int idx = get_global_id(0); //get thread idx
    //int2 i2 = (1,2);
    //double d2 = __hiloint2double(i2.y, i2.x);
    if (idx >= num_rows)
        return;

    ValueType suma = 0.0;

    for (int k = row_offset[ idx ]; k < row_offset[idx + 1]; k++)
        suma += val[ k ] * x [ col[k] ];

    y[ idx ] = suma;
}
#pragma OPENCL EXTENSION cl_khr_fp64 : disable

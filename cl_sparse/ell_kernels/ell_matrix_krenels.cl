#define ValueType float
__kernel void s_kernel_ell_spmv( const int num_rows,
                                 const int num_cols,
                                 const int max_row,
                                __global const int *col,
                                __global const ValueType *val,
                                __global const ValueType *x,
                                __global       ValueType *y)
{

    int row = get_global_id(0);

    if (row >= num_rows)
        return;

    ValueType sum = (ValueType)0;
    for (int i = 0; i < max_row; i++)
    {
        const int ind = i * num_rows + row;
        const int column = col[ind];

        if (column < 0)
            break;
        sum += val[ind] * x[column];
    }
    y[row] = sum;

}


/**
    Double precision kernels
*/
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#define ValueType double
__kernel void d_kernel_ell_spmv( const int num_rows,
                                 const int num_cols,
                                 const int max_row,
                                __global const int *col,
                                __global const ValueType *val,
                                __global const ValueType *x,
                                __global       ValueType *y)
{

    int row = get_global_id(0);

    if (row >= num_rows)
        return;

    ValueType sum = (ValueType)0;
    for (int i = 0; i < max_row; i++)
    {
        const int ind = i * num_rows + row;
        const int column = col[ind];

        if (column < 0)
            break;

        sum += val[ind] * x[column];
    }
    y[row] = sum;

}

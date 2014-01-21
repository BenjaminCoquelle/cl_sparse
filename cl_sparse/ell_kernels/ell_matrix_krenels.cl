#define ValueType float
#define IndexType int
__kernel void s_kernel_ell_spmv( const int num_rows,
                                 const int num_cols,
                                 const int num_cols_per_row,
                                __global const int *Acol,
                                __global const ValueType *Aval,
                                __global const ValueType *x,
                                __global       ValueType *y)
{

    int row = get_global_id(0);

    if (row < num_rows)
    {
        ValueType sum = (ValueType)0;
        for (int n=0; n<num_cols_per_row; ++n)
        {
            const int ind = n * num_rows + row;
            const int col = Acol[ind];

            if ((col >= 0) && (col < num_cols))
                sum += Aval[ind] * x[col];
        }

        y[row] = sum;

    }
}


/**
    Double precision kernels
*/
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#define ValueType double
#define IndexType int
__kernel void d_kernel_ell_spmv( const int num_rows,
                                 const int num_cols,
                                 const int num_cols_per_row,
                                __global const int *Acol,
                                __global const ValueType *Aval,
                                __global const ValueType *x,
                                __global       ValueType *y)
{

    int row = get_global_id(0);

    if (row < num_rows)
    {
        ValueType sum = (ValueType)0;
        for (int n=0; n<num_cols_per_row; ++n)
        {
            const int ind = n * num_rows + row;
            const int col = Acol[ind];

            if ((col >= 0) && (col < num_cols))
                sum += Aval[ind] * x[col];
        }

        y[row] = sum;

    }
}

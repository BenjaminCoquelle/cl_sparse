#define ValueType float
#define IndexType int

#define BLOCK_SIZE 192
#define WARP_SIZE 32
__kernel void kernel_csr_spmv_vector(const IndexType num_rows,
                                     __global const IndexType * row_offset,
                                     __global const IndexType * col,
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

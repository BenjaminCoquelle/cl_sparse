#include "gather.h"

template<typename scalar>
void gather(const int map_first, const int map_last, const Vector<scalar, CPU>& in, const Vector<int, CPU>& map, Vector<scalar, CPU>& out)
{
    int num_elements = map_last - map_first;

    for (int i = 0; i < num_elements; i++)
        out[i+map_first] = in[map[i]]; //or input[i]
}

template
void gather(const int map_first, const int map_last, const Vector<float, CPU>& in, const Vector<int, CPU>& map, Vector<float, CPU>& out);

template
void gather(const int map_first, const int map_last, const Vector<double, CPU>& in, const Vector<int, CPU>& map, Vector<double, CPU>& out);

template
void gather(const int map_first, const int map_last, const Vector<int, CPU>& in, const Vector<int, CPU>& map, Vector<int, CPU>& out);


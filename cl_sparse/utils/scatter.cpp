#include "scatter.h"

template<typename scalar>
void scatter (int first, int last, const Vector<scalar, CPU> &in, const Vector<int, CPU>& map, Vector<scalar, CPU> &result)
{

    int num_elements = last - first;

    scalar* r = result.get_data();

    for (int i = 0; i < num_elements; i++)
        r [ map[i] ] = in[first+i];
}


template<typename scalar>
void scatter_permuted(int first, int last, const Vector<scalar, CPU> &in, const Vector<int, CPU>& p, const Vector<int, CPU>& map, Vector<scalar, CPU> &result)
{
    int num_elements = last - first;

    scalar* r = result.get_data();

    for (int i = 0; i < num_elements; i++)
        r [ map[i]] = in [ p [first + i] ];
}

template
void scatter (int first, int last, const Vector<int, CPU> &in, const Vector<int, CPU>& map, Vector<int, CPU> &result);
template
void scatter (int first, int last, const Vector<float, CPU> &in, const Vector<int, CPU>& map, Vector<float, CPU> &result);
template
void scatter (int first, int last, const Vector<double, CPU> &in, const Vector<int, CPU>& map, Vector<double, CPU> &result);


template
void scatter_permuted(int first, int last, const Vector<int, CPU> &in, const Vector<int, CPU>& p, const Vector<int, CPU>& map, Vector<int, CPU> &result);
template
void scatter_permuted(int first, int last, const Vector<float, CPU> &in, const Vector<int, CPU>& p, const Vector<int, CPU>& map, Vector<float, CPU> &result);
template
void scatter_permuted(int first, int last, const Vector<double, CPU> &in, const Vector<int, CPU>& p, const Vector<int, CPU>& map, Vector<double, CPU> &result);

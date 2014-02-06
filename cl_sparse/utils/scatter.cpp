#include "scatter.h"


template<typename scalar>
void scatter (const int first, const int last, const Vector<scalar, CPU> &in, const Vector<int, CPU>& map, Vector<scalar, CPU> &out)
{

    //try something different to use shifts
    int num_elements = last - first;

    //scalar* r = out.get_data();

    for (int i = first; i < last; i++)
        out [ map[i-first] ] = in[i];
}


template<typename scalar>
void scatter_permuted(const int first, const int last, const Vector<scalar, CPU> &in, const Vector<int, CPU>& p, const Vector<int, CPU>& map, Vector<scalar, CPU> &out)
{
    //int num_elements = last - first;

    //scalar* r = out.get_data();

    for (int i = first; i < last; i++)
        out [ map[i-first]] = in [ p [i] ];
}

template<typename scalar>
void scatter_if(const int first, const int last, const Vector<scalar, CPU>& in,
                const Vector<int, CPU>& map, const Vector<int, CPU>& stencil,
                Vector<scalar, CPU>& out, const int shift)
{
    //int num_elements = last - first;

    for (int i = first; i < last; i++)
    {
        if (stencil[i])
            out [map [i-first] - shift] = in[i];
    }
}

template<typename scalar>
void scatter_if_permuted(const int first, const int last,
                          const Vector<scalar, CPU>& in, const Vector<int, CPU>& p,
                          const Vector<int, CPU>& map, const Vector<int, CPU>& stencil,
                          Vector<scalar, CPU>& out, const int shift)
{

    //int num_elements = last - first;

    for (int i = first; i < last; i++)
    {
        if (stencil[i])
            out [map [i-first] - shift] = in[ p [i]];
    }

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

template
void scatter_if(int first, int last, const Vector<float, CPU>& in,
                const Vector<int, CPU>& map, const Vector<int, CPU>& stencil,
                Vector<float, CPU>& out, const int shift);
template
void scatter_if(int first, int last, const Vector<double, CPU>& in,
                const Vector<int, CPU>& map, const Vector<int, CPU>& stencil,
                Vector<double, CPU>& out, const int shift);
template
void scatter_if(int first, int last, const Vector<int, CPU>& in,
                const Vector<int, CPU>& map, const Vector<int, CPU>& stencil,
                Vector<int, CPU>& out, const int shift);
template
void scatter_if_permuted(const int first, const int last,
                          const Vector<int, CPU>& in, const Vector<int, CPU>& p,
                          const Vector<int, CPU>& map, const Vector<int, CPU>& stencil,
                          Vector<int, CPU>& out, const int shift);
template
void scatter_if_permuted(const int first, const int last,
                          const Vector<float, CPU>& in, const Vector<int, CPU>& p,
                          const Vector<int, CPU>& map, const Vector<int, CPU>& stencil,
                          Vector<float, CPU>& out, const int shift);
template
void scatter_if_permuted(const int first, const int last,
                          const Vector<double, CPU>& in, const Vector<int, CPU>& p,
                          const Vector<int, CPU>& map, const Vector<int, CPU>& stencil,
                          Vector<double, CPU>& out, const int shift);

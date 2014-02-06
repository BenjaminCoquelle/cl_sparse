#include "scan.h"


template<typename scalar>
void exclusive_scan(const int first, const int last, const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out)
{

    if (first != last)
    {
        scalar tmp = in[first]; //tmp val allows inplace scan
        scalar accumulator = (scalar)0;

        out[first] = accumulator;

        accumulator += tmp;
        for (int i = first+1; i < last; i++)
        {
            tmp = in[i];
            out[i] = accumulator;
            //accumulator = operator(accumulator, tmp)
            accumulator += tmp; // this op is just accumulator = accumulator + tmp = accumulator = sum(acc, tmp)

        }
    }
}

template <typename scalar>
void inclusive_scan(const int first, const int last, const Vector<scalar, CPU>& in, Vector<scalar, CPU>& out)
{

    if (first != last)
    {
        scalar accumulator = in[first];
        out[first] = accumulator;

        for ( int i = first + 1; i < last; i++)
            out[i] = accumulator += in[i];
    }
}

template <typename scalar>
void inclusive_scan_max(const int first, const int last,
                        const Vector<scalar, CPU>&in,
                        Vector<scalar, CPU>& out)
{
    if (first != last)
    {
        scalar accumulator = in[first];
        out[first] = accumulator;

        for ( int i = first + 1; i < last; i++)
        {
//            //if max = true then take greater value else take in[i]
//                if( accumulator > in[i])
//                    accumulator = accumulator;
//                else
//                    accumulator = in[i];

            out[i] = accumulator = accumulator > in[i] ? accumulator : in[i];
        }
    }

}

template<typename scalar>
void inclusive_scan_by_key(const int first, const int last,
                           const Vector<scalar, CPU>& in,
                           const Vector<int, CPU>& keys,
                           Vector<scalar, CPU>& out)
{
    if (first != last)
    {
        int     prev_key   = keys[first];
        scalar  prev_value = in[first];

        out[first] = prev_value;
        for ( int i = first + 1; i < last; i++)
        {
            int key = keys[i];
            if(prev_key == key)
                out[i] = prev_value = prev_value + in[i];
            else
                out[i] = prev_value = in[i];

            prev_key = key;
        }
    }

}




template
void exclusive_scan(const int first, const int last, const Vector<int, CPU>& in, Vector<int, CPU>& out);
template
void exclusive_scan(const int first, const int last, const Vector<float, CPU>& in, Vector<float, CPU>& out);
template
void exclusive_scan(const int first, const int last, const Vector<double, CPU>& in, Vector<double, CPU>& out);


template
void inclusive_scan(const int first, const int last, const Vector<int, CPU>& in, Vector<int, CPU>& out);
template
void inclusive_scan(const int first, const int last, const Vector<float, CPU>& in, Vector<float, CPU>& out);
template
void inclusive_scan(const int first, const int last, const Vector<double, CPU>& in, Vector<double, CPU>& out);

template
void inclusive_scan_max(const int first, const int last, const Vector<int, CPU>& in, Vector<int, CPU>& out);
template
void inclusive_scan_max(const int first, const int last, const Vector<float, CPU>& in, Vector<float, CPU>& out);
template
void inclusive_scan_max(const int first, const int last, const Vector<double, CPU>& in, Vector<double, CPU>& out);



template
void inclusive_scan_by_key(const int first, const int last,
                           const Vector<int, CPU>& in,
                           const Vector<int, CPU>& keys,
                           Vector<int, CPU>& out);

template
void inclusive_scan_by_key(const int first, const int last,
                           const Vector<float, CPU>& in,
                           const Vector<int, CPU>& keys,
                           Vector<float, CPU>& out);

template
void inclusive_scan_by_key(const int first, const int last,
                           const Vector<double, CPU>& in,
                           const Vector<int, CPU>& keys,
                           Vector<double, CPU>& out);


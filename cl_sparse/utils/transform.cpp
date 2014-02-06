#include <utils/transform.h>

template<typename scalar>
void transform(const int first, const int last,
               const Vector<scalar, CPU>& A, const Vector<int, CPU>& Ap,
               const Vector<scalar, CPU>& B, const Vector<int, CPU>& Bp,
               Vector<scalar, CPU>& V)
{
    for (int i = first; i < last; i++)
        V[i] = A[Ap[i]] * B[Bp[i]];
}





template
void transform (const int first, const int last,
                const Vector<int, CPU>& A, const Vector<int, CPU>& Ap,
                const Vector<int, CPU>& B, const Vector<int, CPU>& Bp,
                Vector<int, CPU>& V);

template
void transform (const int first, const int last,
                const Vector<float, CPU>& A, const Vector<int, CPU>& Ap,
                const Vector<float, CPU>& B, const Vector<int, CPU>& Bp,
                Vector<float, CPU>& V);
template
void transform (const int first, const int last,
                const Vector<double, CPU>& A, const Vector<int, CPU>& Ap,
                const Vector<double, CPU>& B, const Vector<int, CPU>& Bp,
                Vector<double, CPU>& V);

#ifndef OCL_VECTOR_H
#define OCL_VECTOR_H

#include <algorithm>
#include <math.h>
#include <types.h>
#include <string>

#include "ocl.h"
#include "types.h"

/*********************************

    GPU version of dense vector

**********************************/
template <typename scalar>
class Vector<scalar, GPU>
{
public:
    Vector();
    Vector(int size);
    Vector(int size, scalar value, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);
    Vector(const Vector<scalar, CPU>& other);
    Vector(const Vector<scalar, GPU>& other);


    Vector<scalar, GPU>& operator= (const Vector<scalar, CPU>& other);
    Vector<scalar, GPU>& operator= (const Vector<scalar, GPU>& other);

    //element-wise additon. Creates new object! use ++ instead;
    const Vector operator+ (const Vector& other);
    //element-wise subtraction. Creates new object!
    const Vector operator- (const Vector& other);
    //element-wise multiplication. Creates new object!
    const Vector operator* (const Vector& other);
    //scale
    const Vector operator* (const scalar alpha);

    const Vector operator* (const cl_mem alpha);
    //equivalent for above comment
    //const Vector get_scaled(const cl_mem alpha);

    Vector& operator+= (const Vector& other);
    Vector& operator-= (const Vector& other);

    Vector& operator*= (const Vector& other);
    //scale
    Vector& operator*= (const scalar alpha);

    Vector& operator*= (const cl_mem alpha);
    //void scale(const cl_mem alpha);

    int     get_size();
    int const get_csize() const;

    cl_mem  get_data();
    cl_mem const get_cdata() const;


    //dot product with other vector - result to cpu
    scalar  dot(const Vector& other);
    //dot product with other vector - result to GPU
    void    dot(const Vector& other, cl_mem* result);

    //calculate vector norm l = 1 norm 1, l=2 norm 2
    scalar  norm(const int l = 2);
    //calculate vector norm l = 1 norm 1, l=2 norm 2
    //result is on GPu
    void    norm(cl_mem* result, const int l = 2);

    //reduction with final reduction on cpu
    scalar  sum();
    //reduction with final reduction on gpu
    void    sum(cl_mem* result);

    //set vector values to value
    void    set(scalar value = 0);

    //here just print several first not the last one to avoid copying whole mem
    void    print(int n = 5);

    void    save(std::string fname);
    void    load(std::string fname);
    ~Vector();

private:
    bool equal(const Vector& other);
    size_t roundUp(int group_size, int global_size);

    //norm 1 implementation
    inline scalar _norm1();
    //norm 2 implementation
    inline scalar _norm2();

    //norm 1 implementation to gpu
    inline void _norm1(cl_mem* result);
    //norm 2 implementation to gpu
    inline void _norm2(cl_mem* result);

    scalar reduce();
    int size;
    cl_mem data;

};

#endif // OCL_VECTOR_H

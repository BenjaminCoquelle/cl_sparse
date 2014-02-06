#ifndef CPU_VECTOR_H
#define CPU_VECTOR_H

#include <string>
#include "types.h"


template <typename scalar, DeviceType device>
class Vector;

template <typename scalar>
class Vector<scalar, CPU>
{
public:
    Vector();
    Vector(int size);
    Vector(int size, scalar value);
    explicit Vector(int size, const scalar* data);
    explicit Vector(const int begin, const int end, const int size); //size added to distinct between other constr;
    Vector(const Vector& other);

    Vector& operator= (const Vector& other);

    //element-wise additon. Creates new object! use ++ instead;
    const Vector operator+ (const Vector& other);
    //element-wise subtraction. Creates new object!
    const Vector operator- (const Vector& other);
    //element-wise multiplication. Creates new object!
    const Vector operator* (const Vector& other);
    //scale
    const Vector operator* (const scalar alpha);

    //+= operator, changes this values
    Vector& operator+= (const Vector& other);
    //-= operator, changes this values
    Vector& operator-= (const Vector& other);
    //*= operator, changes this values
    Vector& operator*= (const Vector& other);
    //scale
    Vector& operator*= (const scalar alpha);


    bool operator== (const Vector<scalar, CPU>& other);
    bool operator== (const Vector<scalar, GPU>& other);


    const scalar& operator[] (int i) const;
    scalar& operator[] (int i);

    // get size of the vector
    int     get_size()  const;
    int     get_csize() const;
    // get pointer to data array
    scalar* get_data();
    // get const pointer to data array
    scalar* get_cdata() const;

    void resize(const int n);

    //dot product with other vector
    scalar  dot(const Vector& other);
    //vector norm, l = 1 - l1 norm. l = 2, l2 norm
    scalar  norm(const int l = 2);
    // sum //reduction algorithm
    scalar  sum();

    // fill data with value
    void    set(scalar value = 0);

    //print n elements
    void    print(int n = 5);

    //save to flat file
    void    save(std::string fname);
    void    load(std::string fname);

    void set_pointerData(scalar* ptr, const int size);
    void release_pointerData();

    ~Vector();

private:
    //compare sizes
    bool equal(const Vector<scalar, GPU>& other);
    bool equal(const Vector<scalar, CPU>& other);

    int size;

    scalar* data;

    //private members of CPU Vector are available for GPU
    //dont fuck with const correctnes
    friend class Vector<scalar, GPU>;
};


#endif // CPU_VECTOR_H

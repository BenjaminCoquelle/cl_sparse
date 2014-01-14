#include "ocl.h"
#include "vector.h"
#include <QFile>

template<typename scalar>
Vector<scalar,CPU>::Vector()
{
    this->size = 0;
    this->data = NULL;
}

template<typename scalar>
Vector<scalar,CPU>::Vector(int size)
{
    this->size = size;
    data = new scalar[size];
}

template<typename scalar>
Vector<scalar, CPU>::Vector(int size, scalar value)
{
    this->size = size;
    data = new scalar[size];

    set(value);
}

template<typename scalar>
Vector<scalar, CPU>::Vector(const Vector& other)
{
    this->size = other.size;
    this->data = new scalar[size];

    memcpy(this->data, other.data, size*sizeof(scalar));
}


template<typename scalar>
Vector<scalar, CPU>& Vector<scalar,CPU>::operator= (const Vector& other)
{
    // we need just to realloc the data when they have differen sizes otherwise copy over the old data
    if (!equal(other))
    {
        if (data != NULL)
        {
            qDebug() << "DELETING DATA";
            delete data;
        }
        this->size = other.size;
        this->data = new scalar[size];

        //other option
        /*
          this->data = (scalar*) realloc (this->data, this->size * sizeof(scalar));
         */

    }
    memcpy(this->data, other.data, size*sizeof(scalar));
    return *this;
}


template<typename scalar>
const Vector<scalar, CPU> Vector<scalar,CPU>::operator+ (const Vector& other)
{
    if (!equal(other))
    {
        qCritical() << "Error Vector (CPU) + : vectors have different sizes";
        return *this;
    }

    Vector result(*this);

    for (int i = 0; i < this->size; i++)
    {
        result.data[i] += other.data[i];
    }
    return result;
}

template<typename scalar>
const Vector<scalar, CPU> Vector<scalar,CPU>::operator- (const Vector& other)
{
    if (!equal(other))
    {
        qCritical() << "Error Vector (CPU) - : vectors have different sizes";
        return *this;
    }
    Vector result(*this);

    for (int i = 0; i < this->size; i++)
    {
        result.data[i] -= other.data[i];
    }
    return result;
}

template<typename scalar>
const Vector<scalar, CPU> Vector<scalar,CPU>::operator* (const Vector& other)
{
    if (!equal(other))
    {
        qCritical() << "Error Vector (CPU) * : vectors have different sizes";
        return *this;
    }
    Vector result(*this);

    for (int i = 0; i < this->size; i++)
    {
        result.data[i] *= other.data[i];
    }
    return result;
}

template<typename scalar>
Vector<scalar, CPU>& Vector<scalar, CPU>::operator+= (const Vector& other)
{
    if (!equal(other))
    {
        qCritical() << "Error Vector (CPU) += : vectors have different sizes";
        return *this;
    }
    for (int i = 0; i < this->size; i++)
    {
        this->data[i] += other.data[i];
    }
    return *this;
}

template<typename scalar>
Vector<scalar, CPU>& Vector<scalar, CPU>::operator-= (const Vector& other)
{
    if (!equal(other))
    {
        qCritical() << "Error Vector (CPU) - : vectors have different sizes";
        return *this;
    }
    for (int i = 0; i < this->size; i++)
    {
        this->data[i] -= other.data[i];
    }
    return *this;
}

template<typename scalar>
Vector<scalar, CPU>& Vector<scalar, CPU>::operator*= (const Vector& other)
{
    if (!equal(other))
    {
        qCritical() << "Error Vector (CPU) * : vectors have different sizes";
        return *this;
    }
    for (int i = 0; i < this->size; i++)
    {
        this->data[i] *= other.data[i];
    }
    return *this;
}

template<typename scalar>
const scalar& Vector<scalar, CPU>::operator[] (int i)
{
    if(i < size)
        return this->data[i];
    else
        qDebug() << "Vector (CPU) []: index out of range i=" << i << " size=" << size;

}

template<typename scalar>
void Vector<scalar, CPU>::set(scalar value)
{
    std::fill(data, data+size, value);
}

template<typename scalar>
int Vector<scalar, CPU>::get_size() const
{
    return size;
}

template<typename scalar>
scalar* Vector<scalar, CPU>::get_data()
{
    return data;
}

template<typename scalar>
scalar const* Vector<scalar, CPU>::get_cdata() const
{
    return data;
}

template<typename scalar>
scalar  Vector<scalar, CPU>::dot(const Vector& other)
{
    if (!equal(other))
    {
        qCritical() << "Vector (CPU) dot: Vector sizes are different";
        return 0;
    }

    scalar sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += data[i] * other.data[i];
    }
    return sum;
}


template<typename scalar>
scalar  Vector<scalar, CPU>::norm(const int l)
{

    scalar sum = 0;
    if (l == 1)
    {
        for(int i = 0; i < size; i++)
        {
            sum += fabs(data[i]);
        }
    } else if (l == 2)
    {
        sum = sqrt(dot(*this));
    }
    else
    {
        qDebug() << "Vector (CPU) norm: Wrong norm type, " << l << " given.\n";
    }
    return sum;
}


template<typename scalar>
scalar  Vector<scalar, CPU>::sum()
{
    scalar sum = 0;
    for(int i = 0; i < size; i++)
    {
        sum += data[i];
    }
    return sum;
}

template<typename scalar>
bool Vector<scalar, CPU>::equal(const Vector& other)
{
    bool r = (this->size == other.size);
    return r;
}

template<typename scalar>
void Vector<scalar, CPU>::print(int n)
{
    std::ostringstream info;
    info << "Vector (CPU):\n";
    if (size < n)
    {
        for (int i = 0; i < size-1; i++)
            info << data[i] << ", ";

        info << "..., " << data[size-1] << "]";
    }
    else
    {
        for (int i = 0; i < n-1; i++)
            info << data[i] << ", ";

        info << "..., " << data[n-1] << "]";
    }
    qDebug() << qPrintable(QString::fromStdString(info.str()));
}


template<typename scalar>
void Vector<scalar, CPU>::save(std::string fname)
{
    QFile file(fname.c_str());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Vector (CPU): Problem with opening [" << fname.c_str() <<"] to save vector data";
       return;
    }

    QTextStream out (&file);
    out.setRealNumberNotation(QTextStream::ScientificNotation);
    for (int i = 0; i < size; i++)
        out << data[i] << "\n";
    file.close();
}

template<typename scalar>
Vector<scalar, CPU>::~Vector()
{
    delete [] data;
    data = NULL;
    size = 0;
}

//initialization of templates
template class Vector<float, CPU>;
template class Vector<double, CPU>;

#include "kernel.h"
#include <typeinfo>

template <typename type>
void set_parameter(unsigned int index, type t, cl_kernel k, QString name)
{
    //qDebug() << "Setting #" << index << " parameter for kernel " << name << " " << typeid(type).name();
    cl_int error = clSetKernelArg(k, index, sizeof(t), &t);

    if(error!=CL_SUCCESS)
        qCritical() << "Set kernel argument failed: " << OpenCL::getError(error);
}

unsigned int next_pow2( unsigned int x ) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

Kernel::Kernel()
{
    name = "EMPTY";
    id = NULL;
}

Kernel::Kernel(cl_kernel id, QString name)
{
    this->id = id;
    this->name = name;
}

Kernel::Kernel(const Kernel &other)
{
    id = other.id;
    name = other.name;
}

Kernel& Kernel::operator =(const Kernel& other)
{
    id = other.id;
    name = other.name;

    return *this;
}

cl_kernel Kernel::get_id()
{
    return id;
}

QString Kernel::get_name()
{
    return name;
}

void Kernel::execute(size_t local, size_t global, cl_event* event, std::vector<cl_event>* wait_list )
{
    qDebug() << "Executing kernel " << name << " (" << local << "x" << global << ")";

    int event_count = 0;
    cl_event *waitforit = NULL;

    if (wait_list!=NULL)
    {
        qWarning() << "Events are not fully supported!";
        event_count = wait_list->size();
        waitforit = &(wait_list->front());
    }
    cl_int error =
            clEnqueueNDRangeKernel(OpenCL::get_queue(), id, 1, NULL, &global, &local, event_count, waitforit, event);

    if (error != CL_SUCCESS)
        qDebug() << "Execution failed. " << OpenCL::getError(error);
}

size_t Kernel::get_max_local_threads()
{
    if (!OpenCL::device_initialized)
    {
        qCritical() << "Device not initialized!";
        return 0;
    }

    size_t thread_num;
    clGetKernelWorkGroupInfo( id, OpenCL::get_device(), CL_KERNEL_WORK_GROUP_SIZE, sizeof( size_t ), &thread_num, NULL );

    return thread_num;
}

size_t Kernel::round_up(int group_size, int global_size)
{
    int r = global_size % group_size;
    if(r == 0)
    {
        return global_size;
    } else
    {
        return global_size + group_size - r;
    }
}


void Kernel::set_buffer (unsigned int index, cl_mem buffer)
{
    set_parameter<cl_mem>(index, buffer, id, name);
}

void Kernel::set_char (unsigned int index, cl_char c)
{
    set_parameter<cl_char>(index, c, id, name);
}

void Kernel::set_double(unsigned int index, cl_double d)
{
    set_parameter<cl_double>(index, d, id, name);
}

void Kernel::set_float(unsigned int index, cl_float f)
{
    set_parameter<cl_float>(index, f, id, name);
}

void Kernel::set_int(unsigned int index, cl_int i)
{
    set_parameter<cl_int>(index, i, id, name);
}

void Kernel::set_long(unsigned int index, cl_long l)
{
    set_parameter<cl_long>(index, l, id, name);
}

void Kernel::set_short(unsigned int index, cl_short si)
{
    set_parameter<cl_short>(index, si, id, name);
}

//Exceptional implementation. when we provide a local memory to kernel we just pass it's lenght
void Kernel::set_local(unsigned int index, size_t size)
{
    //qDebug() << "Setting #" << index << " parameter for kernel " << name << " " << "local" ;
    cl_int error = clSetKernelArg(id, index, size, NULL);

    if(error!=CL_SUCCESS)
        qCritical() << "Set kernel argument failed: " << OpenCL::getError(error);
}

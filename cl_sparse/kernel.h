#ifndef KERNEL_H
#define KERNEL_H

#include "ocl.h"

class Kernel
{
public:
    Kernel();

    Kernel(cl_kernel id, QString name);
    Kernel(const Kernel& other);

    Kernel& operator= (const Kernel& other);

    // kernel id
    cl_kernel   get_id();

    // kernel name
    QString     get_name();

    //kernel setter functions
    void set_buffer (unsigned int index, cl_mem buffer);
    void set_char   (unsigned int index, cl_char c);
    void set_short  (unsigned int index, cl_short si);
    void set_int    (unsigned int index, cl_int i);
    void set_long   (unsigned int index, cl_long l);
    void set_float  (unsigned int index, cl_float f);
    void set_double (unsigned int index, cl_double d);

    //Exceptional implementation,
    //when we provide a local memory to kernel we just pass it's lenght
    void set_local(unsigned int index, size_t size);

    // get max number of local threads for device
    // takes id from OpenCL::device;
    size_t get_max_local_threads();

    //get maximum rounup of group_size in global_size;
    size_t round_up(int group_size, int global_size);

    // execute kernel
    void execute(size_t local, size_t global, cl_event* event = NULL, std::vector<cl_event>* wait_list = NULL );

private:
    cl_kernel id;
    QString name;

};

unsigned int next_pow2( unsigned int x );

#endif // KERNEL_H

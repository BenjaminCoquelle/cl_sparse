#ifndef _OCL_H_
#define _OCL_H_

#include <CL/cl.h>
#include <QVector>
#include <QString>
#include <QDebug>
#include <QFile>

#include <sstream>
#include "kernel.h"

//TODO:
//  - funkcja release queue, context, device
//  - funkcje do operacji na pamięci GPU <---> CPU // inny plik
//  - struktura do operacji na kernelu // inny plik?
//  - timer który będzie miał wywołanie przy clEnqueueNDRangeKernel //inny plik

//forward declaration
class Kernel;

class OpenCL
{

public:
    OpenCL (int device_id);

    static cl_context get_context();
    static cl_command_queue get_queue();
    static cl_device_id get_device();

    //Create buffer dest on device;
    //when CL_MEM_USE_HOST_PTR the data are copied from source in this call;
    //memory is guaranteed to be consistent only at completion events of enqueued operations;
    static void allocate (cl_mem* dest, size_t size, cl_mem_flags mem_flag = CL_MEM_READ_WRITE, void* source = NULL);

    static void free(cl_mem* buffer);

    static void copy (cl_mem dest, const void* source, size_t size, cl_bool blocking = CL_TRUE, cl_event* event = NULL);

    static void copy (void* dest, const cl_mem source, size_t size, cl_bool blocking = CL_TRUE, cl_event* event = NULL);

    static void copy (cl_mem dest, const cl_mem source, size_t size, cl_bool blocking = CL_TRUE, cl_event* event = NULL);

    static QString getError(cl_int e);

    static QMap<QString, Kernel> kernels;


    static bool context_initialized;
    static bool platform_initialized;
    static bool queue_initialized;
    static bool device_initialized;

    void shutdown();

private:

    struct Device
    {
        cl_device_id        id;
        QString             name;
        QString             vendor;
        cl_uint             max_compute_units;
        cl_uint             max_work_item_dimensions;
        size_t              max_work_item_size[3];
        size_t              max_group_size;
        cl_uint             max_clock_freq;
        cl_ulong            max_alloc_size;
        cl_ulong            local_mem_size;
        cl_ulong            total_mem_size;
        size_t              max_2d_width;
        size_t              max_2d_height;

        cl_uint             ui_support_double;
        bool                supportDouble;

        QString getInfo()
        {
            std::ostringstream info;

            info << "Device [" << id << "] - "<< name.toStdString()
                 << " ("<< vendor.toStdString() << ")" << std::endl;

            info << "\tMax compute units: " << max_compute_units << std::endl;
            info << "\tMax work-item dimensions: " << max_work_item_dimensions << std::endl;
            info << "\tMax work-item size: ("
                 << max_work_item_size[0] << ", "
                 << max_work_item_size[1] << ", "
                 << max_work_item_size[2] << ")" << std::endl;
            info << "\tMax work-group size: " << max_group_size << std::endl;
            info << "\tMax clock freq: " << max_clock_freq << std::endl;
            info << "\tMax alloc size: " << float(max_alloc_size*9.53674316*1e-7) << " MB"<< std::endl;
            info << "\tLocal mem size: " << float(local_mem_size*9.53674316*1e-7) << " MB" << std::endl;
            info << "\tTotal mem size: " << float(total_mem_size*9.53674316*1e-7) << " MB" << std::endl;
            info << "\t2D image max widht: " << max_2d_width << std::endl;
            info << "\t2D image max height: " << max_2d_height<< std::endl;

            info << "\tSupport double precision: " << supportDouble << std::endl;


            info << std::endl;
            return QString::fromStdString(info.str());
        }
    };

    struct Platform
    {
        cl_platform_id      id;
        QString         vendor;
        QString         name;
        QString         version;
        QString         profile;
        QString         extension;
        QVector<Device> devices;

        QString getInfo()
        {
            std::ostringstream info;

            info << "Platform [" << id << "]" << std::endl;
            info << "\tVendor: "  << vendor.toStdString() << std::endl;
            info << "\tName: "    << name.toStdString()   << std::endl;
            info << "\tVersion: " << version.toStdString()<< std::endl;
            info << "\tProfile: " << profile.toStdString()<< std::endl;
            info << "\tExtension: " << extension.toStdString() << std::endl;
            info << "\tNo of Devices: " << devices.size() << std::endl << std::endl;

            for (int i = 0; i < devices.size(); i++)
                info << devices[i].getInfo().toStdString();

           info << std::endl;

           return QString::fromStdString(info.str());
        }
    };

    //initialize fastest or device_id device
    void initialize (int device_id = -1);

    //search for OCL platforms;
    //can be many if there are AMD and NVIDIA in the same PC (only in windows)
    void find_platforms();
    //search for GPUs for given platform
    void find_devices (Platform &);

    //init context and queue for given device on platform
    //sets the static variables;
    void init_device(int platform, int device_id);

    //fill the Device struct and return it
    Device get_device(cl_device_id device_id);

    // get the best device by max compute units;
    // return by platform and device;
    void get_best_device(int& platform, int& device);

    QVector<Platform> platforms;

    //http://www.drdobbs.com/parallel/a-gentle-introduction-to-opencl/231002854
    static cl_context          context;     // cards table
    static cl_command_queue    queue;       // player's hand, can recieve cards (kernels), each device have one queue
    static cl_device_id        device;      // player at the table, one context(table) can manage many devices

    //functions related to errors handling
    static QMap<cl_int, QString> errors;
    void build_errors_map();

    //append kernel to the list, kernel is stored in file [file_name
    void append_kernel(QList<std::string>* kenrnels, QString file_name);
    //internal function to build kernels from source
    void build_program(const QList<std::string> &kernels_list);
    //generate custom vector kernels map;
    void build_kernels_map(bool display = 0);
    //generate Kernel clas objects to map
    Kernel get_kernel(const cl_program& program, std::string kernel_name);


}; //namespace OpenCL


//getter of various opencl parameters
//with proper size allocation in one function
template <typename Func, typename idType, typename paramType>
cl_int GetInfoHelper(Func f, idType id, cl_uint name, paramType* param)
{
    size_t required;
    cl_int err = f(id, name, 0, NULL, &required);
    if (err != CL_SUCCESS) {
        return err;
    }

    char* value = (char*) alloca(required);
    err = f(id, name, required, value, NULL);
    if (err != CL_SUCCESS) {
        return err;
    }

    *param = value;
    return CL_SUCCESS;
}

//helper function to read kernel source from file_name to QString kernel_source variable
bool GetKernelSource(QString file_name, std::string *kernel_source );


#endif // _OCL_H_

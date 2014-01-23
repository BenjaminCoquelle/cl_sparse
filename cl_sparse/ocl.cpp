#include "ocl.h"
#ifdef CL_BLAS
#include "clBLAS.h"
#endif
#include "iostream"
#include <QStringList>

// init static private variables; (exists across all instnces)
cl_context       OpenCL::context = NULL;
cl_device_id     OpenCL::device  = NULL;
cl_command_queue OpenCL::queue   = NULL;

bool OpenCL::context_initialized  = false;
bool OpenCL::device_initialized   = false;
bool OpenCL::platform_initialized = false;
bool OpenCL::queue_initialized = false;

QMap<cl_int, QString> OpenCL::errors;

QMap<QString, Kernel> OpenCL::kernels;

OpenCL::OpenCL (int device_id)
{
    //initialize device = device_id
    OpenCL::context_initialized  = false;
    OpenCL::device_initialized   = false;
    OpenCL::platform_initialized = false;
    OpenCL::queue_initialized = false;

    build_errors_map();
    initialize (device_id);

    build_kernels_map();
#ifdef CL_BLAS
    cl_int err = clblasSetup();
        if (err != CL_SUCCESS) {
            qDebug() << "clblasSetup failed\n";
        }
#endif
}


void OpenCL::allocate(cl_mem *dest, size_t size, cl_mem_flags mem_flag, void *source)
{
    cl_int error = 0;

    *dest = clCreateBuffer( context, mem_flag, size, source, &error );

    if(error!=CL_SUCCESS)
        qCritical() << "ClMem allocation failed. " << getError(error) << ".";
}

void OpenCL::free(cl_mem *buffer)
{
    cl_int error = clReleaseMemObject(*buffer);
    if (error != CL_SUCCESS)
        qDebug() << "Problem with releasing vector buffer: "
                 << OpenCL::getError(error);
}

void OpenCL::copy(cl_mem dest, const void *source, size_t size, cl_bool blocking, cl_event* event)
{
    cl_int error = clEnqueueWriteBuffer(queue, dest, blocking, 0, size, source, 0, 0, event);

    if (error != CL_SUCCESS)
        qCritical() << "Problem with copying to device. " << getError(error);

}

void OpenCL::copy(void *dest, const cl_mem source, size_t size, cl_bool blocking, cl_event* event)
{
    cl_int error = clEnqueueReadBuffer(queue, source, blocking, 0, size, dest, 0, 0, event);

    if (error != CL_SUCCESS)
        qCritical() << "Problem with copying to host. " << getError(error);
}

void OpenCL::copy(cl_mem dest, const cl_mem source, size_t size, cl_bool blocking, cl_event* event)
{
    cl_int error = clEnqueueCopyBuffer(queue, source, dest, 0, 0, size, 0, 0, event);

    if (error != CL_SUCCESS)
        qCritical() << "Problem with copying GPU <-> GPU. " << getError(error);
}

void OpenCL::initialize (int device_id)
{
    find_platforms();

    int choosen_platform = -1;
    int choosen_local_device = -1;
    if (device_id >=0)
    {
        // there can be many platforms with many devices;
        // we have to find proper platform and gpu based on global id;
        for (int p = 0; p < platforms.size(); p++)
        {
            int platform_dev_count = platforms[p].devices.size();
            if ( platform_dev_count <= device_id  )
            {
                device_id -= platform_dev_count;

            }
            else if (device_id < 0)
            {
                qDebug() << "Device out of range";
                break;

            }
            else
            {
                choosen_platform = p;
                choosen_local_device = device_id;
                break;
            }
        }
    }
    else
    {
        get_best_device(choosen_platform, choosen_local_device);
        qDebug() << "Picking device by max compute units";
    }
    platform_initialized = true;

    qDebug() << qPrintable(platforms[choosen_platform].devices[choosen_local_device].getInfo());
    init_device(choosen_platform, choosen_local_device);
}

void OpenCL::init_device(int platform, int device_id)
{
    cl_int error;
    cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[platform].id, 0 }; // not necessary, can be 0;

    // within one context there can be many gpus
    cl_uint num_devices = 1;
    device =  platforms[platform].devices[device_id].id;

    context = clCreateContext(properties, num_devices, &device, NULL, NULL, &error);
    if (error != CL_SUCCESS)
    {
        qCritical() << "ERROR: Problem with creation of context for platform "
                 << platform << " device " << device_id
                 << " Msg: " << getError(error);
        return;
    }
    context_initialized = true;

    queue = clCreateCommandQueue(context, device, 0, &error);
    if (error != CL_SUCCESS)
    {
        qCritical() << "ERROR: Problem with creation of queue for platform "
                 << platform << " device " << device_id
                 << " Msg: " << getError(error);
        return;
    }
    queue_initialized = true;

}

void OpenCL::find_platforms()
{
    cl_uint platforms_count;
    cl_platform_id* platform_ids;

    //check how many platforms are available
    clGetPlatformIDs(0, NULL, &platforms_count);
    if ( platforms_count == 0)
    {
        qFatal("There are no OpenCL platforms on this PC");
    }
    //fill the platforms id to identify them;
    platform_ids = new cl_platform_id[platforms_count];
    clGetPlatformIDs(platforms_count, platform_ids, &platforms_count);

    //loop trough all platforms;
    for (uint i = 0; i < platforms_count; i++)
    {
        Platform p;
        p.id = platform_ids[i];

        GetInfoHelper(clGetPlatformInfo, p.id, CL_PLATFORM_VENDOR, &(p.vendor));
        GetInfoHelper(clGetPlatformInfo, p.id, CL_PLATFORM_NAME, &(p.name));
        GetInfoHelper(clGetPlatformInfo, p.id, CL_PLATFORM_VERSION, &(p.version));
        GetInfoHelper(clGetPlatformInfo, p.id, CL_PLATFORM_PROFILE, &(p.profile));
        GetInfoHelper(clGetPlatformInfo, p.id, CL_PLATFORM_EXTENSIONS, &(p.extension));

        find_devices(p);

        if (p.devices.size() > 0)
        {
                platforms.push_back(p);
        }
    }
    delete [] platform_ids;

    for(int p = 0; p < platforms.size(); p++)
        qDebug() << qPrintable(platforms[p].getInfo());
}

void OpenCL::find_devices(Platform& p)
{
    cl_uint number_of_devices = 0;
    cl_device_id *device_ids;

    /*
     *  #define CL_DEVICE_TYPE_DEFAULT                      (1 << 0)
        #define CL_DEVICE_TYPE_CPU                          (1 << 1)
        #define CL_DEVICE_TYPE_GPU                          (1 << 2)
        #define CL_DEVICE_TYPE_ACCELERATOR                  (1 << 3)
        #define CL_DEVICE_TYPE_CUSTOM                       (1 << 4)
        #define CL_DEVICE_TYPE_ALL                          0xFFFFFFFF
    */
    cl_device_type device_type = CL_DEVICE_TYPE_GPU;
    clGetDeviceIDs(p.id, device_type, 0, NULL, &number_of_devices );
    device_ids = new cl_device_id[number_of_devices];
    clGetDeviceIDs(p.id, device_type, number_of_devices, device_ids, &number_of_devices);

    for (unsigned int i = 0; i < number_of_devices; i++ )
        p.devices.push_back(get_device(device_ids[i]));

    delete [] device_ids;

}

OpenCL::Device OpenCL::get_device(cl_device_id device_id)
{
    Device d;

    d.id = device_id;

    GetInfoHelper(clGetDeviceInfo, d.id, CL_DEVICE_NAME, &(d.name));
    GetInfoHelper(clGetDeviceInfo, d.id, CL_DEVICE_VENDOR, &(d.vendor));

    clGetDeviceInfo(d.id, CL_DEVICE_MAX_COMPUTE_UNITS,         sizeof(d.max_compute_units),        &(d.max_compute_units),          NULL);
    clGetDeviceInfo(d.id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,  sizeof(d.max_work_item_dimensions), &(d.max_work_item_dimensions),   NULL);
    clGetDeviceInfo(d.id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(d.max_work_item_size),             &(d.max_work_item_size),         NULL);
    clGetDeviceInfo(d.id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(d.max_group_size), &(d.max_group_size), NULL );
    clGetDeviceInfo(d.id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(d.max_clock_freq), &(d.max_clock_freq), NULL );
    clGetDeviceInfo(d.id,  CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(d.max_alloc_size), &(d.max_alloc_size), NULL );
    clGetDeviceInfo(d.id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(d.local_mem_size), &(d.local_mem_size), NULL);
    clGetDeviceInfo(d.id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(d.total_mem_size), &(d.total_mem_size), NULL);
    clGetDeviceInfo(d.id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(d.ui_support_double), &(d.ui_support_double), NULL);

    if (d.ui_support_double ==0)
        d.supportDouble = false;
    else
        d.supportDouble = true;

    return d;
}

void OpenCL::get_best_device(int &platform, int &device)
{
    cl_uint max_compute_units = 0;

    for (int p = 0; p < platforms.size(); p++)
        for (int d = 0; d < platforms[p].devices.size(); d++)
            if (platforms[p].devices[d].max_compute_units >= max_compute_units) // change here to ">" if you wish to take first
            {
                platform = p;
                device = d;
                max_compute_units = platforms[p].devices[d].max_compute_units;
            }
}

QString OpenCL::getError(cl_int e)
{
    if (errors.find(e) == errors.end())
        return "Unknown Error";
    return errors[e];

}

void OpenCL::build_errors_map()
{
    errors[CL_SUCCESS] = "Success";
    errors[CL_DEVICE_NOT_FOUND] = "Device not found";
    errors[CL_DEVICE_NOT_AVAILABLE] = "Device not available";
    errors[CL_COMPILER_NOT_AVAILABLE] = "Compiler not available";
    errors[CL_MEM_OBJECT_ALLOCATION_FAILURE] = "Memory object allocation failure";
    errors[CL_OUT_OF_RESOURCES] = "Out of resources";
    errors[CL_OUT_OF_HOST_MEMORY] = "Out of host memory";
    errors[CL_PROFILING_INFO_NOT_AVAILABLE] = "Profiling info not available";
    errors[CL_MEM_COPY_OVERLAP] = "Memory copy overlap";
    errors[CL_IMAGE_FORMAT_MISMATCH] = "Image format mismatch";
    errors[CL_IMAGE_FORMAT_NOT_SUPPORTED] = "Image format not supported";
    errors[CL_BUILD_PROGRAM_FAILURE] = "Build program failure";
    errors[CL_MAP_FAILURE] = "Map failure";
    errors[CL_MISALIGNED_SUB_BUFFER_OFFSET] = "Misaligned sub-buffer offset";
    errors[CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST] = "Execute status error for events in wait list";
    errors[CL_INVALID_VALUE] = "Invalid value";
    errors[CL_INVALID_DEVICE_TYPE] = "Invalid device type";
    errors[CL_INVALID_PLATFORM] = "Invalid platform";
    errors[CL_INVALID_DEVICE] = "Invalid device";
    errors[CL_INVALID_CONTEXT] = "Invalid context";
    errors[CL_INVALID_QUEUE_PROPERTIES] = "Invalid queue properties";
    errors[CL_INVALID_COMMAND_QUEUE] = "Invalid command queue";
    errors[CL_INVALID_HOST_PTR] = "Invalid host pointer";
    errors[CL_INVALID_MEM_OBJECT] = "Invalid memory object";
    errors[CL_INVALID_IMAGE_FORMAT_DESCRIPTOR] = "Invalid image format descriptor";
    errors[CL_INVALID_IMAGE_SIZE] = "Invalid image size";
    errors[CL_INVALID_SAMPLER] = "Invalid sampler";
    errors[CL_INVALID_BINARY] = "Invalid binary";
    errors[CL_INVALID_BUILD_OPTIONS] = "Invalid build options";
    errors[CL_INVALID_PROGRAM] = "Invalid program";
    errors[CL_INVALID_PROGRAM_EXECUTABLE] = "Invalid program executable";
    errors[CL_INVALID_KERNEL_NAME] = "Invalid kernel name";
    errors[CL_INVALID_KERNEL_DEFINITION] = "Invalid kernel definition";
    errors[CL_INVALID_KERNEL] = "Invalid kernel";
    errors[CL_INVALID_ARG_INDEX] = "Invalid argument index";
    errors[CL_INVALID_ARG_VALUE] = "Invalid argument value";
    errors[CL_INVALID_ARG_SIZE] = "Invalid argument size";
    errors[CL_INVALID_KERNEL_ARGS] = "Invalid kernel arguments";
    errors[CL_INVALID_WORK_DIMENSION] = "Invalid work dimension";
    errors[CL_INVALID_WORK_GROUP_SIZE] = "Invalid work group size";
    errors[CL_INVALID_WORK_ITEM_SIZE] = "Invalid work item size";
    errors[CL_INVALID_GLOBAL_OFFSET] = "Invalid global offset";
    errors[CL_INVALID_EVENT_WAIT_LIST] = "Invalid event wait list";
    errors[CL_INVALID_EVENT] = "Invalid event";
    errors[CL_INVALID_OPERATION] = "Invalid operation";
    errors[CL_INVALID_GL_OBJECT] = "Invalid gl object";
    errors[CL_INVALID_BUFFER_SIZE] = "Invalid buffer size";
    errors[CL_INVALID_MIP_LEVEL] = "Invalid MIP level";
    errors[CL_INVALID_GLOBAL_WORK_SIZE] = "Invalid global work size";
    errors[CL_INVALID_PROPERTY] = "Invalid property";
}

void OpenCL::shutdown()
{
#ifdef CL_BLAS
    clblasTeardown();
#endif
    //cleanup created OpenCL structures

    //command queue
    if (queue_initialized)
        clReleaseCommandQueue(queue);

    //kernels
    foreach (QString key, kernels.keys ())
    {

        Kernel k = kernels.value (key);
        //qDebug() << "Releasing kernel " << key;
        clReleaseKernel(k.get_id ());
    }

    if (context_initialized)
        clReleaseContext(context);

}

cl_context OpenCL::get_context()
{
    if (context_initialized)
        return context;

    qCritical()<< "Context not initialized";
    return NULL;
}

cl_command_queue OpenCL::get_queue()
{
    if (queue_initialized)
        return queue;

    qCritical()<< "Queue not initialized";
    return NULL;
}

cl_device_id OpenCL::get_device()
{
    if (device_initialized)
        return device;

    qCritical() << "Device not initialized!";
    return NULL;
}

void OpenCL::build_kernels_map(bool display)
{
    //QString cause problems in proper conversion to const char*
    QList<std::string> kernels;


    /**
      TODO:
      - only one file per data type;
      - in build_program read proper kernel names
      */
    //vector kernels
    append_kernel(&kernels, "../../clsparse/cl_sparse/vector_kernels/vector_kernels.cl");

    //coo matrix
    append_kernel(&kernels, "../../clsparse/cl_sparse/coo_kernels/coo_matrix_krenels.cl");

    //csr matrix
    append_kernel(&kernels, "../../clsparse/cl_sparse/csr_kernels/csr_matrix_krenels.cl");

    //ell matrix
    append_kernel(&kernels, "../../clsparse/cl_sparse/ell_kernels/ell_matrix_krenels.cl");

    if(display)
    {
        int i = 0;
        for(QList<std::string>::iterator it = kernels.begin();
            it != kernels.end();
            it++)
        {
            std::string current = *it;
            qDebug() << "Kernel no: " << i++ << "\n[" << current.c_str() <<"]\n";
        }
    }
    build_program(kernels);
}

void OpenCL::append_kernel(QList<std::string>* kernels, QString file_name)
{
    std::string source;
    if (GetKernelSource(file_name, &source))
         kernels->append(source);
    else
        qCritical() << "Kernel [" << file_name << "] not loaded correctly";
}

void OpenCL::build_program(const QList<std::string>& kernels_list)
{
    int err;

    //buld const char* array to fill the requirements;
    const char* source_list[kernels_list.length()];
    int i = 0;
    foreach(std::string k, kernels_list)
           source_list[i++] = k.c_str();

    //create program object
    cl_program program = clCreateProgramWithSource(context,
                                                   kernels_list.length(),
                                                   source_list, NULL, &err);

    if (err != CL_SUCCESS)
    {
        qCritical() << "Unable to create program";
    }

    // Compile it.
    err = clBuildProgram(program, 1, &device, "-w -cl-mad-enable -cl-no-signed-zeros", NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                              sizeof(buffer), buffer, &len);
        fprintf(stderr, "%s\n", buffer);
        qCritical() << "Unable to build program";

    }

    /**
      TODO:
      - only one file per data type;
      - in build_program read proper kernel names
      */

    //fill the kernels map;
    kernels["s_vector_add"] = get_kernel(program, "s_vector_add");
    kernels["d_vector_add"] = get_kernel(program, "d_vector_add");

    kernels["s_vector_sub"] = get_kernel(program, "s_vector_sub");
    kernels["d_vector_sub"] = get_kernel(program, "d_vector_sub");

    kernels["s_vector_mul"] = get_kernel(program, "s_vector_mul");
    kernels["d_vector_mul"] = get_kernel(program, "d_vector_mul");

    kernels["s_vector_sum"] = get_kernel(program, "s_vector_sum");
    kernels["d_vector_sum"] = get_kernel(program, "d_vector_sum");

    kernels["s_vector_scale_scalar"] = get_kernel(program, "s_vector_scale_scalar");
    kernels["s_vector_scale_mem"] = get_kernel(program, "s_vector_scale_mem");

    kernels["d_vector_scale_scalar"] = get_kernel(program, "d_vector_scale_scalar");
    kernels["d_vector_scale_mem"] = get_kernel(program, "d_vector_scale_mem");


    kernels["s_vector_dot"] = get_kernel(program, "s_vector_dot");
    kernels["d_vector_dot"] = get_kernel(program, "d_vector_dot");

    kernels["s_vector_norm"] = get_kernel(program, "s_vector_norm");
    kernels["d_vector_norm"] = get_kernel(program, "d_vector_norm");

    kernels["s_vector_norm2"] = get_kernel(program, "s_vector_norm2");
    kernels["d_vector_norm2"] = get_kernel(program, "d_vector_norm2");

    kernels["s_vector_sum_scalar"] = get_kernel(program, "s_vector_sum_scalar");
    kernels["d_vector_sum_scalar"] = get_kernel(program, "d_vector_sum_scalar");

    kernels["s_vector_sum_sqrt"] = get_kernel(program, "s_vector_sum_sqrt");
    kernels["d_vector_sum_sqrt"] = get_kernel(program, "d_vector_sum_sqrt");


    //coo matrix
    kernels["s_kernel_coo_spmv_flat"] = get_kernel(program, "s_kernel_coo_spmv_flat");
    kernels["s_kernel_coo_spmv_reduce_update"] = get_kernel(program, "s_kernel_coo_spmv_reduce_update");
    kernels["s_kernel_coo_spmv_serial"] = get_kernel(program, "s_kernel_coo_spmv_serial");

    kernels["d_kernel_coo_spmv_flat"] = get_kernel(program, "d_kernel_coo_spmv_flat");
    kernels["d_kernel_coo_spmv_reduce_update"] = get_kernel(program, "d_kernel_coo_spmv_reduce_update");
    kernels["d_kernel_coo_spmv_serial"] = get_kernel(program, "d_kernel_coo_spmv_serial");

    //csr matrix
    kernels["s_kernel_csr_spmv_vector"] = get_kernel(program, "s_kernel_csr_spmv_vector");
    kernels["d_kernel_csr_spmv_vector"] = get_kernel(program, "d_kernel_csr_spmv_vector");

    kernels["s_kernel_ell_spmv"] = get_kernel(program, "s_kernel_ell_spmv");
    kernels["d_kernel_ell_spmv"] = get_kernel(program, "d_kernel_ell_spmv");

    clReleaseProgram(program);
}

Kernel OpenCL::get_kernel(const cl_program& program, std::string kenrel_name)
{
    cl_int err;
    cl_kernel id = clCreateKernel(program, kenrel_name.c_str(), &err);
    if (err != CL_SUCCESS)
    {
        qCritical() << "Unable to create kernel: [" << kenrel_name.c_str() << "] : " << getError(err);
    }

    Kernel k(id, QString(kenrel_name.c_str()));
    return k;
}

//some helper
bool GetKernelSource(QString file_name, std::string* kernel_source )
{
    QFile f(file_name);

    if (!f.exists())
    {
        qCritical() << "File [" << file_name <<"] does not exists!";
        return false;
    }

    f.open(QFile::ReadOnly);
    QByteArray source = f.readAll();
    (*kernel_source) = std::string(source.data());
    return true;
}



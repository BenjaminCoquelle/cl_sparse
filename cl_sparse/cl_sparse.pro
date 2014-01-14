#-------------------------------------------------
#
# Project created by QtCreator 2013-10-30T09:30:34
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = SPMV_OCL
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/local/cuda/include \
               /opt/intel/opencl-1.2-3.1.1.11385/include
LIBS +=      -L/opt/intel/opencl-1.2-3.1.1.11385/lib64 \
             -lOpenCL

SOURCES += main.cpp \
    ocl.cpp \
    vector_cpu.cpp \
    vector_gpu.cpp \
    vector_gpu.cpp \
    kernel.cpp \
    timer.cpp \
    matrixcoo_cpu.cpp \
    mmio.cpp \
    matrixcoo_gpu.cpp

HEADERS += ocl.h \
    vector.h \
    kernel.h \
    timer.h \
    types.h \
    matrixcoo.h \
    matrix_types.h \
    mmio.h

OTHER_FILES += \
    resources/brief_csr_multip.txt \
    coo_kernels/coo_matrix_krenels.cl \
    vector_kernels/vector_kernels.cl

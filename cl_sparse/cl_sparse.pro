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

TEMPLATE = app #lib #app

INCLUDEPATH += /usr/local/cuda/include \

LIBS +=      -lOpenCL

#DEFINES+= CL_BLAS

#Dependencies from clBlas library
contains(DEFINES, CL_BLAS) {
#    message("Compilation with clBLAS library, make sure that clBLAS is built first")
#    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../clBLAS/bin/library/release/ -lclBLAS
#    else:unix: LIBS += -L$$PWD/../clBLAS/bin/library/ -lclBLAS

#    INCLUDEPATH += $$PWD/../clBLAS/src
#    DEPENDPATH += $$PWD/../clBLAS/src
}


SOURCES += main.cpp \
    ocl.cpp \
    kernel.cpp \
    timer.cpp \
    io/mmio.cpp \
    matrix/matrixcoo_cpu.cpp \
    matrix/matrixcoo_gpu.cpp \
    matrix/matrixell_cpu.cpp \
    matrix/matrixcsr_cpu.cpp \
    matrix/matrixcsr_gpu.cpp \
    matrix/matrixell_gpu.cpp \
    matrix/matrixhyb_cpu.cpp \
    matrix/matrixhyb_gpu.cpp \
    vector/ocl_vector.cpp \
    vector/cpu_vector.cpp \
    utils/scatter.cpp \
    iterator.cpp

HEADERS += ocl.h \
    vector.h \
    kernel.h \
    timer.h \
    types.h \
    io/mmio.h \
    matrix_types.h \
    matrix/matrixcoo.h  \
    matrix/matrixell.h  \
    matrix/matrixcsr.h  \
    matrix/matrixhyb.h  \
    vector/ocl_vector.h \
    vector/cpu_vector.h \
    matrix.h \
    utils/scatter.h \
    iterator.h

OTHER_FILES += \
    resources/brief_csr_multip.txt \
    coo_kernels/coo_matrix_krenels.cl \
    csr_kernels/csr_matrix_krenels.cl \
    vector_kernels/vector_kernels.cl \
    ell_kernels/ell_matrix_krenels.cl


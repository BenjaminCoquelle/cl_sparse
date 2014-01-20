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

TEMPLATE = lib #app #lib

INCLUDEPATH += /usr/local/cuda/include \

LIBS +=      -lOpenCL

SOURCES += main.cpp \
    ocl.cpp \
    vector_cpu.cpp \
    vector_gpu.cpp \
    vector_gpu.cpp \
    kernel.cpp \
    timer.cpp \
    matrixcoo_cpu.cpp \
    mmio.cpp \
    matrixcoo_gpu.cpp \
    matrixell_cpu.cpp \
    matrixcsr_cpu.cpp \
    matrixcsr_gpu.cpp

HEADERS += ocl.h \
    vector.h \
    kernel.h \
    timer.h \
    types.h \
    matrixcoo.h \
    matrix_types.h \
    mmio.h \
    matrixell.h \
    matrixcsr.h

OTHER_FILES += \
    resources/brief_csr_multip.txt \
    coo_kernels/coo_matrix_krenels.cl \
    csr_kernels/csr_matrix_krenels.cl \
    vector_kernels/vector_kernels.cl

#DEFINES+= CL_BLAS

#Dependencies from clBlas library

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../clBLAS/bin/library/release/ -lclBLAS
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../clBLAS/bin/library/debug/ -lclBLAS
else:symbian: LIBS += -lclBLAS
else:unix: LIBS += -L$$PWD/../clBLAS/bin/library/ -lclBLAS

INCLUDEPATH += $$PWD/../clBLAS/src
DEPENDPATH += $$PWD/../clBLAS/src

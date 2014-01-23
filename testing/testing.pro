#-------------------------------------------------
#
# Project created by QtCreator 2014-01-14T13:24:09
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = cl_sparse_tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#Dependency from SPMV_OCL
DEFINES += SRCDIR=\\\"$$PWD/\\\"
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../cl_sparse/release/ -lSPMV_OCL
else:unix: LIBS += -L$$OUT_PWD/../cl_sparse/ -lSPMV_OCL

INCLUDEPATH += $$PWD/../cl_sparse
DEPENDPATH += $$PWD/../cl_sparse


#Dependency from CL_BLAS
contains(DEFINES, CL_BLAS) { #position of this bracket is important!
message("Compilation with clBLAS library, make sure that clBLAS is built first")
#    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../clBLAS/bin/library/release/ -lclBLAS
#    else:unix: LIBS += -L$$PWD/../clBLAS/bin/library/ -lclBLAS

#    INCLUDEPATH += $$PWD/../clBLAS/src
#    DEPENDPATH += $$PWD/../clBLAS/src
}


INCLUDEPATH += /usr/local/cuda/include \
               /opt/intel/opencl-1.2-3.1.1.11385/include
LIBS +=      -L/opt/intel/opencl-1.2-3.1.1.11385/lib64 \
             -lOpenCL

SOURCES += \
    main.cpp \
    vector_test.cpp \
    vector_benchmark.cpp \
    matrix_benchmark.cpp


HEADERS += \
    vector_test.h \
    vector_benchmark.h \
    matrix_benchmark.h




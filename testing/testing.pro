#-------------------------------------------------
#
# Project created by QtCreator 2014-01-14T13:24:09
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_vector
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/local/cuda/include \
               /opt/intel/opencl-1.2-3.1.1.11385/include
LIBS +=      -L/opt/intel/opencl-1.2-3.1.1.11385/lib64 \
             -lOpenCL

SOURCES += \
    main.cpp \
    vector_test.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../cl_sparse/release/ -lSPMV_OCL
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../cl_sparse/debug/ -lSPMV_OCL
else:symbian: LIBS += -lSPMV_OCL
else:unix: LIBS += -L$$OUT_PWD/../cl_sparse/ -lSPMV_OCL

INCLUDEPATH += $$PWD/../cl_sparse
DEPENDPATH += $$PWD/../cl_sparse

HEADERS += \
    vector_test.h

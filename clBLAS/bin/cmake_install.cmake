# Install script for directory: /home/kuba/Projects/my_clMath/clsparse/clBLAS/src

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/package")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./include" TYPE FILE FILES
    "/home/kuba/Projects/my_clMath/clsparse/clBLAS/src/clBLAS.h"
    "/home/kuba/Projects/my_clMath/clsparse/clBLAS/src/clAmdBlas.h"
    "/home/kuba/Projects/my_clMath/clsparse/clBLAS/src/clAmdBlas.version.h"
    "/home/kuba/Projects/my_clMath/clsparse/clBLAS/src/clBLAS-complex.h"
    "/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/include/version.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/client/cmake_install.cmake")
  INCLUDE("/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/scripts/perf/cmake_install.cmake")
  INCLUDE("/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/cmake_install.cmake")
  INCLUDE("/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tools/tune/cmake_install.cmake")
  INCLUDE("/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tools/ktest/cmake_install.cmake")
  INCLUDE("/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples/cmake_install.cmake")
  INCLUDE("/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/tests/cmake_install.cmake")
  INCLUDE("/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/blas/gens/tests/cmake_install.cmake")
  INCLUDE("/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/blas/gens/legacy/tests/cmake_install.cmake")
  INCLUDE("/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/common/tests/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)

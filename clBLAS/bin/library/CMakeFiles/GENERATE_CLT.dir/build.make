# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kuba/Projects/my_clMath/clsparse/clBLAS/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin

# Utility rule file for GENERATE_CLT.

# Include the progress variables for this target.
include library/CMakeFiles/GENERATE_CLT.dir/progress.make

library/CMakeFiles/GENERATE_CLT:
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/library/blas/gens/clTemplates && /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen-build/tplgen -o /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/include gemm.cl gemm_helper.cl gbmv.cl ger.cl her.cl symm_helper.cl syr2_her2.cl syr_her.cl trsv.cl her2.cl symm.cl syr2.cl syr.cl trmv.cl trsv_gemv.cl swap.cl scal.cl copy.cl axpy.cl dot.cl reduction.cl rotg.cl rotmg.cl rotm.cl iamax.cl nrm2.cl asum.cl

GENERATE_CLT: library/CMakeFiles/GENERATE_CLT
GENERATE_CLT: library/CMakeFiles/GENERATE_CLT.dir/build.make
.PHONY : GENERATE_CLT

# Rule to build all files generated by this target.
library/CMakeFiles/GENERATE_CLT.dir/build: GENERATE_CLT
.PHONY : library/CMakeFiles/GENERATE_CLT.dir/build

library/CMakeFiles/GENERATE_CLT.dir/clean:
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library && $(CMAKE_COMMAND) -P CMakeFiles/GENERATE_CLT.dir/cmake_clean.cmake
.PHONY : library/CMakeFiles/GENERATE_CLT.dir/clean

library/CMakeFiles/GENERATE_CLT.dir/depend:
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kuba/Projects/my_clMath/clsparse/clBLAS/src /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/library /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/CMakeFiles/GENERATE_CLT.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : library/CMakeFiles/GENERATE_CLT.dir/depend

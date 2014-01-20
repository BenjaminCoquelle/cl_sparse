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

# Include any dependencies generated for this target.
include samples/CMakeFiles/version.dir/depend.make

# Include the progress variables for this target.
include samples/CMakeFiles/version.dir/progress.make

# Include the compile flags for this target's objects.
include samples/CMakeFiles/version.dir/flags.make

samples/CMakeFiles/version.dir/clBlasVersion.c.o: samples/CMakeFiles/version.dir/flags.make
samples/CMakeFiles/version.dir/clBlasVersion.c.o: /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/clBlasVersion.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object samples/CMakeFiles/version.dir/clBlasVersion.c.o"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/version.dir/clBlasVersion.c.o   -c /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/clBlasVersion.c

samples/CMakeFiles/version.dir/clBlasVersion.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/version.dir/clBlasVersion.c.i"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/clBlasVersion.c > CMakeFiles/version.dir/clBlasVersion.c.i

samples/CMakeFiles/version.dir/clBlasVersion.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/version.dir/clBlasVersion.c.s"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/clBlasVersion.c -o CMakeFiles/version.dir/clBlasVersion.c.s

samples/CMakeFiles/version.dir/clBlasVersion.c.o.requires:
.PHONY : samples/CMakeFiles/version.dir/clBlasVersion.c.o.requires

samples/CMakeFiles/version.dir/clBlasVersion.c.o.provides: samples/CMakeFiles/version.dir/clBlasVersion.c.o.requires
	$(MAKE) -f samples/CMakeFiles/version.dir/build.make samples/CMakeFiles/version.dir/clBlasVersion.c.o.provides.build
.PHONY : samples/CMakeFiles/version.dir/clBlasVersion.c.o.provides

samples/CMakeFiles/version.dir/clBlasVersion.c.o.provides.build: samples/CMakeFiles/version.dir/clBlasVersion.c.o

# Object files for target version
version_OBJECTS = \
"CMakeFiles/version.dir/clBlasVersion.c.o"

# External object files for target version
version_EXTERNAL_OBJECTS =

samples/version: samples/CMakeFiles/version.dir/clBlasVersion.c.o
samples/version: /usr/lib/libOpenCL.so
samples/version: library/libclBLAS.so.2.1.0
samples/version: /usr/lib/libOpenCL.so
samples/version: samples/CMakeFiles/version.dir/build.make
samples/version: samples/CMakeFiles/version.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable version"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/version.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/CMakeFiles/version.dir/build: samples/version
.PHONY : samples/CMakeFiles/version.dir/build

samples/CMakeFiles/version.dir/requires: samples/CMakeFiles/version.dir/clBlasVersion.c.o.requires
.PHONY : samples/CMakeFiles/version.dir/requires

samples/CMakeFiles/version.dir/clean:
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && $(CMAKE_COMMAND) -P CMakeFiles/version.dir/cmake_clean.cmake
.PHONY : samples/CMakeFiles/version.dir/clean

samples/CMakeFiles/version.dir/depend:
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kuba/Projects/my_clMath/clsparse/clBLAS/src /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples/CMakeFiles/version.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samples/CMakeFiles/version.dir/depend

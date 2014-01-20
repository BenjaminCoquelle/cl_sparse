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
include samples/CMakeFiles/example_strsm.dir/depend.make

# Include the progress variables for this target.
include samples/CMakeFiles/example_strsm.dir/progress.make

# Include the compile flags for this target's objects.
include samples/CMakeFiles/example_strsm.dir/flags.make

samples/CMakeFiles/example_strsm.dir/example_strsm.c.o: samples/CMakeFiles/example_strsm.dir/flags.make
samples/CMakeFiles/example_strsm.dir/example_strsm.c.o: /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/example_strsm.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object samples/CMakeFiles/example_strsm.dir/example_strsm.c.o"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/example_strsm.dir/example_strsm.c.o   -c /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/example_strsm.c

samples/CMakeFiles/example_strsm.dir/example_strsm.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/example_strsm.dir/example_strsm.c.i"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/example_strsm.c > CMakeFiles/example_strsm.dir/example_strsm.c.i

samples/CMakeFiles/example_strsm.dir/example_strsm.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/example_strsm.dir/example_strsm.c.s"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/example_strsm.c -o CMakeFiles/example_strsm.dir/example_strsm.c.s

samples/CMakeFiles/example_strsm.dir/example_strsm.c.o.requires:
.PHONY : samples/CMakeFiles/example_strsm.dir/example_strsm.c.o.requires

samples/CMakeFiles/example_strsm.dir/example_strsm.c.o.provides: samples/CMakeFiles/example_strsm.dir/example_strsm.c.o.requires
	$(MAKE) -f samples/CMakeFiles/example_strsm.dir/build.make samples/CMakeFiles/example_strsm.dir/example_strsm.c.o.provides.build
.PHONY : samples/CMakeFiles/example_strsm.dir/example_strsm.c.o.provides

samples/CMakeFiles/example_strsm.dir/example_strsm.c.o.provides.build: samples/CMakeFiles/example_strsm.dir/example_strsm.c.o

# Object files for target example_strsm
example_strsm_OBJECTS = \
"CMakeFiles/example_strsm.dir/example_strsm.c.o"

# External object files for target example_strsm
example_strsm_EXTERNAL_OBJECTS =

samples/example_strsm: samples/CMakeFiles/example_strsm.dir/example_strsm.c.o
samples/example_strsm: /usr/lib/libOpenCL.so
samples/example_strsm: library/libclBLAS.so.2.1.0
samples/example_strsm: /usr/lib/libOpenCL.so
samples/example_strsm: samples/CMakeFiles/example_strsm.dir/build.make
samples/example_strsm: samples/CMakeFiles/example_strsm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable example_strsm"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example_strsm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/CMakeFiles/example_strsm.dir/build: samples/example_strsm
.PHONY : samples/CMakeFiles/example_strsm.dir/build

samples/CMakeFiles/example_strsm.dir/requires: samples/CMakeFiles/example_strsm.dir/example_strsm.c.o.requires
.PHONY : samples/CMakeFiles/example_strsm.dir/requires

samples/CMakeFiles/example_strsm.dir/clean:
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && $(CMAKE_COMMAND) -P CMakeFiles/example_strsm.dir/cmake_clean.cmake
.PHONY : samples/CMakeFiles/example_strsm.dir/clean

samples/CMakeFiles/example_strsm.dir/depend:
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kuba/Projects/my_clMath/clsparse/clBLAS/src /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples/CMakeFiles/example_strsm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samples/CMakeFiles/example_strsm.dir/depend


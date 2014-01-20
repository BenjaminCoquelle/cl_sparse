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
include samples/CMakeFiles/example_isamax.dir/depend.make

# Include the progress variables for this target.
include samples/CMakeFiles/example_isamax.dir/progress.make

# Include the compile flags for this target's objects.
include samples/CMakeFiles/example_isamax.dir/flags.make

samples/CMakeFiles/example_isamax.dir/example_isamax.c.o: samples/CMakeFiles/example_isamax.dir/flags.make
samples/CMakeFiles/example_isamax.dir/example_isamax.c.o: /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/example_isamax.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object samples/CMakeFiles/example_isamax.dir/example_isamax.c.o"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/example_isamax.dir/example_isamax.c.o   -c /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/example_isamax.c

samples/CMakeFiles/example_isamax.dir/example_isamax.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/example_isamax.dir/example_isamax.c.i"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/example_isamax.c > CMakeFiles/example_isamax.dir/example_isamax.c.i

samples/CMakeFiles/example_isamax.dir/example_isamax.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/example_isamax.dir/example_isamax.c.s"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples/example_isamax.c -o CMakeFiles/example_isamax.dir/example_isamax.c.s

samples/CMakeFiles/example_isamax.dir/example_isamax.c.o.requires:
.PHONY : samples/CMakeFiles/example_isamax.dir/example_isamax.c.o.requires

samples/CMakeFiles/example_isamax.dir/example_isamax.c.o.provides: samples/CMakeFiles/example_isamax.dir/example_isamax.c.o.requires
	$(MAKE) -f samples/CMakeFiles/example_isamax.dir/build.make samples/CMakeFiles/example_isamax.dir/example_isamax.c.o.provides.build
.PHONY : samples/CMakeFiles/example_isamax.dir/example_isamax.c.o.provides

samples/CMakeFiles/example_isamax.dir/example_isamax.c.o.provides.build: samples/CMakeFiles/example_isamax.dir/example_isamax.c.o

# Object files for target example_isamax
example_isamax_OBJECTS = \
"CMakeFiles/example_isamax.dir/example_isamax.c.o"

# External object files for target example_isamax
example_isamax_EXTERNAL_OBJECTS =

samples/example_isamax: samples/CMakeFiles/example_isamax.dir/example_isamax.c.o
samples/example_isamax: /usr/lib/libOpenCL.so
samples/example_isamax: library/libclBLAS.so.2.1.0
samples/example_isamax: /usr/lib/libOpenCL.so
samples/example_isamax: samples/CMakeFiles/example_isamax.dir/build.make
samples/example_isamax: samples/CMakeFiles/example_isamax.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable example_isamax"
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example_isamax.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/CMakeFiles/example_isamax.dir/build: samples/example_isamax
.PHONY : samples/CMakeFiles/example_isamax.dir/build

samples/CMakeFiles/example_isamax.dir/requires: samples/CMakeFiles/example_isamax.dir/example_isamax.c.o.requires
.PHONY : samples/CMakeFiles/example_isamax.dir/requires

samples/CMakeFiles/example_isamax.dir/clean:
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples && $(CMAKE_COMMAND) -P CMakeFiles/example_isamax.dir/cmake_clean.cmake
.PHONY : samples/CMakeFiles/example_isamax.dir/clean

samples/CMakeFiles/example_isamax.dir/depend:
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kuba/Projects/my_clMath/clsparse/clBLAS/src /home/kuba/Projects/my_clMath/clsparse/clBLAS/src/samples /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/samples/CMakeFiles/example_isamax.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samples/CMakeFiles/example_isamax.dir/depend

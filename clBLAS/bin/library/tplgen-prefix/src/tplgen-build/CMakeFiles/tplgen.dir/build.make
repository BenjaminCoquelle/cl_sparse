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
CMAKE_SOURCE_DIR = /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen-build

# Include any dependencies generated for this target.
include CMakeFiles/tplgen.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tplgen.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tplgen.dir/flags.make

CMakeFiles/tplgen.dir/tplgen.cpp.o: CMakeFiles/tplgen.dir/flags.make
CMakeFiles/tplgen.dir/tplgen.cpp.o: /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen/tplgen.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen-build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/tplgen.dir/tplgen.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/tplgen.dir/tplgen.cpp.o -c /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen/tplgen.cpp

CMakeFiles/tplgen.dir/tplgen.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tplgen.dir/tplgen.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen/tplgen.cpp > CMakeFiles/tplgen.dir/tplgen.cpp.i

CMakeFiles/tplgen.dir/tplgen.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tplgen.dir/tplgen.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen/tplgen.cpp -o CMakeFiles/tplgen.dir/tplgen.cpp.s

CMakeFiles/tplgen.dir/tplgen.cpp.o.requires:
.PHONY : CMakeFiles/tplgen.dir/tplgen.cpp.o.requires

CMakeFiles/tplgen.dir/tplgen.cpp.o.provides: CMakeFiles/tplgen.dir/tplgen.cpp.o.requires
	$(MAKE) -f CMakeFiles/tplgen.dir/build.make CMakeFiles/tplgen.dir/tplgen.cpp.o.provides.build
.PHONY : CMakeFiles/tplgen.dir/tplgen.cpp.o.provides

CMakeFiles/tplgen.dir/tplgen.cpp.o.provides.build: CMakeFiles/tplgen.dir/tplgen.cpp.o

# Object files for target tplgen
tplgen_OBJECTS = \
"CMakeFiles/tplgen.dir/tplgen.cpp.o"

# External object files for target tplgen
tplgen_EXTERNAL_OBJECTS =

tplgen: CMakeFiles/tplgen.dir/tplgen.cpp.o
tplgen: CMakeFiles/tplgen.dir/build.make
tplgen: CMakeFiles/tplgen.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable tplgen"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tplgen.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tplgen.dir/build: tplgen
.PHONY : CMakeFiles/tplgen.dir/build

CMakeFiles/tplgen.dir/requires: CMakeFiles/tplgen.dir/tplgen.cpp.o.requires
.PHONY : CMakeFiles/tplgen.dir/requires

CMakeFiles/tplgen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tplgen.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tplgen.dir/clean

CMakeFiles/tplgen.dir/depend:
	cd /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen-build /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen-build /home/kuba/Projects/my_clMath/clsparse/clBLAS/bin/library/tplgen-prefix/src/tplgen-build/CMakeFiles/tplgen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tplgen.dir/depend

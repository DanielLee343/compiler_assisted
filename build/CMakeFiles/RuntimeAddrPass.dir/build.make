# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/newdrive/compiler_assisted

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/newdrive/compiler_assisted/build

# Include any dependencies generated for this target.
include CMakeFiles/RuntimeAddrPass.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/RuntimeAddrPass.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/RuntimeAddrPass.dir/flags.make

CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o: CMakeFiles/RuntimeAddrPass.dir/flags.make
CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o: ../RuntimeAddr.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/newdrive/compiler_assisted/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o -c /mnt/newdrive/compiler_assisted/RuntimeAddr.cpp

CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/newdrive/compiler_assisted/RuntimeAddr.cpp > CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.i

CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/newdrive/compiler_assisted/RuntimeAddr.cpp -o CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.s

# Object files for target RuntimeAddrPass
RuntimeAddrPass_OBJECTS = \
"CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o"

# External object files for target RuntimeAddrPass
RuntimeAddrPass_EXTERNAL_OBJECTS =

RuntimeAddrPass.so: CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o
RuntimeAddrPass.so: CMakeFiles/RuntimeAddrPass.dir/build.make
RuntimeAddrPass.so: /usr/lib/llvm-18/lib/libLLVM-18.so.1
RuntimeAddrPass.so: CMakeFiles/RuntimeAddrPass.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/newdrive/compiler_assisted/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module RuntimeAddrPass.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RuntimeAddrPass.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/RuntimeAddrPass.dir/build: RuntimeAddrPass.so

.PHONY : CMakeFiles/RuntimeAddrPass.dir/build

CMakeFiles/RuntimeAddrPass.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/RuntimeAddrPass.dir/cmake_clean.cmake
.PHONY : CMakeFiles/RuntimeAddrPass.dir/clean

CMakeFiles/RuntimeAddrPass.dir/depend:
	cd /mnt/newdrive/compiler_assisted/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/newdrive/compiler_assisted /mnt/newdrive/compiler_assisted /mnt/newdrive/compiler_assisted/build /mnt/newdrive/compiler_assisted/build /mnt/newdrive/compiler_assisted/build/CMakeFiles/RuntimeAddrPass.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/RuntimeAddrPass.dir/depend


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
CMAKE_SOURCE_DIR = /home/cc/compiler_assisted

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cc/compiler_assisted/build

# Include any dependencies generated for this target.
include passes/CMakeFiles/RuntimeAddrPass.dir/depend.make

# Include the progress variables for this target.
include passes/CMakeFiles/RuntimeAddrPass.dir/progress.make

# Include the compile flags for this target's objects.
include passes/CMakeFiles/RuntimeAddrPass.dir/flags.make

passes/CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o: passes/CMakeFiles/RuntimeAddrPass.dir/flags.make
passes/CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o: ../passes/RuntimeAddr.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cc/compiler_assisted/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object passes/CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o"
	cd /home/cc/compiler_assisted/build/passes && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o -c /home/cc/compiler_assisted/passes/RuntimeAddr.cpp

passes/CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.i"
	cd /home/cc/compiler_assisted/build/passes && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cc/compiler_assisted/passes/RuntimeAddr.cpp > CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.i

passes/CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.s"
	cd /home/cc/compiler_assisted/build/passes && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cc/compiler_assisted/passes/RuntimeAddr.cpp -o CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.s

# Object files for target RuntimeAddrPass
RuntimeAddrPass_OBJECTS = \
"CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o"

# External object files for target RuntimeAddrPass
RuntimeAddrPass_EXTERNAL_OBJECTS =

passes/RuntimeAddrPass.so: passes/CMakeFiles/RuntimeAddrPass.dir/RuntimeAddr.cpp.o
passes/RuntimeAddrPass.so: passes/CMakeFiles/RuntimeAddrPass.dir/build.make
passes/RuntimeAddrPass.so: /usr/lib/llvm-17/lib/libLLVM-17.so.1
passes/RuntimeAddrPass.so: passes/CMakeFiles/RuntimeAddrPass.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/cc/compiler_assisted/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module RuntimeAddrPass.so"
	cd /home/cc/compiler_assisted/build/passes && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RuntimeAddrPass.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
passes/CMakeFiles/RuntimeAddrPass.dir/build: passes/RuntimeAddrPass.so

.PHONY : passes/CMakeFiles/RuntimeAddrPass.dir/build

passes/CMakeFiles/RuntimeAddrPass.dir/clean:
	cd /home/cc/compiler_assisted/build/passes && $(CMAKE_COMMAND) -P CMakeFiles/RuntimeAddrPass.dir/cmake_clean.cmake
.PHONY : passes/CMakeFiles/RuntimeAddrPass.dir/clean

passes/CMakeFiles/RuntimeAddrPass.dir/depend:
	cd /home/cc/compiler_assisted/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cc/compiler_assisted /home/cc/compiler_assisted/passes /home/cc/compiler_assisted/build /home/cc/compiler_assisted/build/passes /home/cc/compiler_assisted/build/passes/CMakeFiles/RuntimeAddrPass.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : passes/CMakeFiles/RuntimeAddrPass.dir/depend


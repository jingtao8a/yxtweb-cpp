# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yuxintao/yxtweb-cpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yuxintao/yxtweb-cpp/build

# Include any dependencies generated for this target.
include CMakeFiles/test_httpserver.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_httpserver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_httpserver.dir/flags.make

CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.o: CMakeFiles/test_httpserver.dir/flags.make
CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.o: ../tests/test_httpserver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yuxintao/yxtweb-cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.o -c /home/yuxintao/yxtweb-cpp/tests/test_httpserver.cpp

CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yuxintao/yxtweb-cpp/tests/test_httpserver.cpp > CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.i

CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yuxintao/yxtweb-cpp/tests/test_httpserver.cpp -o CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.s

# Object files for target test_httpserver
test_httpserver_OBJECTS = \
"CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.o"

# External object files for target test_httpserver
test_httpserver_EXTERNAL_OBJECTS =

../bin/test_httpserver: CMakeFiles/test_httpserver.dir/tests/test_httpserver.cpp.o
../bin/test_httpserver: CMakeFiles/test_httpserver.dir/build.make
../bin/test_httpserver: ../lib/libyxtwebcpp.so
../bin/test_httpserver: CMakeFiles/test_httpserver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yuxintao/yxtweb-cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/test_httpserver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_httpserver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_httpserver.dir/build: ../bin/test_httpserver

.PHONY : CMakeFiles/test_httpserver.dir/build

CMakeFiles/test_httpserver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_httpserver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_httpserver.dir/clean

CMakeFiles/test_httpserver.dir/depend:
	cd /home/yuxintao/yxtweb-cpp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yuxintao/yxtweb-cpp /home/yuxintao/yxtweb-cpp /home/yuxintao/yxtweb-cpp/build /home/yuxintao/yxtweb-cpp/build /home/yuxintao/yxtweb-cpp/build/CMakeFiles/test_httpserver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_httpserver.dir/depend

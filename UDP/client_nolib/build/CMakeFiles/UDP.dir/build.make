# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib/build

# Include any dependencies generated for this target.
include CMakeFiles/UDP.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/UDP.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/UDP.dir/flags.make

CMakeFiles/UDP.dir/demo_01_client.cpp.o: CMakeFiles/UDP.dir/flags.make
CMakeFiles/UDP.dir/demo_01_client.cpp.o: ../demo_01_client.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/topeet/vscode/github/Linux-applictions/UDP/client_nolib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/UDP.dir/demo_01_client.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/UDP.dir/demo_01_client.cpp.o -c /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib/demo_01_client.cpp

CMakeFiles/UDP.dir/demo_01_client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/UDP.dir/demo_01_client.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib/demo_01_client.cpp > CMakeFiles/UDP.dir/demo_01_client.cpp.i

CMakeFiles/UDP.dir/demo_01_client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/UDP.dir/demo_01_client.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib/demo_01_client.cpp -o CMakeFiles/UDP.dir/demo_01_client.cpp.s

CMakeFiles/UDP.dir/demo_01_client.cpp.o.requires:

.PHONY : CMakeFiles/UDP.dir/demo_01_client.cpp.o.requires

CMakeFiles/UDP.dir/demo_01_client.cpp.o.provides: CMakeFiles/UDP.dir/demo_01_client.cpp.o.requires
	$(MAKE) -f CMakeFiles/UDP.dir/build.make CMakeFiles/UDP.dir/demo_01_client.cpp.o.provides.build
.PHONY : CMakeFiles/UDP.dir/demo_01_client.cpp.o.provides

CMakeFiles/UDP.dir/demo_01_client.cpp.o.provides.build: CMakeFiles/UDP.dir/demo_01_client.cpp.o


# Object files for target UDP
UDP_OBJECTS = \
"CMakeFiles/UDP.dir/demo_01_client.cpp.o"

# External object files for target UDP
UDP_EXTERNAL_OBJECTS =

UDP: CMakeFiles/UDP.dir/demo_01_client.cpp.o
UDP: CMakeFiles/UDP.dir/build.make
UDP: CMakeFiles/UDP.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/topeet/vscode/github/Linux-applictions/UDP/client_nolib/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable UDP"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/UDP.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/UDP.dir/build: UDP

.PHONY : CMakeFiles/UDP.dir/build

CMakeFiles/UDP.dir/requires: CMakeFiles/UDP.dir/demo_01_client.cpp.o.requires

.PHONY : CMakeFiles/UDP.dir/requires

CMakeFiles/UDP.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/UDP.dir/cmake_clean.cmake
.PHONY : CMakeFiles/UDP.dir/clean

CMakeFiles/UDP.dir/depend:
	cd /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib/build /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib/build /home/topeet/vscode/github/Linux-applictions/UDP/client_nolib/build/CMakeFiles/UDP.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/UDP.dir/depend

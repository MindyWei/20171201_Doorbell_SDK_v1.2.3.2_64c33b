# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.1

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
CMAKE_COMMAND = D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/tool/bin/cmake.exe

# The command to remove a file.
RM = D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/tool/bin/cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer

# Include any dependencies generated for this target.
include openrtos/malloc/CMakeFiles/malloc.dir/depend.make

# Include the progress variables for this target.
include openrtos/malloc/CMakeFiles/malloc.dir/progress.make

# Include the compile flags for this target's objects.
include openrtos/malloc/CMakeFiles/malloc.dir/flags.make

openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj: openrtos/malloc/CMakeFiles/malloc.dir/flags.make
openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj: ../../../openrtos/malloc/calloc.c
	$(CMAKE_COMMAND) -E cmake_progress_report D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/malloc.dir/calloc.c.obj   -c D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/calloc.c

openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/malloc.dir/calloc.c.i"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -E D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/calloc.c > CMakeFiles/malloc.dir/calloc.c.i

openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/malloc.dir/calloc.c.s"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -S D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/calloc.c -o CMakeFiles/malloc.dir/calloc.c.s

openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj.requires:
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj.requires

openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj.provides: openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj.requires
	$(MAKE) -f openrtos/malloc/CMakeFiles/malloc.dir/build.make openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj.provides.build
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj.provides

openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj.provides.build: openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj

openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj: openrtos/malloc/CMakeFiles/malloc.dir/flags.make
openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj: ../../../openrtos/malloc/malign.c
	$(CMAKE_COMMAND) -E cmake_progress_report D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/malloc.dir/malign.c.obj   -c D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/malign.c

openrtos/malloc/CMakeFiles/malloc.dir/malign.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/malloc.dir/malign.c.i"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -E D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/malign.c > CMakeFiles/malloc.dir/malign.c.i

openrtos/malloc/CMakeFiles/malloc.dir/malign.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/malloc.dir/malign.c.s"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -S D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/malign.c -o CMakeFiles/malloc.dir/malign.c.s

openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj.requires:
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj.requires

openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj.provides: openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj.requires
	$(MAKE) -f openrtos/malloc/CMakeFiles/malloc.dir/build.make openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj.provides.build
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj.provides

openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj.provides.build: openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj

openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj: openrtos/malloc/CMakeFiles/malloc.dir/flags.make
openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj: ../../../openrtos/malloc/malloc.c
	$(CMAKE_COMMAND) -E cmake_progress_report D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/malloc.dir/malloc.c.obj   -c D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/malloc.c

openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/malloc.dir/malloc.c.i"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -E D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/malloc.c > CMakeFiles/malloc.dir/malloc.c.i

openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/malloc.dir/malloc.c.s"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -S D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/malloc.c -o CMakeFiles/malloc.dir/malloc.c.s

openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj.requires:
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj.requires

openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj.provides: openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj.requires
	$(MAKE) -f openrtos/malloc/CMakeFiles/malloc.dir/build.make openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj.provides.build
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj.provides

openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj.provides.build: openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj

openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj: openrtos/malloc/CMakeFiles/malloc.dir/flags.make
openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj: ../../../openrtos/malloc/mallocr.c
	$(CMAKE_COMMAND) -E cmake_progress_report D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/malloc.dir/mallocr.c.obj   -c D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/mallocr.c

openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/malloc.dir/mallocr.c.i"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -E D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/mallocr.c > CMakeFiles/malloc.dir/mallocr.c.i

openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/malloc.dir/mallocr.c.s"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -S D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/mallocr.c -o CMakeFiles/malloc.dir/mallocr.c.s

openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj.requires:
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj.requires

openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj.provides: openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj.requires
	$(MAKE) -f openrtos/malloc/CMakeFiles/malloc.dir/build.make openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj.provides.build
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj.provides

openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj.provides.build: openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj

openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj: openrtos/malloc/CMakeFiles/malloc.dir/flags.make
openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj: ../../../openrtos/malloc/msize.c
	$(CMAKE_COMMAND) -E cmake_progress_report D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/malloc.dir/msize.c.obj   -c D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/msize.c

openrtos/malloc/CMakeFiles/malloc.dir/msize.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/malloc.dir/msize.c.i"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -E D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/msize.c > CMakeFiles/malloc.dir/msize.c.i

openrtos/malloc/CMakeFiles/malloc.dir/msize.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/malloc.dir/msize.c.s"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -S D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/msize.c -o CMakeFiles/malloc.dir/msize.c.s

openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj.requires:
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj.requires

openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj.provides: openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj.requires
	$(MAKE) -f openrtos/malloc/CMakeFiles/malloc.dir/build.make openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj.provides.build
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj.provides

openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj.provides.build: openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj

openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj: openrtos/malloc/CMakeFiles/malloc.dir/flags.make
openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj: ../../../openrtos/malloc/mstats.c
	$(CMAKE_COMMAND) -E cmake_progress_report D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/malloc.dir/mstats.c.obj   -c D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/mstats.c

openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/malloc.dir/mstats.c.i"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -E D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/mstats.c > CMakeFiles/malloc.dir/mstats.c.i

openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/malloc.dir/mstats.c.s"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -S D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/mstats.c -o CMakeFiles/malloc.dir/mstats.c.s

openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj.requires:
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj.requires

openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj.provides: openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj.requires
	$(MAKE) -f openrtos/malloc/CMakeFiles/malloc.dir/build.make openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj.provides.build
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj.provides

openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj.provides.build: openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj

openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj: openrtos/malloc/CMakeFiles/malloc.dir/flags.make
openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj: ../../../openrtos/malloc/mtrim.c
	$(CMAKE_COMMAND) -E cmake_progress_report D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/malloc.dir/mtrim.c.obj   -c D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/mtrim.c

openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/malloc.dir/mtrim.c.i"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -E D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/mtrim.c > CMakeFiles/malloc.dir/mtrim.c.i

openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/malloc.dir/mtrim.c.s"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -S D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/mtrim.c -o CMakeFiles/malloc.dir/mtrim.c.s

openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj.requires:
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj.requires

openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj.provides: openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj.requires
	$(MAKE) -f openrtos/malloc/CMakeFiles/malloc.dir/build.make openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj.provides.build
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj.provides

openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj.provides.build: openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj

openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj: openrtos/malloc/CMakeFiles/malloc.dir/flags.make
openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj: ../../../openrtos/malloc/realloc.c
	$(CMAKE_COMMAND) -E cmake_progress_report D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/malloc.dir/realloc.c.obj   -c D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/realloc.c

openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/malloc.dir/realloc.c.i"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -E D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/realloc.c > CMakeFiles/malloc.dir/realloc.c.i

openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/malloc.dir/realloc.c.s"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -S D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/realloc.c -o CMakeFiles/malloc.dir/realloc.c.s

openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj.requires:
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj.requires

openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj.provides: openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj.requires
	$(MAKE) -f openrtos/malloc/CMakeFiles/malloc.dir/build.make openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj.provides.build
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj.provides

openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj.provides.build: openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj

openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj: openrtos/malloc/CMakeFiles/malloc.dir/flags.make
openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj: ../../../openrtos/malloc/valloc.c
	$(CMAKE_COMMAND) -E cmake_progress_report D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/CMakeFiles $(CMAKE_PROGRESS_9)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/malloc.dir/valloc.c.obj   -c D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/valloc.c

openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/malloc.dir/valloc.c.i"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -E D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/valloc.c > CMakeFiles/malloc.dir/valloc.c.i

openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/malloc.dir/valloc.c.s"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && C:/ITEGCC/bin/arm-none-eabi-gcc.exe  $(C_DEFINES) $(C_FLAGS) -S D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc/valloc.c -o CMakeFiles/malloc.dir/valloc.c.s

openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj.requires:
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj.requires

openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj.provides: openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj.requires
	$(MAKE) -f openrtos/malloc/CMakeFiles/malloc.dir/build.make openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj.provides.build
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj.provides

openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj.provides.build: openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj

# Object files for target malloc
malloc_OBJECTS = \
"CMakeFiles/malloc.dir/calloc.c.obj" \
"CMakeFiles/malloc.dir/malign.c.obj" \
"CMakeFiles/malloc.dir/malloc.c.obj" \
"CMakeFiles/malloc.dir/mallocr.c.obj" \
"CMakeFiles/malloc.dir/msize.c.obj" \
"CMakeFiles/malloc.dir/mstats.c.obj" \
"CMakeFiles/malloc.dir/mtrim.c.obj" \
"CMakeFiles/malloc.dir/realloc.c.obj" \
"CMakeFiles/malloc.dir/valloc.c.obj"

# External object files for target malloc
malloc_EXTERNAL_OBJECTS =

lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj
lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj
lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj
lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj
lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj
lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj
lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj
lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj
lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj
lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/build.make
lib/fa626/libmalloc.a: openrtos/malloc/CMakeFiles/malloc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C static library ../../lib/fa626/libmalloc.a"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && $(CMAKE_COMMAND) -P CMakeFiles/malloc.dir/cmake_clean_target.cmake
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/malloc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
openrtos/malloc/CMakeFiles/malloc.dir/build: lib/fa626/libmalloc.a
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/build

openrtos/malloc/CMakeFiles/malloc.dir/requires: openrtos/malloc/CMakeFiles/malloc.dir/calloc.c.obj.requires
openrtos/malloc/CMakeFiles/malloc.dir/requires: openrtos/malloc/CMakeFiles/malloc.dir/malign.c.obj.requires
openrtos/malloc/CMakeFiles/malloc.dir/requires: openrtos/malloc/CMakeFiles/malloc.dir/malloc.c.obj.requires
openrtos/malloc/CMakeFiles/malloc.dir/requires: openrtos/malloc/CMakeFiles/malloc.dir/mallocr.c.obj.requires
openrtos/malloc/CMakeFiles/malloc.dir/requires: openrtos/malloc/CMakeFiles/malloc.dir/msize.c.obj.requires
openrtos/malloc/CMakeFiles/malloc.dir/requires: openrtos/malloc/CMakeFiles/malloc.dir/mstats.c.obj.requires
openrtos/malloc/CMakeFiles/malloc.dir/requires: openrtos/malloc/CMakeFiles/malloc.dir/mtrim.c.obj.requires
openrtos/malloc/CMakeFiles/malloc.dir/requires: openrtos/malloc/CMakeFiles/malloc.dir/realloc.c.obj.requires
openrtos/malloc/CMakeFiles/malloc.dir/requires: openrtos/malloc/CMakeFiles/malloc.dir/valloc.c.obj.requires
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/requires

openrtos/malloc/CMakeFiles/malloc.dir/clean:
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc && $(CMAKE_COMMAND) -P CMakeFiles/malloc.dir/cmake_clean.cmake
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/clean

openrtos/malloc/CMakeFiles/malloc.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/malloc D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/malloc/CMakeFiles/malloc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : openrtos/malloc/CMakeFiles/malloc.dir/depend


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

# Utility rule file for tlb_wt.o.

# Include the progress variables for this target.
include openrtos/boot/CMakeFiles/tlb_wt.o.dir/progress.make

openrtos/boot/CMakeFiles/tlb_wt.o: lib/fa626/tlb_wt.o

lib/fa626/tlb_wt.o: ../../../openrtos/boot/fa626/tlb.c
	$(CMAKE_COMMAND) -E cmake_progress_report D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating ../../lib/fa626/tlb_wt.o"
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/boot && C:/ITEGCC/bin/arm-none-eabi-gcc.exe -DCFG_CHIP_REV_A1 -DCFG_CHIP_PKG_IT9854 -DCFG_OSC_CLK=12000000 -DCFG_RTC_EXTCLK=12000000 -DCFG_CHIP_FAMILY=9850 -DCFG_RAM_SIZE=0x4000000 -DCFG_MEMDBG_ENABLE -DCFG_CPU_WB -DCFG_WT_SIZE=0 -DCFG_LCD_ENABLE -DCFG_LCD_WIDTH=800 -DCFG_LCD_HEIGHT=480 -DCFG_LCD_PITCH=1600 -DCFG_LCD_BPP=2 -DCFG_LCD_TRIPLE_BUFFER -DCFG_BACKLIGHT_ENABLE -DCFG_BACKLIGHT_FREQ=1000 "-DCFG_BACKLIGHT_DUTY_CYCLES=95, 92, 88, 82, 75, 65, 55, 40, 25, 5" -DCFG_BACKLIGHT_DEFAULT_DUTY_CYCLE=5 -DCFG_BACKLIGHT_BOOTLODER_DELAY=200 -DCFG_CMDQ_SIZE=0 -DCFG_FONT_FILENAME="WenQuanYiMicroHeiMono.ttf" -DCFG_PRIVATE_DRIVE="A" -DCFG_PUBLIC_DRIVE="B" -DCFG_TEMP_DRIVE="C" -DCFG_RTC_DEFAULT_TIMESTAMP=1325376000 -DCFG_SPI0_ENABLE -DCFG_SPI0_MOSI_GPIO=18 -DCFG_SPI0_MISO_GPIO=19 -DCFG_SPI0_CLOCK_GPIO=20 -DCFG_SPI0_CHIP_SEL_GPIO=14 -DCFG_SPI_ENABLE -DCFG_UART1_ENABLE -DCFG_UART1_BAUDRATE=115200 -DCFG_UART_INTR -DCFG_UART_BUF_SIZE=16384 -DCFG_GPIO_BACKLIGHT_PWM=64 -DCFG_GPIO_UART1_TX=4 -DCFG_GPIO_UART1_RX=14 -DCFG_GPIO_ETHERNET_LINK=-1 -DCFG_NET_ETHERNET_PHY_ADDR=-1 -DCFG_NET_ETHERNET_COUNT=1 -DCFG_DBG_PRINTBUF_SIZE=0 -DCFG_DBG_UART1 -DCFG_UPGRADE_FILENAME="ITEPKG03.PKG" -DCFG_UPGRADE_FILENAME_LIST="ITEPKG03.PKG" -DCFG_UPGRADE_ENC_KEY=0 -DCFG_NORMAL_BUFFER_MODE -DCFG_BUILD_RELEASE -DCFG_VERSION_MAJOR=1 -DCFG_VERSION_MINOR=2 -DCFG_VERSION_PATCH=3 -DCFG_VERSION_CUSTOM=2 -DCFG_VERSION_TWEAK= -DCFG_VERSION_MAJOR_STR="1" -DCFG_VERSION_MINOR_STR="2" -DCFG_VERSION_PATCH_STR="3" -DCFG_VERSION_CUSTOM_STR="2" -DCFG_VERSION_TWEAK_STR="" -DCFG_SYSTEM_NAME="ITE Castor3" -DCFG_MANUFACTURER="www.ite.com.tw" -DCFG_PROJECT_NAME="test_timer" -D__DYNAMIC_REENT__ -D__OPENRTOS__=1 -D_POSIX_BARRIERS -D_POSIX_C_SOURCE=200809L -D_POSIX_MONOTONIC_CLOCK -D_POSIX_PRIORITY_SCHEDULING=1 -D_POSIX_THREADS -D_POSIX_TIMERS -D_UNIX98_THREAD_MUTEX_ATTRIBUTES -DOSIP_MT -DCFG_GCC_LTO -DMALLOC_ALIGNMENT=32 -DCFG_OPENRTOS_HEAP_SIZE=0 -DCFG_MMAP_SIZE=0 -DCFG_LCDC_HEIGHT=480 -DCFG_LCDC_PITCH=1600 -DFORCE_CPU_WRITE_THROUGH -c -o D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/lib/fa626/tlb_wt.o D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/boot/fa626/tlb.c

tlb_wt.o: openrtos/boot/CMakeFiles/tlb_wt.o
tlb_wt.o: lib/fa626/tlb_wt.o
tlb_wt.o: openrtos/boot/CMakeFiles/tlb_wt.o.dir/build.make
.PHONY : tlb_wt.o

# Rule to build all files generated by this target.
openrtos/boot/CMakeFiles/tlb_wt.o.dir/build: tlb_wt.o
.PHONY : openrtos/boot/CMakeFiles/tlb_wt.o.dir/build

openrtos/boot/CMakeFiles/tlb_wt.o.dir/clean:
	cd D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/boot && $(CMAKE_COMMAND) -P CMakeFiles/tlb_wt.o.dir/cmake_clean.cmake
.PHONY : openrtos/boot/CMakeFiles/tlb_wt.o.dir/clean

openrtos/boot/CMakeFiles/tlb_wt.o.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/openrtos/boot D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/boot D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/boot/CMakeFiles/tlb_wt.o.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : openrtos/boot/CMakeFiles/tlb_wt.o.dir/depend


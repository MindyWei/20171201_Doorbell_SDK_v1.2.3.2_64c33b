# Install script for directory: D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/test_timer")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/sdk/cmake_install.cmake")
  include("D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/openrtos/cmake_install.cmake")
  include("D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/project/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

file(WRITE "D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/${CMAKE_INSTALL_MANIFEST}" "")
foreach(file ${CMAKE_INSTALL_MANIFEST_FILES})
  file(APPEND "D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
endforeach()

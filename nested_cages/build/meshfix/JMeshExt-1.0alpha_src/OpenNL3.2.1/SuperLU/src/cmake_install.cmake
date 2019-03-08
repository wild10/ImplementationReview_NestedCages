# Install script for directory: /home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/ewilderd/nested_cages/build/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/binaries/lib/libsuperlu.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/superlu" TYPE FILE FILES
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/colamd.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/html_mainpage.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/slu_cdefs.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/slu_Cnames.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/slu_dcomplex.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/slu_ddefs.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/slu_scomplex.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/slu_sdefs.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/slu_util.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/slu_zdefs.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/superlu_enum_consts.h"
    "/home/ewilderd/nested_cages/meshfix/JMeshExt-1.0alpha_src/OpenNL3.2.1/SuperLU/src/supermatrix.h"
    )
endif()


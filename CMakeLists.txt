# This file is part of the rc_genicam_api package.
#
# Copyright (c) 2017 Roboception GmbH
# All rights reserved
#
# Author: Heiko Hirschmueller
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required (VERSION 2.8.12)

project(rc_genicam_api)
include(cmake/project_version.cmake)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# - Configuration for code optimization -

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE CACHE STRING "Build type: DEBUG or RELEASE" FORCE)
endif ()

# - Standard definitions -
set(rc_genicam_api_COMPILE_CXX_OPTIONS)
list(APPEND rc_genicam_api_COMPILE_CXX_OPTIONS -Wall)
if (CMAKE_MAJOR_VERSION VERSION_LESS "3.1.0")
list(APPEND rc_genicam_api_COMPILE_CXX_OPTIONS -std=c++11)
else ()
  set(CMAKE_CXX_STANDARD 11)
endif ()

if (CMAKE_COMPILER_IS_GNUCC)
  list(APPEND rc_genicam_api_COMPILE_CXX_OPTIONS -Wno-unknown-pragmas)
endif ()

# In case of Visual Studio, enable exporting all symbols and disable some
# warnings

if (WIN32)
  list(APPEND rc_genicam_api_COMPILE_CXX_OPTIONS /DGENICAM_NO_AUTO_IMPLIB)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

if (MSVC)
  list(APPEND rc_genicam_api_COMPILE_CXX_OPTIONS "/wd4003" 
  "/wd4514" 
  "/wd4710"
  "/wd4820"
  "/wd4435"
  "/wd4668"
  "/wd4127"
  "/wd4265"
  "/wd4996"
  "/wd4437"
  "/wd4571"
  "/wd4355"
  "/wd4061"
  "/wd4800"
  "/wd4711"
  "/wd4774"
  "/wd4625"
  "/wd4626"
  "/wd5026"
  "/wd5027"
  "/wd5039")
endif ()

# - Options -

option(BUILD_TOOLS "Build commandline tools" ON)
option(BUILD_DOC "Add target for building doxygen docs" ON)
option(BUILD_SHARED_LIBS "Build shared libs" ON)
option(INSTALL_COMPLETION "Install bash completion" OFF)

# - Build individual parts -
find_package(Genicam REQUIRED NO_CMAKE_PATH PATHS ${CMAKE_CURRENT_SOURCE_DIR}/genicam CONFIG NO_DEFAULT_PATH)
add_subdirectory(baumer)
add_subdirectory(genicam)

add_subdirectory(rc_genicam_api)
if (BUILD_TOOLS)
  add_subdirectory(tools)
endif()
if (INSTALL_COMPLETION)
  add_subdirectory(completion)
endif ()

# export project targets

# Declare (shared) libraries that are created by your project. For all
# libraries that appear here, the following line should exist in a cmake
# file in a sub directory:
# install(TARGETS rcXXX EXPORT PROJECTTargets COMPONENT bin DESTINATION lib)
if (BUILD_SHARED_LIBS)
  set(PROJECT_LIBRARIES rc_genicam_api)
endif ()

# Declare static libraries that are created by your project. For all
# libraries that appear here, the following line should exist in a cmake
# file in a sub directory:
# install(TARGETS rcXXX_static EXPORT PROJECTTargets COMPONENT dev DESTINATION lib)
#set(PROJECT_STATIC_LIBRARIES rc_genicam_api_static)

include(cmake/configure_link_libs.cmake)

# add a target to generate API documentation with Doxygen
if (BUILD_DOC)
  add_subdirectory(doc)
endif()

# Install catkin package.xml
install(FILES package.xml COMPONENT dev DESTINATION share/${PROJECT_NAME_LOWER})
install(FILES net_perf_check.sh COMPONENT dev DESTINATION share/${PROJECT_NAME_LOWER}
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ
                    WORLD_EXECUTE WORLD_READ)

# - Define information for packaging -

# optionally set description and maintainer (if not set will use defaults from package_debian.cmake)
#set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Roboception package description")

# optionally set package maintainer (otherwise defaults to Roboception <support@roboception.de>)
#set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.name@roboception.de>")

# optionally specify dependencies of the debian package here (comma separated!)
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "")

include(cmake/package_debian.cmake)

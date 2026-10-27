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

cmake_minimum_required (VERSION 3.16)

project(rc_genicam_api)

include(cmake/install_dirs.cmake)
include(cmake/project_name.cmake)
include(cmake/project_version.cmake)

# For CMake Version >= 3.0: issue FATAL_ERROR if link dependency contains
# double-colons but is not an imported target.
if (POLICY CMP0028)
  cmake_policy(SET CMP0028 NEW)
endif ()

enable_testing()

# - Configuration for code optimization -

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE CACHE STRING "Build type: DEBUG or RELEASE" FORCE)
endif ()

# - Standard definitions -
set(CMAKE_CXX_STANDARD 11)
if (WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

# - Options -

option(BUILD_TOOLS "Build commandline tools" ON)
option(BUILD_DOC "Add target for building doxygen docs" ON)
option(BUILD_SHARED_LIBS "Build shared libs" ON)
option(INSTALL_COMPLETION "Install bash completion" OFF)

find_package(PNG)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)

# - Build individual parts -
find_package(Genicam REQUIRED NO_CMAKE_PATH PATHS ${CMAKE_CURRENT_SOURCE_DIR}/genicam CONFIG NO_DEFAULT_PATH)
add_library(${PROJECT_NAMESPACE}::genicam ALIAS genicam)
install(TARGETS genicam
    EXPORT PROJECTTargets
    COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory(baumer)
add_subdirectory(genicam)

add_subdirectory(rc_genicam_api)
add_subdirectory(examples)
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

# optionally specify dependencies of the debian package here (comma separated!)
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "")

# LICENSE.md also contains notes about third party software, which are
# described in copyright_extra for the copyright file of the package

file(READ "${PROJECT_SOURCE_DIR}/LICENSE.md" RC_LICENSE_MD)
string(FIND "${RC_LICENSE_MD}" "Third party software" RC_LICENSE_END)
string(SUBSTRING "${RC_LICENSE_MD}" 0 ${RC_LICENSE_END} RC_LICENSE_MD)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/LICENSE" "${RC_LICENSE_MD}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/LICENSE.md")

set(RC_PACKAGE_LICENSE "BSD-3-Clause")
set(RC_PACKAGE_LICENSE_FILE "${CMAKE_CURRENT_BINARY_DIR}/LICENSE")
set(RC_COPYRIGHT_EXTRA_FILE "${PROJECT_SOURCE_DIR}/copyright_extra")

include(cmake/package_debian.cmake)

# The GenICam license requires distributing License_ReadMe.txt and the
# licenses of the third party software in GenICam. They are installed next to
# the copyright file of the package, which needs CPACK_PACKAGE_NAME.

install(DIRECTORY genicam/licenses/ COMPONENT bin
  DESTINATION /usr/share/doc/${CPACK_PACKAGE_NAME}/GenICam)
install(FILES genicam/License_ReadMe.txt COMPONENT bin
  DESTINATION /usr/share/doc/${CPACK_PACKAGE_NAME}/GenICam)

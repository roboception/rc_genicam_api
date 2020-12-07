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

project(tools CXX)

# build programs

add_executable(gc_info gc_info.cc)
target_link_libraries(gc_info
  PRIVATE
      ${PROJECT_NAMESPACE}::rc_genicam_api_static)
target_compile_options(gc_info PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

add_executable(gc_config gc_config.cc)
target_link_libraries(gc_config
  PRIVATE
    ${PROJECT_NAMESPACE}::rc_genicam_api_static)
target_compile_options(gc_config PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

add_executable(gc_stream gc_stream.cc)
target_link_libraries(gc_stream
  PRIVATE
    ${PROJECT_NAMESPACE}::rc_genicam_api_static)
target_compile_options(gc_stream PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

if (UNIX)
  find_package(Threads REQUIRED)
  target_link_libraries(gc_stream
    PRIVATE
      ${CMAKE_THREAD_LIBS_INIT})
endif ()

add_executable(gc_pointcloud gc_pointcloud.cc)
target_link_libraries(gc_pointcloud
  PRIVATE
    ${PROJECT_NAMESPACE}::rc_genicam_api_static)
target_compile_options(gc_pointcloud PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

add_executable(gc_file gc_file.cc)
target_link_libraries(gc_file
  PRIVATE
      ${PROJECT_NAMESPACE}::rc_genicam_api_static)
target_compile_options(gc_file PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

# install tools

install(TARGETS gc_info
    EXPORT PROJECTTargets
    COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS gc_config
    EXPORT PROJECTTargets
    COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS gc_stream
    EXPORT PROJECTTargets
    COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS gc_pointcloud
    EXPORT PROJECTTargets
    COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS gc_file
    EXPORT PROJECTTargets
    COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

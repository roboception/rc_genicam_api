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

project(rc_genicam_api CXX)

set(src
  exception.cc
  system.cc
  interface.cc
  device.cc
  stream.cc
  cport.cc
  buffer.cc
  config.cc
  image.cc
  imagelist.cc
  image_store.cc
  pointcloud.cc
  ${CMAKE_CURRENT_BINARY_DIR}/project_version.cc
  $<$<PLATFORM_ID:Linux>:gentl_wrapper_linux.cc>
  $<$<PLATFORM_ID:Windows>:gentl_wrapper_win32.cc>)

set(hh
  exception.h
  system.h
  interface.h
  device.h
  stream.h
  buffer.h
  config.h
  image.h
  imagelist.h
  image_store.h
  pointcloud.h
  pixel_formats.h
  ${CMAKE_CURRENT_BINARY_DIR}/project_version.h)

list(APPEND MSVC_DISABLED_WARNINGS
    "/wd4003"
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
    "/wd4267"
    "/wd5039")

if (UNIX)
  set(GENTL_INSTALL_PATH "${CMAKE_INSTALL_FULL_LIBDIR}/${PROJECT_NAME_LOWER}")
elseif (WIN32)
  set(GENTL_INSTALL_PATH "${CMAKE_INSTALL_FULL_BINDIR}")
endif ()

if (PNG_FOUND)
  include_directories(${PNG_INCLUDE_DIRS})
  add_definitions(-DINCLUDE_PNG)
  add_definitions(${PNG_DEFINITIONS})
endif ()

add_library(rc_genicam_api_private_properties INTERFACE)
add_library(${PROJECT_NAMESPACE}::rc_genicam_api_private_properties ALIAS
    rc_genicam_api_private_properties)

target_link_libraries(rc_genicam_api_private_properties
  INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:dl>)
target_compile_options(rc_genicam_api_private_properties
  INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wno-unknown-pragmas>
    $<$<CXX_COMPILER_ID:MSVC>:${MSVC_DISABLED_WARNINGS}>)
target_compile_definitions(rc_genicam_api_private_properties
  INTERFACE
    -DGENTL_INSTALL_PATH=\"${GENTL_INSTALL_PATH}\")

add_library(rc_genicam_api_static STATIC ${src})
add_library(${PROJECT_NAMESPACE}::rc_genicam_api_static ALIAS rc_genicam_api_static)

target_include_directories(rc_genicam_api_static
  PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
target_link_libraries(rc_genicam_api_static
  PUBLIC
    ${PROJECT_NAMESPACE}::genicam
    ${PNG_LIBRARIES}
  PRIVATE
    ${PROJECT_NAMESPACE}::rc_genicam_api_private_properties)
target_compile_options(rc_genicam_api_static
  PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/DGENICAM_NO_AUTO_IMPLIB>)

if (BUILD_SHARED_LIBS)
  add_library(rc_genicam_api SHARED ${src})
  add_library(${PROJECT_NAMESPACE}::rc_genicam_api ALIAS rc_genicam_api)

  target_include_directories(rc_genicam_api
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
      $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
  target_link_libraries(rc_genicam_api
    PUBLIC
      ${PROJECT_NAMESPACE}::genicam
    PRIVATE
      ${PNG_LIBRARIES}
      ${PROJECT_NAMESPACE}::rc_genicam_api_private_properties)
  target_compile_options(rc_genicam_api
    PUBLIC
      $<$<CXX_COMPILER_ID:MSVC>:/DGENICAM_NO_AUTO_IMPLIB>)

  set_target_properties(rc_genicam_api PROPERTIES SOVERSION ${abiversion})

  install(TARGETS rc_genicam_api
      EXPORT PROJECTTargets
      COMPONENT bin
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

# only install headers if we build and install the shared lib
if (BUILD_SHARED_LIBS)
  install(FILES ${hh} COMPONENT dev DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rc_genicam_api)
endif ()

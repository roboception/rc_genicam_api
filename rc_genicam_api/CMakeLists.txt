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

string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)
add_definitions("-DGENTL_INSTALL_PATH=\"${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME_LOWER}\"")

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
  ${CMAKE_CURRENT_BINARY_DIR}/project_version.cc
)

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
  pixel_formats.h
  ${CMAKE_CURRENT_BINARY_DIR}/project_version.h
)

if (UNIX)
  set(src ${src} gentl_wrapper_linux.cc)
endif ()

add_library(rc_genicam_api_static STATIC ${src})
target_link_libraries(rc_genicam_api_static ${GENICAM_LIBRARIES})
if (UNIX)
  target_link_libraries(rc_genicam_api_static dl)
endif ()

#install(TARGETS rc_genicam_api_static EXPORT PROJECTTargets COMPONENT dev DESTINATION lib)

if (BUILD_SHARED_LIBS)
  add_library(rc_genicam_api SHARED ${src})
  target_link_libraries(rc_genicam_api LINK_PRIVATE ${GENICAM_LIBRARIES})
  if (UNIX)
    target_link_libraries(rc_genicam_api LINK_PRIVATE dl)
  endif ()
  set_target_properties(rc_genicam_api PROPERTIES SOVERSION ${abiversion})

  install(TARGETS rc_genicam_api EXPORT PROJECTTargets COMPONENT bin DESTINATION lib)
endif ()

# only install headers if we build and install the shared lib
if (BUILD_SHARED_LIBS)
  install(FILES ${hh} COMPONENT dev DESTINATION include/rc_genicam_api)
endif ()

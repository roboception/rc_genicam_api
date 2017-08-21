# Author: Heiko Hirschmueller

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
  pfnc.h
)

if (UNIX)
  set(src ${src} gentl_wrapper_linux.cc)
endif ()

add_library(rc_genicam_api_static STATIC ${src})
target_link_libraries(rc_genicam_api_static ${GENICAM_LIBRARIES})

add_library(rc_genicam_api SHARED ${src})
target_link_libraries(rc_genicam_api LINK_PRIVATE ${GENICAM_LIBRARIES})

if (UNIX)
target_link_libraries(rc_genicam_api_static dl)
target_link_libraries(rc_genicam_api LINK_PRIVATE dl)
endif ()

install(TARGETS rc_genicam_api EXPORT PROJECTTargets COMPONENT bin DESTINATION lib)
set_target_properties(rc_genicam_api PROPERTIES SOVERSION ${abiversion})
install(TARGETS rc_genicam_api_static EXPORT PROJECTTargets COMPONENT dev DESTINATION lib)
install(FILES ${hh} COMPONENT dev DESTINATION include/rc_genicam_api)
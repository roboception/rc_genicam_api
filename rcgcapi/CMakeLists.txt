# Author: Heiko Hirschmueller

project(rcgcapi CXX)

set(src
  exception.cc
  system.cc
  interface.cc
  device.cc
  stream.cc
  cport.cc
  buffer.cc
  config.cc
)

set(hh
  exception.h
  system.h
  interface.h
  device.h
  stream.h
  buffer.h
  config.h
)

if (UNIX)
  set(src ${src} gentl_wrapper_linux.cc)
endif ()

add_library(rcgcapi_static STATIC ${src})
target_link_libraries(rcgcapi_static ${GENICAM_LIBRARIES})

add_library(rcgcapi SHARED ${src})
target_link_libraries(rcgcapi LINK_PRIVATE ${GENICAM_LIBRARIES})

if (UNIX)
target_link_libraries(rcgcapi_static dl)
target_link_libraries(rcgcapi LINK_PRIVATE dl)
endif ()

install(TARGETS rcgcapi EXPORT PROJECTTargets COMPONENT bin DESTINATION lib)
set_target_properties(rcgcapi PROPERTIES SOVERSION ${abiversion})
install(TARGETS rcgcapi_static EXPORT PROJECTTargets COMPONENT dev DESTINATION lib)
install(FILES ${hh} COMPONENT dev DESTINATION include/rcgcapi)
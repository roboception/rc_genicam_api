project(genicam CXX)

EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message(STATUS "Detected architecture ${ARCHITECTURE}")

if (${ARCHITECTURE} STREQUAL "armv7l")
  set(GENICAM_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libGCBase_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libMathParser_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libGenApi_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libNodeMapData_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/liblog4cpp_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libXmlParser_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libLog_gcc46_v3_0.so
  )
elseif (${ARCHITECTURE} STREQUAL "i686")
  set(GENICAM_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_i86/libGCBase_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_i86/libMathParser_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_i86/libGenApi_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_i86/libNodeMapData_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_i86/liblog4cpp_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_i86/libXmlParser_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_i86/libLog_gcc421_v3_0.so
  )
elseif (${ARCHITECTURE} STREQUAL "x86_64")
  set(GENICAM_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libGCBase_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libMathParser_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libGenApi_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libNodeMapData_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/liblog4cpp_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libXmlParser_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libLog_gcc421_v3_0.so
  )
else ()
  message(FATAL_ERROR "Unknown architecture")
endif ()

set(GENICAM_LIBRARIES ${GENICAM_LIBRARIES} PARENT_SCOPE)

install(FILES ${GENICAM_LIBRARIES} COMPONENT bin DESTINATION lib)

# only install headers if we build and install the shared lib
if (BUILD_SHARED_LIBS)
  install(DIRECTORY library/CPP/include/ COMPONENT dev DESTINATION include/rc_genicam_api/genicam)
endif ()

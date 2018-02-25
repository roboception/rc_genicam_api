project(genicam CXX)

if (UNIX)
  # try to get architecture from compiler
  EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine COMMAND tr -d '\n' OUTPUT_VARIABLE CXX_MACHINE)
  string(REGEX REPLACE "([a-zA-Z_0-9]+).*" "\\1" ARCHITECTURE ${CXX_MACHINE})
elseif (WIN32)
  if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(ARCHITECTURE WIN32_i86)
  else ()
    set(ARCHITECTURE WIN64_x64)
  endif ()
endif ()

message(STATUS "Detected architecture ${ARCHITECTURE}")

if ("${ARCHITECTURE}" STREQUAL "arm")
  set(GENICAM_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libGCBase_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libMathParser_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libGenApi_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libNodeMapData_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/liblog4cpp_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libXmlParser_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf/libLog_gcc46_v3_0.so
  )
elseif ("${ARCHITECTURE}" STREQUAL "aarch64")
  set(GENICAM_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_ARM/libGCBase_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_ARM/libMathParser_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_ARM/libGenApi_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_ARM/libNodeMapData_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_ARM/liblog4cpp_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_ARM/libXmlParser_gcc46_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_ARM/libLog_gcc46_v3_0.so
  )
elseif ("${ARCHITECTURE}" STREQUAL "i686")
  set(GENICAM_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_i86/libGCBase_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_i86/libMathParser_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_i86/libGenApi_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_i86/libNodeMapData_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_i86/liblog4cpp_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_i86/libXmlParser_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_i86/libLog_gcc421_v3_0.so
  )
elseif ("${ARCHITECTURE}" STREQUAL "x86_64")
  set(GENICAM_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libGCBase_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libMathParser_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libGenApi_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libNodeMapData_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/liblog4cpp_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libXmlParser_gcc421_v3_0.so
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64/libLog_gcc421_v3_0.so
  )
elseif ("${ARCHITECTURE}" STREQUAL "WIN32_i86")
  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/GCBase_MD_VC120_v3_0.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/GenApi_MD_VC120_v3_0.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/Log_MD_VC120_v3_0.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/MathParser_MD_VC120_v3_0.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/msvcp120.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/msvcr120.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/NodeMapData_MD_VC120_v3_0.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/XmlParser_MD_VC120_v3_0.dll
	COMPONENT bin DESTINATION bin
  )
  set(GENICAM_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/CPP/lib/Win32_i86/GCBase_MD_VC120_v3_0.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/library/CPP/lib/Win32_i86/GenApi_MD_VC120_v3_0.lib
  )
elseif ("${ARCHITECTURE}" STREQUAL "WIN64_x64")
  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/GCBase_MD_VC120_v3_0.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/GenApi_MD_VC120_v3_0.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/Log_MD_VC120_v3_0.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/MathParser_MD_VC120_v3_0.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/msvcp120.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/msvcr120.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/NodeMapData_MD_VC120_v3_0.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/XmlParser_MD_VC120_v3_0.dll
	COMPONENT bin DESTINATION bin
  )
  set(GENICAM_LIBRARIES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/CPP/lib/Win64_x64/GCBase_MD_VC120_v3_0.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/library/CPP/lib/Win64_x64/GenApi_MD_VC120_v3_0.lib
  )
else ()
  message(FATAL_ERROR "Unknown architecture")
endif ()

install(FILES ${GENICAM_LIBRARIES} COMPONENT bin DESTINATION lib)

set(GENICAM_LIBRARIES ${GENICAM_LIBRARIES} PARENT_SCOPE)

# only install headers if we build and install the shared lib
if (BUILD_SHARED_LIBS)
  install(DIRECTORY library/CPP/include/ COMPONENT dev DESTINATION include/rc_genicam_api/genicam)
endif ()

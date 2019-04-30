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

set(GENICAM_LIB_SUFFIX)

if ("${ARCHITECTURE}" STREQUAL "arm")
  set(GENICAM_LIBRARIES
    libGCBase_gcc46_v3_1
    libMathParser_gcc46_v3_1
    libGenApi_gcc46_v3_1
    libNodeMapData_gcc46_v3_1
    liblog4cpp_gcc46_v3_1
    libXmlParser_gcc46_v3_1
    libLog_gcc46_v3_1
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "aarch64")
  set(GENICAM_LIBRARIES
    libGCBase_gcc48_v3_1
    libMathParser_gcc48_v3_1
    libGenApi_gcc48_v3_1
    libNodeMapData_gcc48_v3_1
    liblog4cpp_gcc48_v3_1
    libXmlParser_gcc48_v3_1
    libLog_gcc48_v3_1
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_ARM)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "i686")
  set(GENICAM_LIBRARIES
    libGCBase_gcc42_v3_1
    libMathParser_gcc42_v3_1
    libGenApi_gcc42_v3_1
    libNodeMapData_gcc42_v3_1
    liblog4cpp_gcc42_v3_1
    libXmlParser_gcc42_v3_1
    libLog_gcc42_v3_1
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_i86)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "x86_64")
  set(GENICAM_LIBRARIES
    libGCBase_gcc42_v3_1
    libMathParser_gcc42_v3_1
    libGenApi_gcc42_v3_1
    libNodeMapData_gcc42_v3_1
    liblog4cpp_gcc42_v3_1
    libXmlParser_gcc42_v3_1
    libLog_gcc42_v3_1
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "WIN32_i86")
  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/GCBase_MD_VC120_v3_1.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/GenApi_MD_VC120_v3_1.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/Log_MD_VC120_v3_1.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/MathParser_MD_VC120_v3_1.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/msvcp120.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/msvcr120.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/NodeMapData_MD_VC120_v3_1.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/XmlParser_MD_VC120_v3_1.dll
	COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
  set(GENICAM_LIBRARIES
    GCBase_MD_VC120_v3_1
    GenApi_MD_VC120_v3_1
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/library/CPP/lib/Win32_i86)
  set(GENICAM_LIB_SUFFIX ".lib")
elseif ("${ARCHITECTURE}" STREQUAL "WIN64_x64")
  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/GCBase_MD_VC120_v3_1.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/GenApi_MD_VC120_v3_1.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/Log_MD_VC120_v3_1.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/MathParser_MD_VC120_v3_1.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/msvcp120.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/msvcr120.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/NodeMapData_MD_VC120_v3_1.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/XmlParser_MD_VC120_v3_1.dll
	COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
  set(GENICAM_LIBRARIES
    GCBase_MD_VC120_v3_1
    GenApi_MD_VC120_v3_1
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/library/CPP/lib/Win64_x64)
  set(GENICAM_LIB_SUFFIX ".lib")
else ()
  message(FATAL_ERROR "Unknown architecture")
endif ()

set(GENICAM_LIBRARIES_IN_BUILD_DIR)
foreach(GENICAM_LIB ${GENICAM_LIBRARIES})
  set(GENICAM_LIB_FILE "${GENICAM_LIBRARIES_DIR}/${GENICAM_LIB}${GENICAM_LIB_SUFFIX}")
  list(APPEND GENICAM_LIBRARIES_IN_BUILD_DIR "${GENICAM_LIB_FILE}")
endforeach()

install(FILES ${GENICAM_LIBRARIES_IN_BUILD_DIR} COMPONENT bin DESTINATION ${GENICAM_INSTALL_LIBDIRS})

# only install headers if we build and install the shared lib
if (BUILD_SHARED_LIBS)
  install(DIRECTORY library/CPP/include/ COMPONENT dev DESTINATION ${GENICAM_INSTALL_INCLUDEDIRS})
endif ()

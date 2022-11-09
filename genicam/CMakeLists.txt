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
    libGCBase_gcc494_v3_4
    libGenApi_gcc494_v3_4
    liblog4cpp_gcc494_v3_4
    libLog_gcc494_v3_4
    libMathParser_gcc494_v3_4
    libNodeMapData_gcc494_v3_4
    libXmlParser_gcc494_v3_4
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "aarch64")
  set(GENICAM_LIBRARIES
    libGCBase_gcc49_v3_4
    libGenApi_gcc49_v3_4
    liblog4cpp_gcc49_v3_4
    libLog_gcc49_v3_4
    libMathParser_gcc49_v3_4
    libNodeMapData_gcc49_v3_4
    libXmlParser_gcc49_v3_4
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_ARM)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "i686")
  set(GENICAM_LIBRARIES
    libGCBase_gcc48_v3_4
    libGenApi_gcc48_v3_4
    liblog4cpp_gcc48_v3_4
    libLog_gcc48_v3_4
    libMathParser_gcc48_v3_4
    libNodeMapData_gcc48_v3_4
    libXmlParser_gcc48_v3_4
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_i86)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "x86_64")
  set(GENICAM_LIBRARIES
    libGCBase_gcc48_v3_4
    libGenApi_gcc48_v3_4
    liblog4cpp_gcc48_v3_4
    libLog_gcc48_v3_4
    libMathParser_gcc48_v3_4
    libNodeMapData_gcc48_v3_4
    libXmlParser_gcc48_v3_4
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "WIN32_i86")
  install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win32_i86/
	COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
  set(GENICAM_LIBRARIES
    GCBase_MD_VC141_v3_4
    GenApi_MD_VC141_v3_4
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/library/CPP/lib/Win32_i86)
  set(GENICAM_LIB_SUFFIX ".lib")
elseif ("${ARCHITECTURE}" STREQUAL "WIN64_x64")
  install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64_x64/
	COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
  set(GENICAM_LIBRARIES
    GCBase_MD_VC141_v3_4
    GenApi_MD_VC141_v3_4
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
install(DIRECTORY licenses/ COMPONENT bin DESTINATION ${CMAKE_INSTALL_DOCDIR}/GenICam/)

# only install headers if we build and install the shared lib
if (BUILD_SHARED_LIBS)
  install(DIRECTORY library/CPP/include/ COMPONENT dev DESTINATION ${GENICAM_INSTALL_INCLUDEDIRS})
endif ()

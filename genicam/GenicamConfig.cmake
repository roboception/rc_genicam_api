# Config file for Genicam
#
# It defines the requires genicam libraries as imported targets and defines:
# - Genicam_TARGETS: list of all imported target with their prefix
# - Genicam_LIBRARIES: list of libraries

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

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
    libXmlParser_gcc494_v3_4)
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/bin/Linux32_ARMhf)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "aarch64")
  set(GENICAM_LIBRARIES
    libGCBase_gcc49_v3_4
    libGenApi_gcc49_v3_4
    liblog4cpp_gcc49_v3_4
    libLog_gcc49_v3_4
    libMathParser_gcc49_v3_4
    libNodeMapData_gcc49_v3_4
    libXmlParser_gcc49_v3_4)
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/bin/Linux64_ARM)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "i686")
  set(GENICAM_LIBRARIES
    libGCBase_gcc48_v3_4
    libGenApi_gcc48_v3_4
    liblog4cpp_gcc48_v3_4
    libLog_gcc48_v3_4
    libMathParser_gcc48_v3_4
    libNodeMapData_gcc48_v3_4
    libXmlParser_gcc48_v3_4)
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/bin/Linux32_i86)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "x86_64")
  set(GENICAM_LIBRARIES
    libGCBase_gcc48_v3_4
    libGenApi_gcc48_v3_4
    liblog4cpp_gcc48_v3_4
    libLog_gcc48_v3_4
    libMathParser_gcc48_v3_4
    libNodeMapData_gcc48_v3_4
    libXmlParser_gcc48_v3_4)
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/bin/Linux64_x64)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "WIN32_i86")
  set(GENICAM_LIBRARIES
    GCBase_MD_VC141_v3_4
    GenApi_MD_VC141_v3_4)
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/library/CPP/lib/Win32_i86)
  set(GENICAM_LIB_SUFFIX ".lib")
elseif ("${ARCHITECTURE}" STREQUAL "WIN64_x64")
  set(GENICAM_LIBRARIES
    GCBase_MD_VC141_v3_4
    GenApi_MD_VC141_v3_4)
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/library/CPP/lib/Win64_x64)
  set(GENICAM_LIB_SUFFIX ".lib")
else ()
  message(FATAL_ERROR "Unknown architecture")
endif ()

set(GENICAM_NAMESPACED_TARGETS)
foreach(GENICAM_LIB ${GENICAM_LIBRARIES})
  set(GENICAM_NAMESPACE_LIB "genicam::${GENICAM_LIB}")
  list(APPEND GENICAM_NAMESPACED_TARGETS ${GENICAM_NAMESPACE_LIB})
  add_library(${GENICAM_NAMESPACE_LIB} SHARED IMPORTED)
  message(STATUS "Adding imported: ${GENICAM_NAMESPACE_LIB}")
  set_property(TARGET ${GENICAM_NAMESPACE_LIB} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
  set_property(TARGET ${GENICAM_NAMESPACE_LIB} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)

  set(GENICAM_LIB_FILE "${GENICAM_LIBRARIES_DIR}/${GENICAM_LIB}${GENICAM_LIB_SUFFIX}")
  if (UNIX)
	  set_target_properties(${GENICAM_NAMESPACE_LIB}
	      PROPERTIES
			  INTERFACE_INCLUDE_DIRECTORIES "${PACKAGE_PREFIX_DIR}/library/CPP/include/"
		      IMPORTED_LOCATION_DEBUG "${GENICAM_LIB_FILE}"
		      IMPORTED_LOCATION_RELEASE "${GENICAM_LIB_FILE}"
		      IMPORTED_LOCATION "${GENICAM_LIB_FILE}")
  elseif (WIN32)
	  set_target_properties(${GENICAM_NAMESPACE_LIB}
	      PROPERTIES
			  INTERFACE_INCLUDE_DIRECTORIES "${PACKAGE_PREFIX_DIR}/library/CPP/include/"
		      IMPORTED_IMPLIB_DEBUG  "${GENICAM_LIB_FILE}"
		      IMPORTED_IMPLIB_RELEASE "${GENICAM_LIB_FILE}"
		      IMPORTED_IMPLIB "${GENICAM_LIB_FILE}")
  endif ()
endforeach()

add_library(genicam INTERFACE)
add_library(genicam::genicam ALIAS genicam)
target_link_libraries(genicam INTERFACE ${GENICAM_NAMESPACED_TARGETS})

set(Genicam_LIBRARIES ${GENICAM_LIBRARIES} CACHE STRING "Genicam libraries")
set(Genicam_TARGETS ${GENICAM_NAMESPACED_TARGETS} CACHE STRING "Genicam targets with namespace")
set(Genicam_FOUND ON CACHE BOOL "")
set(PACKAGE_PREFIX_DIR)

# Config file for Genicam
#
# It defines the requires genicam libraries as imported targets and defines:
# - Genicam_LIBRARIES: list of all imported target with their prefix

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
    libGCBase_gcc46_v3_1
    libMathParser_gcc46_v3_1
    libGenApi_gcc46_v3_1
    libNodeMapData_gcc46_v3_1
    liblog4cpp_gcc46_v3_1
    libXmlParser_gcc46_v3_1
    libLog_gcc46_v3_1
  )
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/bin/Linux32_ARMhf)
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
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/bin/Linux64_ARM)
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
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/bin/Linux32_i86)
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
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/bin/Linux64_x64)
  set(GENICAM_LIB_SUFFIX ".so")
elseif ("${ARCHITECTURE}" STREQUAL "WIN32_i86")
  install(FILES
    ${PACKAGE_PREFIX_DIR}/bin/Win32_i86/GCBase_MD_VC120_v3_1.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win32_i86/GenApi_MD_VC120_v3_1.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win32_i86/Log_MD_VC120_v3_1.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win32_i86/MathParser_MD_VC120_v3_1.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win32_i86/msvcp120.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win32_i86/msvcr120.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win32_i86/NodeMapData_MD_VC120_v3_1.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win32_i86/XmlParser_MD_VC120_v3_1.dll
	COMPONENT bin DESTINATION bin
  )
  set(GENICAM_LIBRARIES
    GCBase_MD_VC120_v3_1
    GenApi_MD_VC120_v3_1
  )
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/library/CPP/lib/Win32_i86)
  set(GENICAM_LIB_SUFFIX ".lib")
elseif ("${ARCHITECTURE}" STREQUAL "WIN64_x64")
  install(FILES
    ${PACKAGE_PREFIX_DIR}/bin/Win64_x64/GCBase_MD_VC120_v3_1.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win64_x64/GenApi_MD_VC120_v3_1.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win64_x64/Log_MD_VC120_v3_1.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win64_x64/MathParser_MD_VC120_v3_1.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win64_x64/msvcp120.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win64_x64/msvcr120.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win64_x64/NodeMapData_MD_VC120_v3_1.dll
    ${PACKAGE_PREFIX_DIR}/bin/Win64_x64/XmlParser_MD_VC120_v3_1.dll
	COMPONENT bin DESTINATION bin
  )
  set(GENICAM_LIBRARIES
    GCBase_MD_VC120_v3_1
    GenApi_MD_VC120_v3_1
  )
  set(GENICAM_LIBRARIES_DIR ${PACKAGE_PREFIX_DIR}/library/CPP/lib/Win64_x64)
  set(GENICAM_LIB_SUFFIX ".lib")
else ()
  message(FATAL_ERROR "Unknown architecture")
endif ()

set(GENICAM_NAMESPACE_LIBS)
foreach(GENICAM_LIB ${GENICAM_LIBRARIES})
  set(GENICAM_NAMESPACE_LIB "Genicam::${GENICAM_LIB}")
  list(APPEND GENICAM_NAMESPACE_LIBS ${GENICAM_NAMESPACE_LIB})
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
		      IMPORTED_LOCATION "${GENICAM_LIB_FILE}"
	  )
  elseif (WIN32)
	  set_target_properties(${GENICAM_NAMESPACE_LIB}
	      PROPERTIES
			  INTERFACE_INCLUDE_DIRECTORIES "${PACKAGE_PREFIX_DIR}/library/CPP/include/"
		      IMPORTED_IMPLIB_DEBUG  "${GENICAM_LIB_FILE}"
		      IMPORTED_IMPLIB_RELEASE "${GENICAM_LIB_FILE}"
		      IMPORTED_IMPLIB "${GENICAM_LIB_FILE}"
	  )
  endif ()
endforeach()


set(Genicam_LIBRARIES ${GENICAM_NAMESPACE_LIBS} CACHE STRING "Genicam targets with namespace")
set(Genicam_FOUND ON CACHE BOOL "")
set(PACKAGE_PREFIX_DIR)

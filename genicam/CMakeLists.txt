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
    libGCBase_gcc46_v3_1.so
    libMathParser_gcc46_v3_1.so
    libGenApi_gcc46_v3_1.so
    libNodeMapData_gcc46_v3_1.so
    liblog4cpp_gcc46_v3_1.so
    libXmlParser_gcc46_v3_1.so
    libLog_gcc46_v3_1.so
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_ARMhf)
elseif ("${ARCHITECTURE}" STREQUAL "aarch64")
  set(GENICAM_LIBRARIES
    libGCBase_gcc48_v3_1.so
    libMathParser_gcc48_v3_1.so
    libGenApi_gcc48_v3_1.so
    libNodeMapData_gcc48_v3_1.so
    liblog4cpp_gcc48_v3_1.so
    libXmlParser_gcc48_v3_1.so
    libLog_gcc48_v3_1.so
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_ARM)
elseif ("${ARCHITECTURE}" STREQUAL "i686")
  set(GENICAM_LIBRARIES
    libGCBase_gcc42_v3_1.so
    libMathParser_gcc42_v3_1.so
    libGenApi_gcc42_v3_1.so
    libNodeMapData_gcc42_v3_1.so
    liblog4cpp_gcc42_v3_1.so
    libXmlParser_gcc42_v3_1.so
    libLog_gcc42_v3_1.so
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux32_i86)
elseif ("${ARCHITECTURE}" STREQUAL "x86_64")
  set(GENICAM_LIBRARIES
    libGCBase_gcc42_v3_1.so
    libMathParser_gcc42_v3_1.so
    libGenApi_gcc42_v3_1.so
    libNodeMapData_gcc42_v3_1.so
    liblog4cpp_gcc42_v3_1.so
    libXmlParser_gcc42_v3_1.so
    libLog_gcc42_v3_1.so
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux64_x64)
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
	COMPONENT bin DESTINATION bin
  )
  set(GENICAM_LIBRARIES
    GCBase_MD_VC120_v3_1.lib
    GenApi_MD_VC120_v3_1.lib
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/library/CPP/lib/Win32_i86)
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
	COMPONENT bin DESTINATION bin
  )
  set(GENICAM_LIBRARIES
    GCBase_MD_VC120_v3_1.lib
    GenApi_MD_VC120_v3_1.lib
  )
  set(GENICAM_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/library/CPP/lib/Win64_x64)
else ()
  message(FATAL_ERROR "Unknown architecture")
endif ()

function(PREPEND var prefix)
  set(listVar "")
  foreach(f ${ARGN})
    list(APPEND listVar "${prefix}/${f}")
  endforeach(f)
  set(${var} "${listVar}" PARENT_SCOPE)
endfunction(PREPEND)

prepend(FILES_TO_INSTALL ${GENICAM_LIBRARIES_DIR} ${GENICAM_LIBRARIES})

install(FILES ${FILES_TO_INSTALL} COMPONENT bin DESTINATION lib)

set(GENICAM_LIBRARIES ${GENICAM_LIBRARIES} PARENT_SCOPE)
set(GENICAM_LIBRARIES_DIR ${GENICAM_LIBRARIES_DIR} PARENT_SCOPE)

# only install headers if we build and install the shared lib
if (BUILD_SHARED_LIBS)
  install(DIRECTORY library/CPP/include/ COMPONENT dev DESTINATION include/rc_genicam_api/genicam)
endif ()

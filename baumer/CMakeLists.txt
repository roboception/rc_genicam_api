
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

install(DIRECTORY DESTINATION ${CMAKE_INSTALL_LIBDIR}/rc_genicam_api
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

if ("${ARCHITECTURE}" STREQUAL "arm")
  install(
    FILES
      Ubuntu-14.04/armv7l/libbgapi2_gige.cti
      Ubuntu-14.04/armv7l/libbgapi2_usb.cti
      bsysgige.xml
      bsysusb.xml
      license.txt
    COMPONENT bin
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/rc_genicam_api
  )
elseif ("${ARCHITECTURE}" STREQUAL "aarch64")
  install(
    FILES
      Ubuntu-16.04/arm64/libbgapi2_gige.cti
      Ubuntu-16.04/arm64/libbgapi2_usb.cti
      bsysgige.xml
      bsysusb.xml
      license.txt
    COMPONENT bin
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/rc_genicam_api
  )
elseif ("${ARCHITECTURE}" STREQUAL "x86_64")
  install(
    FILES
      Ubuntu-16.04/x86_64/libbgapi2_gige.cti
      Ubuntu-16.04/x86_64/libbgapi2_usb.cti
      bsysgige.xml
      bsysusb.xml
      license.txt
    COMPONENT bin
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/rc_genicam_api
  )
elseif ("${ARCHITECTURE}" STREQUAL "i686")
  install(
    FILES
      Ubuntu-16.04/i686/libbgapi2_gige.cti
      Ubuntu-16.04/i686/libbgapi2_usb.cti
      bsysgige.xml
      bsysusb.xml
      license.txt
    COMPONENT bin
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/rc_genicam_api
  )
elseif ("${ARCHITECTURE}" STREQUAL "WIN32_i86")
  install(
    FILES
      Win32_i86/bgapi2_gige.cti
      Win32_i86/bgapi2_usb.cti
      bsysgige.xml
      bsysusb.xml
      license.txt
    COMPONENT bin
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
elseif ("${ARCHITECTURE}" STREQUAL "WIN64_x64")
  install(
    FILES
      Win64_x64/bgapi2_gige.cti
      Win64_x64/bgapi2_usb.cti
      bsysgige.xml
      bsysusb.xml
      license.txt
    COMPONENT bin
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
else ()
  message(FATAL_ERROR "Unknown architecture")
endif ()

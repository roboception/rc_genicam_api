# Installs the prebuilt GenTL producer rc_gev_producer as the default transport
# layer. The files of this directory come unmodified from the redistributable
# bundle of rc_gev_producer, see update.sh.
#
# The producer is installed into the directory that the library uses as
# GENTL_INSTALL_PATH if GENICAM_GENTL64_PATH is not set. Its license is
# installed next to it, since the producer is not open source.

if (UNIX)
  # try to get architecture from compiler
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine COMMAND tr -d '\n' OUTPUT_VARIABLE CXX_MACHINE)
  string(REGEX REPLACE "([a-zA-Z_0-9]+).*" "\\1" ARCHITECTURE ${CXX_MACHINE})
  set(GEV_PRODUCER_PLATFORM linux/${ARCHITECTURE})
  set(GEV_PRODUCER_DESTINATION ${CMAKE_INSTALL_LIBDIR}/rc_genicam_api)
elseif (WIN32)
  if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set(GEV_PRODUCER_PLATFORM windows/x64)
  else ()
    set(GEV_PRODUCER_PLATFORM windows/x86)
  endif ()
  set(GEV_PRODUCER_DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

install(DIRECTORY DESTINATION ${GEV_PRODUCER_DESTINATION}
        DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

# The library and the tools are still useful without a default producer, e.g.
# with one of a camera vendor in GENICAM_GENTL64_PATH, so a missing platform
# is no error.

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${GEV_PRODUCER_PLATFORM}/rc_gev_producer.cti)
  message(WARNING "There is no prebuilt rc_gev_producer for ${GEV_PRODUCER_PLATFORM}. "
    "A GenTL producer must be provided through GENICAM_GENTL64_PATH.")
  return()
endif ()

message(STATUS "Installing prebuilt rc_gev_producer for ${GEV_PRODUCER_PLATFORM}")

install(
  FILES ${GEV_PRODUCER_PLATFORM}/rc_gev_producer.cti
  COMPONENT bin
  DESTINATION ${GEV_PRODUCER_DESTINATION}
)

install(
  FILES LICENSE.txt
  COMPONENT bin
  DESTINATION ${GEV_PRODUCER_DESTINATION}
  RENAME rc_gev_producer_license.txt
)

# Only the Windows version contains third party code that requires its
# license texts to be shipped with the binary. Under Linux, the copyright file
# of the debian package covers the producer.

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${GEV_PRODUCER_PLATFORM}/licenses)
  install(
    DIRECTORY ${GEV_PRODUCER_PLATFORM}/licenses/
    COMPONENT bin
    DESTINATION ${GEV_PRODUCER_DESTINATION}/rc_gev_producer_licenses
  )
endif ()

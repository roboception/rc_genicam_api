# This cmake code sets the install directories as shown in the GNUInstallDirs
# documentation.
#
# E.g., the following variables are set:
#
# - CMAKE_INSTALL_INCLUDEDIR
# - CMAKE_INSTALL_LIBDIR


# XXX workaround to make sure the behavior of GNUInstallDirs of CMake 2.8.12
# matches the one of CMake > 3.0
#
# GNUInstallDirs of CMake > 3.0 sets CMAKE_INSTALL_LIBDIR to /usr/lib/<arch>
# only if CMAKE_INSTALL_PREFIX matches "^/usr/?$".
# GNUInstallDirs of CMake 2.8.12 sets CMAKE_INSTALL_LIBDIR always to
# /usr/lib/<arch> as long as CMAKE_LIBRARY_ARCHITECTURE is set.
#
# Therefore, unset CMAKE_LIBRARY_ARCHITECTURE if CMAKE_INSTALL_LIBDIR does
# NOT match above regex.
set(_CMAKE_LIBRARY_ARCHITECTURE ${CMAKE_LIBRARY_ARCHITECTURE})
if(NOT "${CMAKE_INSTALL_PREFIX}" MATCHES "^/usr/?$")
  unset(CMAKE_LIBRARY_ARCHITECTURE)
endif ()

include(GNUInstallDirs)

set(CMAKE_LIBRARY_ARCHITECTURE ${_CMAKE_LIBRARY_ARCHITECTURE})

set(GENICAM_INSTALL_LIBDIRS "${CMAKE_INSTALL_LIBDIR}")
set(GENICAM_INSTALL_INCLUDEDIRS "${CMAKE_INSTALL_INCLUDEDIR}/rc_genicam_api/genicam")

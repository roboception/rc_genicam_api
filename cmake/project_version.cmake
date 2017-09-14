#
# Sets PROJECT_VERSION and PACKAGE_VERSION
#

# Don't set PROJECT_VERSION to empty string when no VERSION is given to project() command.
if(POLICY CMP0048)
  cmake_policy(SET CMP0048 OLD)
endif()

# Split a version number into separate components
# version the version number to split
# major variable name to store the major version in
# minor variable name to store the minor version in
# patch variable name to store the patch version in
# extra variable name to store a version suffix in
function(version_split version major minor patch extra)
    string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)(.*)?" version_valid ${version})
    if(version_valid)
        string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)(.*)?" "\\1;\\2;\\3;\\4" VERSION_MATCHES ${version})
        list(GET VERSION_MATCHES 0 version_major)
        set(${major} ${version_major} PARENT_SCOPE)
        list(GET VERSION_MATCHES 1 version_minor)
        set(${minor} ${version_minor} PARENT_SCOPE)
        list(GET VERSION_MATCHES 2 version_patch)
        set(${patch} ${version_patch} PARENT_SCOPE)
        list(GET VERSION_MATCHES 3 version_extra)
        set(${extra} ${version_extra} PARENT_SCOPE)
    else(version_valid)
        message(AUTHOR_WARNING "Bad version ${version}; falling back to 0 (have you made an initial release?)")
        set(${major} "0" PARENT_SCOPE)
        set(${minor} "0" PARENT_SCOPE)
        set(${patch} "0" PARENT_SCOPE)
        set(${extra} "" PARENT_SCOPE)
    endif(version_valid)
endfunction(version_split)

##############################
# get PROJECT_VERSION from git
##############################
find_program(GIT_CMD git)
mark_as_advanced(GIT_CMD)
if (GIT_CMD)
    execute_process(COMMAND ${GIT_CMD} rev-parse --show-toplevel
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_TOPLEVEL
            ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
if (GIT_CMD AND NOT "${GIT_TOPLEVEL}" STREQUAL "")
    execute_process(COMMAND ${GIT_CMD} rev-parse --short HEAD
            WORKING_DIRECTORY ${GIT_TOPLEVEL}
            OUTPUT_VARIABLE GIT_SHA1
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    #message(STATUS "GIT_SHA1: " ${GIT_SHA1})
    execute_process(COMMAND ${GIT_CMD} describe --match "*[0-9].[0-9]*" HEAD
            WORKING_DIRECTORY ${GIT_TOPLEVEL}
            OUTPUT_VARIABLE GIT_DESCRIBE
            ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    #message(STATUS "GIT_DESCRIBE: " ${GIT_DESCRIBE})

    if (GIT_DESCRIBE)
        string(REGEX REPLACE "v?([0-9.]+).*" "\\1" GIT_VERSION ${GIT_DESCRIBE})
        #message(STATUS "GIT_VERSION: " ${GIT_VERSION})
        if(PROJECT_VERSION)
            if(NOT GIT_DESCRIBE MATCHES ${PROJECT_VERSION})
                message(WARNING "Version from git (${GIT_VERSION}) doesn't match already specified PROJECT_VERSION (${PROJECT_VERSION})")
            endif()
        endif()

        # project version is MAJOR.MINOR.PATCH
        set(PROJECT_VERSION ${GIT_VERSION})

        # as package version we use the full version from git describe: 1.7.1+7+ge324c81
        if (GIT_DESCRIBE MATCHES ".*-g.*")
            # convert a git describe string to usable debian version, e.g. v1.7.1-7-ge324c81 to 1.7.1+7+ge324c81
            string(REGEX REPLACE "v?([0-9]*.[0-9.]*).*-([0-9]*)-([a-g0-9]*)" "\\1+\\2+\\3" DEB_GIT_VERSION ${GIT_DESCRIBE})
            set(PACKAGE_VERSION ${DEB_GIT_VERSION})
        else()
            # current HEAD is git tag (i.e. releaase), directly use the version
            set(PACKAGE_VERSION ${GIT_VERSION})
        endif()
    else ()
        # no (suitable) tag found
        if (PROJECT_VERSION)
            message(WARNING "Could not extract version from git! Using manually specified PROJECT_VERSION.")
        else ()
            message(WARNING "Could not extract version from git and PROJECT_VERSION not set. Defaulting to 0.0.0")
            set(PROJECT_VERSION "0.0.0")
        endif ()
        # get number of commits in repo
        execute_process(COMMAND ${GIT_CMD} rev-list --count HEAD
                WORKING_DIRECTORY ${GIT_TOPLEVEL}
                OUTPUT_VARIABLE GIT_COMMIT_COUNT
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(PACKAGE_VERSION ${PROJECT_VERSION}+${GIT_COMMIT_COUNT}+g${GIT_SHA1})
    endif ()
endif ()

# check that project version is now set
if (NOT PROJECT_VERSION)
    message(WARNING "PROJECT_VERSION not set. Defaulting to 0.0.0")
    set(PROJECT_VERSION "0.0.0")
endif ()
if (NOT PACKAGE_VERSION)
    message(WARNING "PACKAGE_VERSION not set! Falling back to (${PROJECT_VERSION})")
    set(PACKAGE_VERSION ${PROJECT_VERSION})
endif ()

version_split(${PROJECT_VERSION} PACKAGE_VERSION_MAJOR PACKAGE_VERSION_MINOR PACKAGE_VERSION_PATCH extra)
#message(STATUS "PACKAGE_VERSION_MAJOR: " ${PACKAGE_VERSION_MAJOR})
#message(STATUS "PACKAGE_VERSION_MINOR: " ${PACKAGE_VERSION_MINOR})
#message(STATUS "PACKAGE_VERSION_PATCH: " ${PACKAGE_VERSION_PATCH})

# generate an integer version number: major * 10000 + minor * 100 + patch
math(EXPR PROJECT_VERSION_INT "${PACKAGE_VERSION_MAJOR} * 10000 + ${PACKAGE_VERSION_MINOR} * 100 + ${PACKAGE_VERSION_PATCH}")

message(STATUS "PROJECT_VERSION: " ${PROJECT_VERSION})
message(STATUS "PACKAGE_VERSION: " ${PACKAGE_VERSION})

# make PROJECT_VERSION available as define in the project source
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")
add_definitions(-DPROJECT_VERSION_INT=${PROJECT_VERSION_INT})
add_definitions(-DPACKAGE_VERSION="${PACKAGE_VERSION}")
add_definitions(-DPACKAGE_VERSION_MAJOR=${PACKAGE_VERSION_MAJOR})
add_definitions(-DPACKAGE_VERSION_MINOR=${PACKAGE_VERSION_MINOR})
add_definitions(-DPACKAGE_VERSION_PATCH=${PACKAGE_VERSION_PATCH})

# set ABI version to major.minor, which will be used for the SOVERSION
set(abiversion "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}")

# generate a version.h file in the binary output dir, don't forget to install it...
string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)

# These files provide compile-time and runtime version information about your project.
# To offer the version info to the users of your library, you need to
# adapt the following lines in your respective CMakeLists.txt:
#   add_library(<yourlibraryname> SHARED <your code files> ${CMAKE_CURRENT_BINARY_DIR}/project_version.cc)
#   install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/project_version.h COMPONENT dev DESTINATION include/<your-include-dir>)
# To use it within your library or tests you need to add the include directory:
# > target_include_directories(yourtarget PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
configure_file(cmake/project_version.h.in  ${PROJECT_NAME}/project_version.h @ONLY)
configure_file(cmake/project_version.cc.in ${PROJECT_NAME}/project_version.cc @ONLY)

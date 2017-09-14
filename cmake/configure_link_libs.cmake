# This cmake code creates the configuration that is found and used by
# find_package() of another cmake project

# Do not Error on non-existent target in get_target_property.
if(POLICY CMP0045)
  cmake_policy(SET CMP0045 OLD)
endif()

# Go through all static libraries and add the INTERFACE_LINK_LIBRARIES of all
# dependencies also to the INTERFACE_LINK_LIBRARIES of the static libraries

foreach (LIB ${PROJECT_STATIC_LIBRARIES})
  get_target_property(LIB_DEPS ${LIB} INTERFACE_LINK_LIBRARIES)

  if (LIB_DEPS)
    set(ADD_LIB_DEBS)
    foreach (DEP ${LIB_DEPS})
      get_target_property(DEP_DEP ${DEP} INTERFACE_LINK_LIBRARIES)

      if (NOT DEP_DEP)
        get_target_property(DEP_DEP ${DEP} IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE)
      endif ()

      if (DEP_DEP)
        set(ADD_LIB_DEPS ${ADD_LIB_DEPS} ${DEP_DEP})
      endif ()
    endforeach ()

    set(LIB_DEPS ${LIB_DEPS} ${ADD_LIB_DEPS})
    set_target_properties(${LIB} PROPERTIES INTERFACE_LINK_LIBRARIES "${LIB_DEPS}")
  endif ()
endforeach ()

# get lower and upper case project name for the configuration files

string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)
string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)

# configure and install the configuration files

#export(TARGETS ${PROJECT_LIBRARIES} ${PROJECT_STATIC_LIBRARIES}
#  FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME_UPPER}Targets.cmake")

configure_file(cmake/PROJECTConfig.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_UPPER}Config.cmake" @ONLY)

configure_file(cmake/PROJECTConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_UPPER}ConfigVersion.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_UPPER}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_UPPER}ConfigVersion.cmake"
  COMPONENT dev
  DESTINATION lib/${PROJECT_NAME_LOWER})

if (PROJECTTargets)
  install(EXPORT PROJECTTargets
          FILE ${PROJECT_NAME_UPPER}Targets.cmake
          DESTINATION lib/${PROJECT_NAME_LOWER})
endif ()

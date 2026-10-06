#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "GenICam::GCBase" for configuration "Release"
set_property(TARGET GenICam::GCBase APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::GCBase PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libGCBase_gcc8_v3_5.so"
  IMPORTED_SONAME_RELEASE "libGCBase_gcc8_v3_5.so"
  )

list(APPEND _cmake_import_check_targets GenICam::GCBase )
list(APPEND _cmake_import_check_files_for_GenICam::GCBase "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libGCBase_gcc8_v3_5.so" )

# Import target "GenICam::Log" for configuration "Release"
set_property(TARGET GenICam::Log APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::Log PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libLog_gcc8_v3_5.so"
  IMPORTED_SONAME_RELEASE "libLog_gcc8_v3_5.so"
  )

list(APPEND _cmake_import_check_targets GenICam::Log )
list(APPEND _cmake_import_check_files_for_GenICam::Log "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libLog_gcc8_v3_5.so" )

# Import target "GenICam::GenApi" for configuration "Release"
set_property(TARGET GenICam::GenApi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::GenApi PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "GenICam::NodeMapData"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libGenApi_gcc8_v3_5.so"
  IMPORTED_SONAME_RELEASE "libGenApi_gcc8_v3_5.so"
  )

list(APPEND _cmake_import_check_targets GenICam::GenApi )
list(APPEND _cmake_import_check_files_for_GenICam::GenApi "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libGenApi_gcc8_v3_5.so" )

# Import target "GenICam::MathParser" for configuration "Release"
set_property(TARGET GenICam::MathParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::MathParser PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libMathParser_gcc8_v3_5.so"
  IMPORTED_SONAME_RELEASE "libMathParser_gcc8_v3_5.so"
  )

list(APPEND _cmake_import_check_targets GenICam::MathParser )
list(APPEND _cmake_import_check_files_for_GenICam::MathParser "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libMathParser_gcc8_v3_5.so" )

# Import target "GenICam::NodeMapData" for configuration "Release"
set_property(TARGET GenICam::NodeMapData APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::NodeMapData PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libNodeMapData_gcc8_v3_5.so"
  IMPORTED_SONAME_RELEASE "libNodeMapData_gcc8_v3_5.so"
  )

list(APPEND _cmake_import_check_targets GenICam::NodeMapData )
list(APPEND _cmake_import_check_files_for_GenICam::NodeMapData "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libNodeMapData_gcc8_v3_5.so" )

# Import target "GenICam::XmlParser" for configuration "Release"
set_property(TARGET GenICam::XmlParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::XmlParser PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "GenICam::NodeMapData"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libXmlParser_gcc8_v3_5.so"
  IMPORTED_SONAME_RELEASE "libXmlParser_gcc8_v3_5.so"
  )

list(APPEND _cmake_import_check_targets GenICam::XmlParser )
list(APPEND _cmake_import_check_files_for_GenICam::XmlParser "${_IMPORT_PREFIX}/bin/Linux32_ARMhf/libXmlParser_gcc8_v3_5.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

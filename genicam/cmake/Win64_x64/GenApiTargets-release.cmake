#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "GenICam::GCBase" for configuration "Release"
set_property(TARGET GenICam::GCBase APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::GCBase PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/GCBase_MD_VC142_v3_5.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Win64_x64/GCBase_MD_VC142_v3_5.dll"
  )

list(APPEND _cmake_import_check_targets GenICam::GCBase )
list(APPEND _cmake_import_check_files_for_GenICam::GCBase "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/GCBase_MD_VC142_v3_5.lib" "${_IMPORT_PREFIX}/bin/Win64_x64/GCBase_MD_VC142_v3_5.dll" )

# Import target "GenICam::Log" for configuration "Release"
set_property(TARGET GenICam::Log APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::Log PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/Log_MD_VC142_v3_5.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Win64_x64/Log_MD_VC142_v3_5.dll"
  )

list(APPEND _cmake_import_check_targets GenICam::Log )
list(APPEND _cmake_import_check_files_for_GenICam::Log "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/Log_MD_VC142_v3_5.lib" "${_IMPORT_PREFIX}/bin/Win64_x64/Log_MD_VC142_v3_5.dll" )

# Import target "GenICam::GenApi" for configuration "Release"
set_property(TARGET GenICam::GenApi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::GenApi PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/GenApi_MD_VC142_v3_5.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "GenICam::NodeMapData"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Win64_x64/GenApi_MD_VC142_v3_5.dll"
  )

list(APPEND _cmake_import_check_targets GenICam::GenApi )
list(APPEND _cmake_import_check_files_for_GenICam::GenApi "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/GenApi_MD_VC142_v3_5.lib" "${_IMPORT_PREFIX}/bin/Win64_x64/GenApi_MD_VC142_v3_5.dll" )

# Import target "GenICam::MathParser" for configuration "Release"
set_property(TARGET GenICam::MathParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::MathParser PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/MathParser_MD_VC142_v3_5.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Win64_x64/MathParser_MD_VC142_v3_5.dll"
  )

list(APPEND _cmake_import_check_targets GenICam::MathParser )
list(APPEND _cmake_import_check_files_for_GenICam::MathParser "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/MathParser_MD_VC142_v3_5.lib" "${_IMPORT_PREFIX}/bin/Win64_x64/MathParser_MD_VC142_v3_5.dll" )

# Import target "GenICam::NodeMapData" for configuration "Release"
set_property(TARGET GenICam::NodeMapData APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::NodeMapData PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/NodeMapData_MD_VC142_v3_5.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Win64_x64/NodeMapData_MD_VC142_v3_5.dll"
  )

list(APPEND _cmake_import_check_targets GenICam::NodeMapData )
list(APPEND _cmake_import_check_files_for_GenICam::NodeMapData "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/NodeMapData_MD_VC142_v3_5.lib" "${_IMPORT_PREFIX}/bin/Win64_x64/NodeMapData_MD_VC142_v3_5.dll" )

# Import target "GenICam::XmlParser" for configuration "Release"
set_property(TARGET GenICam::XmlParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GenICam::XmlParser PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/XmlParser_MD_VC142_v3_5.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "GenICam::NodeMapData"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Win64_x64/XmlParser_MD_VC142_v3_5.dll"
  )

list(APPEND _cmake_import_check_targets GenICam::XmlParser )
list(APPEND _cmake_import_check_files_for_GenICam::XmlParser "${_IMPORT_PREFIX}/library/CPP/lib/Win64_x64/XmlParser_MD_VC142_v3_5.lib" "${_IMPORT_PREFIX}/bin/Win64_x64/XmlParser_MD_VC142_v3_5.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

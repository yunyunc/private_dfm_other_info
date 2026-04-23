#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "FeatureRecognizer::featureRecognizer" for configuration "Release"
set_property(TARGET FeatureRecognizer::featureRecognizer APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(FeatureRecognizer::featureRecognizer PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/featureRecognizer.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/featureRecognizer.dll"
  )

list(APPEND _cmake_import_check_targets FeatureRecognizer::featureRecognizer )
list(APPEND _cmake_import_check_files_for_FeatureRecognizer::featureRecognizer "${_IMPORT_PREFIX}/lib/featureRecognizer.lib" "${_IMPORT_PREFIX}/bin/featureRecognizer.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

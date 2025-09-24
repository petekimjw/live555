#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "live555::UsageEnvironment" for configuration "MinSizeRel"
set_property(TARGET live555::UsageEnvironment APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(live555::UsageEnvironment PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/MinSizeRel/UsageEnvironment.lib"
  )

list(APPEND _cmake_import_check_targets live555::UsageEnvironment )
list(APPEND _cmake_import_check_files_for_live555::UsageEnvironment "${_IMPORT_PREFIX}/lib/MinSizeRel/UsageEnvironment.lib" )

# Import target "live555::groupsock" for configuration "MinSizeRel"
set_property(TARGET live555::groupsock APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(live555::groupsock PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "C;CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/MinSizeRel/groupsock.lib"
  )

list(APPEND _cmake_import_check_targets live555::groupsock )
list(APPEND _cmake_import_check_files_for_live555::groupsock "${_IMPORT_PREFIX}/lib/MinSizeRel/groupsock.lib" )

# Import target "live555::BasicUsageEnvironment" for configuration "MinSizeRel"
set_property(TARGET live555::BasicUsageEnvironment APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(live555::BasicUsageEnvironment PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/MinSizeRel/BasicUsageEnvironment.lib"
  )

list(APPEND _cmake_import_check_targets live555::BasicUsageEnvironment )
list(APPEND _cmake_import_check_files_for_live555::BasicUsageEnvironment "${_IMPORT_PREFIX}/lib/MinSizeRel/BasicUsageEnvironment.lib" )

# Import target "live555::liveMedia" for configuration "MinSizeRel"
set_property(TARGET live555::liveMedia APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(live555::liveMedia PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "C;CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/MinSizeRel/liveMedia.lib"
  )

list(APPEND _cmake_import_check_targets live555::liveMedia )
list(APPEND _cmake_import_check_files_for_live555::liveMedia "${_IMPORT_PREFIX}/lib/MinSizeRel/liveMedia.lib" )

# Import target "live555::EpollTaskScheduler" for configuration "MinSizeRel"
set_property(TARGET live555::EpollTaskScheduler APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(live555::EpollTaskScheduler PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "C;CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/MinSizeRel/EpollTaskScheduler.lib"
  )

list(APPEND _cmake_import_check_targets live555::EpollTaskScheduler )
list(APPEND _cmake_import_check_files_for_live555::EpollTaskScheduler "${_IMPORT_PREFIX}/lib/MinSizeRel/EpollTaskScheduler.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

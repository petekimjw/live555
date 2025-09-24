#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "live555::UsageEnvironment" for configuration "Debug"
set_property(TARGET live555::UsageEnvironment APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(live555::UsageEnvironment PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/Debug/UsageEnvironment.lib"
  )

list(APPEND _cmake_import_check_targets live555::UsageEnvironment )
list(APPEND _cmake_import_check_files_for_live555::UsageEnvironment "${_IMPORT_PREFIX}/lib/Debug/UsageEnvironment.lib" )

# Import target "live555::groupsock" for configuration "Debug"
set_property(TARGET live555::groupsock APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(live555::groupsock PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C;CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/Debug/groupsock.lib"
  )

list(APPEND _cmake_import_check_targets live555::groupsock )
list(APPEND _cmake_import_check_files_for_live555::groupsock "${_IMPORT_PREFIX}/lib/Debug/groupsock.lib" )

# Import target "live555::BasicUsageEnvironment" for configuration "Debug"
set_property(TARGET live555::BasicUsageEnvironment APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(live555::BasicUsageEnvironment PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/Debug/BasicUsageEnvironment.lib"
  )

list(APPEND _cmake_import_check_targets live555::BasicUsageEnvironment )
list(APPEND _cmake_import_check_files_for_live555::BasicUsageEnvironment "${_IMPORT_PREFIX}/lib/Debug/BasicUsageEnvironment.lib" )

# Import target "live555::liveMedia" for configuration "Debug"
set_property(TARGET live555::liveMedia APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(live555::liveMedia PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C;CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/Debug/liveMedia.lib"
  )

list(APPEND _cmake_import_check_targets live555::liveMedia )
list(APPEND _cmake_import_check_files_for_live555::liveMedia "${_IMPORT_PREFIX}/lib/Debug/liveMedia.lib" )

# Import target "live555::EpollTaskScheduler" for configuration "Debug"
set_property(TARGET live555::EpollTaskScheduler APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(live555::EpollTaskScheduler PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C;CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/Debug/EpollTaskScheduler.lib"
  )

list(APPEND _cmake_import_check_targets live555::EpollTaskScheduler )
list(APPEND _cmake_import_check_files_for_live555::EpollTaskScheduler "${_IMPORT_PREFIX}/lib/Debug/EpollTaskScheduler.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

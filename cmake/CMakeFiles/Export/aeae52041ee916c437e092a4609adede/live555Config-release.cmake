#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "live555::UsageEnvironment" for configuration "Release"
set_property(TARGET live555::UsageEnvironment APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(live555::UsageEnvironment PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Release/UsageEnvironment.lib"
  )

list(APPEND _cmake_import_check_targets live555::UsageEnvironment )
list(APPEND _cmake_import_check_files_for_live555::UsageEnvironment "${_IMPORT_PREFIX}/lib/Release/UsageEnvironment.lib" )

# Import target "live555::groupsock" for configuration "Release"
set_property(TARGET live555::groupsock APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(live555::groupsock PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Release/groupsock.lib"
  )

list(APPEND _cmake_import_check_targets live555::groupsock )
list(APPEND _cmake_import_check_files_for_live555::groupsock "${_IMPORT_PREFIX}/lib/Release/groupsock.lib" )

# Import target "live555::BasicUsageEnvironment" for configuration "Release"
set_property(TARGET live555::BasicUsageEnvironment APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(live555::BasicUsageEnvironment PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Release/BasicUsageEnvironment.lib"
  )

list(APPEND _cmake_import_check_targets live555::BasicUsageEnvironment )
list(APPEND _cmake_import_check_files_for_live555::BasicUsageEnvironment "${_IMPORT_PREFIX}/lib/Release/BasicUsageEnvironment.lib" )

# Import target "live555::liveMedia" for configuration "Release"
set_property(TARGET live555::liveMedia APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(live555::liveMedia PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Release/liveMedia.lib"
  )

list(APPEND _cmake_import_check_targets live555::liveMedia )
list(APPEND _cmake_import_check_files_for_live555::liveMedia "${_IMPORT_PREFIX}/lib/Release/liveMedia.lib" )

# Import target "live555::EpollTaskScheduler" for configuration "Release"
set_property(TARGET live555::EpollTaskScheduler APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(live555::EpollTaskScheduler PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/Release/EpollTaskScheduler.lib"
  )

list(APPEND _cmake_import_check_targets live555::EpollTaskScheduler )
list(APPEND _cmake_import_check_files_for_live555::EpollTaskScheduler "${_IMPORT_PREFIX}/lib/Release/EpollTaskScheduler.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

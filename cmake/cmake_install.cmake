# Install script for directory: D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/live555")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/UsageEnvironment/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/groupsock/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/BasicUsageEnvironment/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/liveMedia/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/EpollTaskScheduler/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/proxyServer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/mediaServer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/hlsProxy/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/live555" TYPE FILE FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/live555ConfigVersion.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE FILES
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/MPEG2TransportStreamIndexer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/mikeyParse"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/openRTSP"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/playSIP"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/registerRTSPStream"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/sapWatch"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testAMRAudioStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testDVVideoStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testH264VideoStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testH264VideoToHLSSegments"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testH264VideoToTransportStream"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testH265VideoStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testH265VideoToTransportStream"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMKVSplitter"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMKVStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMP3Receiver"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMP3Streamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMPEG1or2AudioVideoStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMPEG1or2ProgramToTransportStream"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMPEG1or2Splitter"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMPEG1or2VideoReceiver"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMPEG1or2VideoStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMPEG2TransportReceiver"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMPEG2TransportStreamTrickPlay"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMPEG2TransportStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testMPEG4VideoStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testOggStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testOnDemandRTSPServer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testRTSPClient"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testRelay"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testReplicator"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/testWAVAudioStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/testProgs/vobStreamer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/mediaServer/live555MediaServer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/proxyServer/live555ProxyServer"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/hlsProxy/live555HLSProxy"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/live555/live555Config.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/live555/live555Config.cmake"
         "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/CMakeFiles/Export/aeae52041ee916c437e092a4609adede/live555Config.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/live555/live555Config-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/live555/live555Config.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/live555" TYPE FILE FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/CMakeFiles/Export/aeae52041ee916c437e092a4609adede/live555Config.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/live555" TYPE FILE FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/CMakeFiles/Export/aeae52041ee916c437e092a4609adede/live555Config-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/live555" TYPE FILE FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/CMakeFiles/Export/aeae52041ee916c437e092a4609adede/live555Config-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/live555" TYPE FILE FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/CMakeFiles/Export/aeae52041ee916c437e092a4609adede/live555Config-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/live555" TYPE FILE FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/CMakeFiles/Export/aeae52041ee916c437e092a4609adede/live555Config-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

# Install script for directory: D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/UsageEnvironment

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/Debug" TYPE STATIC_LIBRARY FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/UsageEnvironment/Debug/UsageEnvironment.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/Release" TYPE STATIC_LIBRARY FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/UsageEnvironment/Release/UsageEnvironment.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/MinSizeRel" TYPE STATIC_LIBRARY FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/UsageEnvironment/MinSizeRel/UsageEnvironment.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/RelWithDebInfo" TYPE STATIC_LIBRARY FILES "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/cmake/UsageEnvironment/RelWithDebInfo/UsageEnvironment.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/UsageEnvironment" TYPE FILE FILES
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/UsageEnvironment/include/Boolean.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/UsageEnvironment/include/HashTable.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/UsageEnvironment/include/UsageEnvironment.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/UsageEnvironment/include/UsageEnvironment_version.hh"
    "D:/sgkim/Dev_OpenSource/live555-master-melchi45/live555-master/UsageEnvironment/include/strDup.hh"
    )
endif()


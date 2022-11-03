# Install script for directory: /home/yuxintao/yxtweb-cpp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/yxtwebcpp" TYPE FILE FILES
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/log.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/config.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/thread.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/mutex.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/util.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/macro.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/fiber.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/scheduler.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/iomanager.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/hook.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/fd_manager.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/address.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/endian.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/socket.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/bytearray.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/tcpserver.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/yxtwebcpp/http" TYPE FILE FILES
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/http/http.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/http/http11_parser.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/http/httpclient_parser.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/http/http_parser.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/http/httpserver.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/yxtwebcpp/streams" TYPE FILE FILES
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/streams/stream.hpp"
    "/home/yuxintao/yxtweb-cpp/yxtwebcpp/streams/socket_stream.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyxtwebcpp.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyxtwebcpp.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyxtwebcpp.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/yuxintao/yxtweb-cpp/lib/libyxtwebcpp.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyxtwebcpp.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyxtwebcpp.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libyxtwebcpp.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/yuxintao/yxtweb-cpp/lib/libyxtwebcpp.a")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/yuxintao/yxtweb-cpp/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

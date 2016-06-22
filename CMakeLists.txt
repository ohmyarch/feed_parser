cmake_minimum_required(VERSION 3.5.2)

project(feed_parser VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

option(BUILD_EXAMPLES "Build examples." ON)
option(BUILD_SHARED_LIBS "Build shared Libraries." ON)

if(UNIX)
  option(FEED_PARSER_INSTALL_HEADERS "Install header files." ON)
  if(FEED_PARSER_INSTALL_HEADERS)
    file(GLOB FEED_PARSER_HEADERS_FEED feed/*.h)
    install(FILES ${FEED_PARSER_HEADERS_FEED} DESTINATION include/feed)
    file(GLOB FEED_PARSER_HEADERS_DATE_TIME feed/date_time/tz.h feed/date_time/date.h feed/date_time/tz_private.h)
    install(FILES ${FEED_PARSER_HEADERS_DATE_TIME} DESTINATION include/feed/date_time)
  endif()

  if(APPLE AND NOT OPENSSL_ROOT_DIR)
    file(GLOB OPENSSL_ROOT_DIR /usr/local/Cellar/openssl/*)
    list(REVERSE OPENSSL_ROOT_DIR)
  endif()
  set(_OPENSSL_VERSION "")
  find_package(OpenSSL 1.0.0 REQUIRED)
elseif(WIN32)
  option(Boost_USE_STATIC_LIBS ON)

  add_definitions(-DUNICODE -D_UNICODE)
endif()

find_package(Threads REQUIRED)
find_package(Casablanca 2.8.0 REQUIRED)

if(BUILD_EXAMPLES)
  find_package(Boost REQUIRED COMPONENTS random chrono system thread program_options)
else()
  find_package(Boost REQUIRED COMPONENTS random chrono system thread)
endif()

file(GLOB_RECURSE HEADER_FILES *.h)
add_custom_target(headers SOURCES ${HEADER_FILES})

include_directories(${CMAKE_SOURCE_DIR} ${OPENSSL_INCLUDE_DIR} ${CASABLANCA_INCLUDE_DIRS})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message("-- Setting clang options")

  set(WARNINGS "${WARNINGS} ${OSX_SUPPRESSIONS}")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wno-return-type-c-linkage -Wno-unneeded-internal-declaration")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-strict-aliasing")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  message("-- Setting gcc options")

  set(WARNINGS "-Wall -Wextra -Wunused-parameter -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winit-self -Winvalid-pch -Wmissing-format-attribute -Wmissing-include-dirs -Wpacked -Wredundant-decls -Wunreachable-code")

  set(LD_FLAGS "${LD_FLAGS} -Wl,-z,defs")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-strict-aliasing")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  message("-- Setting msvc options")
  set(WARNINGS)
else()
  message("-- Unknown compiler, success is doubtful.")
  message("CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CONFIG_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}")


add_subdirectory(src)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

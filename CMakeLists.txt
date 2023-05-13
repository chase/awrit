# Copyright (c) 2023 Chase Colman. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(awrit VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 OLD)
endif()

set(CEF_VERSION 112.3.0+gb09c4ca+chromium-112.0.5615.165)
# TODO: fetch the checksum from the CDN itself
set(CEF_WIN_64_SHA ac0fb8d46e4b9a38eea18c65319010e344658b3f)
set(CEF_LINUX_64_SHA ef1a146d2e184ce910c1bb35b66f93c3271dd789)
set(CEF_MAC_ARM64_SHA 97e064f35f9a6c375366584738fb5d1ef7e53017)

set(GTEST_VERSION v1.13.0)
SET(GTEST_SHA bfa4b5131b6eaac06962c251742c96aab3f7aa78)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(OS_MAC 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(OS_LINUX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(OS_WINDOWS 1)
else()
  message(FATAL_ERROR "Unsupported platform ${CMAKE_SYSTEM_NAME}")
endif()

if(NOT DEFINED PROJECT_ARCH)
  if(("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "arm64") OR
     ("${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" STREQUAL "ARM64"))
    set(PROJECT_ARCH "arm64")
  elseif(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PROJECT_ARCH "x86_64")
  else()
    message(FATAL_ERROR "Unsupported architecture")
  endif()
endif()

if (OS_MAC AND PROJECT_ARCH STREQUAL "x86_64")
  message(FATAL_ERROR "Only arm64 is supported on macOS")
elseif (NOT OS_MAC AND PROJECT_ARCH STREQUAL "arm64")
  message(FATAL_ERROR "Only x64 is supported on Windows and Linux")
endif()

if (OS_MAC)
  set(CEF_PLATFORM "macosarm64")
  set(CEF_SHA ${CEF_MAC_ARM64_SHA})
elseif(OS_LINUX)
  set(CEF_PLATFORM "linux64")
  set(CEF_SHA ${CEF_LINUX_64_SHA})
elseif(OS_WINDOWS)
  set(CEF_PLATFORM "windows64")
  set(CEF_SHA ${CEF_WIN_64_SHA})
endif()

include(FetchContent)
# Download CEF if necessary
Set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
  cef_minimal
  URL https://cef-builds.spotifycdn.com/cef_binary_${CEF_VERSION}_${CEF_PLATFORM}_minimal.tar.bz2
  URL_HASH SHA1=${CEF_SHA}
  SOURCE_SUBDIR cmake
)
FetchContent_MakeAvailable(cef_minimal)

# gtest v1.13.0
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/${GTEST_VERSION}.tar.gz
  URL_HASH SHA1=${GTEST_SHA}
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

# Setup CEF
set(CEF_ROOT "${cef_minimal_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

find_package(CEF REQUIRED)
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

add_subdirectory(third_party)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add the main project
add_subdirectory(awrit)

# Display configuration settings.
PRINT_CEF_CONFIG()

cmake_language(DEFER DIRECTORY "${CMAKE_BINARY_DIR}" CALL _copy_compile_commands())
function(_copy_compile_commands)
if (EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
configure_file("${CMAKE_BINARY_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}/compile_commands.json")
endif()
endfunction(_copy_compile_commands)

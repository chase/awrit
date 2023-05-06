# Copyright (c) 2023 Chase Colman. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

cmake_minimum_required(VERSION 3.22)

# TODO: add macOS/Windows support

set(AWRIT_SRCS
  main.cc
  awrit.h
  awrit.cc
  shared_memory.h
  tui.h
  tui.cc
  )

set(AWRIT_SRCS_LINUX
  shared_memory_posix.cc
  )
set(AWRIT_SRCS_MAC
  main_mac.mm
  process_helper_mac.cc
  shared_memory_posix.cc
  )
set(AWRIT_SRCS_WINDOWS
  shared_memory_win.cc
  )
APPEND_PLATFORM_SOURCES(AWRIT_SRCS)
source_group(awrit FILES ${AWRIT_SRCS})

set(AWRIT_SRCS
  ${AWRIT_SRCS}
  )

set(CEF_TARGET "awrit")

if(NOT OS_MAC)
  ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
endif()

# Determine the target output directory.
SET_CEF_TARGET_OUT_DIR()

if(OS_LINUX)

# Executable target.
add_executable(${CEF_TARGET} ${AWRIT_SRCS})

SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

# Set rpath so that libraries can be placed next to the executable.
set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

# Copy binary and resource files to the target output directory.
COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

COPY_SINGLE_FILE("${CEF_TARGET}" "${CMAKE_SOURCE_DIR}/LICENSE.txt" "${CEF_TARGET_OUT_DIR}/LICENSE.txt")

SET_LINUX_SUID_PERMISSIONS("${CEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")

endif()

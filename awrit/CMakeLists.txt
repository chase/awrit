# Copyright (c) 2023 Chase Colman. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

cmake_minimum_required(VERSION 3.22)

# Testing
enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(shm)
add_subdirectory(string)
add_subdirectory(tty)

set(AWRIT_INTERNAL_LIBS
  shm
  string
  tty
  )

# TODO: add macOS/Windows support

set(AWRIT_SRCS
  main.cc
  input_event_handler.h
  input_event_handler.cc
  awrit.h
  awrit.cc
  tui.h
  tui.cc
  )

set(AWRIT_SRCS_LINUX
  )
set(AWRIT_SRCS_MAC
  main_mac.mm
  process_helper_mac.cc
  )
set(AWRIT_SRCS_WINDOWS
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

# LINUX
if(OS_LINUX)

# Executable target.
add_executable(${CEF_TARGET} ${AWRIT_SRCS})

SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} ${AWRIT_INTERNAL_LIBS})

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

set(AWRIT_UNIT_TEST_SRCS
  shm/shared_memory_unittest.cc
  string/string_utils_unittest.cc
  tty/escape_parser_unittest.cc
  tty/kitty_keys_unittest.cc
  )

source_group(awrit_unit_tests FILES ${AWRIT_UNIT_TEST_SRCS})

add_executable(awrit_unit_tests ${AWRIT_UNIT_TEST_SRCS})
target_link_libraries(awrit_unit_tests ${AWRIT_INTERNAL_LIBS} GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(awrit_unit_tests)

add_custom_target(test
  COMMAND "${CEF_TARGET_OUT_DIR}/awrit_unit_tests"
  DEPENDS "${CEF_TARGET_OUT_DIR}/awrit_unit_tests"
)

add_executable(input_event_test tty/input_event_test.cc)
target_link_libraries(input_event_test PRIVATE tty)

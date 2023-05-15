# Copyright (c) 2023 Chase Colman. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

cmake_minimum_required(VERSION 3.22)

# Testing
enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(string)
add_subdirectory(tty)

set(AWRIT_INTERNAL_LIBS
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
  )
set(AWRIT_SRCS_WINDOWS
  )
APPEND_PLATFORM_SOURCES(AWRIT_SRCS)
source_group(awrit FILES ${AWRIT_SRCS})

set(AWRIT_SRCS
  ${AWRIT_SRCS}
  )

set(CEF_HELPER_SRCS_MAC
  process_helper_mac.cc
  )
APPEND_PLATFORM_SOURCES(CEF_HELPER_SRCS)
source_group(cefsimple FILES ${CEF_HELPER_SRCS})

# mac resources.
set(CEF_RESOURCES_MAC_SRCS_MAC
  resources/mac/Info.plist
  )
APPEND_PLATFORM_SOURCES(CEF_RESOURCES_MAC_SRCS)
source_group(awrit\\\\resources\\\\mac FILES ${CEF_RESOURCES_MAC_SRCS})

set(CEF_RESOURCES_MAC_ENGLISH_LPROJ_SRCS_MAC
  resources/mac/English.lproj/InfoPlist.strings
  resources/mac/English.lproj/MainMenu.xib
  )
APPEND_PLATFORM_SOURCES(CEF_RESOURCES_MAC_ENGLISH_LPROJ_SRCS)
source_group(awrit\\\\resources\\\\mac\\\\English.lproj FILES ${CEF_RESOURCES_MAC_ENGLISH_LPROJ_SRCS})

set(CEF_RESOURCES_SRCS
  ${CEF_RESOURCES_MAC_SRCS}
  ${CEF_RESOURCES_MAC_ENGLISH_LPROJ_SRCS}
  )

set(CEF_TARGET "awrit")

if(OS_MAC)
  set(CEF_HELPER_TARGET "awrit_Helper")
  set(CEF_HELPER_OUTPUT_NAME "awrit Helper")
else()
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
target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} ${AWRIT_INTERNAL_LIBS} platform_folders)

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

if(OS_MAC)
  # Output path for the main app bundle.
  set(CEF_APP "${CEF_TARGET_OUT_DIR}/${CEF_TARGET}.app")

  # Variables referenced from the main Info.plist file.
  set(EXECUTABLE_NAME "${CEF_TARGET}")
  set(PRODUCT_NAME "${CEF_TARGET}")

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library.
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
  endif()

  # Main app bundle target.
  add_executable(${CEF_TARGET} MACOSX_BUNDLE ${CEF_RESOURCES_SRCS} ${AWRIT_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
  target_link_libraries(${CEF_TARGET} libcef_dll_wrapper ${CEF_STANDARD_LIBS} ${AWRIT_INTERNAL_LIBS})
  set_target_properties(${CEF_TARGET} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/Info.plist
    )

  # Copy the CEF framework into the Frameworks directory.
  add_custom_command(
    TARGET ${CEF_TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
            "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
    VERBATIM
    )

  # Create the multiple Helper app bundle targets.
  foreach(_suffix_list ${CEF_HELPER_APP_SUFFIXES})
    # Convert to a list and extract the suffix values.
    string(REPLACE ":" ";" _suffix_list ${_suffix_list})
    list(GET _suffix_list 0 _name_suffix)
    list(GET _suffix_list 1 _target_suffix)
    list(GET _suffix_list 2 _plist_suffix)

    # Define Helper target and output names.
    set(_helper_target "${CEF_HELPER_TARGET}${_target_suffix}")
    set(_helper_output_name "${CEF_HELPER_OUTPUT_NAME}${_name_suffix}")

    # Create Helper-specific variants of the helper-Info.plist file. Do this
    # manually because the configure_file command (which is executed as part of
    # MACOSX_BUNDLE_INFO_PLIST) uses global env variables and would insert the
    # wrong values with multiple targets.
    set(_helper_info_plist "${CMAKE_CURRENT_BINARY_DIR}/helper-Info${_target_suffix}.plist")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/helper-Info.plist" _plist_contents)
    string(REPLACE "\${EXECUTABLE_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
    string(REPLACE "\${PRODUCT_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
    string(REPLACE "\${BUNDLE_ID_SUFFIX}" "${_plist_suffix}" _plist_contents ${_plist_contents})
    file(WRITE ${_helper_info_plist} ${_plist_contents})

    # Create Helper executable target.
    add_executable(${_helper_target} MACOSX_BUNDLE ${CEF_HELPER_SRCS})
    SET_EXECUTABLE_TARGET_PROPERTIES(${_helper_target})
    add_dependencies(${_helper_target} libcef_dll_wrapper)
    target_link_libraries(${_helper_target} libcef_dll_wrapper ${CEF_STANDARD_LIBS})
    set_target_properties(${_helper_target} PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST ${_helper_info_plist}
      OUTPUT_NAME ${_helper_output_name}
      )

    if(USE_SANDBOX)
      target_link_libraries(${_helper_target} cef_sandbox_lib)
    endif()

    # Add the Helper as a dependency of the main executable target.
    add_dependencies(${CEF_TARGET} "${_helper_target}")

    # Copy the Helper app bundle into the Frameworks directory.
    add_custom_command(
      TARGET ${CEF_TARGET}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
              "${CEF_TARGET_OUT_DIR}/${_helper_output_name}.app"
              "${CEF_APP}/Contents/Frameworks/${_helper_output_name}.app"
      VERBATIM
      )
  endforeach()

  # Manually process and copy over resource files.
  # The Xcode generator can support this via the set_target_properties RESOURCE
  # directive but that doesn't properly handle nested resource directories.
  # Remove these prefixes from input file paths.
  set(PREFIXES
    "resources/mac/"
    "resources/"
    "../shared/resources/"
    )
  set(RESOURCES ${CEF_RESOURCES_SRCS})
  COPY_MAC_RESOURCES("${RESOURCES}" "${PREFIXES}" "${CEF_TARGET}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_APP}")
endif()

set(AWRIT_UNIT_TEST_SRCS
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

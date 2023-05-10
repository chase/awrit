# Copyright (c) 2023 Chase Colman. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

cmake_minimum_required(VERSION 3.22)

project(tty)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(TTY_SRCS
  escape_parser.h
  escape_parser.cc
  input.h
  input_event.h
  input_event.cc
  kitty_keys.h
  kitty_keys.cc
  output.h
  output.cc
  )

set(TTY_WIN_SRCS
  input_win.cc
  )
set(TTY_POSIX_SRCS
  input_posix.cc
  )

if(WIN32)
  set(TTY_SRCS ${TTY_SRCS} ${TTY_WIN_SRCS})
elseif(UNIX)
  set(TTY_SRCS ${TTY_SRCS} ${TTY_POSIX_SRCS})
endif()

source_group(tty ${TTY_SRCS})
add_library(tty STATIC ${TTY_SRCS})
target_link_libraries(tty PRIVATE utf8_decode modp_b64)

target_include_directories(tty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_compile_options(tty PRIVATE /W4 /WX)
elseif(UNIX)
    target_compile_options(tty PRIVATE -Wall -Wextra -Werror -pedantic)
endif()

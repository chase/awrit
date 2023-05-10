# Copyright (c) 2023 Chase Colman. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

cmake_minimum_required(VERSION 3.22)

project(shm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SHM_SRCS
  shared_memory.h
  )

set(SHM_WIN_SRCS
  shared_memory_win.cc
  )
set(SHM_POSIX_SRCS
  shared_memory_posix.cc
  )

if(WIN32)
  set(SHM_SRCS ${SHM_SRCS} ${SHM_WIN_SRCS})
elseif(UNIX)
  set(SHM_SRCS ${SHM_SRCS} ${SHM_POSIX_SRCS})
endif()

source_group(shm ${SHM_SRCS})
add_library(shm STATIC ${SHM_SRCS})

if(WIN32)
    target_compile_options(shm PRIVATE /W4 /WX)
elseif(UNIX)
    target_compile_options(shm PRIVATE -Wall -Wextra -Werror -pedantic)
endif()

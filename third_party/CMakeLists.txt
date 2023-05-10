# Copyright (c) 2023 Chase Colman. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

cmake_minimum_required(VERSION 3.22)

project(third_party)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(modp_b64 STATIC modp_b64.h modp_b64.cc)
add_library(utf8_decode STATIC utf8_decode.h utf8_decode.cc)

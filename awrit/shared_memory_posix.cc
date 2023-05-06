// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "include/cef_parser.h"
#include "shared_memory.h"
#include "tui.h"

namespace shm {

std::string write(const void* buffer, size_t size) {
  std::srand(time(nullptr));
  std::string name = "/dev/shm/awrit-" + std::to_string(std::rand());
  int fd_ = open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
  if (fd_ < 0) return {};
  if (ftruncate(fd_, size) < 0) return {};
  void* mem_ = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if (mem_ == MAP_FAILED) {
    return {};
  }
  memcpy(mem_, buffer, size);
  if (mem_) munmap(mem_, size);
  if (fd_) {
    close(fd_);
  }

  return CefBase64Encode(name.data(), name.length()).ToString();
}
}  // namespace shm

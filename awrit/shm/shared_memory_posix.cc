// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <random>

#include "shared_memory.h"

namespace shm {

std::string write(const void* buffer, size_t size) {
  std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<unsigned int> dist(0);

  std::string name = "/dev/shm/awrit-" + std::to_string(dist(engine));
  int fd_ = open(name.c_str(), O_RDWR | O_CREAT, 0600);
  if (fd_ < 0) {
    fprintf(stderr, "NO FD %s\n", name.c_str());
    return {};
  }
  if (ftruncate(fd_, size) < 0) {
    fprintf(stderr, "BAD SIZE %lu\n", size);
    return {};
  }
  void* mem_ = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if (mem_ == MAP_FAILED) {
    return {};
  }
  memcpy(mem_, buffer, size);
  if (mem_) munmap(mem_, size);
  if (fd_) {
    close(fd_);
  }

  return name;
}

}  // namespace shm

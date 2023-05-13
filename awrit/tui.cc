// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "tui.h"

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <random>

#include "include/cef_parser.h"
#include "tty/escape_codes.h"
#include "tty/input.h"
#include "tty/kitty_keys.h"
#include "tty/output.h"
#include "tty/sgr_mouse.h"

void Initialize() {
  tty::out::Setup();
  tty::in::Setup();
  tty::keys::Enable();
  tty::sgr_mouse::Enable();
}

void Restore() {
  tty::keys::Disable();
  tty::in::Cleanup();
  tty::out::Cleanup();
}

void Paint(const std::vector<CefRect>& dirtyRects, const void* buffer,
           int width, int height) {
  if (width == 0 || height == 0) [[unlikely]]
    return;
  size_t buffer_size = width * height * sizeof(uint32_t);

  // buffer is BGRA but RGBA is needed by tty::out::PaintBitmap
  static auto bgra_img = CefImage::CreateImage();
  static bool first = true;
  static std::string name;

  // clears last buffer image
  if (first) [[unlikely]] {
    std::mt19937 engine(std::random_device{}());
    std::uniform_int_distribution<unsigned int> dist(1);
    name = "/awrit-" + std::to_string(dist(engine));
    first = false;
  } else [[likely]] {
    bgra_img->RemoveRepresentation(1.0);
  }

  bgra_img->AddBitmap(1.0, width, height, CEF_COLOR_TYPE_BGRA_8888,
                      CEF_ALPHA_TYPE_PREMULTIPLIED, buffer, buffer_size);
  int n_width, n_height;  // should be the same as width/height :shrug:
  auto rgba_bitmap =
      bgra_img->GetAsBitmap(1.0, CEF_COLOR_TYPE_RGBA_8888,
                            CEF_ALPHA_TYPE_PREMULTIPLIED, n_width, n_height);

  if (!rgba_bitmap) [[unlikely]] {
    fprintf(stderr, "Unable to convert BGRA to RGBA\r\n");
    return;
  }

  int fd_ = shm_open(name.c_str(), O_RDWR | O_CREAT, 0600);
  if (fd_ < 0) {
    fprintf(stderr, "NO FD %s\r\n", name.c_str());
    return;
  }

  if (ftruncate(fd_, buffer_size) < 0) {
    fprintf(stderr, "BAD SIZE %lu\r\n", buffer_size);
    return;
  }

  void* mem_ =
      mmap(nullptr, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if (mem_ == MAP_FAILED) {
    fprintf(stderr, "MAP FAILED\r\rn");
    return;
  }

  rgba_bitmap->GetData(mem_, buffer_size, 0);

  if (mem_) [[likely]]
    munmap(mem_, buffer_size);
  if (fd_) [[likely]] {
    close(fd_);
  }

  tty::out::PaintBitmap(name, {width, height}, {0, 0}, tty::out::NameType::shm);
}

CefSize WindowSize() {
  auto size = tty::out::WindowSize();
  return {size.width, size.height};
}

// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "tui.h"

#include <sys/ioctl.h>

#include "include/cef_parser.h"
#include "shm/shared_memory.h"
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
  size_t buffer_size = width * height * sizeof(uint32_t);
  auto filename = shm::write(buffer, buffer_size);
  tty::out::PaintBitmapFile(filename, {width, height});
}

CefSize WindowSize() {
  auto size = tty::out::WindowSize();
  return {size.width, size.height};
}

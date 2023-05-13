// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_TTY_OUTPUT_H
#define AWRIT_TTY_OUTPUT_H

#include <string>
#include <string_view>
#include <vector>

#include "escape_codes.h"

namespace tty::out {

void ClearScreen();
void SetTitle(const std::string& title);

struct Size {
  int width;
  int height;
};

struct Point {
  int x;
  int y;
};

Size WindowSize();
void PlaceCursor(Point point = {0, 0});

void Setup();
void Cleanup();

enum NameType : char { shm = 's', file = 't' };

void PaintBitmap(const std::string_view name, const Size size,
                 const Point point = {0, 0},
                 const NameType type = NameType::shm);

// VT100/DEC Modes
enum Mode : int {
  cursor_key_to_app = 1,         // DECCKM
  reverse_video = 5,             // DECSCNM
  auto_wrap = 7,                 // DECAW
  auto_repeat = 8,               // DECARM
  text_cursor = 25,              // DECTCEM
  mouse_button_tracking = 1000,  // buttons only
  mouse_motion_tracking = 1002,  // buttons and drag
  mouse_move_tracking = 1003,    // full mouse
  focus_tracking = 1004,
  mouse_utf8_mode = 1005,
  mouse_sgr_mode = 1006,
  mouse_sgr_pixel_mode = 1016,
  alternate_screen = 1049,
  bracketed_paste = 2004,
  pending_update = 2026
};

void SetModes(const std::vector<Mode>& modes, bool enabled);

}  // namespace tty::out

#endif  // AWRIT_TTY_OUTPUT_H

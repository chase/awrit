// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "output.h"

#include <sys/ioctl.h>

#include <cstdio>

#include "third_party/modp_b64.h"

namespace tty::out {

void ClearScreen() { fputs(CLEAR_SCREEN, stdout); }
void SetTitle(const std::string& title) {
  std::string out = ESC "]2;" + title + "\a";
  fputs(out.c_str(), stdout);
  fflush(stdout);
}

Size WindowSize() {
  struct winsize sz;
  ioctl(0, TIOCGWINSZ, &sz);
  return {sz.ws_xpixel, sz.ws_ypixel};
}

void PlaceCursor(Point point) { printf(CSI "%d;%dH", point.x, point.y); }

void PaintBitmapFile(const std::string_view filename, const Size size,
                     const Point point) {
  size_t encoded_len = modp_b64_encode_data_len(filename.size());
  std::string encoded;
  encoded.resize(encoded_len);
  encoded_len =
      modp_b64_encode_data(encoded.data(), filename.data(), filename.size());

  PlaceCursor({0, 0});
  fprintf(stdout, ESC "_Gf=32,a=T,s=%d,v=%d,t=t,x=%d,y=%d,C=1;", size.width,
          size.height, point.x, point.y);
  fwrite(encoded.data(), encoded.length(), 1, stdout);
  fprintf(stdout, ESC "\\");
  fflush(stdout);
}

void SetModes(const std::vector<Mode>& modes, bool enabled) {
  std::string buf = "";
  for (const auto& mode : modes) {
    buf += CSI MODE + std::to_string(static_cast<int>(mode)) +
           (enabled ? "h" : "l");
  }
  fputs(buf.c_str(), stdout);
}

void Setup() {
  fputs(S7C1T SAVE_CURSOR SAVE_PRIVATE_MODE_VALUES SAVE_COLORS
            DECSACE_DEFAULT_REGION_SELECT RESET_IRM,
        stdout);
  // clang-format off
  SetModes({
    cursor_key_to_app,
    reverse_video,
    bracketed_paste,
    focus_tracking,
    mouse_button_tracking,
    mouse_motion_tracking,
    mouse_move_tracking,
    mouse_utf8_mode,
    mouse_sgr_mode
  }, false);
  // clang-format on

  // clang-format off
  SetModes({
    auto_repeat,
    auto_wrap,
    text_cursor,
    alternate_screen,
  }, true);
  // clang-format on
  fputs(CLEAR_SCREEN, stdout);

  fflush(stdout);
}

void Cleanup() {
  SetModes({alternate_screen}, false);
  fputs(RESTORE_PRIVATE_MODE_VALUES RESTORE_CURSOR RESTORE_COLORS, stdout);
  fflush(stdout);
}

}  // namespace tty::out

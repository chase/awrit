// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "tui.h"

#include <sys/ioctl.h>

#include "include/cef_parser.h"
#include "kitty_keys.h"
#include "shared_memory.h"

// simple escape codes
#define ESC "\x1B"
#define CSI ESC "["
#define MODE "?"  // technically a private mode
#define S7C1T ESC " F"
#define SAVE_CURSOR ESC "7"
#define RESTORE_CURSOR ESC "8"
#define SAVE_PRIVATE_MODE_VALUES CSI "?s"
#define RESTORE_PRIVATE_MODE_VALUES CSI "?r"
#define SAVE_COLORS CSI "#P"
#define RESTORE_COLORS CSI "#Q"
#define DECSACE_DEFAULT_REGION_SELECT CSI "*x"
#define CLEAR_SCREEN CSI "H" CSI "2J"
#define RESET_IRM CSI "4l"

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

// see https://sw.kovidgoyal.net/kitty/keyboard-protocol/
void set_key_enhancements() {
  using namespace KittyKeys;
  fprintf(stdout, CSI ">%d",
          Flags::disambiguate_escape_codes | Flags::report_event_types |
              Flags::report_alternate_keys |
              Flags::report_all_keys_as_escape_codes |
              Flags::report_associated_text);
}

void restore_key_enhancements() { fputs(CSI "<u", stdout); }

void clear_screen() { fputs(CSI "H" CSI "2J", stdout); }

void set_modes(const std::vector<Mode>& modes, bool enabled) {
  std::string buf = "";
  for (const auto& mode : modes) {
    buf += CSI MODE + std::to_string(static_cast<int>(mode)) +
           (enabled ? "h" : "l");
  }
  fputs(buf.c_str(), stdout);
}

void Initialize() {
  fputs(S7C1T SAVE_CURSOR SAVE_PRIVATE_MODE_VALUES SAVE_COLORS
            DECSACE_DEFAULT_REGION_SELECT RESET_IRM,
        stdout);
  set_modes({text_cursor, cursor_key_to_app, reverse_video, bracketed_paste,
             focus_tracking, mouse_button_tracking, mouse_motion_tracking,
             mouse_move_tracking, mouse_utf8_mode, mouse_sgr_mode},
            false);
  set_modes(
      {
          auto_repeat,
          auto_wrap,
          alternate_screen,
      },
      true);
  fputs(CLEAR_SCREEN, stdout);
  // set_key_enhancements();
  set_modes({mouse_sgr_pixel_mode, mouse_move_tracking}, true);
  fflush(stdout);
}

void Restore() {
  // restore_key_enhancements();
  set_modes(
      {
          alternate_screen,
      },
      false);
  fputs(RESTORE_PRIVATE_MODE_VALUES RESTORE_CURSOR RESTORE_COLORS, stdout);
  fflush(stdout);
}

CefSize WindowSize() {
  struct winsize sz;
  ioctl(0, TIOCGWINSZ, &sz);
  return CefSize(sz.ws_xpixel, sz.ws_ypixel);
}

void SetTitle(const std::string& title) {
  std::string out = ESC "]2;" + title + "\a";
  fputs(out.c_str(), stdout);
  fflush(stdout);
}

void Paint(const std::vector<CefRect>& dirtyRects, const void* buffer,
           int width, int height) {
  size_t buffer_size = width * height * sizeof(uint32_t);

  PlaceCursor(0, 0);
  auto encoded = shm::write(buffer, buffer_size);
  if (encoded.empty()) return;

  fprintf(stdout, "\x1B_Gf=32,a=T,s=%d,v=%d,t=t,x=0,y=0,C=1;", width, height);
  fwrite(encoded.data(), encoded.length(), 1, stdout);
  fprintf(stdout, "\x1B\\");
  fflush(stdout);
}

void PlaceCursor(int x, int y) { printf(CSI "%d;%dH", 0, 0); }

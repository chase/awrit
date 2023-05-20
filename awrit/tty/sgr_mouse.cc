// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "sgr_mouse.h"

#include "mouse.h"
#include "output.h"
#include "string_utils.h"

namespace tty::sgr_mouse {

void Enable() {
  using namespace tty::out;
  SetModes({mouse_sgr_pixel_mode, mouse_move_tracking}, true);
  fflush(stdout);
}

std::optional<mouse::MouseEvent> MouseEventFromCSI(
    std::string_view csi) noexcept {
  using namespace mouse;
  using namespace mouse::Button;
  using namespace mouse::Modifier;

  static constexpr Button::Type button_map[]{Left, Middle, Right};
  static constexpr Button::Type extended_map[]{Fourth, Fifth, Sixth, Seventh};
  static constexpr Button::Type wheel_map[]{WheelUp, WheelDown, WheelLeft,
                                            WheelRight};

  // check that its a SGR Pixel mode escape
  if (csi.empty() || (csi.back() != 'm' && csi.back() != 'M') ||
      csi.front() != '<')
    return {};

  char last = csi.back();
  csi.remove_suffix(1);
  csi.remove_prefix(1);

  if (csi.empty()) return {};
  auto parts = string::split(csi, ';');

  if (parts.size() != 3) return {};
  auto desc = string::strtoint(parts[0]);
  if (!desc) return {};

  auto x = string::strtoint(parts[1]);
  if (!x) return {};
  auto y = string::strtoint(parts[2]);
  if (!y) return {};

  MouseEvent result;
  result.x = *x;
  result.y = *y;

  if (last == 'm') {
    result.type = Event::Release;
  } else if (*desc & Motion) {
    result.type = Event::Move;
    result.modifiers |= Motion;
  }

  auto buttons = *desc & 0b11;
  if (*desc >= 1 << 7) {
    result.buttons |= extended_map[buttons];
  } else if (*desc >= 1 << 6) {
    result.buttons |= wheel_map[buttons];
  } else if (buttons < 3) {
    result.buttons |= button_map[buttons];
  }

  result.modifiers |= *desc & (Shift | Alt | Ctrl);

  return result;
}

}  // namespace tty::sgr_mouse

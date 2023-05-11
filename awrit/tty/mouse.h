// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_TTY_MOUSE_H
#define AWRIT_TTY_MOUSE_H

namespace tty::mouse {

namespace Event {
enum Type { Press, Release, Move };
}

namespace Modifier {
enum Type : int {
  Shift = 1 << 2,
  Alt = 1 << 3,
  Ctrl = 1 << 4,
  Motion = 1 << 5,
  None = 0,
};
}

namespace Button {
enum Type : int {
  Left = 1 << 0,
  Middle = 1 << 1,
  Right = 1 << 2,
  Fourth = 1 << 3,
  Fifth = 1 << 4,
  Sixth = 1 << 5,
  Seventh = 1 << 6,
  WheelUp = 1 << 7,
  WheelDown = 1 << 8,
  WheelLeft = 1 << 9,
  WheelRight = 1 << 10,
  None = 0,
};
}

struct MouseEvent {
  Event::Type type = Event::Press;
  int buttons = Button::None;
  int modifiers = Modifier::None;
  int x = -1;
  int y = -1;
};

}  // namespace tty::mouse

#endif  // AWRIT_TTY_MOUSE_H

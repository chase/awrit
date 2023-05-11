// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_KITTY_KEYS_H
#define AWRIT_KITTY_KEYS_H

#include <cstdint>
#include <optional>
#include <string_view>

// see https://sw.kovidgoyal.net/kitty/keyboard-protocol/
namespace tty::keys {

namespace Modifiers {
enum Type : int {
  Shift = 1 << 0,
  Alt = 1 << 1,
  Ctrl = 1 << 2,
  Super = 1 << 3,
  Hyper = 1 << 4,
  Meta = 1 << 5,
  CapsLock = 1 << 6,
  NumLock = 1 << 7,

  None = 0
};

}  // namespace Modifiers

namespace Event {
enum Type : int {
  Down = 1,
  Repeat = 2,
  Up = 3,
};
}

namespace Flags {
enum Type : int {
  DisambiguateEscapeCodes = 1,
  ReportEventTypes = 2,
  ReportAlternateKeys = 4,
  ReportAllKeysAsEscapeCodes = 8,
  ReportAssociatedText = 16,
  None = 0
};
}

struct KeyEvent {
  Event::Type type = Event::Down;
  uint32_t windows_key_code = 0;
  uint32_t key = 0;
  uint32_t shifted_key = 0;
  uint32_t alternate_key = 0;
  int modifiers = 0;
};

void Enable();
void Disable();

std::optional<KeyEvent> KeyEventFromCSI(std::string_view csi) noexcept;

}  // namespace tty::keys

#endif  // AWRIT_KITTY_KEYS_H

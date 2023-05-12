// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_TTY_INPUT_EVENT_H
#define AWRIT_TTY_INPUT_EVENT_H

#include "tty/escape_parser.h"
#include "tty/kitty_keys.h"
#include "tty/mouse.h"

namespace tty {

class InputEventParser : public EscapeCodeParser {
 protected:
  virtual void HandleKey(const tty::keys::KeyEvent&) = 0;
  virtual void HandleMouse(const tty::mouse::MouseEvent&) = 0;
  bool HandleCSI(const std::string& str) override;
};

}  // namespace tty

#endif  // AWRIT_TTY_INPUT_EVENT_H

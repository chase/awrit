// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "input_event.h"
#include "sgr_mouse.h"

namespace tty {

bool InputEventParser::HandleCSI(const std::string& str) {
  auto kc = tty::keys::KeyEventFromCSI(str);
  if (kc) {
    HandleKey(*kc);
    return true;
  }
  auto mc = tty::sgr_mouse::MouseEventFromCSI(str);
  if (mc) {
    HandleMouse(*mc);
    return true;
  }
  return true;
}

}  // namespace tty

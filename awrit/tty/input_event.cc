// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "input_event.h"

namespace tty {

// TODO: actually convert to input events
bool InputEventParser::HandleCSI(const std::string& str) {
  (void)str;
  return true;
}

}  // namespace tty

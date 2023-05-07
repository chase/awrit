// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "tty_input_handler.h"

namespace tty {

// TODO: actually convert to input events
bool InputEventParser::HandleCSI(const std::string& str) { return true; }

}  // namespace tty

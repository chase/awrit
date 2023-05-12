// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_INPUT_EVENT_HANDLER_H
#define AWRIT_INPUT_EVENT_HANDLER_H

#include "tty/input_event.h"

class InputEventParserImpl : public tty::InputEventParser {
protected:
  void HandleKey(const tty::keys::KeyEvent& key_event) override;
  void HandleMouse(const tty::mouse::MouseEvent& key_event) override;
};

#endif  // AWRIT_INPUT_EVENT_HANDLER_H

// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "input_handler.h"

#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <cstdio>

#include "include/base/cef_callback.h"
#include "include/base/cef_ref_counted.h"
#include "tty/input.h"

class InputEventParserImpl : public tty::InputEventParser {
 public:
  InputEventParserImpl(const InputHandler handler) : handler_(handler) {}

 private:
  void HandleKey(const tty::KeyboardInputEvent& event) override {}
  void HandleMouse(const tty::MouseInputEvent& event) override {}

  const InputHandler handler_;
};

void ListenForInput(scoped_refptr<base::RefCountedData<base::AtomicFlag>> quit,
                    InputHandler handler) {
  InputEventParserImpl parser(handler);

  while (!quit->data.IsSet()) {
    if (!tty::WaitForInput()) continue;

    parser.Parse(tty::ReadInput());
  }
}

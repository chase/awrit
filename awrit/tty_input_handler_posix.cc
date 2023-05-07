// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <cstdio>

#include "include/base/cef_callback.h"
#include "tty_input_handler.h"

namespace tty {

struct termios* get_terminal() {
  static struct termios terminal;
  return &terminal;
}

void set_raw(termios& t) {
  t.c_iflag &=
      ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  t.c_oflag &= ~OPOST;
  t.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  t.c_cflag &= ~(CSIZE | PARENB);
  t.c_cflag |= CS8;
  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 0;
}

void SetupTTY() {
  auto* terminal = get_terminal();
  tcgetattr(STDIN_FILENO, terminal);
  set_raw(*terminal);
  tcsetattr(STDIN_FILENO, TCSANOW, terminal);
}

void CleanupTTY() { tcsetattr(STDIN_FILENO, TCSANOW, get_terminal()); }

bool stdin_ready() {
  static constexpr int kTimeout_ms = 20;
  static constexpr int kTimeout_us = kTimeout_ms * 1000;
  static timeval tv = {0, kTimeout_us};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}

void ListenForInput(scoped_refptr<base::AtomicFlag> quit,
                    InputHandler handler) {
  static constexpr size_t kBufferSize = 4096;
  InputEventParser parser(handler);
  std::array<char, kBufferSize> buffer;

  while (!quit->IsSet()) {
    if (!stdin_ready()) continue;

    size_t actual_size = read(STDIN_FILENO, buffer.data(), kBufferSize);
    if (actual_size) {
      parser.Parse({buffer.data(), actual_size});
    }

    // clear for next
    buffer.fill(0);
  }
}

}  // namespace tty

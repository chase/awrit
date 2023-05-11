// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <array>

#include "input.h"

namespace tty::in {

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

void Setup() {
  auto* terminal = get_terminal();
  tcgetattr(STDIN_FILENO, terminal);
  set_raw(*terminal);
  tcsetattr(STDIN_FILENO, TCSANOW, terminal);
}

void Cleanup() { tcsetattr(STDIN_FILENO, TCSANOW, get_terminal()); }

bool WaitForReady(int timeout_ms) {
  const int kTimeout_us = timeout_ms * 1000;

  static timeval tv = {0, kTimeout_us};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}

std::string Read() {
  static constexpr size_t kBufferSize = 4096;
  static std::array<char, kBufferSize> buffer;
  size_t actual_size = read(STDIN_FILENO, buffer.data(), kBufferSize);

  if (actual_size == 0) return {};

  return {buffer.data(), actual_size};
}

}  // namespace tty::in

// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_TTY_INPUT_H
#define AWRIT_TTY_INPUT_H

#include <string>

namespace tty::in {
void Setup();
bool WaitForReady(int timeout_ms = 20);
std::string Read();
void Cleanup();
}  // namespace tty::in

#endif  // AWRIT_TTY_INPUT_H

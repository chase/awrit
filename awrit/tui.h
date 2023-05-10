// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_TUI_H
#define AWRIT_TUI_H

#include "include/cef_base.h"
#include "include/cef_render_handler.h"

CefSize WindowSize();

void Initialize();
void Restore();
void Paint(const std::vector<CefRect>& dirtyRects, const void* buffer,
           int width, int height);

#endif  // AWRIT_TUI_H

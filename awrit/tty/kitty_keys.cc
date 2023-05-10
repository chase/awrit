// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "kitty_keys.h"

#include <cstdio>

#include "escape_codes.h"

namespace tty::keys {

void Enable() {
  fprintf(stdout, CSI ">%d",
          Flags::disambiguate_escape_codes | Flags::report_event_types |
              Flags::report_alternate_keys |
              Flags::report_all_keys_as_escape_codes |
              Flags::report_associated_text);
  fflush(stdout);
}

void Disable() {
  fputs(CSI "<u", stdout);
  fflush(stdout);
}

}  // namespace tty::keys

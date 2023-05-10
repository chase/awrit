// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_INPUT_HANDLER_H
#define AWRIT_INPUT_HANDLER_H

#include <type_traits>

#include "include/base/cef_atomic_flag.h"
#include "include/base/cef_callback.h"
#include "include/base/cef_ref_counted.h"
#include "tty/input_event.h"

using InputHandler = base::RepeatingCallback<void(const tty::InputEvent)>;
void ListenForInput(scoped_refptr<base::RefCountedData<base::AtomicFlag>> quit,
                    InputHandler handler);

#endif  // AWRIT_INPUT_HANDLER_H

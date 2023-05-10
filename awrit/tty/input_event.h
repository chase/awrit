// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_TTY_INPUT_EVENT_H
#define AWRIT_TTY_INPUT_EVENT_H
#include <type_traits>

#include "kitty_keys.h"
#include "escape_parser.h"

namespace tty {

enum class InputType { Mouse, Keyboard };

struct InputEvent {
  const InputType type;

  explicit InputEvent(InputType inputType) : type(inputType) {}

  template <typename T, typename = typename std::enable_if<
                            std::is_base_of<InputEvent, T>::value>::type>
  T* As() {
    static_assert(T::kInputType != InputType(-1), "Type must have kInputType.");
    return type == T::kInputType ? static_cast<T*>(this) : nullptr;
  }
};

struct MouseInputEvent : public InputEvent {
  static const InputType kInputType = InputType::Mouse;
  enum class State { Pressed, Released };

  const int x;
  const int y;
  const int button;
  const State state;

  MouseInputEvent(int x_, int y_, int button_, State state_)
      : InputEvent(kInputType), x(x_), y(y_), button(button_), state(state_) {}
};

struct KeyboardInputEvent : public InputEvent {
  static const InputType kInputType = InputType::Keyboard;
  const uint32_t key;
  const int modifiers;
  const keys::Event event;

  explicit KeyboardInputEvent(uint32_t key_, int modifiers_,
                              keys::Event event_)
      : InputEvent(kInputType),
        key(key_),
        modifiers(modifiers_),
        event(event_) {}
};

class InputEventParser : public EscapeCodeParser {
 protected:
  virtual void HandleKey(const KeyboardInputEvent& event) = 0;
  virtual void HandleMouse(const MouseInputEvent& event) = 0;
  bool HandleCSI(const std::string& str) override;
};

}  // namespace tty

#endif  // AWRIT_TTY_INPUT_EVENT_H

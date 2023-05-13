// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "input_event_handler.h"

#include <locale>
#include <unordered_map>

#include "awrit.h"
#include "output.h"
#include "third_party/keycodes/keyboard_codes_posix.h"
#include "tty/input_event.h"
#include "tty/kitty_keys.h"
#include "tty/mouse.h"

void InputEventParserImpl::HandleKey(const tty::keys::KeyEvent& key_event) {
  using namespace tty::keys;

  if (key_event.modifiers == Modifiers::Ctrl && key_event.key == 'c') {
    AwritClient::GetInstance()->CloseAllBrowsers(true);
    return;
  }

  auto active = AwritClient::GetInstance()->Active();
  if (!active) return;

#ifdef OS_MAC
  // Since there is no UI and consequently no menu on macOS, the Cmd+A shortcut doesn't work as one might expect without this hack
if (key_event.modifiers == Modifiers::Super && key_event.key == 'a') {
  static const std::string kSelectAllInsideInput = R"(
if(['textarea', 'input'].includes(document.activeElement.tagName.toLowerCase())) { document.activeElement.select(); }
  )";
  active->GetMainFrame()->ExecuteJavaScript(kSelectAllInsideInput, active->GetMainFrame()->GetURL(), 0);
  return;
}
#endif

  CefKeyEvent event;
  event.type = key_event.type == Event::Up ? KEYEVENT_KEYUP : KEYEVENT_RAWKEYDOWN;

  if (key_event.type == Event::Repeat) event.modifiers |= EVENTFLAG_IS_REPEAT;

  if (key_event.modifiers & Modifiers::Ctrl)
    event.modifiers |= EVENTFLAG_CONTROL_DOWN;
  if (key_event.modifiers & Modifiers::Alt)
    event.modifiers |= EVENTFLAG_ALT_DOWN;
  if (key_event.modifiers & Modifiers::Shift)
    event.modifiers |= EVENTFLAG_SHIFT_DOWN;
  if (key_event.modifiers & (Modifiers::Meta | Modifiers::Super))
    event.modifiers |= EVENTFLAG_COMMAND_DOWN;
  if (key_event.modifiers & Modifiers::CapsLock)
    event.modifiers |= EVENTFLAG_CAPS_LOCK_ON;
  if (key_event.modifiers & Modifiers::NumLock)
    event.modifiers |= EVENTFLAG_NUM_LOCK_ON;

  switch (key_event.windows_key_code) {
    case KeyboardCode::VKEY_LCONTROL:
    case KeyboardCode::VKEY_LSHIFT:
    case KeyboardCode::VKEY_LMENU:
    case KeyboardCode::VKEY_LWIN:
      event.modifiers &= ~EVENTFLAG_IS_RIGHT;
      event.modifiers |= EVENTFLAG_IS_LEFT;
      break;
    case KeyboardCode::VKEY_RCONTROL:
    case KeyboardCode::VKEY_RSHIFT:
    case KeyboardCode::VKEY_RMENU:
    case KeyboardCode::VKEY_RWIN:
      event.modifiers &= ~EVENTFLAG_IS_LEFT;
      event.modifiers |= EVENTFLAG_IS_RIGHT;
      break;
  }

  event.windows_key_code = key_event.windows_key_code;

  switch (key_event.windows_key_code) {
    case KeyboardCode::VKEY_RCONTROL:
    case KeyboardCode::VKEY_LCONTROL:
      event.character = event.windows_key_code;
      event.unmodified_character = event.windows_key_code;
      event.windows_key_code = KeyboardCode::VKEY_CONTROL;
      event.modifiers |= EVENTFLAG_CONTROL_DOWN;
      break;
    case KeyboardCode::VKEY_LSHIFT:
    case KeyboardCode::VKEY_RSHIFT:
      event.character = event.windows_key_code;
      event.unmodified_character = event.windows_key_code;
      event.windows_key_code = KeyboardCode::VKEY_SHIFT;
      event.modifiers |= EVENTFLAG_SHIFT_DOWN;
      break;
    case KeyboardCode::VKEY_LMENU:
    case KeyboardCode::VKEY_RMENU:
      event.character = event.windows_key_code;
      event.unmodified_character = event.windows_key_code;
      event.windows_key_code = KeyboardCode::VKEY_MENU;
      event.modifiers |= EVENTFLAG_ALT_DOWN;
      break;
    case KeyboardCode::VKEY_LWIN:
    case KeyboardCode::VKEY_RWIN:
      event.character = event.windows_key_code;
      event.unmodified_character = event.windows_key_code;
      event.windows_key_code = KeyboardCode::VKEY_LWIN;
      event.modifiers |= EVENTFLAG_COMMAND_DOWN;
      break;
  }

  // hardware key scan codes for certain keys need to be set to be fixed
  //
  // Reference:
  // https://developer.mozilla.org/en-US/docs/Web/API/UI_Events/Keyboard_event_code_values#code_values_on_linux_x11
  // https://developer.mozilla.org/en-US/docs/Web/API/UI_Events/Keyboard_event_code_values#code_values_on_mac
  // clang-format off
  static const std::unordered_map<KeyboardCode::Type, char16> windows_key_to_native_key {
#if defined(OS_LINUX)
    {KeyboardCode::VKEY_RCONTROL, 0x0069},
    {KeyboardCode::VKEY_LCONTROL, 0x0025},
    {KeyboardCode::VKEY_RSHIFT, 0x003E},
    {KeyboardCode::VKEY_LSHIFT, 0x0032},
    {KeyboardCode::VKEY_RMENU, 0x006C},
    {KeyboardCode::VKEY_LMENU, 0x0040},
    {KeyboardCode::VKEY_RWIN, 0x0086},
    {KeyboardCode::VKEY_LWIN, 0x0087},
#elif defined(OS_MAC)
    {KeyboardCode::VKEY_RCONTROL, 0x003E},
    {KeyboardCode::VKEY_LCONTROL, 0x003B},
    {KeyboardCode::VKEY_CAPITAL, 0x0039},
    {KeyboardCode::VKEY_RSHIFT, 0x003C},
    {KeyboardCode::VKEY_LSHIFT, 0x0038},
    {KeyboardCode::VKEY_RMENU, 0x003D},
    {KeyboardCode::VKEY_LMENU, 0x003A},
    {KeyboardCode::VKEY_RWIN, 0x0036},
    {KeyboardCode::VKEY_LWIN, 0x0037},
    {KeyboardCode::VKEY_RETURN, 0x24},
    {KeyboardCode::VKEY_BACK, 0x33},
    {KeyboardCode::VKEY_LEFT, 0x7B},
    {KeyboardCode::VKEY_UP, 0x7E},
    {KeyboardCode::VKEY_RIGHT, 0x7C},
    {KeyboardCode::VKEY_DOWN, 0x7D},
    {KeyboardCode::VKEY_F1, 0x7A},
    {KeyboardCode::VKEY_F2, 0x78},
    {KeyboardCode::VKEY_F3, 0x63},
    {KeyboardCode::VKEY_F4, 0x76},
    {KeyboardCode::VKEY_F5, 0x60},
    {KeyboardCode::VKEY_F6, 0x61},
    {KeyboardCode::VKEY_F7, 0x62},
    {KeyboardCode::VKEY_F8, 0x64},
    {KeyboardCode::VKEY_F9, 0x65},
    {KeyboardCode::VKEY_F10, 0x6D},
    {KeyboardCode::VKEY_F11, 0x67},
    {KeyboardCode::VKEY_F12, 0x6F}
#endif
  };
  // native key, windows key
  static const std::unordered_map<int, std::tuple<char16, int>> key_to_code {
#if defined(OS_LINUX)
    {'-', {0x14, 189}},
    {'_', {0x14, 189}},
    {'=', {0x15, 187}},
    {'+', {0x15, 187}},
    {'`', {0x31, 192}},
    {'~', {0x31, 192}},
#elif defined(OS_MAC)
    {'-', {0x1B, KeyboardCode::VKEY_OEM_MINUS}},
    {'_', {0x1B, KeyboardCode::VKEY_OEM_MINUS}},
    {'=', {0x18, KeyboardCode::VKEY_OEM_PLUS}},
    {'+', {0x18, KeyboardCode::VKEY_OEM_PLUS}},
    {' ', {0x31, KeyboardCode::VKEY_SPACE}},
    {'a', {0x00, KeyboardCode::VKEY_A}},
    {'A', {0x00, KeyboardCode::VKEY_A}},
    {'s', {0x01, KeyboardCode::VKEY_S}},
    {'S', {0x01, KeyboardCode::VKEY_S}},
    {'d', {0x02, KeyboardCode::VKEY_D}},
    {'D', {0x02, KeyboardCode::VKEY_D}},
    {'f', {0x03, KeyboardCode::VKEY_F}},
    {'F', {0x03, KeyboardCode::VKEY_F}},
    {'g', {0x05, KeyboardCode::VKEY_G}},
    {'G', {0x05, KeyboardCode::VKEY_G}},
    {'h', {0x04, KeyboardCode::VKEY_H}},
    {'H', {0x04, KeyboardCode::VKEY_H}},
    {'j', {0x26, KeyboardCode::VKEY_J}},
    {'J', {0x26, KeyboardCode::VKEY_J}},
    {'k', {0x28, KeyboardCode::VKEY_K}},
    {'K', {0x28, KeyboardCode::VKEY_K}},
    {'l', {0x25, KeyboardCode::VKEY_L}},
    {'L', {0x25, KeyboardCode::VKEY_L}},
    {'z', {0x06, KeyboardCode::VKEY_Z}},
    {'Z', {0x06, KeyboardCode::VKEY_Z}},
    {'x', {0x07, KeyboardCode::VKEY_X}},
    {'X', {0x07, KeyboardCode::VKEY_X}},
    {'c', {0x08, KeyboardCode::VKEY_C}},
    {'C', {0x08, KeyboardCode::VKEY_C}},
    {'v', {0x09, KeyboardCode::VKEY_V}},
    {'V', {0x09, KeyboardCode::VKEY_V}},
    {'b', {0x0B, KeyboardCode::VKEY_B}},
    {'B', {0x0B, KeyboardCode::VKEY_B}},
    {'n', {0x2D, KeyboardCode::VKEY_N}},
    {'N', {0x2D, KeyboardCode::VKEY_N}},
    {'m', {0x2E, KeyboardCode::VKEY_M}},
    {'M', {0x2E, KeyboardCode::VKEY_M}},
    {'0', {0x1D, KeyboardCode::VKEY_0}},
    {'1', {0x12, KeyboardCode::VKEY_1}},
    {'2', {0x13, KeyboardCode::VKEY_2}},
    {'3', {0x14, KeyboardCode::VKEY_3}},
    {'4', {0x15, KeyboardCode::VKEY_4}},
    {'5', {0x17, KeyboardCode::VKEY_5}},
    {'6', {0x16, KeyboardCode::VKEY_6}},
    {'7', {0x1A, KeyboardCode::VKEY_7}},
    {'8', {0x1C, KeyboardCode::VKEY_8}},
    {'9', {0x19, KeyboardCode::VKEY_9}},
    {';', {0x29, KeyboardCode::VKEY_OEM_1}},
    {':', {0x29, KeyboardCode::VKEY_OEM_1}},
    {',', {0x2B, KeyboardCode::VKEY_OEM_COMMA}},
    {'<', {0x2B, KeyboardCode::VKEY_OEM_COMMA}},
    {'.', {0x2F, KeyboardCode::VKEY_OEM_PERIOD}},
    {'>', {0x2F, KeyboardCode::VKEY_OEM_PERIOD}},
    {'/', {0x2C, KeyboardCode::VKEY_OEM_2}},
    {'?', {0x2C, KeyboardCode::VKEY_OEM_2}},
    {'`', {0x32, KeyboardCode::VKEY_OEM_3}},
    {'~', {0x32, KeyboardCode::VKEY_OEM_3}},
    {'[', {0x21, KeyboardCode::VKEY_OEM_4}},
    {'{', {0x21, KeyboardCode::VKEY_OEM_4}},
    {'\\', {0x2A, KeyboardCode::VKEY_OEM_5}},
    {'|', {0x2A, KeyboardCode::VKEY_OEM_5}},
    {']', {0x1E, KeyboardCode::VKEY_OEM_6}},
    {'}', {0x1E, KeyboardCode::VKEY_OEM_6}},
    {'\'', {0x27, KeyboardCode::VKEY_OEM_7}},
    {'"', {0x27, KeyboardCode::VKEY_OEM_7}},
#endif
  };
  // clang-format on

  auto mapped_native_key = windows_key_to_native_key.find(
      (KeyboardCode::Type)key_event.windows_key_code);
  if (mapped_native_key != windows_key_to_native_key.end()) {
    event.native_key_code = mapped_native_key->second;

#ifdef OS_MAC
    event.character = 0;
    event.unmodified_character = 0;
#endif
  }
  if (key_event.key) {
    event.character = key_event.key;
    event.unmodified_character = key_event.key;
    event.windows_key_code =
        std::toupper((wchar_t)key_event.key, std::locale(""));
    // event.native_key_code = key_event.key;
  }

  if (key_event.shifted_key) {
    event.character = key_event.shifted_key;
    event.windows_key_code = key_event.shifted_key;
  }

  if (key_event.key || key_event.shifted_key) {
    auto mapped_code = key_event.shifted_key
                           ? key_to_code.find(key_event.shifted_key)
                           : key_to_code.find(key_event.key);
    if (mapped_code != key_to_code.end()) {
      std::tie(event.native_key_code, event.windows_key_code) =
          mapped_code->second;
    }
  }

  // event.is_system_key = key_event.modifiers;
  event.focus_on_editable_field = !key_event.modifiers;

  active->GetHost()->SendKeyEvent(event);
  if (key_event.type != Event::Up && key_event.key) {
    event.type = KEYEVENT_CHAR;
    active->GetHost()->SendKeyEvent(event);
  }
}

void InputEventParserImpl::HandleMouse(
    const tty::mouse::MouseEvent& mouse_event) {
  auto active = AwritClient::GetInstance()->Active();
  if (!active) return;
  using namespace tty::mouse;

  CefMouseEvent event;
  if (mouse_event.modifiers & Modifier::Ctrl) {
    event.modifiers |= EVENTFLAG_CONTROL_DOWN;
  }
  if (mouse_event.modifiers & Modifier::Alt) {
    event.modifiers |= EVENTFLAG_ALT_DOWN;
  }
  if (mouse_event.modifiers & Modifier::Shift) {
    event.modifiers |= EVENTFLAG_SHIFT_DOWN;
  }
  if (mouse_event.buttons & Button::Left) {
    event.modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
  }
  if (mouse_event.buttons & Button::Right) {
    event.modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
  }
  if (mouse_event.buttons & Button::Middle) {
    event.modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
  }

#if defined(OS_MAC)
  extern float MacGetScale();
  float scale = MacGetScale();
  event.x = mouse_event.x / scale;
  event.y = mouse_event.y / scale;
#else
  event.x = mouse_event.x;
  event.y = mouse_event.y;
#endif

  if (mouse_event.modifiers & Modifier::Motion) {
    active->GetHost()->SendMouseMoveEvent(event, false);
  } else if (mouse_event.buttons &
             (Button::Left | Button::Middle | Button::Right)) {
    cef_mouse_button_type_t type = MBT_LEFT;
    if (mouse_event.buttons & Button::Right) type = MBT_RIGHT;
    if (mouse_event.buttons & Button::Middle) type = MBT_MIDDLE;
    active->GetHost()->SendMouseClickEvent(
        event, type, mouse_event.type == Event::Release, 1);
  } else if (mouse_event.buttons & Button::WheelUp ||
             mouse_event.buttons & Button::WheelDown) {
    active->GetHost()->SendMouseWheelEvent(
        event, 0, mouse_event.buttons & Button::WheelUp ? 10 : -10);
  } else if (mouse_event.buttons & Button::WheelLeft ||
             mouse_event.buttons & Button::WheelRight) {
    active->GetHost()->SendMouseWheelEvent(
        event, mouse_event.buttons & Button::WheelLeft ? -10 : 10, 0);
  }
}

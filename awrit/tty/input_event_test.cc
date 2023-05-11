// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <csignal>
#include <iostream>
#include <thread>
#include <unordered_map>

#include "escape_parser.h"
#include "input.h"
#include "kitty_keys.h"
#include "output.h"
#include "sgr_mouse.h"
#include "third_party/keycodes/keyboard_codes_posix.h"

std::string_view vkey_to_str(KeyboardCode::Type code);

void cleanup(int signum) {
  tty::keys::Disable();
  tty::in::Cleanup();
  tty::out::Cleanup();
  if (signum != SIGUSR1) exit(signum);
}

class InputParser : public tty::EscapeCodeParser {
  bool HandleUTF8Codepoint(uint32_t a) override {
    // std::wcout << "UTF8: " << (wchar_t)a << "\r\n";
    return true;
  };
  bool HandleOSC(const std::string&) override {
    std::cout << "OSC"
              << "\r\n";
    return true;
  };
  bool HandleDCS(const std::string&) override {
    std::cout << "DCS"
              << "\r\n";
    return true;
  };
  bool HandlePM(const std::string&) override {
    std::cout << "PM"
              << "\r\n";
    return true;
  };
  bool HandleSOS(const std::string&) override {
    std::cout << "sos"
              << "\r\n";
    return true;
  };
  bool HandleAPC(const std::string&) override {
    std::cout << "apc"
              << "\r\n";
    return true;
  };

  bool HandleKeyEvent(std::string_view str) {
    auto kc = tty::keys::KeyEventFromCSI(str);
    if (!kc) {
      return false;
    }

    if (kc->modifiers) {
      using namespace tty::keys::Modifiers;
      static const std::unordered_map<tty::keys::Modifiers::Type, std::string>
          typeNames{
              {Shift, "Shift"},       {Alt, "Alt"},         {Ctrl, "Ctrl"},
              {Super, "Super"},       {Hyper, "Hyper"},     {Meta, "Meta"},
              {CapsLock, "CapsLock"}, {NumLock, "NumLock"},
          };

      for (const auto& pair : typeNames) {
        if (kc->modifiers & pair.first) std::cout << pair.second << " + ";
      }
    }

    if (kc->windows_key_code) {
      std::cout << "WC: "
                << vkey_to_str((KeyboardCode::Type)kc->windows_key_code);
    } else {
      std::wcout << "K: " << (wchar_t)kc->key;
    }

    switch (kc->type) {
      case tty::keys::Event::Down:
        std::cout << " DOWN";
        break;
      case tty::keys::Event::Repeat:
        std::cout << " REPEAT";
        break;
      case tty::keys::Event::Up:
        std::cout << " UP";
        break;
    }

    std::cout << "\r\n";

    if (kc->modifiers & tty::keys::Modifiers::Ctrl && kc->key == 'c') {
      cleanup(0);
    }

    return true;
  }

  bool HandleMouseEvent(std::string_view str) {
    auto mc = tty::sgr_mouse::MouseEventFromCSI(str);
    if (!mc) {
      return false;
    }

    std::cout << "(" << mc->x << ", " << mc->y << ") ";

    if (mc->modifiers) {
      using namespace tty::mouse::Modifier;
      static const std::unordered_map<int, std::string> typeNames{
          {Shift, "Shift"},
          {Alt, "Alt"},
          {Ctrl, "Ctrl"},
      };

      for (const auto& pair : typeNames) {
        if (mc->modifiers & pair.first) std::cout << pair.second << " + ";
      }
    }

    {
      using namespace tty::mouse::Event;
      static const std::unordered_map<int, std::string> typeNames{
          {Press, "Press"},
          {Release, "Release"},
          {Move, "Move"},
      };

      for (const auto& pair : typeNames) {
        if (mc->type & pair.first) std::cout << pair.second << " + ";
      }
    }

    if (mc->buttons) {
      using namespace tty::mouse::Button;
      static const std::unordered_map<int, std::string> typeNames{
          {Left, "Left"},
          {Middle, "Middle"},
          {Right, "Right"},
          {Fourth, "Fourth"},
          {Fifth, "Fifth"},
          {Sixth, "Sixth"},
          {Seventh, "Seventh"},
          {WheelUp, "WheelUp"},
          {WheelDown, "WheelDown"},
          {WheelLeft, "WheelLeft"},
          {WheelRight, "WheelRight"},
      };

      for (const auto& pair : typeNames) {
        if (mc->buttons & pair.first) std::cout << pair.second << " ";
      }
    }

    std::cout << "\r\n";

    return true;
  }

  bool HandleCSI(const std::string& str) override {
    if (HandleKeyEvent(str)) return true;
    if (HandleMouseEvent(str)) return true;

    return true;
  }
};

int main() {
  signal(SIGINT, cleanup);
  signal(SIGTERM, cleanup);
  signal(SIGKILL, cleanup);
  signal(SIGSEGV, cleanup);
  signal(SIGUSR1, cleanup);
  tty::in::Setup();
  tty::out::Setup();
  tty::keys::Enable();
  tty::sgr_mouse::Enable();
  bool quit = false;
  InputParser parser;

  // needed to print unicode codepoints from uint32_t cast to wchar_t
  std::setlocale(LC_ALL, "");
  std::wcout.imbue(std::locale(""));

  while (!quit) {
    if (!tty::in::WaitForReady(10)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }
    std::string output = tty::in::Read();
    parser.Parse(output);
  }
}

std::string_view vkey_to_str(KeyboardCode::Type code) {
  using namespace KeyboardCode;
  static const std::unordered_map<KeyboardCode::Type, std::string_view> map{
      {VKEY_ESCAPE, "VKEY_ESCAPE"},
      {VKEY_RETURN, "VKEY_RETURN"},
      {VKEY_TAB, "VKEY_TAB"},
      {VKEY_BACK, "VKEY_BACK"},  // backspace
      {VKEY_INSERT, "VKEY_INSERT"},
      {VKEY_DELETE, "VKEY_DELETE"},
      {VKEY_LEFT, "VKEY_LEFT"},
      {VKEY_RIGHT, "VKEY_RIGHT"},
      {VKEY_UP, "VKEY_UP"},
      {VKEY_DOWN, "VKEY_DOWN"},
      {VKEY_PRIOR, "VKEY_PRIOR"},  // page up
      {VKEY_NEXT, "VKEY_NEXT"},    // page down
      {VKEY_HOME, "VKEY_HOME"},
      {VKEY_END, "VKEY_END"},
      {VKEY_CAPITAL, "VKEY_CAPITAL"},  // caps lock
      {VKEY_SCROLL, "VKEY_SCROLL"},    // scroll lock
      {VKEY_NUMLOCK, "VKEY_NUMLOCK"},
      {VKEY_SNAPSHOT, "VKEY_SNAPSHOT"},  // print screen
      {VKEY_PAUSE, "VKEY_PAUSE"},
      {VKEY_MENU, "VKEY_MENU"},
      {VKEY_F1, "VKEY_F1"},
      {VKEY_F2, "VKEY_F2"},
      {VKEY_F3, "VKEY_F3"},
      {VKEY_F4, "VKEY_F4"},
      {VKEY_F5, "VKEY_F5"},
      {VKEY_F6, "VKEY_F6"},
      {VKEY_F7, "VKEY_F7"},
      {VKEY_F8, "VKEY_F8"},
      {VKEY_F9, "VKEY_F9"},
      {VKEY_F10, "VKEY_F10"},
      {VKEY_F11, "VKEY_F11"},
      {VKEY_F12, "VKEY_F12"},
      {VKEY_F13, "VKEY_F13"},
      {VKEY_F14, "VKEY_F14"},
      {VKEY_F15, "VKEY_F15"},
      {VKEY_F16, "VKEY_F16"},
      {VKEY_F17, "VKEY_F17"},
      {VKEY_F18, "VKEY_F18"},
      {VKEY_F19, "VKEY_F19"},
      {VKEY_F20, "VKEY_F20"},
      {VKEY_F21, "VKEY_F21"},
      {VKEY_F22, "VKEY_F22"},
      {VKEY_F23, "VKEY_F23"},
      {VKEY_F24, "VKEY_F24"},
      {VKEY_NUMPAD0, "VKEY_NUMPAD0"},
      {VKEY_NUMPAD1, "VKEY_NUMPAD1"},
      {VKEY_NUMPAD2, "VKEY_NUMPAD2"},
      {VKEY_NUMPAD3, "VKEY_NUMPAD3"},
      {VKEY_NUMPAD4, "VKEY_NUMPAD4"},
      {VKEY_NUMPAD5, "VKEY_NUMPAD5"},
      {VKEY_NUMPAD6, "VKEY_NUMPAD6"},
      {VKEY_NUMPAD7, "VKEY_NUMPAD7"},
      {VKEY_NUMPAD8, "VKEY_NUMPAD8"},
      {VKEY_NUMPAD9, "VKEY_NUMPAD9"},
      {VKEY_DECIMAL, "VKEY_DECIMAL"},
      {VKEY_DIVIDE, "VKEY_DIVIDE"},
      {VKEY_MULTIPLY, "VKEY_MULTIPLY"},
      {VKEY_SUBTRACT, "VKEY_SUBTRACT"},
      {VKEY_ADD, "VKEY_ADD"},
      {VKEY_SEPARATOR, "VKEY_SEPARATOR"},
      {VKEY_MEDIA_PLAY, "VKEY_MEDIA_PLAY"},
      {VKEY_MEDIA_PAUSE, "VKEY_MEDIA_PAUSE"},
      {VKEY_MEDIA_PLAY_PAUSE, "VKEY_MEDIA_PLAY_PAUSE"},
      {VKEY_MEDIA_STOP, "VKEY_MEDIA_STOP"},
      {VKEY_MEDIA_NEXT_TRACK, "VKEY_MEDIA_NEXT_TRACK"},
      {VKEY_MEDIA_PREV_TRACK, "VKEY_MEDIA_PREV_TRACK"},
      {VKEY_VOLUME_DOWN, "VKEY_VOLUME_DOWN"},
      {VKEY_VOLUME_UP, "VKEY_VOLUME_UP"},
      {VKEY_VOLUME_MUTE, "VKEY_VOLUME_MUTE"},
      {VKEY_LSHIFT, "VKEY_LSHIFT"},
      {VKEY_LCONTROL, "VKEY_LCONTROL"},
      {VKEY_LMENU, "VKEY_LMENU"},  // alt
      {VKEY_LWIN, "VKEY_LWIN"},
      {VKEY_RSHIFT, "VKEY_RSHIFT"},
      {VKEY_RCONTROL, "VKEY_RCONTROL"},
      {VKEY_RMENU, "VKEY_RMENU"},  // alt
      {VKEY_RWIN, "VKEY_RWIN"},
  };

  auto result = map.find(code);
  return result == map.end() ? "VKEY_UNKNOWN" : result->second;
}

// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "escape_parser.h"

namespace tty {

namespace {
csi::Char csi_type(char ch) {
  if ((0x30 <= ch && ch <= 0x3f) || ch == '-') {
    return csi::Char::Parameter;
  }
  if (0x40 <= ch && ch <= 0x7E) {
    return csi::Char::Final;
  }
  if (0x20 <= ch && ch <= 0x2F) {
    return csi::Char::Intermediate;
  }
  return csi::Char::Unknown;
}

[[noreturn]] inline void unreachable() {
#if defined(__GNUC__)  // GCC, Clang, ICC
  __builtin_unreachable();
#elif defined(_MSC_VER)  // MSVC
  __assume(false);
#endif
}

}  // namespace

bool EscapeCodeParser::Reset() {
  buffer_ = {};
  state_ = State::Normal;
  utf8_state_ = utf8::kAccept;
  utf8_codepoint_ = 0;
  handler_ = Handler::None;
  csi_state_ = csi::State::Parameter;

  return false;
}

bool EscapeCodeParser::Parse(char ch) {
  switch (state_) {
    case State::Normal: {
      auto prior_state = utf8_state_;
      switch (utf8::decode(&utf8_state_, &utf8_codepoint_, ch)) {
        case utf8::kAccept:
          if (!UTF8Codepoint(utf8_codepoint_)) {
            return Reset();
          }
          return true;
        case utf8::kReject:
          if (prior_state != utf8::kAccept) {
            return Parse(ch);
          }
          return true;
      }
      return true;
    }

    case State::ESC:
    case State::CSI:
    case State::ST:
    case State::ST_or_BEL:
    case State::ESC_ST:
    case State::C1_ST:
      if (!Byte(ch)) return false;
  }

  return true;
}

bool EscapeCodeParser::Parse(std::string_view buffer) {
  for (char ch : buffer) {
    if (!Parse(ch)) return false;
  }
  return true;
}

bool EscapeCodeParser::UTF8Codepoint(uint32_t ch) {
  switch (ch) {
    case 0x1b:
      state_ = State::ESC;
      break;
    case 0x90:
      state_ = State::ST;
      handler_ = Handler::DCS;
      break;
    case 0x9b:
      state_ = State::CSI;
      handler_ = Handler::CSI;
      break;
    case 0x9d:
      state_ = State::ST_or_BEL;
      handler_ = Handler::OSC;
      break;
    case 0x98:
      state_ = State::ST;
      handler_ = Handler::SOS;
      break;
    case 0x9e:
      state_ = State::ST;
      handler_ = Handler::PM;
      break;
    case 0x9f:
      state_ = State::ST;
      handler_ = Handler::APC;
      break;
    default:
      HandleUTF8Codepoint(ch);
      break;
  }

  return true;
}

bool EscapeCodeParser::Byte(uint8_t ch) {
  switch (state_) {
    case State::ESC:
      return ESC(ch);
    case State::CSI:
      return CSI(ch);
    case State::ST_or_BEL:
      if (ch == 0x7) return EscapeCode();
      [[fallthrough]];
    case State::ST:
      return ST(ch);
    case State::ESC_ST:
      return ESC_ST(ch);
    case State::C1_ST:
      return C1_ST(ch);
    case State::Normal:
      return true;
    default:
      unreachable();
  }

  unreachable();
}

bool EscapeCodeParser::ESC(uint8_t ch) {
  switch (ch) {
    case 'P':
      state_ = State::ST;
      handler_ = Handler::DCS;
      break;
    case '[':
      state_ = State::CSI;
      handler_ = Handler::CSI;
      break;
    case ']':
      state_ = State::ST_or_BEL;
      handler_ = Handler::OSC;
      break;
    case '^':
      state_ = State::ST;
      handler_ = Handler::PM;
      break;
    case '_':
      state_ = State::ST;
      handler_ = Handler::APC;
      break;
    default:
      // drop dangling <ESC>, try again
      Reset();
      return Parse(ch);
  }
  return true;
}

bool EscapeCodeParser::CSI(uint8_t ch) {
  buffer_ += ch;
  switch (csi_state_) {
    case csi::State::Parameter:
      switch (csi_type(ch)) {
        case csi::Char::Unknown:
          Invalid();
          break;
        case csi::Char::Intermediate:
          csi_state_ = csi::State::Intermediate;
          break;
        case csi::Char::Final:
          return EscapeCode();
        case csi::Char::Parameter:
          break;
      }
      break;
    case csi::State::Intermediate:
      switch (csi_type(ch)) {
        case csi::Char::Unknown:
        case csi::Char::Parameter:
          Invalid();
          break;
        case csi::Char::Final:
          return EscapeCode();
        case csi::Char::Intermediate:
          break;
      }
      break;
  }
  return true;
}

bool EscapeCodeParser::ST(uint8_t ch) {
  switch (ch) {
    case 0x1b:
      state_ = State::ESC_ST;
      break;
    case 0xc2:
      state_ = State::C1_ST;
      break;
    default:
      buffer_ += ch;
      break;
  }
  return true;
}

bool EscapeCodeParser::ESC_ST(uint8_t ch) {
  if (ch == '\\') {
    return EscapeCode();
  } else {
    state_ = State::ST;
    buffer_ += '\x1b';
    if (ch != 0x1b) buffer_ += ch;
  }
  return true;
}

bool EscapeCodeParser::C1_ST(uint8_t ch) {
  if (ch == 0x9c) return EscapeCode();

  state_ = State::ST;
  buffer_ += '\xc2';
  buffer_ += ch;
  return true;
}

bool EscapeCodeParser::EscapeCode() {
  bool result = true;
  switch (handler_) {
    case Handler::CSI:
      result = HandleCSI(buffer_);
      break;
    case Handler::OSC:
      result = HandleOSC(buffer_);
      break;
    case Handler::DCS:
      result = HandleDCS(buffer_);
      break;
    case Handler::PM:
      result = HandlePM(buffer_);
      break;
    case Handler::SOS:
      result = HandleSOS(buffer_);
      break;
    case Handler::APC:
      result = HandleAPC(buffer_);
      break;
    case Handler::None:
      break;
  }
  Reset();
  return result;
}

}  // namespace tty

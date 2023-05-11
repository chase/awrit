// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "kitty_keys.h"

#include <charconv>
#include <cstdio>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "escape_codes.h"
#include "string/string_utils.h"
#include "third_party/keycodes/keyboard_codes_posix.h"

namespace tty::keys {

void Enable() {
  fprintf(stdout, CSI ">%du",
          Flags::DisambiguateEscapeCodes | Flags::ReportEventTypes |
              Flags::ReportAlternateKeys | Flags::ReportAllKeysAsEscapeCodes |
              Flags::ReportAssociatedText);
  fflush(stdout);
}

void Disable() {
  fputs(CSI "<u", stdout);
  fflush(stdout);
}

namespace {

KeyboardCode::Type functional_key_number_to_vkey(uint16_t key_number) {
  using namespace KeyboardCode;
  static const std::unordered_map<uint16_t, KeyboardCode::Type> map{
      {57344, VKEY_ESCAPE},
      {57345, VKEY_RETURN},
      {57346, VKEY_TAB},
      {57347, VKEY_BACK},  // backspace
      {57348, VKEY_INSERT},
      {57349, VKEY_DELETE},
      {57350, VKEY_LEFT},
      {57351, VKEY_RIGHT},
      {57352, VKEY_UP},
      {57353, VKEY_DOWN},
      {57354, VKEY_PRIOR},  // page up
      {57355, VKEY_NEXT},   // page down
      {57356, VKEY_HOME},
      {57357, VKEY_END},
      {57358, VKEY_CAPITAL},  // caps lock
      {57359, VKEY_SCROLL},   // scroll lock
      {57360, VKEY_NUMLOCK},
      {57361, VKEY_SNAPSHOT},  // print screen
      {57362, VKEY_PAUSE},
      {57363, VKEY_MENU},
      {57364, VKEY_F1},
      {57365, VKEY_F2},
      {57366, VKEY_F3},
      {57367, VKEY_F4},
      {57368, VKEY_F5},
      {57369, VKEY_F6},
      {57370, VKEY_F7},
      {57371, VKEY_F8},
      {57372, VKEY_F9},
      {57373, VKEY_F10},
      {57374, VKEY_F11},
      {57375, VKEY_F12},
      {57376, VKEY_F13},
      {57377, VKEY_F14},
      {57378, VKEY_F15},
      {57379, VKEY_F16},
      {57380, VKEY_F17},
      {57381, VKEY_F18},
      {57382, VKEY_F19},
      {57383, VKEY_F20},
      {57384, VKEY_F21},
      {57385, VKEY_F22},
      {57386, VKEY_F23},
      {57387, VKEY_F24},
      {57399, VKEY_NUMPAD0},
      {57400, VKEY_NUMPAD1},
      {57401, VKEY_NUMPAD2},
      {57402, VKEY_NUMPAD3},
      {57403, VKEY_NUMPAD4},
      {57404, VKEY_NUMPAD5},
      {57405, VKEY_NUMPAD6},
      {57406, VKEY_NUMPAD7},
      {57407, VKEY_NUMPAD8},
      {57408, VKEY_NUMPAD9},
      {57409, VKEY_DECIMAL},
      {57410, VKEY_DIVIDE},
      {57411, VKEY_MULTIPLY},
      {57412, VKEY_SUBTRACT},
      {57413, VKEY_ADD},
      {57414, VKEY_RETURN},
      {57416, VKEY_SEPARATOR},
      {57417, VKEY_LEFT},
      {57418, VKEY_RIGHT},
      {57419, VKEY_UP},
      {57420, VKEY_DOWN},
      {57421, VKEY_PRIOR},  // page up
      {57422, VKEY_NEXT},   // page down
      {57423, VKEY_HOME},
      {57424, VKEY_END},
      {57425, VKEY_INSERT},
      {57426, VKEY_DELETE},
      {57428, VKEY_MEDIA_PLAY},
      {57429, VKEY_MEDIA_PAUSE},
      {57430, VKEY_MEDIA_PLAY_PAUSE},
      {57432, VKEY_MEDIA_STOP},
      {57435, VKEY_MEDIA_NEXT_TRACK},
      {57436, VKEY_MEDIA_PREV_TRACK},
      {57438, VKEY_VOLUME_DOWN},
      {57439, VKEY_VOLUME_UP},
      {57440, VKEY_VOLUME_MUTE},
      {57441, VKEY_LSHIFT},
      {57442, VKEY_LCONTROL},
      {57443, VKEY_LMENU},  // alt
      {57444, VKEY_LWIN},
      {57445, VKEY_LWIN},
      {57446, VKEY_LWIN},
      {57447, VKEY_RSHIFT},
      {57448, VKEY_RCONTROL},
      {57449, VKEY_RMENU},  // alt
      {57450, VKEY_RWIN},
      {57451, VKEY_RWIN},
      {57452, VKEY_RWIN},
  };
  auto result = map.find(key_number);
  return result == map.end() ? VKEY_UNKNOWN : result->second;
}

std::optional<uint16_t> csi_number_to_functional_number(uint16_t csi) {
  static const std::unordered_map<uint16_t, uint16_t> map{
      {2, 57348},   {3, 57349},  {5, 57354},  {6, 57355},  {7, 57356},
      {8, 57357},   {9, 57346},  {11, 57364}, {12, 57365}, {13, 57345},
      {14, 57367},  {15, 57368}, {17, 57369}, {18, 57370}, {19, 57371},
      {20, 57372},  {21, 57373}, {23, 57374}, {24, 57375}, {27, 57344},
      {127, 57347},
  };
  auto result = map.find(csi);
  if (result == map.end()) return {};

  return result->second;
}

std::optional<uint16_t> letter_trailer_to_csi_number(char trailer) {
  static const std::unordered_map<char, uint16_t> map{
      {'A', 57352}, {'B', 57353}, {'C', 57351}, {'D', 57350}, {'E', 57427},
      {'F', 8},     {'H', 7},     {'P', 11},    {'Q', 12},    {'S', 14},
  };
  auto result = map.find(trailer);
  if (result == map.end()) return {};

  return result->second;
};

std::vector<int> get_sub_sections(std::string_view section,
                                  int missing) noexcept {
  std::vector<std::string_view> p = string::split(section, ':');
  std::vector<int> result(p.size(), missing);

  for (size_t i = 0; i < p.size(); ++i) {
    if (p[i].empty()) {
      result[i] = missing;
    } else {
      const std::from_chars_result conv =
          std::from_chars(p[i].begin(), p[i].end(), result[i]);
      if (conv.ec != std::errc()) return {};
    }
  }

  return result;
}

}  // namespace

std::optional<KeyEvent> KeyEventFromCSI(std::string_view csi) noexcept {
  if (csi.empty()) {
    return {};
  }

  char last_char = csi.back();
  csi.remove_suffix(1);

  static constexpr std::string_view possible_trailers{"u~ABCDEHFPQRS"};

  if (possible_trailers.find(last_char) == std::string_view::npos ||
      (last_char == '~' && (csi == "200" || csi == "201"))) {
    return {};
  }

  std::vector<std::string_view> sections = string::split(csi, ';');
  std::vector<int> first_section;
  if (sections.size() > 0) first_section = get_sub_sections(sections[0], 0);
  std::vector<int> second_section;
  if (sections.size() > 1) second_section = get_sub_sections(sections[1], 1);
  std::vector<int> third_section;
  if (sections.size() > 2) third_section = get_sub_sections(sections[2], 0);

  KeyEvent result = {};
  uint32_t keynum = 0;
  auto maybe_csi_number = letter_trailer_to_csi_number(last_char);
  if (maybe_csi_number) {
    keynum = maybe_csi_number.value();
  } else {
    if (first_section.empty()) return {};
    keynum = first_section[0];
  }

  if (keynum == 13) {
    if (last_char == 'u')
      result.windows_key_code = KeyboardCode::VKEY_RETURN;
    else
      result.windows_key_code = KeyboardCode::VKEY_F3;
  } else if (keynum != 0) {
    auto functional_number = csi_number_to_functional_number(keynum);
    if (functional_number) {
      keynum = functional_number.value();
    }
    result.windows_key_code = functional_key_number_to_vkey(keynum);
  }

  if (!result.windows_key_code) {
    result.key = keynum;
  }
  if (first_section.size() > 1) {
    result.shifted_key = first_section[1];
  }
  if (first_section.size() > 2) {
    result.shifted_key = first_section[2];
  }
  if (second_section.size() > 0) {
    result.modifiers = second_section[0] - 1;
  }
  if (second_section.size() > 1) {
    result.type = (Event::Type)second_section[1];
  }

  return result;
}
}  // namespace tty::keys

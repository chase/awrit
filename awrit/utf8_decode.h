// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#ifndef AWRIT_UTF8_DECODE_H
#define AWRIT_UTF8_DECODE_H

#include <cstdint>

namespace utf8 {

using State = uint32_t;

static constexpr uint32_t kAccept = 0;
static constexpr uint32_t kReject = 1;

/**
UTF-8 is a variable length character encoding. To decode a character one or more
bytes have to be read from a string. The decode function implements a single
step in this process. It takes two parameters maintaining state and a byte, and
returns the state achieved after processing the byte. Specifically, it returns
the value UTF8_ACCEPT (0) if enough bytes have been read for a character,
UTF8_REJECT (1) if the byte is not allowed to occur at its position, and some
other positive value if more bytes have to be read.

When decoding the first byte of a string, the caller must set the state variable
to UTF8_ACCEPT. If, after decoding one or more bytes the state UTF8_ACCEPT is
reached again, then the decoded Unicode character value is available through the
codep parameter. If the state UTF8_REJECT is entered, that state will never be
exited unless the caller intervenes. See the examples below for more information
on usage and error handling, and the section on implementation details for how
the decoder is constructed.
*/
State inline decode(State* state, uint32_t* code_point, uint8_t byte) {
  // clang-format off
static constexpr uint8_t utf8_data[] = {
  // The first part of the table maps bytes to character classes that
  // to reduce the size of the transition table and create bitmasks.
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

  // The second part is a transition table that maps a combination
  // of a state of the automaton and a character class to a state.
   0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
  12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
  12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
  12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
  12,36,12,12,12,12,12,12,12,12,12,12,
};
  // clang-format on

  uint32_t type = utf8_data[byte];

  *code_point = (*state != kAccept) ? (byte & 0x3fu) | (*code_point << 6)
                                    : (0xff >> type) & (byte);

  *state = utf8_data[256 + *state + type];
  return *state;
}

}  // namespace utf8

#endif  // AWRIT_UTF8_DECODE_H

/*
Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#ifndef UTF8_DECODE_H
#define UTF8_DECODE_H

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
State decode(State* state, uint32_t* code_point, uint8_t byte);

}  // namespace utf8

#endif  // UTF8_DECODE_H

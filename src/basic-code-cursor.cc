// Copyright 2024 Maicol Castro (maicolcastro.abc@gmail.com).
// Distributed under the BSD 3-Clause License.
// See LICENSE.txt in the root directory of this project
// or at https://opensource.org/license/bsd-3-clause.

#include <skite/basic-code-cursor.hh>

using namespace skite;

BasicCodeCursor::BasicCodeCursor(const char* chars, size_t size) :
    chars(chars), chars_size(size), current_state { 0, 1, 0 }
{
    states.reserve(4);
}

bool BasicCodeCursor::advance() {
    if (current_state.offset >= chars_size)
        return false;

    current_state.offset += 1;

    if (chars[current_state.offset] == '\n') {
        current_state.line_number += 1;
        current_state.column_number = 0;
    }

    return true;
}

char32_t BasicCodeCursor::get() {
    return chars[current_state.offset];
}

void BasicCodeCursor::push_state() {
    states.push_back(current_state);
}

void BasicCodeCursor::pop_state() {
    if (states.size() != 0) {
        current_state = states.back();
        states.pop_back();
    }
    else {
        current_state = { 0, 1, 0 };
    }
}

void BasicCodeCursor::delete_previous_state() {
    states.pop_back();
}

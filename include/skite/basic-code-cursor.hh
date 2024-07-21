// Copyright 2024 Maicol Castro (maicolcastro.abc@gmail.com).
// Distributed under the BSD 3-Clause License.
// See LICENSE.txt in the root directory of this project
// or at https://opensource.org/license/bsd-3-clause.

#pragma once

#include "code-cursor.hh"
#include <vector>
#include <cstddef>

namespace skite {

class BasicCodeCursor final : public CodeCursor {
public:
    BasicCodeCursor(const char* chars, size_t size);

    /// Advances to the next character.
    /// Returns false if the cursor reaches the end.
    bool advance();

    /// Returns the current character.
    char32_t get();

    /// Pushes the current state of the cursor.
    /// To recover it use `pop_state`.
    void push_state();

    /// Recovers the previous pushed state, or reset the cursor if
    /// none is found.
    void pop_state();

    /// Deletes the last state pushed, or do nothing if no one is found.
    void delete_previous_state();

    /// Returns the line number of the current state.
    inline int get_line_number() {
        return current_state.line_number;
    }

    /// Returns the column number of the current state.
    inline int get_column_number() {
        return current_state.column_number;
    }

private:
    struct State {
        int offset;
        int line_number;
        int column_number;
    };

    const char* chars;
    size_t chars_size;

    State current_state;
    std::vector<State> states;
};

} // namespace skite

// Copyright 2024 Maicol Castro (maicolcastro.abc@gmail.com).
// Distributed under the BSD 3-Clause License.
// See LICENSE.txt in the root directory of this project
// or at https://opensource.org/license/bsd-3-clause.

#pragma once

namespace skite {

class CodeCursor {
public:
    virtual ~CodeCursor() = default;

    /// Advances to the next character.
    /// Returns false if the cursor reaches the end.
    virtual bool advance() = 0;

    /// Returns the current character.
    virtual char32_t get() = 0;

    /// Pushes the current state of the cursor.
    /// To recover it use `pop_state`.
    virtual void push_state() = 0;

    /// Recovers the previous pushed state, or reset the cursor if
    /// none is found.
    virtual void pop_state() = 0;

    /// Deletes the last state pushed.
    virtual void delete_previous_state() = 0;

    /// Returns the line number of the current state.
    virtual int get_line_number() = 0;

    /// Returns the column number of the current state.
    virtual int get_column_number() = 0;
};

} // namespace skite

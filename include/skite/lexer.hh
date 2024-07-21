// Copyright 2024 Maicol Castro (maicolcastro.abc@gmail.com).
// Distributed under the BSD 3-Clause License.
// See LICENSE.txt in the root directory of this project
// or at https://opensource.org/license/bsd-3-clause.

#pragma once

#include "token.hh"
#include "code-cursor.hh"

namespace skite {

class Lexer {
public:
    void init(CodeCursor* cursor);

    void advance();

    /// Returns a reference to the current token.
    inline Token const& get() const {
        return token;
    }

    /// Takes the current token.
    /// @note subsequent calls to this function or `get`,
    ///       will return a token of kind `TOKEN_KIND_NONE`.
    /// @note call `advance` to update the current token.
    inline Token take() {
        return std::move(token);
    }

private:
    CodeCursor* cursor;
    char32_t current_char;

    Token token;

    /// Pushes the current lexer state.
    /// @note use `pop_state` to recover the last state pushed.
    void push_state();

    /// Pops the last lexer state pushed.
    void pop_state();

    /// Deletes the last state pushed.
    void delete_previous_state();

    /// Gets the next characters and updates `current_char`.
    char32_t next_char();

    /// Skips whitespace characters.
    void skip_whitespace();

    /// Attempts to read an identifier into `output`,
    /// returns false if none is found, otherwise true.
    bool read_identifier_raw(std::string& output);

    bool read_puntuaction();
    bool read_separator();
    bool read_identifier_or_keyword();
    bool read_string();
    bool read_number();
    bool read_comment();
    bool read_special();
    void process_single_line_comment();
    void process_multi_line_comment();
    bool process_escape_sequence(std::string& output);
};

} // namespace skite

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

    /// Advances to the next token and returns it.
    Token next();

private:
    CodeCursor* cursor;

    char32_t current_char;

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

    bool process_escape_sequence(std::string& output);

    bool read_puntuaction(Token& token);
    bool read_separator(Token& token);
    bool read_identifier_or_keyword(Token& token);
    bool read_string(Token& token);
    bool read_number(Token& token);
    bool read_comment(Token& token);
    bool read_special(Token& token);

    void process_single_line_comment(Token& token);
    void process_multi_line_comment(Token& token);
};

} // namespace skite::lexer

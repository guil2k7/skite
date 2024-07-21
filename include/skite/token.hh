// Copyright 2024 Maicol Castro (maicolcastro.abc@gmail.com).
// Distributed under the BSD 3-Clause License.
// See LICENSE.txt in the root directory of this project
// or at https://opensource.org/license/bsd-3-clause.

#pragma once

#include <string>

namespace skite {

enum TokenKind {
    TOKEN_KIND_NONE,
    TOKEN_KIND_ERROR,
    TOKEN_KIND_STRING,
    TOKEN_KIND_COMMENT,
    TOKEN_KIND_INTEGER,
    TOKEN_KIND_IDENTIFIER,
    TOKEN_KIND_KEYWORD,
    TOKEN_KIND_PUNTUACTION,
    TOKEN_KIND_SEPARATOR,
};

enum Keyword {
    KEYWORD_PUBLIC,
    KEYWORD_NEW,
    KEYWORD_RETURN,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
};

enum Separator {
    SEPARATOR_LPAR,
    SEPARATOR_RPAR,
    SEPARATOR_LSQB,
    SEPARATOR_RSQB,
    SEPARATOR_LBRACE,
    SEPARATOR_RBRACE,
    SEPARATOR_SEMI,
    SEPARATOR_COMMA,
};

enum Puntuaction {
    PUNTUACTION_PLUS,
    PUNTUACTION_MINUS,
    PUNTUACTION_STAR,
    PUNTUACTION_SLASH,
};

enum LexerError {
    LEXER_ERROR_UNKNOWN_CHAR,
    LEXER_ERROR_UNKNOWN_ESCAPE,
    // LEXER_ERROR_EXPECTED_STRING_END,
};

union TokenValue {
    TokenValue() : number { 0 } {}

    TokenValue(TokenKind kind, TokenValue const& that);
    TokenValue(TokenKind kind, TokenValue&& that);

    ~TokenValue() {}

    void release(TokenKind kind);

    std::string string;
    std::string identifier;

    struct {
        bool is_multiline;
        bool is_documentation;
        std::string text;
    } comment;

    union {
        uintptr_t u;
        intptr_t i;
        double f;
    } number;

    Keyword keyword;
    Separator separator;
    Puntuaction puntuaction;
    LexerError error;
};

struct TokenPosition {
    int line = 0;
    int column = 0;
    int line_end = 0;
    int column_end = 0;
};

struct Token {
    Token(TokenPosition position = {}) :
        kind(TOKEN_KIND_NONE), value {}, position(position) {}

    Token(TokenKind kind, TokenValue const& value, TokenPosition position = {}) :
        kind(kind), value(kind, value), position(position) {}

    Token(TokenKind kind, TokenValue&& value, TokenPosition position = {}) :
        kind(kind), value(kind, std::move(value)), position(position) {}

    Token(Token&& that) :
        kind(that.kind), value(that.kind, std::move(that.value))
    {
        that.kind = TOKEN_KIND_NONE;
    }

    inline ~Token() {
        value.release(kind);
    }

    inline void release_value() {
        value.release(kind);
        kind = TOKEN_KIND_NONE;
    }

    TokenKind kind;
    TokenValue value;
    TokenPosition position;
};

} // namespace skite

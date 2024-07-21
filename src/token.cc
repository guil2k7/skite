// Copyright 2024 Maicol Castro (maicolcastro.abc@gmail.com).
// Distributed under the BSD 3-Clause License.
// See LICENSE.txt in the root directory of this project
// or at https://opensource.org/license/bsd-3-clause.

#include <skite/token.hh>

using namespace skite;

TokenValue::TokenValue(TokenKind kind, TokenValue const& that) {
    switch (kind) {
    case TOKEN_KIND_IDENTIFIER:
        identifier = that.identifier;
        break;

    case TOKEN_KIND_PUNTUACTION:
        puntuaction = that.puntuaction;
        break;

    case TOKEN_KIND_INTEGER:
        number = that.number;
        break;

    case TOKEN_KIND_SEPARATOR:
        separator = that.separator;
        break;

    case TOKEN_KIND_KEYWORD:
        keyword = that.keyword;
        break;

    case TOKEN_KIND_COMMENT:
        comment = that.comment;
        break;

    case TOKEN_KIND_STRING:
        string = that.string;
        break;

    case TOKEN_KIND_END:
        break;

    case TOKEN_KIND_ERROR:
        error = that.error;
        break;
    }
}

TokenValue::TokenValue(TokenKind kind, TokenValue&& that) {
    switch (kind) {
    case TOKEN_KIND_IDENTIFIER:
        identifier = std::move(that.identifier);
        break;

    case TOKEN_KIND_PUNTUACTION:
        puntuaction = that.puntuaction;
        break;

    case TOKEN_KIND_INTEGER:
        number = that.number;
        break;

    case TOKEN_KIND_SEPARATOR:
        separator = that.separator;
        break;

    case TOKEN_KIND_KEYWORD:
        keyword = that.keyword;
        break;

    case TOKEN_KIND_COMMENT:
        comment = std::move(that.comment);
        break;

    case TOKEN_KIND_STRING:
        string = std::move(that.string);
        break;

    case TOKEN_KIND_END:
        break;

    case TOKEN_KIND_ERROR:
        error = that.error;
        break;
    }
}

void TokenValue::release(TokenKind kind) {
    switch (kind) {
    case TOKEN_KIND_STRING:
        string.~basic_string();
        break;

    case TOKEN_KIND_COMMENT:
        comment.text.~basic_string();
        break;

    case TOKEN_KIND_IDENTIFIER:
        identifier.~basic_string();
        break;

    default:
        break;
    }
}

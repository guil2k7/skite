// Copyright 2024 Maicol Castro (maicolcastro.abc@gmail.com).
// Distributed under the BSD 3-Clause License.
// See LICENSE.txt in the root directory of this project
// or at https://opensource.org/license/bsd-3-clause.

#include <skite/lexer.hh>
#include <unordered_map>

using namespace skite;

static std::unordered_map<std::string_view, Keyword> KEYWORD_MAP;

static void init_keyword_map() {
    KEYWORD_MAP.insert({"public", KEYWORD_PUBLIC});
    KEYWORD_MAP.insert({"new", KEYWORD_NEW});
    KEYWORD_MAP.insert({"return", KEYWORD_RETURN});
    KEYWORD_MAP.insert({"if", KEYWORD_IF});
    KEYWORD_MAP.insert({"else", KEYWORD_ELSE});
    KEYWORD_MAP.insert({"while", KEYWORD_WHILE});
    KEYWORD_MAP.insert({"for", KEYWORD_FOR});
}

static inline bool is_digit(char32_t chr) {
    return chr >= '0' && chr <= '9';
}

static inline bool is_alpha(char32_t chr) {
    return (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z');
}

static inline bool is_alphanum(char32_t chr) {
    return is_alpha(chr) || is_digit(chr);
}

void Lexer::init(CodeCursor* cursor) {
    if (KEYWORD_MAP.size() == 0)
        init_keyword_map();

    this->cursor = cursor;

    // The code cursor is lazy, so to obtain the first character we need to call
    // `next`.
    next_char();
}

void Lexer::advance() {
    static bool (Lexer::*const READ_TABLE[])() = {
        &Lexer::read_comment,
        &Lexer::read_puntuaction,
        &Lexer::read_separator,
        &Lexer::read_identifier_or_keyword,
        &Lexer::read_number,
        &Lexer::read_string,
        &Lexer::read_special
    };

    constexpr size_t READ_TABLE_SIZE
        = sizeof READ_TABLE / sizeof READ_TABLE[0];

    if (token.kind != TOKEN_KIND_NONE)
        token.release_value();

    skip_whitespace();

    token.position.line = cursor->get_line_number();
    token.position.column = cursor->get_column_number();

    for (size_t i = 0; i < READ_TABLE_SIZE; ++i) {
        if ((*this.*READ_TABLE[i])())
            break;
    }

    token.position.line_end = cursor->get_line_number();
    token.position.column_end = cursor->get_column_number();
}

inline void Lexer::push_state() {
    cursor->push_state();
}

inline void Lexer::pop_state() {
    cursor->pop_state();
    current_char = cursor->get();
}

inline char32_t Lexer::next_char() {
    current_char = cursor->advance() ? cursor->get() : '\0';
    return current_char;
}

void Lexer::skip_whitespace() {
    char chr = current_char;

    for (;;) {
        switch (chr) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            chr = next_char();
            break;

        default:
            return;
        }
    }
}

bool Lexer::read_identifier_raw(std::string& output) {
    if (!is_alpha(current_char))
        return false;

    output.reserve(12);

    char chr = current_char;

    do {
        output.push_back(chr);
        chr = next_char();
    } while (is_alphanum(chr) || chr == '_');

    return true;
}

bool Lexer::read_puntuaction() {
    Puntuaction puntuaction;

    switch (current_char) {
    case '+': puntuaction = PUNTUACTION_PLUS; break;
    case '-': puntuaction = PUNTUACTION_MINUS; break;
    case '*': puntuaction = PUNTUACTION_STAR; break;
    case '/': puntuaction = PUNTUACTION_SLASH; break;

    default:
        return false;
    }

    next_char();

    token.kind = TOKEN_KIND_PUNTUACTION;
    token.value.puntuaction = puntuaction;

    return true;
}

bool Lexer::read_separator() {
    Separator separator;

    switch (current_char) {
    case '(': separator = SEPARATOR_LPAR; break;
    case ')': separator = SEPARATOR_RPAR; break;
    case '[': separator = SEPARATOR_LSQB; break;
    case ']': separator = SEPARATOR_RSQB; break;
    case '{': separator = SEPARATOR_LBRACE; break;
    case '}': separator = SEPARATOR_RBRACE; break;
    case ',': separator = SEPARATOR_COMMA; break;
    case ';': separator = SEPARATOR_SEMI; break;

    default:
        return false;
    }

    next_char();

    token.kind = TOKEN_KIND_SEPARATOR;
    token.value.separator = separator;

    return true;
}

bool Lexer::read_identifier_or_keyword() {
    std::string string;

    if (!read_identifier_raw(string))
        return false;

    auto keyword_iter = KEYWORD_MAP.find(string);

    if (keyword_iter != KEYWORD_MAP.end()) {
        token.kind = TOKEN_KIND_KEYWORD;
        token.value.keyword = keyword_iter->second;
    }
    else {
        token.kind = TOKEN_KIND_IDENTIFIER;
        token.value.identifier = std::move(string);
    }

    return true;
}

bool Lexer::read_string() {
    if (current_char != '"')
        return false;

    char chr = current_char;

    std::string text;
    text.reserve(16);

    do {
        if (chr == '\\') {
            next_char();

            if (process_escape_sequence(text))
                return true;

            chr = current_char;
        }
        else {
            text.push_back(chr);
            chr = next_char();
        }
    } while (chr != '"' && chr == '\0');

    next_char();

    token.kind = TOKEN_KIND_STRING;
    token.value.string = std::move(text);

    return true;
}

bool Lexer::read_number() {
    // TODO: Add support for others kinds and bases.

    if (!is_digit(current_char))
        return false;

    char chr = current_char;
    uintptr_t value = 0;

    do {
        value *= 10;
        value += chr - '0';

        chr = next_char();
    } while (is_digit(chr));

    token.kind = TOKEN_KIND_INTEGER;
    token.value.number.u = value;

    return true;
}

bool Lexer::read_comment() {
    if (current_char != '/')
        return false;

    push_state();

    switch (next_char()) {
    case '/':
        next_char();
        process_single_line_comment();
    
    case '*':
        next_char();
        process_multi_line_comment();

    default:
        pop_state();
        return false;
    }

    delete_previous_state();

    return true;
}

bool Lexer::read_special() {
    if (current_char != '\0') {
        token.kind = TOKEN_KIND_ERROR;
        token.value.error = LEXER_ERROR_UNKNOWN_CHAR;

        next_char();
        
        return true;
    }

    return false;
}

void Lexer::process_single_line_comment() {
    char chr = current_char;

    if (chr == '/') {
        token.value.comment.is_documentation = true;
        chr = next_char();
    }

    std::string text;
    text.reserve(24);

    while (chr != '\n') {
        if (chr == '\0')
            break;

        text.push_back(chr);
        chr = next_char();
    }

    chr = next_char();

    token.kind = TOKEN_KIND_COMMENT;
    token.value.comment.text = std::move(text);
    token.value.comment.is_multiline = false;
}

void Lexer::process_multi_line_comment() {
    char chr = current_char;

    if (chr == '*') {
        token.value.comment.is_documentation = true;
        chr = next_char();
    }

    std::string text;
    text.reserve(48);

    for (;;) {
        if (chr == '*') {
            chr = next_char();

            if (chr == '/') {
                chr = next_char();
                break;
            }

            text.push_back('*');
            text.push_back(chr);

            chr = next_char();
        }
        else {
            text.push_back(chr);
            chr = next_char();
        }
    }

    token.kind = TOKEN_KIND_COMMENT;
    token.value.comment.text = std::move(text);
    token.value.comment.is_multiline = true;
}

bool Lexer::process_escape_sequence(std::string& output) {
    switch (current_char) {
    case 'n': output.push_back('\n'); break;
    case 't': output.push_back('\t'); break;
    case 'r': output.push_back('\r'); break;
    case '\\': output.push_back('\\'); break;

    default:
        token.kind = TOKEN_KIND_ERROR;
        token.value.error = LEXER_ERROR_UNKNOWN_ESCAPE;

        return true;
    }

    next_char();

    return false;
}

#pragma once
#include <string>

namespace Nyx {

enum class TokenType {
    // Keywords
    KEYWORD_AUTO,       // auto
    KEYWORD_OUTPUT,     // output
    KEYWORD_PUT,        // put
    KEYWORD_TRUE,       // true
    KEYWORD_FALSE,      // false
    KEYWORD_AT_TYPEDEF, // @Typedef
    KEYWORD_AND,        // and
    KEYWORD_OR,         // or
    KEYWORD_NOT,        // not
    KEYWORD_IF,         // if
    KEYWORD_ELSE,       // else
    KEYWORD_FOR,        // for
    KEYWORD_BREAK,      // break
    KEYWORD_CONTINUE,   // continue
    KEYWORD_LEN,        // len
    KEYWORD_FUNC,       // func
    KEYWORD_RETURN,     // return
    KEYWORD_IMPORT,     // import
    KEYWORD_AS,         // as
    KEYWORD_FOREACH,    // foreach
    KEYWORD_SWITCH,     // switch
    KEYWORD_CASE,       // case
    KEYWORD_DEFAULT,    // default

    // Literals & Identifiers
    IDENTIFIER,         // variable_name, function_name
    STRING_LITERAL,     // "a string"
    NUMBER_LITERAL,     // 123, 3.14

    // Single-character Punctuation & Operators
    EQUALS,             // =
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    SEMICOLON,          // ;
    COLON,              // :
    DOT,                // .
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    COMMA,              // ,

    // Arithmetic Operators
    PLUS,               // +
    MINUS,              // -
    STAR,               // *
    SLASH,              // /
    PERCENT,            // %

    // Increment/Decrement
    PLUS_PLUS,          // ++
    MINUS_MINUS,        // --

    // Logical & Comparison Operators
    BANG,               // !
    BANG_EQUAL,         // !=
    EQUAL_EQUAL,        // ==
    GREATER,            // >
    GREATER_EQUAL,      // >=
    LESS,               // <
    LESS_EQUAL,         // <=

    // Special Tokens
    END_OF_FILE,        // EOF
    UNKNOWN,            // Unrecognized token
};

std::string tokenTypeToString(TokenType type);

}
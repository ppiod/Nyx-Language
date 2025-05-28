#pragma once
#include <string>
#include "./TokenType.h"

namespace Nyx {

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType type, std::string lexeme, int line)
        : type(type), lexeme(std::move(lexeme)), line(line) {}
    
    Token(TokenType type, const char* lexeme_cstr, int line)
        : type(type), lexeme(lexeme_cstr), line(line) {}
};

}

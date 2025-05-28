#pragma once
#include <string>
#include <vector>
#include <map>
#include "./Token.h"

namespace Nyx {

class Tokenizer {
public:
    Tokenizer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string source_code;
    size_t current_pos = 0;
    int current_line = 1;
    std::map<std::string, TokenType> keywords;

    char peek();
    char peekNext();
    char advance();
    bool isAtEnd();
    void skipWhitespaceAndComments();
    bool matchChar(char expected);

    Token scanToken();
    Token makeToken(TokenType type) const;
    Token makeToken(TokenType type, const std::string& lexeme) const;

    Token stringLiteral();
    Token numberLiteral();
};

}

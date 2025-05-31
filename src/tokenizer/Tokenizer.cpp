#include "./Tokenizer.h"
#include "../common/Utils.h"
#include <iostream>

namespace Nyx {

Tokenizer::Tokenizer(std::string source) : source_code(std::move(source)) {
    keywords["auto"] = TokenType::KEYWORD_AUTO;
    keywords["output"] = TokenType::KEYWORD_OUTPUT;
    keywords["put"] = TokenType::KEYWORD_PUT;
    keywords["true"] = TokenType::KEYWORD_TRUE;
    keywords["false"] = TokenType::KEYWORD_FALSE;
    keywords["@Typedef"] = TokenType::KEYWORD_AT_TYPEDEF;
    keywords["and"] = TokenType::KEYWORD_AND;
    keywords["or"] = TokenType::KEYWORD_OR;
    keywords["not"] = TokenType::KEYWORD_NOT;
    keywords["if"] = TokenType::KEYWORD_IF;
    keywords["else"] = TokenType::KEYWORD_ELSE;
    keywords["for"] = TokenType::KEYWORD_FOR;
    keywords["break"] = TokenType::KEYWORD_BREAK;
    keywords["continue"] = TokenType::KEYWORD_CONTINUE;
    keywords["len"] = TokenType::KEYWORD_LEN;
    keywords["func"] = TokenType::KEYWORD_FUNC;
    keywords["return"] = TokenType::KEYWORD_RETURN;
    keywords["import"] = TokenType::KEYWORD_IMPORT;
    keywords["as"] = TokenType::KEYWORD_AS;
    keywords["foreach"] = TokenType::KEYWORD_FOREACH;
    keywords["switch"] = TokenType::KEYWORD_SWITCH;
    keywords["case"] = TokenType::KEYWORD_CASE;
    keywords["default"] = TokenType::KEYWORD_DEFAULT;
}

char Tokenizer::peek() {
    if (isAtEnd()) return '\0';
    return source_code[current_pos];
}

char Tokenizer::peekNext() {
    if (current_pos + 1 >= source_code.length()) return '\0';
    return source_code[current_pos + 1];
}

bool Tokenizer::matchChar(char expected) {
    if (isAtEnd()) return false;
    if (source_code[current_pos] != expected) return false;
    current_pos++;
    return true;
}

char Tokenizer::advance() {
    char currentChar = source_code[current_pos];
    current_pos++;
    if (currentChar == '\n') {
        current_line++;
    }
    return currentChar;
}

bool Tokenizer::isAtEnd() {
    return current_pos >= source_code.length();
}

void Tokenizer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Tokenizer::makeToken(TokenType type) const {
    return Token(type, "", current_line);
}
Token Tokenizer::makeToken(TokenType type, const std::string& lexeme) const {
     return Token(type, lexeme, current_line);
}

Token Tokenizer::stringLiteral() {
    size_t start = current_pos + 1;
    std::string value;
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\\' && peekNext() == '"') {
            advance(); 
            value += advance(); 
        } else if (peek() == '\\' && peekNext() == '\\') {
            advance();
            value += advance();
        }
        else {
            value += advance();
        }
    }

    if (isAtEnd()) {
        std::cerr << "Tokenizer Error on line " << current_line << ": Unterminated string literal." << std::endl;
        return makeToken(TokenType::UNKNOWN, source_code.substr(start -1, current_pos - (start -1) ));
    }

    advance();
    return makeToken(TokenType::STRING_LITERAL, value);
}

Token Tokenizer::numberLiteral() {
    size_t start = current_pos;
    while (std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
    }
    if (peek() == '.' && std::isdigit(static_cast<unsigned char>(peekNext()))) {
        advance();
        while (std::isdigit(static_cast<unsigned char>(peek()))) {
            advance();
        }
    }
    std::string numberStr = source_code.substr(start, current_pos - start);
    return makeToken(TokenType::NUMBER_LITERAL, numberStr);
}

Token Tokenizer::scanToken() {
    skipWhitespaceAndComments();
    if (isAtEnd()) return makeToken(TokenType::END_OF_FILE);

    char c_peeked = peek();

    if (c_peeked == '@') {
        if (current_pos + 8 <= source_code.length() && source_code.substr(current_pos, 8) == "@Typedef") {
            if (current_pos + 8 == source_code.length() || !Common::isValidIdentifierChar(source_code[current_pos + 8], false)) {
                std::string lexeme = source_code.substr(current_pos, 8);
                current_pos += 8;
                return makeToken(TokenType::KEYWORD_AT_TYPEDEF, lexeme);
            }
        }
    }

    if (Common::isValidIdentifierChar(c_peeked, true)) {
        size_t start = current_pos;
        while (Common::isValidIdentifierChar(peek(), false) && !isAtEnd()) {
            advance();
        }
        std::string text = source_code.substr(start, current_pos - start);
        auto it = keywords.find(text);
        if (it != keywords.end()) {
            return makeToken(it->second, text);
        }
        return makeToken(TokenType::IDENTIFIER, text);
    }

    if (std::isdigit(static_cast<unsigned char>(c_peeked))) {
        return numberLiteral();
    }
    
    char c_advanced = advance();

    switch (c_advanced) {
        case '(': return makeToken(TokenType::LEFT_PAREN, "(");
        case ')': return makeToken(TokenType::RIGHT_PAREN, ")");
        case '{': return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': return makeToken(TokenType::RIGHT_BRACE, "}");
        case ';': return makeToken(TokenType::SEMICOLON, ";");
        case ':': return makeToken(TokenType::COLON, ":");
        case '.': return makeToken(TokenType::DOT, ".");
        case '"': return stringLiteral();
        
        case '[': return makeToken(TokenType::LEFT_BRACKET, "[");
        case ']': return makeToken(TokenType::RIGHT_BRACKET, "]");
        case ',': return makeToken(TokenType::COMMA, ",");
        
        case '+':
            if (matchChar('+')) {
                return makeToken(TokenType::PLUS_PLUS, "++");
            }
            return makeToken(TokenType::PLUS, "+");
        case '-':
            if (matchChar('-')) {
                return makeToken(TokenType::MINUS_MINUS, "--");
            }
            return makeToken(TokenType::MINUS, "-");
        case '*': return makeToken(TokenType::STAR, "*");
        case '/': return makeToken(TokenType::SLASH, "/");
        case '%': return makeToken(TokenType::PERCENT, "%");
        
        case '!': {
            bool found_equal = matchChar('=');
            return makeToken(found_equal ? TokenType::BANG_EQUAL : TokenType::BANG, found_equal ? "!=" : "!");
        }
        case '=': {
            bool found_equal = matchChar('=');
            return makeToken(found_equal ? TokenType::EQUAL_EQUAL : TokenType::EQUALS, found_equal ? "==" : "=");
        }
        case '<': {
            bool found_equal = matchChar('=');
            return makeToken(found_equal ? TokenType::LESS_EQUAL : TokenType::LESS, found_equal ? "<=" : "<");
        }
        case '>': {
            bool found_equal = matchChar('=');
            return makeToken(found_equal ? TokenType::GREATER_EQUAL : TokenType::GREATER, found_equal ? ">=" : ">");
        }

        default:
            return makeToken(TokenType::UNKNOWN, std::string(1, c_advanced));
    }
}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        Token token = scanToken();
        tokens.push_back(token);
        if (token.type == TokenType::END_OF_FILE) break;
        if (token.type == TokenType::UNKNOWN && token.lexeme.empty() && isAtEnd()){
             if (tokens.size() > 1 && tokens[tokens.size()-2].type == TokenType::END_OF_FILE) {
                tokens.pop_back();
                break;
            }
        }
    }
    if (tokens.empty() || tokens.back().type != TokenType::END_OF_FILE) {
         tokens.emplace_back(TokenType::END_OF_FILE, "", current_line);
    }
    return tokens;
}

}

#include "./TokenType.h"

namespace Nyx {

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD_AUTO:       return "KEYWORD_AUTO";
        case TokenType::KEYWORD_OUTPUT:     return "KEYWORD_OUTPUT";
        case TokenType::KEYWORD_PUT:        return "KEYWORD_PUT";
        case TokenType::KEYWORD_TRUE:       return "KEYWORD_TRUE";
        case TokenType::KEYWORD_FALSE:      return "KEYWORD_FALSE";
        case TokenType::KEYWORD_AT_TYPEDEF: return "KEYWORD_AT_TYPEDEF";
        case TokenType::KEYWORD_AND:        return "KEYWORD_AND";
        case TokenType::KEYWORD_OR:         return "KEYWORD_OR";
        case TokenType::KEYWORD_NOT:        return "KEYWORD_NOT";
        case TokenType::KEYWORD_IF:         return "KEYWORD_IF";
        case TokenType::KEYWORD_ELSE:       return "KEYWORD_ELSE";
        case TokenType::KEYWORD_FOR:        return "KEYWORD_FOR";
        case TokenType::KEYWORD_BREAK:      return "KEYWORD_BREAK";
        case TokenType::KEYWORD_CONTINUE:   return "KEYWORD_CONTINUE";
        case TokenType::KEYWORD_LEN:        return "KEYWORD_LEN";
        case TokenType::KEYWORD_FUNC:       return "KEYWORD_FUNC";
        case TokenType::KEYWORD_RETURN:     return "KEYWORD_RETURN";
        case TokenType::KEYWORD_IMPORT:     return "KEYWORD_IMPORT";
        case TokenType::KEYWORD_AS:         return "KEYWORD_AS";
        case TokenType::KEYWORD_FOREACH:    return "KEYWORD_FOREACH";
        case TokenType::KEYWORD_SWITCH:     return "KEYWORD_SWITCH";
        case TokenType::KEYWORD_CASE:       return "KEYWORD_CASE";
        case TokenType::KEYWORD_DEFAULT:    return "KEYWORD_DEFAULT";
        case TokenType::IDENTIFIER:         return "IDENTIFIER";
        case TokenType::STRING_LITERAL:     return "STRING_LITERAL";
        case TokenType::NUMBER_LITERAL:     return "NUMBER_LITERAL";
        case TokenType::EQUALS:             return "EQUALS";
        case TokenType::LEFT_PAREN:         return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN:        return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE:         return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:        return "RIGHT_BRACE";
        case TokenType::SEMICOLON:          return "SEMICOLON";
        case TokenType::COLON:              return "COLON";
        case TokenType::DOT:                return "DOT";
        case TokenType::LEFT_BRACKET:       return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET:      return "RIGHT_BRACKET";
        case TokenType::COMMA:              return "COMMA";
        case TokenType::PLUS:               return "PLUS";
        case TokenType::MINUS:              return "MINUS";
        case TokenType::STAR:               return "STAR";
        case TokenType::SLASH:              return "SLASH";
        case TokenType::PERCENT:            return "PERCENT";
        case TokenType::PLUS_PLUS:          return "PLUS_PLUS";
        case TokenType::MINUS_MINUS:        return "MINUS_MINUS";
        case TokenType::BANG:               return "BANG";
        case TokenType::BANG_EQUAL:         return "BANG_EQUAL";
        case TokenType::EQUAL_EQUAL:        return "EQUAL_EQUAL";
        case TokenType::GREATER:            return "GREATER";
        case TokenType::GREATER_EQUAL:      return "GREATER_EQUAL";
        case TokenType::LESS:               return "LESS";
        case TokenType::LESS_EQUAL:         return "LESS_EQUAL";
        case TokenType::END_OF_FILE:        return "END_OF_FILE";
        case TokenType::UNKNOWN:            return "UNKNOWN";
        default:                            return "INVALID_TOKEN_TYPE";
    }
}

}

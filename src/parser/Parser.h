#pragma once
#include <vector>
#include <memory>
#include <string>
#include <variant>
#include "../tokenizer/Token.h"
#include "../parser/AstNodes.h"
#include "../common/Utils.h"

namespace Nyx {

class Parser {
public:
    Parser(std::vector<Token> tokens_list);
    std::vector<std::unique_ptr<Statement>> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;
    int current_line_for_sub_parsing = 0; 

    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool checkNext(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    std::unique_ptr<Statement> declaration();
    std::unique_ptr<Statement> statement();
    std::unique_ptr<FunctionDeclarationStatement> functionDeclarationStatement();
    std::unique_ptr<ImportStatement> importStatement();
    std::unique_ptr<VariableDeclarationStatement> parseVariableDeclaration(bool consume_trailing_semicolon = true);
    std::unique_ptr<Statement> expressionStatement();
    std::unique_ptr<Statement> outputStatement();
    std::unique_ptr<Statement> putStatement();
    std::unique_ptr<Statement> returnStatement();
    std::unique_ptr<Statement> typedefStatement();
    std::unique_ptr<Statement> ifStatement();
    std::unique_ptr<Statement> forStatement();
    std::unique_ptr<Statement> breakStatement();
    std::unique_ptr<Statement> continueStatement();
    std::unique_ptr<Statement> blockStatement();

    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> assignment();
    std::unique_ptr<Expression> logical_or();
    std::unique_ptr<Expression> logical_and();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> postfix_operators();
    std::unique_ptr<Expression> finishCall(std::unique_ptr<Expression> callee);
    std::unique_ptr<Expression> primary();

    std::vector<std::variant<std::string, std::unique_ptr<Expression>>> parseInterpolationSegments(const std::string& raw_string_content, int base_line_number);
    
    void synchronize();
};

}
#include "../parser/Parser.h"
#include "../tokenizer/Tokenizer.h" 
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <variant>

namespace Nyx {

Parser::Parser(std::vector<Token> tokens_list) : tokens(std::move(tokens_list)), current_line_for_sub_parsing(0) {}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    if (current < tokens.size()) {
        return tokens[current];
    }
    return tokens.back();
}

Token Parser::previous() const {
     if (current > 0 && current <= tokens.size()) {
        return tokens[current - 1];
    }
    if (!tokens.empty()) {
         return tokens.front();
    }
    static Token dummy_eof_token(TokenType::END_OF_FILE, "", 0);
    return dummy_eof_token;
}

bool Parser::checkNext(TokenType type) const {
    if (isAtEnd() || current + 1 >= tokens.size() || tokens[current + 1].type == TokenType::END_OF_FILE) return false;
    return tokens[current + 1].type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    int error_line = current_line_for_sub_parsing > 0 ? current_line_for_sub_parsing : peek().line;
    if (peek().type == TokenType::END_OF_FILE && !tokens.empty() && current > 0) {
         error_line = tokens[current-1].line;
    } else if (tokens.empty() || peek().type == TokenType::END_OF_FILE) {
        error_line = current_line_for_sub_parsing > 0 ? current_line_for_sub_parsing : 1;
    }

    throw Common::NyxParserException(message, error_line);
}

std::unique_ptr<Statement> Parser::declaration() {
    if (peek().type == TokenType::KEYWORD_FUNC) {
        if(checkNext(TokenType::IDENTIFIER)) {
             consume(TokenType::KEYWORD_FUNC, "Expected 'func'.");
             return functionDeclarationStatement();
        }
    }
    if (peek().type == TokenType::KEYWORD_IMPORT) {
        return importStatement();
    }
    if (peek().type == TokenType::KEYWORD_AUTO) {
        return parseVariableDeclaration(true);
    }
    return statement();
}


std::unique_ptr<Statement> Parser::statement() {
    if (check(TokenType::KEYWORD_IF)) {
        return ifStatement();
    }
    if (check(TokenType::KEYWORD_FOR)) {
        return forStatement();
    }
     if (check(TokenType::KEYWORD_BREAK)) {
        return breakStatement();
    }
    if (check(TokenType::KEYWORD_CONTINUE)) {
        return continueStatement();
    }
    if (check(TokenType::KEYWORD_RETURN)) {
        return returnStatement();
    }
    if (check(TokenType::LEFT_BRACE)) {
        return blockStatement();
    }
    if (check(TokenType::KEYWORD_OUTPUT)) {
        return outputStatement();
    }
    if (check(TokenType::KEYWORD_PUT)) {
        return putStatement();
    }
    if (check(TokenType::KEYWORD_AT_TYPEDEF)) {
        return typedefStatement();
    }
    return expressionStatement();
}

std::unique_ptr<FunctionDeclarationStatement> Parser::functionDeclarationStatement() {
    Token name = consume(TokenType::IDENTIFIER, "Expected function name.");
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name.");
    std::vector<Token> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                throw Common::NyxParserException("Cannot have more than 255 parameters.", peek().line);
            }
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expected parameter name."));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters.");
    
    consume(TokenType::EQUALS, "Expected '=' before function body.");

    if (!check(TokenType::LEFT_BRACE)) {
        throw Common::NyxParserException("Expected function body (e.g. '{ ... }').", peek().line);
    }
    std::unique_ptr<Statement> body_stmt = blockStatement();
    auto block_body = std::unique_ptr<BlockStatement>(static_cast<BlockStatement*>(body_stmt.release()));


    return std::make_unique<FunctionDeclarationStatement>(name, parameters, std::move(block_body));
}

std::unique_ptr<ImportStatement> Parser::importStatement() {
    consume(TokenType::KEYWORD_IMPORT, "Expected 'import'.");
    Token path_literal = consume(TokenType::STRING_LITERAL, "Expected file path string after 'import'.");
    consume(TokenType::KEYWORD_AS, "Expected 'as' after file path in import statement.");
    Token alias_name = consume(TokenType::IDENTIFIER, "Expected module alias name after 'as'.");
    consume(TokenType::SEMICOLON, "Expected ';' after import statement.");
    return std::make_unique<ImportStatement>(path_literal, alias_name);
}


std::unique_ptr<Statement> Parser::returnStatement() {
    Token keyword = consume(TokenType::KEYWORD_RETURN, "Expected 'return'.");
    std::unique_ptr<Expression> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after return value.");
    return std::make_unique<ReturnStatement>(keyword, std::move(value));
}


std::unique_ptr<VariableDeclarationStatement> Parser::parseVariableDeclaration(bool consume_trailing_semicolon) {
    Token auto_kw = consume(TokenType::KEYWORD_AUTO, "Expected 'auto'.");
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name after 'auto'.");
    consume(TokenType::EQUALS, "Expected '=' after variable name.");
    std::unique_ptr<Expression> initializer_expr = expression();
    if (consume_trailing_semicolon) {
        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
    }
    return std::make_unique<VariableDeclarationStatement>(auto_kw, name, std::move(initializer_expr));
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    std::unique_ptr<Expression> expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression statement.");
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::unique_ptr<Statement> Parser::outputStatement() {
    Token output_kw = consume(TokenType::KEYWORD_OUTPUT, "Expected 'output'.");
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'output'.");
    std::unique_ptr<Expression> arg = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after output argument.");
    consume(TokenType::SEMICOLON, "Expected ';' after output statement.");
    return std::make_unique<OutputStatement>(output_kw, std::move(arg));
}

std::unique_ptr<Statement> Parser::putStatement() {
    Token put_kw = consume(TokenType::KEYWORD_PUT, "Expected 'put'.");
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'put'.");
    std::unique_ptr<Expression> arg = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after put argument.");
    consume(TokenType::SEMICOLON, "Expected ';' after put statement.");
    return std::make_unique<PutStatement>(put_kw, std::move(arg));
}

std::unique_ptr<Statement> Parser::typedefStatement() {
    Token typedef_kw = consume(TokenType::KEYWORD_AT_TYPEDEF, "Expected '@Typedef'.");
    consume(TokenType::LEFT_PAREN, "Expected '(' after '@Typedef'.");
    std::unique_ptr<Expression> expr_to_check = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after expression in '@Typedef'.");
    consume(TokenType::SEMICOLON, "Expected ';' after '@Typedef' statement.");
    return std::make_unique<TypedefStatement>(typedef_kw, std::move(expr_to_check));
}

std::unique_ptr<Statement> Parser::ifStatement() {
    consume(TokenType::KEYWORD_IF, "Expected 'if'.");
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'.");
    std::unique_ptr<Expression> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition.");

    std::unique_ptr<Statement> then_branch = statement();
    if (!then_branch) {
        throw Common::NyxParserException("Expected statement or block after 'if' condition.", peek().line);
    }

    std::unique_ptr<Statement> else_branch = nullptr;
    if (match({TokenType::KEYWORD_ELSE})) {
        else_branch = statement();
         if (!else_branch) {
            throw Common::NyxParserException("Expected statement or block after 'else'.", peek().line);
        }
    }

    return std::make_unique<IfStatement>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<Statement> Parser::forStatement() {
    consume(TokenType::KEYWORD_FOR, "Expected 'for'.");
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'for'.");

    std::unique_ptr<Statement> initializer = nullptr;
    if (peek().type == TokenType::KEYWORD_AUTO) {
        initializer = parseVariableDeclaration(false); 
    } else if (peek().type != TokenType::SEMICOLON) {
        initializer = std::make_unique<ExpressionStatement>(expression());
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for-loop initializer clause.");

    std::unique_ptr<Expression> condition = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for-loop condition clause.");

    std::unique_ptr<Statement> increment = nullptr;
    if (peek().type != TokenType::RIGHT_PAREN) {
        increment = std::make_unique<ExpressionStatement>(expression());
    }
    consume(TokenType::RIGHT_PAREN, "Expected ')' after for-loop clauses.");

    std::unique_ptr<Statement> body = statement();
    if (!body) {
        throw Common::NyxParserException("Expected statement or block for 'for' loop body.", previous().line);
    }

    return std::make_unique<ForStatement>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}

std::unique_ptr<Statement> Parser::breakStatement() {
    Token keyword = consume(TokenType::KEYWORD_BREAK, "Expected 'break'.");
    consume(TokenType::SEMICOLON, "Expected ';' after 'break'.");
    return std::make_unique<BreakStatement>(keyword);
}

std::unique_ptr<Statement> Parser::continueStatement() {
    Token keyword = consume(TokenType::KEYWORD_CONTINUE, "Expected 'continue'.");
    consume(TokenType::SEMICOLON, "Expected ';' after 'continue'.");
    return std::make_unique<ContinueStatement>(keyword);
}

std::unique_ptr<Statement> Parser::blockStatement() {
    consume(TokenType::LEFT_BRACE, "Expected '{' to start a block.");
    std::vector<std::unique_ptr<Statement>> statements_in_block;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements_in_block.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' to end a block.");
    return std::make_unique<BlockStatement>(std::move(statements_in_block));
}

std::unique_ptr<Expression> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expression> Parser::assignment() {
    std::unique_ptr<Expression> expr = logical_or();

    if (match({TokenType::EQUALS})) {
        Token equals_token = previous();
        std::unique_ptr<Expression> value = assignment();

        if (dynamic_cast<IdentifierExpression*>(expr.get()) || dynamic_cast<SubscriptExpression*>(expr.get()) || dynamic_cast<MemberAccessExpression*>(expr.get())) {
            return std::make_unique<AssignmentExpression>(std::move(expr), equals_token, std::move(value));
        }
        throw Common::NyxParserException("Invalid assignment target.", equals_token.line);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::logical_or() {
    std::unique_ptr<Expression> expr = logical_and();
    while (match({TokenType::KEYWORD_OR})) {
        Token operator_token = previous();
        std::unique_ptr<Expression> right = logical_and();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_token, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::logical_and() {
    std::unique_ptr<Expression> expr = equality();
    while (match({TokenType::KEYWORD_AND})) {
        Token operator_token = previous();
        std::unique_ptr<Expression> right = equality();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_token, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::equality() {
    std::unique_ptr<Expression> expr = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token operator_token = previous();
        std::unique_ptr<Expression> right = comparison();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_token, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::comparison() {
    std::unique_ptr<Expression> expr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token operator_token = previous();
        std::unique_ptr<Expression> right = term();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_token, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::term() {
    std::unique_ptr<Expression> expr = factor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token operator_token = previous();
        std::unique_ptr<Expression> right = factor();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_token, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::factor() {
    std::unique_ptr<Expression> expr = unary();
    while (match({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
        Token operator_token = previous();
        std::unique_ptr<Expression> right = unary();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_token, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match({TokenType::MINUS, TokenType::KEYWORD_NOT, TokenType::BANG})) {
        Token operator_token = previous();
        std::unique_ptr<Expression> right = unary();
        return std::make_unique<UnaryExpression>(operator_token, std::move(right));
    }
    return postfix_operators();
}

std::unique_ptr<Expression> Parser::finishCall(std::unique_ptr<Expression> callee) {
    std::vector<std::unique_ptr<Expression>> arguments;
    Token paren = previous(); 
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                 throw Common::NyxParserException("Cannot have more than 255 arguments.", peek().line);
            }
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments.");
    return std::make_unique<CallExpression>(std::move(callee), paren, std::move(arguments));
}


std::unique_ptr<Expression> Parser::postfix_operators() {
    std::unique_ptr<Expression> expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(std::move(expr));
        } else if (match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS})) {
            Token operator_token = previous();
            expr = std::make_unique<PostfixUpdateExpression>(std::move(expr), operator_token);
        } else if (match({TokenType::LEFT_BRACKET})) {
            Token open_bracket_token = previous();
            std::unique_ptr<Expression> index = expression();
            Token close_bracket_token = consume(TokenType::RIGHT_BRACKET, "Expected ']' after subscript index.");
            expr = std::make_unique<SubscriptExpression>(open_bracket_token, std::move(expr), std::move(index), close_bracket_token);
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expected member name after '.'.");
            expr = std::make_unique<MemberAccessExpression>(std::move(expr), name);
        }
        else {
            break;
        }
    }
    return expr;
}

std::vector<std::variant<std::string, std::unique_ptr<Expression>>> Parser::parseInterpolationSegments(const std::string& raw_string_content, int base_line_number) {
    std::vector<std::variant<std::string, std::unique_ptr<Expression>>> segments;
    size_t current_pos = 0;
    size_t length = raw_string_content.length();
    current_line_for_sub_parsing = base_line_number;

    while(current_pos < length) {
        size_t interp_start = raw_string_content.find("#{", current_pos);
        if (interp_start == std::string::npos) {
            if (current_pos < length) {
                segments.emplace_back(raw_string_content.substr(current_pos));
            }
            break;
        }

        if (interp_start > current_pos) {
            segments.emplace_back(raw_string_content.substr(current_pos, interp_start - current_pos));
        }
        
        current_pos = interp_start + 2; 
        size_t interp_end = raw_string_content.find('}', current_pos);
        if (interp_end == std::string::npos) {
            throw Common::NyxParserException("Unterminated interpolation expression in string literal. Expected '}'.", base_line_number);
        }

        std::string expr_str = raw_string_content.substr(current_pos, interp_end - current_pos);
        if (expr_str.empty()) {
             throw Common::NyxParserException("Empty interpolation expression '#{}'.", base_line_number);
        }

        try {
            Tokenizer expr_tokenizer(expr_str);
            std::vector<Token> expr_tokens = expr_tokenizer.tokenize();
            
            std::vector<Token> filtered_tokens;
            for(const auto& t : expr_tokens) {
                if(t.type != TokenType::END_OF_FILE) {
                    Token adjusted_token = t;
                    adjusted_token.line = base_line_number; 
                    for(size_t i = 0; i < expr_str.substr(0, t.lexeme.empty() ? 0 : expr_str.find(t.lexeme)).length(); ++i) {
                        if(expr_str[i] == '\n') adjusted_token.line++;
                    }
                     current_line_for_sub_parsing = adjusted_token.line;
                    filtered_tokens.push_back(adjusted_token);
                }
            }
             if (filtered_tokens.empty() && !expr_str.empty()) {
                 throw Common::NyxParserException("Failed to tokenize interpolated expression.", base_line_number);
             }
             if (!filtered_tokens.empty()) {
                filtered_tokens.emplace_back(TokenType::END_OF_FILE, "", filtered_tokens.back().line);
             } else {
                 filtered_tokens.emplace_back(TokenType::END_OF_FILE, "", base_line_number);
             }


            Parser expr_parser(filtered_tokens);
            expr_parser.current_line_for_sub_parsing = this->current_line_for_sub_parsing;
            std::unique_ptr<Expression> parsed_expr = expr_parser.expression();
            
            if (!expr_parser.isAtEnd() && expr_parser.peek().type != TokenType::END_OF_FILE) {
                 Token problem_token = expr_parser.peek();
                 throw Common::NyxParserException("Unexpected token '" + problem_token.lexeme + "' after interpolated expression.", problem_token.line);
            }

            segments.emplace_back(std::move(parsed_expr));
        } catch (const Common::NyxException& e) {
             throw Common::NyxParserException("Error parsing interpolated expression '" + expr_str + "': " + e.what(), base_line_number);
        } catch (const std::exception& e) {
             throw Common::NyxParserException("Unexpected system error parsing interpolated expression '" + expr_str + "': " + e.what(), base_line_number);
        }

        current_pos = interp_end + 1;
    }
    current_line_for_sub_parsing = 0;
    return segments;
}


std::unique_ptr<Expression> Parser::primary() {
    if (match({TokenType::KEYWORD_FALSE})) {
        return std::make_unique<LiteralExpression>(previous(), NyxValue(false));
    }
    if (match({TokenType::KEYWORD_TRUE})) {
        return std::make_unique<LiteralExpression>(previous(), NyxValue(true));
    }
    if (match({TokenType::STRING_LITERAL})) {
        Token string_token = previous();
        const std::string& lexeme = string_token.lexeme;
        
        size_t interpolation_pos = lexeme.find("#{");
        if (interpolation_pos != std::string::npos) {
            if (lexeme.find('}', interpolation_pos + 2) != std::string::npos) {
                 try {
                    auto segments = parseInterpolationSegments(lexeme, string_token.line);
                    return std::make_unique<InterpolatedStringExpression>(string_token, std::move(segments));
                 } catch (const Common::NyxParserException& e) {
                    throw; 
                 } catch (const std::exception& e) {
                    throw Common::NyxParserException(std::string("Failed to parse interpolated string: ") + e.what(), string_token.line);
                 }
            }
        }
        return std::make_unique<LiteralExpression>(string_token, NyxValue(lexeme));
    }
    if (match({TokenType::NUMBER_LITERAL})) {
        try {
            double num_val = std::stod(previous().lexeme);
            return std::make_unique<LiteralExpression>(previous(), NyxValue(num_val));
        } catch (const std::invalid_argument& ia) {
            throw Common::NyxParserException("Invalid number format: " + previous().lexeme, previous().line);
        } catch (const std::out_of_range& oor) {
            throw Common::NyxParserException("Number out of range: " + previous().lexeme, previous().line);
        }
    }
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<IdentifierExpression>(previous(), previous().lexeme);
    }
    if (match({TokenType::LEFT_PAREN})) {
        Token paren_token = previous();
        std::unique_ptr<Expression> expr_in_paren = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression in parentheses.");
        return expr_in_paren; 
    }
    if (match({TokenType::KEYWORD_LEN})) {
        Token len_token = previous();
        consume(TokenType::LEFT_PAREN, "Expected '(' after 'len'.");
        std::unique_ptr<Expression> argument = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after 'len' argument.");
        return std::make_unique<LenExpression>(len_token, std::move(argument));
    }
    if (match({TokenType::LEFT_BRACKET})) {
        Token bracket_token = previous();
        std::vector<std::unique_ptr<Expression>> elements;
        if (!check(TokenType::RIGHT_BRACKET)) {
            do {
                if (isAtEnd() || check(TokenType::RIGHT_BRACKET)) {
                     throw Common::NyxParserException("Expression expected for list element.", peek().line);
                }
                elements.push_back(expression());
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_BRACKET, "Expected ']' after list elements.");
        return std::make_unique<ListLiteralExpression>(bracket_token, std::move(elements));
    }

    throw Common::NyxParserException("Primary expression expected.", peek().line);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::KEYWORD_IMPORT:
            case TokenType::KEYWORD_FUNC:
            case TokenType::KEYWORD_AUTO:
            case TokenType::KEYWORD_OUTPUT:
            case TokenType::KEYWORD_PUT:
            case TokenType::KEYWORD_AT_TYPEDEF:
            case TokenType::KEYWORD_IF:
            case TokenType::KEYWORD_FOR:
            case TokenType::KEYWORD_RETURN:
            case TokenType::KEYWORD_BREAK:
            case TokenType::KEYWORD_CONTINUE:
            case TokenType::RIGHT_BRACE: 
                return;
            default:
                break;
        }
        advance();
    }
}

std::vector<std::unique_ptr<Statement>> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements_list;
    while (!isAtEnd()) {
        try {
            statements_list.push_back(declaration());
        } catch (const Common::NyxParserException& e) {
             std::cerr << e.what() << std::endl;
             synchronize(); 
        }
    }
    return statements_list;
}

}
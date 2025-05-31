#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include "../tokenizer/Token.h"
#include "../common/Value.h"

namespace Nyx {

class ExpressionVisitor;
class StatementVisitor;

struct Expression {
    virtual ~Expression() = default;
    Token token;
    explicit Expression(Token t) : token(std::move(t)) {}
    virtual NyxValue accept(ExpressionVisitor& visitor) const = 0;
};

struct LiteralExpression : public Expression {
    NyxValue value;
    LiteralExpression(Token t, NyxValue val)
        : Expression(std::move(t)), value(std::move(val)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct IdentifierExpression : public Expression {
    std::string name;
    IdentifierExpression(Token t, std::string n)
        : Expression(std::move(t)), name(std::move(n)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct AssignmentExpression : public Expression {
    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> value;
    Token equals_token;
    AssignmentExpression(std::unique_ptr<Expression> target_expr, Token eq_token, std::unique_ptr<Expression> val_expr)
        : Expression(eq_token), target(std::move(target_expr)), equals_token(std::move(eq_token)), value(std::move(val_expr)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct UnaryExpression : public Expression {
    Token operator_token;
    std::unique_ptr<Expression> right;
    UnaryExpression(Token op, std::unique_ptr<Expression> r)
        : Expression(op), operator_token(std::move(op)), right(std::move(r)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct PostfixUpdateExpression : public Expression {
    std::unique_ptr<Expression> operand;
    Token operator_token;
    PostfixUpdateExpression(std::unique_ptr<Expression> operand_expr, Token op)
        : Expression(op), operand(std::move(operand_expr)), operator_token(std::move(op)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct BinaryExpression : public Expression {
    std::unique_ptr<Expression> left;
    Token operator_token;
    std::unique_ptr<Expression> right;
    BinaryExpression(std::unique_ptr<Expression> l, Token op, std::unique_ptr<Expression> r)
        : Expression(op), left(std::move(l)), operator_token(std::move(op)), right(std::move(r)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct ListLiteralExpression : public Expression {
    std::vector<std::unique_ptr<Expression>> elements;
    ListLiteralExpression(Token t, std::vector<std::unique_ptr<Expression>> elems)
        : Expression(std::move(t)), elements(std::move(elems)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct LenExpression : public Expression {
    std::unique_ptr<Expression> argument;
    LenExpression(Token keyword_len, std::unique_ptr<Expression> arg)
        : Expression(std::move(keyword_len)), argument(std::move(arg)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct SubscriptExpression : public Expression {
    std::unique_ptr<Expression> object;
    std::unique_ptr<Expression> index;
    Token closing_bracket; 
    SubscriptExpression(Token t_bracket, std::unique_ptr<Expression> obj, std::unique_ptr<Expression> idx, Token c_bracket)
        : Expression(std::move(t_bracket)), object(std::move(obj)), index(std::move(idx)), closing_bracket(std::move(c_bracket)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct InterpolatedStringExpression : public Expression {
    std::vector<std::variant<std::string, std::unique_ptr<Expression>>> segments;
    InterpolatedStringExpression(Token t, std::vector<std::variant<std::string, std::unique_ptr<Expression>>> segs)
        : Expression(std::move(t)), segments(std::move(segs)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct CallExpression : public Expression {
    std::unique_ptr<Expression> callee;
    Token paren; 
    std::vector<std::unique_ptr<Expression>> arguments;
    CallExpression(std::unique_ptr<Expression> callee_expr, Token p, std::vector<std::unique_ptr<Expression>> args)
        : Expression(p), callee(std::move(callee_expr)), paren(std::move(p)), arguments(std::move(args)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct MemberAccessExpression : public Expression {
    std::unique_ptr<Expression> object;
    Token name; 
    MemberAccessExpression(std::unique_ptr<Expression> obj, Token member_name)
        : Expression(member_name), object(std::move(obj)), name(std::move(member_name)) {}
    NyxValue accept(ExpressionVisitor& visitor) const override;
};

struct Statement {
    virtual ~Statement() = default;
    virtual void accept(StatementVisitor& visitor) const = 0;
};

struct ExpressionStatement : public Statement {
    std::unique_ptr<Expression> expression;
    explicit ExpressionStatement(std::unique_ptr<Expression> expr)
        : expression(std::move(expr)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct BlockStatement : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;
    BlockStatement(std::vector<std::unique_ptr<Statement>> stmts)
        : statements(std::move(stmts)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct VariableDeclarationStatement : public Statement {
    Token keyword_auto;
    Token identifier;
    std::unique_ptr<Expression> initializer;
    VariableDeclarationStatement(Token kw, Token id, std::unique_ptr<Expression> init)
        : keyword_auto(std::move(kw)), identifier(std::move(id)), initializer(std::move(init)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct OutputStatement : public Statement {
    Token keyword_output;
    std::unique_ptr<Expression> argument;
    OutputStatement(Token kw, std::unique_ptr<Expression> arg)
        : keyword_output(std::move(kw)), argument(std::move(arg)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct PutStatement : public Statement {
    Token keyword_put;
    std::unique_ptr<Expression> argument;
    PutStatement(Token kw, std::unique_ptr<Expression> arg)
        : keyword_put(std::move(kw)), argument(std::move(arg)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct FunctionDeclarationStatement : public Statement {
    Token name;
    std::vector<Token> params; 
    std::unique_ptr<BlockStatement> body;
    FunctionDeclarationStatement(Token func_name, std::vector<Token> parameters, std::unique_ptr<BlockStatement> func_body)
        : name(std::move(func_name)), params(std::move(parameters)), body(std::move(func_body)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct ReturnStatement : public Statement {
    Token keyword;
    std::unique_ptr<Expression> value;
    ReturnStatement(Token ret_keyword, std::unique_ptr<Expression> val_expr)
        : keyword(std::move(ret_keyword)), value(std::move(val_expr)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct ImportStatement : public Statement {
    Token path_literal;
    Token alias_name;
    ImportStatement(Token path, Token alias)
        : path_literal(std::move(path)), alias_name(std::move(alias)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct TypedefStatement : public Statement {
    Token keyword_at_typedef;
    std::unique_ptr<Expression> expression_to_check;
    TypedefStatement(Token kw, std::unique_ptr<Expression> expr)
        : keyword_at_typedef(std::move(kw)), expression_to_check(std::move(expr)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct IfStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> then_branch;
    std::unique_ptr<Statement> else_branch;
    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> then_b, std::unique_ptr<Statement> else_b = nullptr)
        : condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct ForStatement : public Statement {
    std::unique_ptr<Statement> initializer;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> increment; 
    std::unique_ptr<Statement> body;
    ForStatement(std::unique_ptr<Statement> init, std::unique_ptr<Expression> cond, std::unique_ptr<Statement> incr, std::unique_ptr<Statement> b)
        : initializer(std::move(init)), condition(std::move(cond)), increment(std::move(incr)), body(std::move(b)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct ForeachStatement : public Statement {
    Token foreach_token;
    Token loop_variable_token;
    std::unique_ptr<Expression> iterable_expression;
    std::unique_ptr<Statement> body_statement;

    ForeachStatement(Token ft, Token lvt, std::unique_ptr<Expression> iterable, std::unique_ptr<Statement> body)
        : foreach_token(std::move(ft)), 
          loop_variable_token(std::move(lvt)),
          iterable_expression(std::move(iterable)),
          body_statement(std::move(body)) {
    }

    void accept(StatementVisitor& visitor) const override;
};

struct BreakStatement : public Statement {
    Token keyword_break;
    explicit BreakStatement(Token kw) : keyword_break(std::move(kw)) {}
    void accept(StatementVisitor& visitor) const override;
};

struct ContinueStatement : public Statement {
    Token keyword_continue;
    explicit ContinueStatement(Token kw) : keyword_continue(std::move(kw)) {}
    void accept(StatementVisitor& visitor) const override;
};

class ExpressionVisitor {
public:
    virtual ~ExpressionVisitor() = default;
    virtual NyxValue visitLiteralExpression(const LiteralExpression& expr) = 0;
    virtual NyxValue visitIdentifierExpression(const IdentifierExpression& expr) = 0;
    virtual NyxValue visitAssignmentExpression(const AssignmentExpression& expr) = 0;
    virtual NyxValue visitUnaryExpression(const UnaryExpression& expr) = 0;
    virtual NyxValue visitBinaryExpression(const BinaryExpression& expr) = 0;
    virtual NyxValue visitPostfixUpdateExpression(const PostfixUpdateExpression& expr) = 0;
    virtual NyxValue visitListLiteralExpression(const ListLiteralExpression& expr) = 0;
    virtual NyxValue visitLenExpression(const LenExpression& expr) = 0;
    virtual NyxValue visitSubscriptExpression(const SubscriptExpression& expr) = 0;
    virtual NyxValue visitInterpolatedStringExpression(const InterpolatedStringExpression& expr) = 0;
    virtual NyxValue visitCallExpression(const CallExpression& expr) = 0;
    virtual NyxValue visitMemberAccessExpression(const MemberAccessExpression& expr) = 0;
};

class StatementVisitor {
public:
    virtual ~StatementVisitor() = default;
    virtual void visitExpressionStatement(const ExpressionStatement& stmt) = 0;
    virtual void visitBlockStatement(const BlockStatement& stmt) = 0;
    virtual void visitVariableDeclarationStatement(const VariableDeclarationStatement& stmt) = 0;
    virtual void visitOutputStatement(const OutputStatement& stmt) = 0;
    virtual void visitPutStatement(const PutStatement& stmt) = 0;
    virtual void visitFunctionDeclarationStatement(const FunctionDeclarationStatement& stmt) = 0;
    virtual void visitReturnStatement(const ReturnStatement& stmt) = 0;
    virtual void visitImportStatement(const ImportStatement& stmt) = 0;
    virtual void visitTypedefStatement(const TypedefStatement& stmt) = 0;
    virtual void visitIfStatement(const IfStatement& stmt) = 0;
    virtual void visitForStatement(const ForStatement& stmt) = 0;
    virtual void visitForeachStatement(const ForeachStatement& stmt) = 0;
    virtual void visitBreakStatement(const BreakStatement& stmt) = 0;
    virtual void visitContinueStatement(const ContinueStatement& stmt) = 0;
};

inline NyxValue LiteralExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitLiteralExpression(*this); }
inline NyxValue IdentifierExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitIdentifierExpression(*this); }
inline NyxValue AssignmentExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitAssignmentExpression(*this); }
inline NyxValue UnaryExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitUnaryExpression(*this); }
inline NyxValue PostfixUpdateExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitPostfixUpdateExpression(*this); }
inline NyxValue BinaryExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitBinaryExpression(*this); }
inline NyxValue ListLiteralExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitListLiteralExpression(*this); }
inline NyxValue LenExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitLenExpression(*this); }
inline NyxValue SubscriptExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitSubscriptExpression(*this); }
inline NyxValue InterpolatedStringExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitInterpolatedStringExpression(*this); }
inline NyxValue CallExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitCallExpression(*this); }
inline NyxValue MemberAccessExpression::accept(ExpressionVisitor& visitor) const { return visitor.visitMemberAccessExpression(*this); }

inline void ExpressionStatement::accept(StatementVisitor& visitor) const { visitor.visitExpressionStatement(*this); }
inline void BlockStatement::accept(StatementVisitor& visitor) const { visitor.visitBlockStatement(*this); }
inline void VariableDeclarationStatement::accept(StatementVisitor& visitor) const { visitor.visitVariableDeclarationStatement(*this); }
inline void OutputStatement::accept(StatementVisitor& visitor) const { visitor.visitOutputStatement(*this); }
inline void PutStatement::accept(StatementVisitor& visitor) const { visitor.visitPutStatement(*this); }
inline void FunctionDeclarationStatement::accept(StatementVisitor& visitor) const { visitor.visitFunctionDeclarationStatement(*this); }
inline void ReturnStatement::accept(StatementVisitor& visitor) const { visitor.visitReturnStatement(*this); }
inline void ImportStatement::accept(StatementVisitor& visitor) const { visitor.visitImportStatement(*this); }
inline void TypedefStatement::accept(StatementVisitor& visitor) const { visitor.visitTypedefStatement(*this); }
inline void IfStatement::accept(StatementVisitor& visitor) const { visitor.visitIfStatement(*this); }
inline void ForStatement::accept(StatementVisitor& visitor) const { visitor.visitForStatement(*this); }
inline void ForeachStatement::accept(StatementVisitor& visitor) const { visitor.visitForeachStatement(*this); }
inline void BreakStatement::accept(StatementVisitor& visitor) const { visitor.visitBreakStatement(*this); }
inline void ContinueStatement::accept(StatementVisitor& visitor) const { visitor.visitContinueStatement(*this); }

}
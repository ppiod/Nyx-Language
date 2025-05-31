#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

#include "./Environment.h"
#include "../parser/AstNodes.h"
#include "../common/Value.h"
#include "../common/Utils.h"
namespace Nyx {

struct NyxModuleData {
    std::shared_ptr<Environment> environment;
    std::vector<std::unique_ptr<Statement>> ast_holder; 
    std::string path; 
};

class NyxDefinedFunction {
public:
    const FunctionDeclarationStatement* declaration_node; 
    std::shared_ptr<Environment> closure_environment;
    std::string name_string;

    NyxDefinedFunction(const FunctionDeclarationStatement* decl_node, std::shared_ptr<Environment> closure)
        : declaration_node(decl_node), closure_environment(std::move(closure)) {
        if (declaration_node) {
            name_string = declaration_node->name.lexeme;
        }
    }

    size_t arity() const {
        if (declaration_node) {
            return declaration_node->params.size();
        }
        return 0;
    }

    std::string name() const {
        return name_string;
    }
};

class Interpreter : public StatementVisitor, public ExpressionVisitor {
public:
    using NativeModuleBuilder = std::function<std::shared_ptr<Environment>()>;

    Interpreter();
    void interpret(const std::string& source_code, 
                   const std::string& script_path_str = "", 
                   const std::vector<std::string>& script_args = {}); 

    void visitExpressionStatement(const ExpressionStatement& stmt) override;
    void visitBlockStatement(const BlockStatement& stmt) override;
    void visitVariableDeclarationStatement(const VariableDeclarationStatement& stmt) override;
    void visitOutputStatement(const OutputStatement& stmt) override;
    void visitPutStatement(const PutStatement& stmt) override;
    void visitFunctionDeclarationStatement(const FunctionDeclarationStatement& stmt) override;
    void visitReturnStatement(const ReturnStatement& stmt) override;
    void visitImportStatement(const ImportStatement& stmt) override;
    void visitTypedefStatement(const TypedefStatement& stmt) override;
    void visitIfStatement(const IfStatement& stmt) override;
    void visitForStatement(const ForStatement& stmt) override;
    void visitForeachStatement(const ForeachStatement& stmt) override;
    void visitSwitchStatement(const SwitchStatement& stmt) override;
    void visitBreakStatement(const BreakStatement& stmt) override;
    void visitContinueStatement(const ContinueStatement& stmt) override;

    NyxValue visitLiteralExpression(const LiteralExpression& expr) override;
    NyxValue visitIdentifierExpression(const IdentifierExpression& expr) override;
    NyxValue visitAssignmentExpression(const AssignmentExpression& expr) override;
    NyxValue visitUnaryExpression(const UnaryExpression& expr) override;
    NyxValue visitBinaryExpression(const BinaryExpression& expr) override;
    NyxValue visitPostfixUpdateExpression(const PostfixUpdateExpression& expr) override;
    NyxValue visitListLiteralExpression(const ListLiteralExpression& expr) override;
    NyxValue visitLenExpression(const LenExpression& expr) override;
    NyxValue visitSubscriptExpression(const SubscriptExpression& expr) override;
    NyxValue visitInterpolatedStringExpression(const InterpolatedStringExpression& expr) override;
    NyxValue visitCallExpression(const CallExpression& expr) override;
    NyxValue visitMemberAccessExpression(const MemberAccessExpression& expr) override;
    
    void registerNativeModule(const std::string& name, NativeModuleBuilder builder);
    std::shared_ptr<Environment> globals;

    bool isDoubleInteger(double n) const;
    NyxValue executeFunctionBody(const NyxDefinedFunction& function, const std::vector<NyxValue>& arguments);

private:
    std::shared_ptr<Environment> environment;
    
    std::vector<std::unique_ptr<Statement>> main_ast_program; 
    std::string current_script_directory; 
    
    static std::map<std::string, NyxValue> loaded_modules_cache; 
    static std::map<std::string, NativeModuleBuilder> native_module_builders; 
    static bool core_modules_registered; 

    NyxValue interpretModule(const std::string& module_source_code, const std::string& module_path);
    void executeProgram(const std::vector<std::unique_ptr<Statement>>& program, std::shared_ptr<Environment> execution_globals, std::shared_ptr<Environment> execution_env);

    NyxValue evaluate(const Expression& expr);
    void execute(const Statement& stmt);
    void executeBlock(const std::vector<std::unique_ptr<Statement>>& statements, std::shared_ptr<Environment> execution_environment);

    bool isTruthy(const NyxValue& value) const;
    bool isEqual(const NyxValue& a, const NyxValue& b) const;

    std::string resolveModulePath(const std::string& importing_file_dir, const std::string& module_path_literal) const;
};

}
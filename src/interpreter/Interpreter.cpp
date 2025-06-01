#include "./Interpreter.h"
#include <iostream>
#include <cmath>
#include <sstream> 
#include <fstream>
#include <filesystem>
#include "../common/ControlFlow.h"
#include "../tokenizer/Tokenizer.h"
#include "../parser/Parser.h"
#include "../stdlib/native_stdlib.h"

namespace Nyx {

std::map<std::string, NyxValue> Interpreter::loaded_modules_cache;
std::map<std::string, Interpreter::NativeModuleBuilder> Interpreter::native_module_builders;
bool Interpreter::core_modules_registered = false;

Interpreter::Interpreter() {
    globals = std::make_shared<Environment>();
    environment = globals;
    current_script_directory = "";

    if (!core_modules_registered) {
        registerNyxStandardLibrary(*this);
        core_modules_registered = true;
    }
}

void Interpreter::registerNativeModule(const std::string& name, NativeModuleBuilder builder) {
    native_module_builders[name] = builder;
}

NyxValue Interpreter::evaluate(const Expression& expr) {
    return expr.accept(*this);
}

void Interpreter::execute(const Statement& stmt) {
    stmt.accept(*this);
}

bool Interpreter::isDoubleInteger(double n) const {
    return std::trunc(n) == n;
}

bool Interpreter::isTruthy(const NyxValue& value_holder) const {
    const auto& value = value_holder.data;
    if (std::holds_alternative<std::monostate>(value)) return false;
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
    if (std::holds_alternative<double>(value)) return std::get<double>(value) != 0.0;
    if (std::holds_alternative<std::string>(value)) return !std::get<std::string>(value).empty();
    if (std::holds_alternative<NyxList>(value)) return !std::get<NyxList>(value).empty();
    if (std::holds_alternative<UserDefinedFunctionPtr>(value)) return true;
    if (std::holds_alternative<NyxModule>(value)) return true;
    if (std::holds_alternative<NativeFunctionPtr>(value)) return true;
    if (std::holds_alternative<SDLWindowNyxPtr>(value)) return std::get<SDLWindowNyxPtr>(value) != nullptr;
    if (std::holds_alternative<SDLRendererNyxPtr>(value)) return std::get<SDLRendererNyxPtr>(value) != nullptr;
    if (std::holds_alternative<SDLFontNyxPtr>(value)) return std::get<SDLFontNyxPtr>(value) != nullptr;
    if (std::holds_alternative<SDLSurfaceNyxPtr>(value)) return std::get<SDLSurfaceNyxPtr>(value) != nullptr;
    if (std::holds_alternative<SDLTextureNyxPtr>(value)) return std::get<SDLTextureNyxPtr>(value) != nullptr;
    return false;
}

bool Interpreter::isEqual(const NyxValue& a_holder, const NyxValue& b_holder) const {
    const auto& a_data = a_holder.data;
    const auto& b_data = b_holder.data;

    if (a_data.index() != b_data.index()) return false;

    if (std::holds_alternative<std::monostate>(a_data)) return true;
    if (std::holds_alternative<bool>(a_data)) return std::get<bool>(a_data) == std::get<bool>(b_data);
    if (std::holds_alternative<double>(a_data)) return std::get<double>(a_data) == std::get<double>(b_data);
    if (std::holds_alternative<std::string>(a_data)) return std::get<std::string>(a_data) == std::get<std::string>(b_data);
    
    if (std::holds_alternative<NyxList>(a_data)) {
        const auto& list_a = std::get<NyxList>(a_data);
        const auto& list_b = std::get<NyxList>(b_data);
        if (list_a.size() != list_b.size()) return false;
        for (size_t i = 0; i < list_a.size(); ++i) {
            if (!isEqual(list_a[i], list_b[i])) return false;
        }
        return true;
    }
    if (std::holds_alternative<UserDefinedFunctionPtr>(a_data)) {
        return std::get<UserDefinedFunctionPtr>(a_data) == std::get<UserDefinedFunctionPtr>(b_data);
    }
    if (std::holds_alternative<NyxModule>(a_data)) {
        return std::get<NyxModule>(a_data) == std::get<NyxModule>(b_data);
    }
    if (std::holds_alternative<NativeFunctionPtr>(a_data)) {
        return std::get<NativeFunctionPtr>(a_data) == std::get<NativeFunctionPtr>(b_data);
    }
    if (std::holds_alternative<SDLWindowNyxPtr>(a_data)) {
        return std::get<SDLWindowNyxPtr>(a_data) == std::get<SDLWindowNyxPtr>(b_data);
    }
    if (std::holds_alternative<SDLRendererNyxPtr>(a_data)) {
        return std::get<SDLRendererNyxPtr>(a_data) == std::get<SDLRendererNyxPtr>(b_data);
    }
    if (std::holds_alternative<SDLFontNyxPtr>(a_data)) {
        return std::get<SDLFontNyxPtr>(a_data) == std::get<SDLFontNyxPtr>(b_data);
    }
    if (std::holds_alternative<SDLSurfaceNyxPtr>(a_data)) {
        return std::get<SDLSurfaceNyxPtr>(a_data) == std::get<SDLSurfaceNyxPtr>(b_data);
    }
    if (std::holds_alternative<SDLTextureNyxPtr>(a_data)) {
        return std::get<SDLTextureNyxPtr>(a_data) == std::get<SDLTextureNyxPtr>(b_data);
    }
    
    return false;
}

void Interpreter::executeBlock(const std::vector<std::unique_ptr<Statement>>& statements, std::shared_ptr<Environment> execution_environment) {
    std::shared_ptr<Environment> previous_env = this->environment;
    this->environment = execution_environment;
    
    try {
        for (const auto& statement_ptr : statements) {
            if (statement_ptr) {
                execute(*statement_ptr);
            }
        }
    } catch (const Common::NyxReturnSignal& ret) {
        this->environment = previous_env;
        throw;
    }
    catch (...) {
        this->environment = previous_env;
        throw;
    }
    this->environment = previous_env;
}


void Interpreter::visitExpressionStatement(const ExpressionStatement& stmt) {
    evaluate(*stmt.expression);
}

void Interpreter::visitBlockStatement(const BlockStatement& stmt) {
    executeBlock(stmt.statements, std::make_shared<Environment>(this->environment));
}

void Interpreter::visitVariableDeclarationStatement(const VariableDeclarationStatement& stmt) {
    NyxValue value = NyxValue(std::monostate{});
    if (stmt.initializer) {
        value = evaluate(*stmt.initializer);
    }
    environment->define(stmt.identifier.lexeme, value);
}

void Interpreter::visitOutputStatement(const OutputStatement& stmt) {
    NyxValue value_holder = evaluate(*stmt.argument);
    std::string str_representation = nyxValueToString(value_holder);

    if (std::holds_alternative<std::string>(value_holder.data)) {
        std::string original_nyx_string = std::get<std::string>(value_holder.data);
        str_representation = Common::process_escapes(original_nyx_string);
    } else {
        str_representation = nyxValueToString(value_holder);
    }
    
    std::cout << str_representation << std::endl;
}

void Interpreter::visitPutStatement(const PutStatement& stmt) {
    NyxValue value_holder = evaluate(*stmt.argument);
    std::string str_representation;

    if (std::holds_alternative<std::string>(value_holder.data)) {
        std::string original_nyx_string = std::get<std::string>(value_holder.data);
        str_representation = Common::process_escapes(original_nyx_string);
    } else {
        str_representation = nyxValueToString(value_holder);
    }

    std::cout << str_representation;
    std::cout.flush();
}

void Interpreter::visitFunctionDeclarationStatement(const FunctionDeclarationStatement& stmt) {
    auto function = std::make_shared<NyxDefinedFunction>(&stmt, this->environment);
    environment->define(stmt.name.lexeme, NyxValue(function));
}

void Interpreter::visitReturnStatement(const ReturnStatement& stmt) {
    NyxValue value(std::monostate{});
    if (stmt.value) {
        value = evaluate(*(stmt.value));
    }
    throw Common::NyxReturnSignal(value);
}

std::string Interpreter::resolveModulePath(const std::string& importing_file_dir, const std::string& module_path_literal) const {
    std::filesystem::path raw_module_path(module_path_literal);
    std::filesystem::path base_import_dir(importing_file_dir);
    std::filesystem::path combined_path;

    if (raw_module_path.is_absolute()) {
        combined_path = raw_module_path;
    } else {
        combined_path = base_import_dir / raw_module_path;
    }
    
    try {
        return std::filesystem::canonical(combined_path).string();
    } catch (const std::filesystem::filesystem_error& e) {
        throw Common::NyxRuntimeException(
            "Error resolving module path '" + module_path_literal +
            "'. Canonical path failure for '" + combined_path.string() + "': " + e.what(),
            0);
    }
}

void Interpreter::visitImportStatement(const ImportStatement& stmt) {
    std::string module_path_or_name = stmt.path_literal.lexeme;

    if (module_path_or_name.rfind("std:", 0) == 0) {
        auto it_cache = loaded_modules_cache.find(module_path_or_name);
        if (it_cache != loaded_modules_cache.end()) {
            environment->define(stmt.alias_name.lexeme, it_cache->second);
            return;
        }

        auto builder_it = native_module_builders.find(module_path_or_name);
        if (builder_it != native_module_builders.end()) {
            std::shared_ptr<Environment> module_env = builder_it->second();
            
            auto module_data = std::make_shared<NyxModuleData>();
            module_data->environment = module_env;
            module_data->path = module_path_or_name;
            
            NyxValue module_val(module_data);
            loaded_modules_cache[module_path_or_name] = module_val;
            environment->define(stmt.alias_name.lexeme, module_val);
            return;
        } else {
            throw Common::NyxRuntimeException("Unknown native module: '" + module_path_or_name + "'.", stmt.path_literal.line);
        }
    }

    std::string resolved_path_str;
    try {
        resolved_path_str = resolveModulePath(this->current_script_directory, module_path_or_name);
    } catch (const Common::NyxRuntimeException& e) {
        throw Common::NyxRuntimeException(e.what(), stmt.path_literal.line);
    }
    
    auto it_cache = loaded_modules_cache.find(resolved_path_str);
    if (it_cache != loaded_modules_cache.end()) {
        environment->define(stmt.alias_name.lexeme, it_cache->second);
        return;
    }

    std::ifstream file(resolved_path_str);
    if (!file.is_open()) {
        throw Common::NyxRuntimeException("Could not open module file '" + resolved_path_str + "'.", stmt.path_literal.line);
    }
    std::string module_source_code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    NyxValue module_value = interpretModule(module_source_code, resolved_path_str);
    loaded_modules_cache[resolved_path_str] = module_value;
    environment->define(stmt.alias_name.lexeme, module_value);
}

void Interpreter::visitTypedefStatement(const TypedefStatement& stmt) {
    NyxValue value_to_check = evaluate(*stmt.expression_to_check);
    std::cout << nyxValueTypeToString(value_to_check) << std::endl;
}

void Interpreter::visitIfStatement(const IfStatement& stmt) {
    NyxValue condition_value = evaluate(*stmt.condition);
    if (isTruthy(condition_value)) {
        execute(*stmt.then_branch);
    } else if (stmt.else_branch != nullptr) {
        execute(*stmt.else_branch);
    }
}

void Interpreter::visitForStatement(const ForStatement& stmt) {
    std::shared_ptr<Environment> previous_scope = this->environment;
    this->environment = std::make_shared<Environment>(previous_scope);

    try {
        if (stmt.initializer) {
            execute(*stmt.initializer);
        }

        while (true) {
            NyxValue condition_val = NyxValue(true);
            if (stmt.condition) {
                condition_val = evaluate(*stmt.condition);
            }

            if (!isTruthy(condition_val)) {
                break;
            }

            try {
                if (stmt.body) {
                     execute(*stmt.body);
                }
            } catch (const Common::NyxContinueSignal&) {
            } catch (const Common::NyxBreakSignal&) {
                this->environment = previous_scope;
                return;
            }

            if (stmt.increment) {
                 execute(*stmt.increment);
            }
        }
    } catch (const Common::NyxBreakSignal&) {
    } catch (const Common::NyxReturnSignal& ret) {
        this->environment = previous_scope;
        throw;
    }
     catch (...) {
        this->environment = previous_scope;
        throw;
    }
    this->environment = previous_scope;
}

void Interpreter::visitForeachStatement(const ForeachStatement& stmt) {
    NyxValue iterable_value = evaluate(*stmt.iterable_expression);

    if (!std::holds_alternative<NyxList>(iterable_value.data)) {
        throw Common::NyxRuntimeException("Foreach loop requires a list as iterable.", stmt.iterable_expression->token.line);
    }

    const NyxList& list_data = std::get<NyxList>(iterable_value.data);

    for (const NyxValue& item_in_list : list_data) {
        std::shared_ptr<Environment> loop_iteration_env = std::make_shared<Environment>(environment);
        
        NyxValue loop_var_value = item_in_list;


        loop_iteration_env->define(stmt.loop_variable_token.lexeme, loop_var_value);
        
        std::shared_ptr<Environment> previous_env = environment;
        environment = loop_iteration_env;

        try {
            execute(*stmt.body_statement);
        } catch (const Common::NyxBreakSignal&) {
            environment = previous_env; 
            break; 
        } catch (const Common::NyxContinueSignal&) {
            environment = previous_env; 
        } catch (...) {
            environment = previous_env; 
            throw;
        }
        environment = previous_env; 
    }
}

void Interpreter::visitSwitchStatement(const SwitchStatement& stmt) {
    NyxValue condition_value = evaluate(*stmt.condition);
    int matched_case_index = -1;
    int default_case_index = -1;

    for (size_t i = 0; i < stmt.cases.size(); ++i) {
        const auto& case_block = stmt.cases[i];
        if (case_block.is_default) {
            default_case_index = static_cast<int>(i);
        } else {
            NyxValue case_expr_value = evaluate(*case_block.value_expression);
            if (isEqual(condition_value, case_expr_value)) {
                matched_case_index = static_cast<int>(i);
                break;
            }
        }
    }

    int start_execution_from_index = -1;
    if (matched_case_index != -1) {
        start_execution_from_index = matched_case_index;
    } else if (default_case_index != -1) {
        start_execution_from_index = default_case_index;
    }

    if (start_execution_from_index != -1) {
        for (size_t i = static_cast<size_t>(start_execution_from_index); i < stmt.cases.size(); ++i) {
            const auto& case_to_execute = stmt.cases[i];

            std::shared_ptr<Environment> case_env = std::make_shared<Environment>(environment);
            std::shared_ptr<Environment> previous_env = environment;
            environment = case_env;

            bool broken_out = false;
            try {
                for (const auto& statement_in_case : case_to_execute.statements) {
                    execute(*statement_in_case);
                }
            } catch (const Common::NyxBreakSignal&) {
                broken_out = true;
            } catch (const Common::NyxContinueSignal&) {
                 environment = previous_env;
                 throw; 
            } catch (const Common::NyxReturnSignal&) {
                 environment = previous_env;
                 throw;
            } catch (...) {
                environment = previous_env;
                throw;
            }
            
            environment = previous_env;
            if (broken_out) {
                break;
            }
        }
    }
}

void Interpreter::visitStructDeclarationStatement(const StructDeclarationStatement& stmt) {
    std::string name = stmt.name_token.lexeme;
    if (environment->isDefinedLocally(name)) { 
        throw Common::NyxRuntimeException("Struct '" + name + "' already defined in this scope.", stmt.name_token.line);
    }

    auto struct_def = std::make_shared<NyxStructDefinition>(name, stmt.field_name_tokens);

    environment->define(name, NyxValue(struct_def)); 
}

NyxValue Interpreter::visitStructInitializerExpression(const StructInitializerExpression& expr) {
    std::string struct_name = expr.name_token.lexeme;
    std::optional<NyxValue> def_value_opt = environment->get(struct_name);

    if (!def_value_opt || !std::holds_alternative<StructDefinitionPtr>(def_value_opt->data)) {
        throw Common::NyxRuntimeException("Undefined struct type '" + struct_name + "'.", expr.name_token.line);
    }
    StructDefinitionPtr struct_def = std::get<StructDefinitionPtr>(def_value_opt->data);

    auto instance = std::make_shared<NyxStructInstance>(struct_def);

    std::vector<bool> initialized_fields(struct_def->field_names_in_order.size(), false);

    for (const auto& pair : expr.initializers) {
        const Token& field_name_token = pair.first;
        const std::string& field_name = field_name_token.lexeme;

        auto it = struct_def->field_indices.find(field_name);
        if (it == struct_def->field_indices.end()) {
            throw Common::NyxRuntimeException("Struct '" + struct_name + "' has no field named '" + field_name + "'.", field_name_token.line);
        }
        size_t field_idx = it->second;
        if (initialized_fields[field_idx]) {
             throw Common::NyxRuntimeException("Field '" + field_name + "' initialized more than once.", field_name_token.line);
        }

        instance->field_values[field_idx] = evaluate(*pair.second);
        initialized_fields[field_idx] = true;
    }

    return NyxValue(instance);
}

void Interpreter::visitBreakStatement(const BreakStatement& stmt) {
    throw Common::NyxBreakSignal();
}

void Interpreter::visitContinueStatement(const ContinueStatement& stmt) {
    throw Common::NyxContinueSignal();
}

NyxValue Interpreter::visitAssignmentExpression(const AssignmentExpression& expr) {
    NyxValue value_to_assign = evaluate(*expr.value);

    if (auto id_target = dynamic_cast<const IdentifierExpression*>(expr.target.get())) {
        if (!environment->assign(id_target->name, value_to_assign)) {
            throw Common::NyxRuntimeException("Undefined variable '" + id_target->name + "' in assignment.", id_target->token.line);
        }
    } else if (auto sub_target = dynamic_cast<const SubscriptExpression*>(expr.target.get())) {
        NyxValue list_obj_holder = evaluate(*sub_target->object);
        
        if (!std::holds_alternative<NyxList>(list_obj_holder.data)) {
            throw Common::NyxRuntimeException("Cannot assign to subscript of non-list type.", sub_target->token.line);
        }

        NyxValue index_holder = evaluate(*sub_target->index);
        if (!std::holds_alternative<double>(index_holder.data)) {
            throw Common::NyxRuntimeException("List index for assignment must be a number.", sub_target->closing_bracket.line);
        }
        double raw_index_double = std::get<double>(index_holder.data);
        if (!isDoubleInteger(raw_index_double)) {
            throw Common::NyxRuntimeException("List index for assignment must be an integer.", sub_target->closing_bracket.line);
        }
        
        long long requested_index = static_cast<long long>(std::trunc(raw_index_double));
        NyxList list_copy = std::get<NyxList>(list_obj_holder.data);
        long long list_size = static_cast<long long>(list_copy.size());

        if (list_size == 0 && (requested_index < 0 || requested_index >= 0) ) {
             throw Common::NyxRuntimeException("List index out of bounds for assignment: list is empty. Index: " + std::to_string(requested_index), sub_target->closing_bracket.line);
        }
        
        if (requested_index < 0) {
            requested_index = list_size + requested_index;
        }
        
        if (requested_index < 0 || requested_index >= list_size) {
            throw Common::NyxRuntimeException("List index out of bounds for assignment. Requested: " + 
                                             std::to_string(static_cast<long long>(std::trunc(raw_index_double))) +
                                             ", Effective: " + std::to_string(requested_index) +
                                             ", Size: " + std::to_string(list_size), sub_target->closing_bracket.line);
        }
        size_t actual_index = static_cast<size_t>(requested_index);
        list_copy[actual_index] = value_to_assign;
        
        if (auto list_identifier = dynamic_cast<const IdentifierExpression*>(sub_target->object.get())) {
            if (!environment->assign(list_identifier->name, NyxValue(list_copy))) {
                 throw Common::NyxRuntimeException("Failed to re-assign modified list to variable '" + list_identifier->name + "'.", list_identifier->token.line);
            }
        } else {
            throw Common::NyxRuntimeException("Cannot assign to subscript of a temporary list or complex expression.", sub_target->token.line);
        }
    } else if (auto member_target = dynamic_cast<const MemberAccessExpression*>(expr.target.get())) {
        NyxValue object_val = evaluate(*member_target->object);
        
        if (std::holds_alternative<NyxModule>(object_val.data)) {
            auto module_data_ptr = std::get<NyxModule>(object_val.data);
            if (!module_data_ptr || !module_data_ptr->environment) {
                 throw Common::NyxRuntimeException("Invalid module object for member assignment.", member_target->token.line);
            }
            if (!module_data_ptr->environment->assign(member_target->name.lexeme, value_to_assign)) {
                throw Common::NyxRuntimeException("Cannot assign to undefined member '" + member_target->name.lexeme + "' in module '" + module_data_ptr->path + "'.", member_target->name.line);
            }
        } else if (std::holds_alternative<StructInstancePtr>(object_val.data)) {
            auto instance_ptr = std::get<StructInstancePtr>(object_val.data);
            if (!instance_ptr || !instance_ptr->definition) {
                throw Common::NyxRuntimeException("Invalid struct instance for field assignment.", member_target->token.line);
            }
            const std::string& field_name = member_target->name.lexeme;
            auto it = instance_ptr->definition->field_indices.find(field_name);
            if (it == instance_ptr->definition->field_indices.end()) {
                throw Common::NyxRuntimeException("Struct '" + instance_ptr->definition->name + "' has no field named '" + field_name + "'.", member_target->name.line);
            }
            size_t field_idx = it->second;
            if (field_idx >= instance_ptr->field_values.size()){
                 throw Common::NyxRuntimeException("Field index out of bounds for struct '" + instance_ptr->definition->name + "'. This should not happen.", member_target->name.line);
            }
            instance_ptr->field_values[field_idx] = value_to_assign;
        }
        else {
            throw Common::NyxRuntimeException("Base of member assignment '.' must be a module or a struct instance.", member_target->token.line);
        }
    }
    else {
        throw Common::NyxRuntimeException("Invalid assignment target.", expr.equals_token.line);
    }
    return value_to_assign;
}

NyxValue Interpreter::visitLiteralExpression(const LiteralExpression& expr) {
    return expr.value;
}

NyxValue Interpreter::visitIdentifierExpression(const IdentifierExpression& expr) {
    std::optional<NyxValue> value_opt = environment->get(expr.name);
    if (value_opt) {
        return *value_opt;
    }
    throw Common::NyxRuntimeException("Undefined variable '" + expr.name + "'.", expr.token.line);
}

NyxValue Interpreter::visitPostfixUpdateExpression(const PostfixUpdateExpression& expr) {
    if (auto id_operand = dynamic_cast<const IdentifierExpression*>(expr.operand.get())) {
        std::string name = id_operand->name;
        Token name_token = id_operand->token;
        std::optional<NyxValue> var_value_opt = environment->get(name);

        if (!var_value_opt) {
            throw Common::NyxRuntimeException("Undefined variable '" + name + "' for '++/--'.", name_token.line);
        }
        NyxValue original_value_holder = *var_value_opt;
        if (!std::holds_alternative<double>(original_value_holder.data)) {
            throw Common::NyxRuntimeException("Operand for '++/--' on variable must be a number.", expr.operator_token.line);
        }
        double number_val = std::get<double>(original_value_holder.data);
        double new_val = (expr.operator_token.type == TokenType::PLUS_PLUS) ? (number_val + 1.0) : (number_val - 1.0);
        environment->assign(name, NyxValue(new_val));
        return original_value_holder;
    } else if (auto sub_operand = dynamic_cast<const SubscriptExpression*>(expr.operand.get())) {
        NyxValue list_obj_holder = evaluate(*sub_operand->object);
        if (!std::holds_alternative<NyxList>(list_obj_holder.data)) {
            throw Common::NyxRuntimeException("Operand for '++/--' with subscript must be a list.", sub_operand->token.line);
        }
        NyxValue index_holder = evaluate(*sub_operand->index);
        if (!std::holds_alternative<double>(index_holder.data)) {
            throw Common::NyxRuntimeException("List index for '++/--' must be a number.", sub_operand->closing_bracket.line);
        }
        double raw_index = std::get<double>(index_holder.data);
        if (!isDoubleInteger(raw_index)) {
            throw Common::NyxRuntimeException("List index for '++/--' must be an integer.", sub_operand->closing_bracket.line);
        }
        double int_index_double = std::trunc(raw_index);
        NyxList list_copy = std::get<NyxList>(list_obj_holder.data);
        if (int_index_double < 0 || static_cast<size_t>(int_index_double) >= list_copy.size()) {
            throw Common::NyxRuntimeException("List index out of bounds for '++/--'.", sub_operand->closing_bracket.line);
        }
        size_t actual_index = static_cast<size_t>(int_index_double);
        NyxValue element_original_value = list_copy[actual_index];
        if (!std::holds_alternative<double>(element_original_value.data)) {
            throw Common::NyxRuntimeException("Element for '++/--' must be a number.", expr.operator_token.line);
        }
        double element_number_val = std::get<double>(element_original_value.data);
        double element_new_val = (expr.operator_token.type == TokenType::PLUS_PLUS) ? (element_number_val + 1.0) : (element_number_val - 1.0);
        list_copy[actual_index] = NyxValue(element_new_val);

        if (auto list_identifier = dynamic_cast<const IdentifierExpression*>(sub_operand->object.get())) {
            environment->assign(list_identifier->name, NyxValue(list_copy));
        } else {
            throw Common::NyxRuntimeException("Cannot apply '++/--' to subscript of a temporary list.", sub_operand->token.line);
        }
        return element_original_value;
    }

    throw Common::NyxRuntimeException("Operand for '++/--' must be an identifier or list element.", expr.operator_token.line);
}

NyxValue Interpreter::visitUnaryExpression(const UnaryExpression& expr) {
    NyxValue right_value_holder = evaluate(*expr.right);
    const auto& right_variant_data = right_value_holder.data;

    switch (expr.operator_token.type) {
        case TokenType::MINUS:
            if (std::holds_alternative<double>(right_variant_data)) {
                return NyxValue(-std::get<double>(right_variant_data));
            }
            throw Common::NyxRuntimeException("Operand for unary '-' must be a number.", expr.operator_token.line);
        case TokenType::KEYWORD_NOT:
        case TokenType::BANG:
            return NyxValue(!isTruthy(right_value_holder));
        default:
            throw Common::NyxRuntimeException("Unknown unary operator.", expr.operator_token.line);
    }
}

NyxValue Interpreter::visitBinaryExpression(const BinaryExpression& expr) {
    NyxValue left_value_holder = evaluate(*expr.left);
    
    if (expr.operator_token.type == TokenType::KEYWORD_OR) {
        if (isTruthy(left_value_holder)) return left_value_holder;
    } else if (expr.operator_token.type == TokenType::KEYWORD_AND) {
        if (!isTruthy(left_value_holder)) return left_value_holder;
    }

    NyxValue right_value_holder = evaluate(*expr.right);
    
    const auto& left_data = left_value_holder.data;
    const auto& right_data = right_value_holder.data;

    switch (expr.operator_token.type) {
        case TokenType::PLUS:
            if (std::holds_alternative<double>(left_data) && std::holds_alternative<double>(right_data)) {
                return NyxValue(std::get<double>(left_data) + std::get<double>(right_data));
            }
            if (std::holds_alternative<std::string>(left_data) && std::holds_alternative<std::string>(right_data)) {
                return NyxValue(std::get<std::string>(left_data) + std::get<std::string>(right_data));
            }
            if (std::holds_alternative<NyxList>(left_data) && std::holds_alternative<NyxList>(right_data)) {
                NyxList result_list = std::get<NyxList>(left_data);
                const auto& list_to_add = std::get<NyxList>(right_data);
                result_list.insert(result_list.end(), list_to_add.begin(), list_to_add.end());
                return NyxValue(result_list);
            }
            throw Common::NyxRuntimeException("Operands for '+' must be two numbers, two strings, or two lists.", expr.operator_token.line);
        case TokenType::MINUS:
            if (std::holds_alternative<double>(left_data) && std::holds_alternative<double>(right_data)) {
                return NyxValue(std::get<double>(left_data) - std::get<double>(right_data));
            }
            throw Common::NyxRuntimeException("Operands for '-' must be numbers.", expr.operator_token.line);
        case TokenType::STAR:
            if (std::holds_alternative<double>(left_data) && std::holds_alternative<double>(right_data)) {
                return NyxValue(std::get<double>(left_data) * std::get<double>(right_data));
            }
            if (std::holds_alternative<NyxList>(left_data) && std::holds_alternative<double>(right_data)) {
                const auto& list_operand = std::get<NyxList>(left_data);
                double num_operand_double = std::get<double>(right_data);
                if (!isDoubleInteger(num_operand_double) || num_operand_double < 0) {
                    throw Common::NyxRuntimeException("List repetition count for '*' must be a non-negative integer.", expr.operator_token.line);
                }
                size_t repeat_count = static_cast<size_t>(num_operand_double);
                NyxList result_list;
                result_list.reserve(list_operand.size() * repeat_count);
                for (size_t i = 0; i < repeat_count; ++i) {
                    result_list.insert(result_list.end(), list_operand.begin(), list_operand.end());
                }
                return NyxValue(result_list);
            }
             if (std::holds_alternative<double>(left_data) && std::holds_alternative<NyxList>(right_data)) {
                const auto& list_operand = std::get<NyxList>(right_data);
                double num_operand_double = std::get<double>(left_data);
                if (!isDoubleInteger(num_operand_double) || num_operand_double < 0) {
                    throw Common::NyxRuntimeException("List repetition count for '*' must be a non-negative integer.", expr.operator_token.line);
                }
                size_t repeat_count = static_cast<size_t>(num_operand_double);
                NyxList result_list;
                result_list.reserve(list_operand.size() * repeat_count);
                for (size_t i = 0; i < repeat_count; ++i) {
                    result_list.insert(result_list.end(), list_operand.begin(), list_operand.end());
                }
                return NyxValue(result_list);
            }
            throw Common::NyxRuntimeException("Operands for '*' must be two numbers or a list and a non-negative integer.", expr.operator_token.line);
        case TokenType::SLASH:
            if (std::holds_alternative<double>(left_data) && std::holds_alternative<double>(right_data)) {
                if (std::get<double>(right_data) == 0.0) {
                    throw Common::NyxRuntimeException("Division by zero.", expr.operator_token.line);
                }
                return NyxValue(std::get<double>(left_data) / std::get<double>(right_data));
            }
            throw Common::NyxRuntimeException("Operands for '/' must be numbers.", expr.operator_token.line);
        case TokenType::PERCENT:
             if (std::holds_alternative<double>(left_data) && std::holds_alternative<double>(right_data)) {
                double left_num = std::get<double>(left_data);
                double right_num = std::get<double>(right_data);
                if (right_num == 0.0) {
                    throw Common::NyxRuntimeException("Modulo by zero.", expr.operator_token.line);
                }
                return NyxValue(std::fmod(left_num, right_num));
            }
            throw Common::NyxRuntimeException("Operands for '%' must be numbers.", expr.operator_token.line);
        
        case TokenType::GREATER:
            if (std::holds_alternative<double>(left_data) && std::holds_alternative<double>(right_data)) {
                return NyxValue(std::get<double>(left_data) > std::get<double>(right_data));
            }
            throw Common::NyxRuntimeException("Operands for '>' must be numbers.", expr.operator_token.line);
        case TokenType::GREATER_EQUAL:
            if (std::holds_alternative<double>(left_data) && std::holds_alternative<double>(right_data)) {
                return NyxValue(std::get<double>(left_data) >= std::get<double>(right_data));
            }
            throw Common::NyxRuntimeException("Operands for '>=' must be numbers.", expr.operator_token.line);
        case TokenType::LESS:
            if (std::holds_alternative<double>(left_data) && std::holds_alternative<double>(right_data)) {
                return NyxValue(std::get<double>(left_data) < std::get<double>(right_data));
            }
            throw Common::NyxRuntimeException("Operands for '<' must be numbers.", expr.operator_token.line);
        case TokenType::LESS_EQUAL:
            if (std::holds_alternative<double>(left_data) && std::holds_alternative<double>(right_data)) {
                return NyxValue(std::get<double>(left_data) <= std::get<double>(right_data));
            }
            throw Common::NyxRuntimeException("Operands for '<=' must be numbers.", expr.operator_token.line);

        case TokenType::EQUAL_EQUAL:
            return NyxValue(isEqual(left_value_holder, right_value_holder));
        case TokenType::BANG_EQUAL:
            return NyxValue(!isEqual(left_value_holder, right_value_holder));

        case TokenType::KEYWORD_OR:
             return right_value_holder;
        case TokenType::KEYWORD_AND:
             return right_value_holder;
        
        default:
            throw Common::NyxRuntimeException("Unknown binary operator.", expr.operator_token.line);
    }
}

NyxValue Interpreter::visitListLiteralExpression(const ListLiteralExpression& expr) {
    NyxList elements_values;
    elements_values.reserve(expr.elements.size());
    for (const auto& element_expr : expr.elements) {
        elements_values.push_back(evaluate(*element_expr));
    }
    return NyxValue(elements_values);
}

NyxValue Interpreter::visitLenExpression(const LenExpression& expr) {
    NyxValue argument_value_holder = evaluate(*expr.argument);
    const auto& arg_data = argument_value_holder.data;

    if (std::holds_alternative<NyxList>(arg_data)) {
        return NyxValue(static_cast<double>(std::get<NyxList>(arg_data).size()));
    } else if (std::holds_alternative<std::string>(arg_data)) {
        return NyxValue(static_cast<double>(std::get<std::string>(arg_data).length()));
    }
    throw Common::NyxRuntimeException("Operand for 'len' must be a list or a string.", expr.token.line);
}

NyxValue Interpreter::visitSubscriptExpression(const SubscriptExpression& expr) {
    NyxValue object_value_holder = evaluate(*expr.object);
    const auto& object_data = object_value_holder.data;

    NyxValue index_value_holder = evaluate(*expr.index);
    const auto& index_data = index_value_holder.data;

    if (std::holds_alternative<NyxList>(object_data)) {
        const auto& list = std::get<NyxList>(object_data);
        if (!std::holds_alternative<double>(index_data)) {
            throw Common::NyxRuntimeException("List index must be a number.", expr.closing_bracket.line);
        }
        double raw_index = std::get<double>(index_value_holder.data);
        if (!isDoubleInteger(raw_index)) {
            throw Common::NyxRuntimeException("List index must be an integer.", expr.closing_bracket.line);
        }
        
        long long requested_index = static_cast<long long>(std::trunc(raw_index));
        long long list_size = static_cast<long long>(list.size());

        if (list_size == 0) { 
             throw Common::NyxRuntimeException("List index out of bounds: list is empty. Requested index: " + std::to_string(requested_index), expr.closing_bracket.line);
        }
        
        if (requested_index < 0) {
            requested_index = list_size + requested_index;
        }
        
        if (requested_index < 0 || requested_index >= list_size) {
             throw Common::NyxRuntimeException("List index out of bounds. Requested: " + 
                                             std::to_string(static_cast<long long>(std::trunc(raw_index))) +
                                             ", Effective: " + std::to_string(requested_index) +
                                             ", Size: " + std::to_string(list_size), expr.closing_bracket.line);
        }
        size_t actual_index = static_cast<size_t>(requested_index);
        return list[actual_index];

    } else if (std::holds_alternative<std::string>(object_data)) {
        const auto& str = std::get<std::string>(object_data);
        if (!std::holds_alternative<double>(index_data)) {
            throw Common::NyxRuntimeException("String index must be a number.", expr.closing_bracket.line);
        }
        double raw_index = std::get<double>(index_value_holder.data);
        if (!isDoubleInteger(raw_index)) {
            throw Common::NyxRuntimeException("String index must be an integer.", expr.closing_bracket.line);
        }

        long long requested_index = static_cast<long long>(std::trunc(raw_index));
        long long str_len = static_cast<long long>(str.length());

        if (str_len == 0) { 
             throw Common::NyxRuntimeException("String index out of bounds: string is empty. Requested index: " + std::to_string(requested_index), expr.closing_bracket.line);
        }

        if (requested_index < 0) {
            requested_index = str_len + requested_index;
        }

        if (requested_index < 0 || requested_index >= str_len) {
            throw Common::NyxRuntimeException("String index out of bounds. Requested: " + 
                                             std::to_string(static_cast<long long>(std::trunc(raw_index))) +
                                             ", Effective: " + std::to_string(requested_index) +
                                             ", Size: " + std::to_string(str_len), expr.closing_bracket.line);
        }
        size_t actual_index = static_cast<size_t>(requested_index);
        return NyxValue(std::string(1, str[actual_index]));
    }
    
    throw Common::NyxRuntimeException("Subscript operator '[]' can only be used on lists or strings.", expr.token.line);
}

NyxValue Interpreter::visitInterpolatedStringExpression(const InterpolatedStringExpression& expr) {
    std::stringstream ss;
    for (const auto& segment : expr.segments) {
        if (std::holds_alternative<std::string>(segment)) {
            ss << std::get<std::string>(segment);
        } else if (std::holds_alternative<std::unique_ptr<Expression>>(segment)) {
            const auto& expr_ptr_variant = std::get<std::unique_ptr<Expression>>(segment);
            if (expr_ptr_variant) {
                NyxValue val = evaluate(*expr_ptr_variant);
                ss << nyxValueToString(val);
            }
        }
    }
    return NyxValue(ss.str());
}

NyxValue Interpreter::visitCallExpression(const CallExpression& expr) {
    NyxValue callee_value = evaluate(*expr.callee);
    const auto& callee_data = callee_value.data;

    std::vector<NyxValue> evaluated_args;
    for (const auto& arg_expr : expr.arguments) {
        evaluated_args.push_back(evaluate(*arg_expr));
    }

    if (std::holds_alternative<UserDefinedFunctionPtr>(callee_data)) {
        auto function_ptr = std::get<UserDefinedFunctionPtr>(callee_data);
        if (!function_ptr) {
             throw Common::NyxRuntimeException("Attempted to call a null function pointer.", expr.paren.line);
        }
        const NyxDefinedFunction& function = *function_ptr;
        if (evaluated_args.size() != function.arity()) {
            throw Common::NyxRuntimeException("Expected " + std::to_string(function.arity()) +
                                            " arguments but got " + std::to_string(evaluated_args.size()) + ".",
                                            expr.paren.line);
        }
        return executeFunctionBody(function, evaluated_args);
    } else if (std::holds_alternative<NativeFunctionPtr>(callee_data)) {
        auto native_func_ptr = std::get<NativeFunctionPtr>(callee_data);
        if (!native_func_ptr) {
            throw Common::NyxRuntimeException("Attempted to call a null native function pointer.", expr.paren.line);
        }
        const NyxNativeFunction& native_function = *native_func_ptr;
        if (native_function.arity != -1 && evaluated_args.size() != static_cast<size_t>(native_function.arity)) {
             throw Common::NyxRuntimeException(
                "Native function '" + native_function.name + "' expected " + std::to_string(native_function.arity) +
                " arguments but got " + std::to_string(evaluated_args.size()) + ".",
                expr.paren.line);
        }
        return native_function.callback(*this, evaluated_args);
    }
    
    throw Common::NyxRuntimeException("Can only call functions or native functions.", expr.paren.line);
}

NyxValue Interpreter::visitMemberAccessExpression(const MemberAccessExpression& expr) {
    NyxValue object_val = evaluate(*expr.object);

    if (std::holds_alternative<NyxModule>(object_val.data)) {
        auto module_data_ptr = std::get<NyxModule>(object_val.data);
        if (!module_data_ptr || !module_data_ptr->environment) {
             throw Common::NyxRuntimeException("Invalid module object.", expr.token.line);
        }
        std::optional<NyxValue> member_val = module_data_ptr->environment->get(expr.name.lexeme);
        if (!member_val) {
            throw Common::NyxRuntimeException("Member '" + expr.name.lexeme + "' not found in module '" + module_data_ptr->path + "'.", expr.name.line);
        }
        return *member_val;
    } else if (std::holds_alternative<StructInstancePtr>(object_val.data)) {
        auto instance_ptr = std::get<StructInstancePtr>(object_val.data);
        if (!instance_ptr || !instance_ptr->definition) {
            throw Common::NyxRuntimeException("Invalid struct instance.", expr.token.line);
        }
        const std::string& field_name = expr.name.lexeme;
        auto it = instance_ptr->definition->field_indices.find(field_name);
        if (it == instance_ptr->definition->field_indices.end()) {
            throw Common::NyxRuntimeException("Struct '" + instance_ptr->definition->name + "' has no field named '" + field_name + "'.", expr.name.line);
        }
        size_t field_idx = it->second;
        return instance_ptr->field_values[field_idx];
    }

    throw Common::NyxRuntimeException("Base of member access '.' must be a module or struct instance.", expr.token.line);
}

NyxValue Interpreter::executeFunctionBody(const NyxDefinedFunction& function, const std::vector<NyxValue>& arguments) {
    auto func_env = std::make_shared<Environment>(function.closure_environment);
    
    if (function.declaration_node) {
        for (size_t i = 0; i < function.declaration_node->params.size(); ++i) {
            func_env->define(function.declaration_node->params[i].lexeme, arguments[i]);
        }
    }

    std::shared_ptr<Environment> previous_env = this->environment;
    this->environment = func_env;

    try {
        if (function.declaration_node && function.declaration_node->body) {
            for (const auto& stmt_ptr : function.declaration_node->body->statements) {
                if (stmt_ptr) {
                    execute(*stmt_ptr);
                }
            }
        }
    } catch (const Common::NyxReturnSignal& ret_signal) {
        this->environment = previous_env;
        return ret_signal.value;
    } catch (...) {
        this->environment = previous_env;
        throw;
    }
    
    this->environment = previous_env;
    return NyxValue(std::monostate{});
}

NyxValue Interpreter::interpretModule(const std::string& module_source_code, const std::string& module_path) {
    Interpreter module_interpreter;
    
    std::filesystem::path fs_module_path(module_path);
    module_interpreter.current_script_directory = fs_module_path.parent_path().string();
    
    module_interpreter.globals = std::make_shared<Environment>(this->globals); // Modules get their own globals, possibly inheriting from the main one or a base one
    module_interpreter.environment = module_interpreter.globals;
    module_interpreter.globals->define("nyx_null", NyxValue(std::monostate{}));


    Tokenizer tokenizer(module_source_code);
    std::vector<Token> tokens;
    try {
        tokens = tokenizer.tokenize();
    } catch (const Common::NyxParserException& e) {
        throw Common::NyxRuntimeException("Error tokenizing module '" + module_path + "': " + e.what(), 0);
    }

    Parser parser(tokens);
    std::vector<std::unique_ptr<Statement>> module_ast_nodes;
     try {
        module_ast_nodes = parser.parse();
    } catch (const std::exception& e) {
        throw Common::NyxRuntimeException("Error parsing module '" + module_path + "': " + e.what(), 0);
    }
    
    auto module_data = std::make_shared<NyxModuleData>();
    module_data->environment = module_interpreter.globals;
    module_data->ast_holder = std::move(module_ast_nodes);
    module_data->path = module_path;

    module_interpreter.executeProgram(module_data->ast_holder, module_interpreter.globals, module_interpreter.environment);

    return NyxValue(module_data);
}

void Interpreter::executeProgram(const std::vector<std::unique_ptr<Statement>>& program, std::shared_ptr<Environment> execution_globals, std::shared_ptr<Environment> execution_env) {
    std::shared_ptr<Environment> previous_globals = this->globals;
    std::shared_ptr<Environment> previous_env = this->environment;

    this->globals = execution_globals;
    this->environment = execution_env;

    try {
        for (const auto& statement_ptr : program) {
            if (statement_ptr) {
                execute(*statement_ptr);
            }
        }
    } catch (const Common::NyxRuntimeException& e) {
        this->globals = previous_globals;
        this->environment = previous_env;
        throw;
    } catch (const Common::NyxControlFlowSignal& sig) {
        this->globals = previous_globals;
        this->environment = previous_env;
        
        std::string signal_type = "Control flow signal";
         if(dynamic_cast<const Common::NyxReturnSignal*>(&sig)) {
            signal_type = "return";
             std::cerr << "Runtime Error: '" << signal_type << "' signal used outside of a function call context." << std::endl;
        } else if(dynamic_cast<const Common::NyxBreakSignal*>(&sig)){
            signal_type = "break";
             std::cerr << "Runtime Error: '" << signal_type << "' signal used outside of a loop context." << std::endl;
        } else if (dynamic_cast<const Common::NyxContinueSignal*>(&sig)){
            signal_type = "continue";
             std::cerr << "Runtime Error: '" << signal_type << "' signal used outside of a loop context." << std::endl;
        } else {
            std::cerr << "Runtime Error: Unhandled '" << signal_type << "'." << std::endl;
        }
    }
    
    this->globals = previous_globals;
    this->environment = previous_env;
}

void Interpreter::interpret(const std::string& source_code, 
                           const std::string& script_path_str, 
                           const std::vector<std::string>& script_args) {
    globals = std::make_shared<Environment>();
    environment = globals;
    globals->define("nyx_null", NyxValue(std::monostate{})); 

    NyxList nyx_script_args_list;
    for (const std::string& arg : script_args) {
        nyx_script_args_list.push_back(NyxValue(arg));
    }
    globals->define("SCRIPT_ARGS", NyxValue(nyx_script_args_list));

    if (!script_path_str.empty()) {
        std::filesystem::path initial_path(script_path_str);
        std::filesystem::path full_script_path;

        if (initial_path.is_absolute()) {
            full_script_path = initial_path;
        } else {
            full_script_path = std::filesystem::current_path() / initial_path;
        }
        
        try {
            if (std::filesystem::exists(full_script_path)) {
                if (std::filesystem::is_directory(full_script_path)) {
                     throw Common::NyxRuntimeException("Script path cannot be a directory: " + full_script_path.string(), 0);
                }
                current_script_directory = std::filesystem::canonical(full_script_path).parent_path().string();
            } else {
                if (full_script_path.has_parent_path()){
                    current_script_directory = full_script_path.parent_path().string();
                    std::cerr << "Warning: Script file '" << full_script_path.string() << "' not found by exists(). Using its directory for relative imports." << std::endl;
                } else {
                     throw Common::NyxRuntimeException("Could not determine directory for script path: " + script_path_str, 0);
                }
            }
        } catch(const std::filesystem::filesystem_error& efs) {
             std::cerr << "Warning: Filesystem error determining script directory for '" << script_path_str << "': " << efs.what() << ". Using CWD for module resolution." << std::endl;
            current_script_directory = std::filesystem::current_path().string();
        }
    } else {
        current_script_directory = std::filesystem::current_path().string();
    }

    Tokenizer tokenizer(source_code);
    std::vector<Token> tokens;
    try {
        tokens = tokenizer.tokenize();
    } catch (const Common::NyxParserException& e) {
        std::cerr << e.what() << std::endl;
        return;
    } catch (const std::exception& e) {
        std::cerr << "Fatal Tokenizer error: " << e.what() << std::endl;
        return;
    }

    Parser parser(tokens);
    
    try {
        main_ast_program = parser.parse();
    } catch (const std::exception& e) {
        std::cerr << "Fatal internal error during parsing phase: " << e.what() << std::endl;
        return;
    }
    
    try {
        executeProgram(main_ast_program, this->globals, this->environment);
    } catch (const Common::NyxRuntimeException& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
    }
}

}

#include "../common/Value.h"
#include "../interpreter/Interpreter.h" 
#include "../interpreter/Environment.h" 
#include "../stdlib/sdl_module.h"
#include "../tokenizer/Token.h"
#include <iomanip>
#include <sstream>
#include <map>


namespace Nyx {

NyxStructDefinition::NyxStructDefinition(std::string n, const std::vector<Token>& field_tokens) : name(std::move(n)) {
    for (const auto& token : field_tokens) {
        field_names_in_order.push_back(token.lexeme);
        field_indices[token.lexeme] = field_names_in_order.size() - 1;
    }
}

NyxStructInstance::NyxStructInstance(StructDefinitionPtr def) : definition(std::move(def)) {
    if (definition) {
        field_values.resize(definition->field_names_in_order.size(), NyxValue(std::monostate{}));
    }
}

std::string nyxValueToString(const NyxValue& value_holder) {
    const auto& var_data = value_holder.data;

    if (std::holds_alternative<std::monostate>(var_data)) {
        return "null";
    } else if (std::holds_alternative<bool>(var_data)) {
        return std::get<bool>(var_data) ? "true" : "false";
    } else if (std::holds_alternative<double>(var_data)) {
        std::string s = std::to_string(std::get<double>(var_data));
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (!s.empty() && s.back() == '.') {
            s.pop_back();
        }
        return s;
    } else if (std::holds_alternative<std::string>(var_data)) {
        return std::get<std::string>(var_data);
    } else if (std::holds_alternative<NyxList>(var_data)) {
        std::stringstream ss;
        ss << "[";
        const auto& list = std::get<NyxList>(var_data);
        for (size_t i = 0; i < list.size(); ++i) {
            if (std::holds_alternative<std::string>(list[i].data)) {
                 ss << "\"" << nyxValueToString(list[i]) << "\"";
            } else {
                 ss << nyxValueToString(list[i]);
            }
            if (i < list.size() - 1) {
                ss << ", ";
            }
        }
        ss << "]";
        return ss.str();
    } else if (std::holds_alternative<UserDefinedFunctionPtr>(var_data)) {
        auto func_ptr = std::get<UserDefinedFunctionPtr>(var_data);
        if (func_ptr) { return "<func " + func_ptr->name() + ">"; }
        return "<func null_ptr>";
    } else if (std::holds_alternative<NyxModule>(var_data)) {
        auto module_data_ptr = std::get<NyxModule>(var_data);
        if (module_data_ptr && !module_data_ptr->path.empty()) { return "<module '" + module_data_ptr->path + "'>";}
        return "<module>";
    } else if (std::holds_alternative<NativeFunctionPtr>(var_data)) {
        auto native_func_ptr = std::get<NativeFunctionPtr>(var_data);
        if (native_func_ptr) { return "<native_func " + native_func_ptr->name + ">"; }
        return "<native_func null_ptr>";
    } else if (std::holds_alternative<SDLWindowNyxPtr>(var_data)) {
        return "<SDL_WindowHandle>";
    } else if (std::holds_alternative<SDLRendererNyxPtr>(var_data)) {
        return "<SDL_RendererHandle>";
    } else if (std::holds_alternative<SDLFontNyxPtr>(var_data)) {
        return "<SDL_FontHandle>";
    } else if (std::holds_alternative<SDLSurfaceNyxPtr>(var_data)) {
        return "<SDL_SurfaceHandle>";
    } else if (std::holds_alternative<SDLTextureNyxPtr>(var_data)) {
        return "<SDL_TextureHandle>";
    } else if (std::holds_alternative<StructDefinitionPtr>(var_data)) {
        auto def_ptr = std::get<StructDefinitionPtr>(var_data);
        if (def_ptr) return "<struct_definition " + def_ptr->name + ">";
        return "<struct_definition null_ptr>";
    } else if (std::holds_alternative<StructInstancePtr>(var_data)) {
        auto instance_ptr = std::get<StructInstancePtr>(var_data);
        if (instance_ptr && instance_ptr->definition) {
            std::stringstream ss;
            ss << instance_ptr->definition->name << "{";
            bool first = true;
            for (size_t i = 0; i < instance_ptr->definition->field_names_in_order.size(); ++i) {
                if (!first) ss << ", ";
                ss << instance_ptr->definition->field_names_in_order[i] << ": " << nyxValueToString(instance_ptr->field_values[i]);
                first = false;
            }
            ss << "}";
            return ss.str();
        }
        return "<struct_instance null_ptr>";
    }
    return "[Unknown NyxValue]";
}

std::string nyxValueTypeToString(const NyxValue& value_holder) {
    const auto& var_data = value_holder.data; 

    if (std::holds_alternative<std::monostate>(var_data)) { return "NULL"; }
    else if (std::holds_alternative<bool>(var_data)) { return "BOOLEAN"; }
    else if (std::holds_alternative<double>(var_data)) { return "NUMBER"; }
    else if (std::holds_alternative<std::string>(var_data)) { return "STRING"; }
    else if (std::holds_alternative<NyxList>(var_data)) { return "LIST"; }
    else if (std::holds_alternative<UserDefinedFunctionPtr>(var_data)) { return "FUNCTION"; }
    else if (std::holds_alternative<NyxModule>(var_data)) { return "MODULE"; }
    else if (std::holds_alternative<NativeFunctionPtr>(var_data)) { return "NATIVE_FUNCTION"; }
    else if (std::holds_alternative<SDLWindowNyxPtr>(var_data)) { return "SDL_WINDOW"; }
    else if (std::holds_alternative<SDLRendererNyxPtr>(var_data)) { return "SDL_RENDERER"; }
    else if (std::holds_alternative<SDLFontNyxPtr>(var_data)) { return "SDL_FONT"; }
    else if (std::holds_alternative<SDLSurfaceNyxPtr>(var_data)) { return "SDL_SURFACE"; }
    else if (std::holds_alternative<SDLTextureNyxPtr>(var_data)) { return "SDL_TEXTURE"; }
    else if (std::holds_alternative<StructDefinitionPtr>(var_data)) { return "STRUCT_DEFINITION"; }
    else if (std::holds_alternative<StructInstancePtr>(var_data)) { 
        auto instance_ptr = std::get<StructInstancePtr>(var_data);
        if (instance_ptr && instance_ptr->definition) {
            return "STRUCT<" + instance_ptr->definition->name + ">";
        }
        return "STRUCT_INSTANCE";
     }
    return "UNKNOWN_TYPE";
}

std::ostream& operator<<(std::ostream& os, const NyxValue& value) {
    os << nyxValueToString(value);
    return os;
}

}
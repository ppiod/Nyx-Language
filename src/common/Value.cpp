#include "./Value.h"
#include "../interpreter/Interpreter.h" 
#include <sstream>

namespace Nyx {

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
    return "UNKNOWN_TYPE";
}

std::ostream& operator<<(std::ostream& os, const NyxValue& value) {
    os << nyxValueToString(value);
    return os;
}

}

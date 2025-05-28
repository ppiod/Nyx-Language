#include "./string_module.h"
#include "../interpreter/Interpreter.h"
#include "../interpreter/Environment.h"
#include "../common/Utils.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cctype> 

namespace Nyx {

NyxValue native_string_toNumber(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<std::string>(args[0].data)) {
        std::cerr << "[Nyx C++] Error: 'string.toNumber' expects one string argument." << std::endl;
        return NyxValue(std::monostate{});
    }
    const std::string& str = std::get<std::string>(args[0].data);
    if (str.empty()) {
        return NyxValue(std::monostate{});
    }
    try {
        size_t processed_chars = 0;
        double result = std::stod(str, &processed_chars);
        
        std::string remaining = str.substr(processed_chars);
        if (Common::trim(remaining).length() > 0) {
             return NyxValue(std::monostate{}); 
        }
        return NyxValue(result);
    } catch (const std::invalid_argument& ia) {
        return NyxValue(std::monostate{});
    } catch (const std::out_of_range& oor) {
        return NyxValue(std::monostate{});
    }
}

NyxValue native_string_trim(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<std::string>(args[0].data)) {
        std::cerr << "[Nyx C++] Error: 'string.trim' expects one string argument." << std::endl;
        return NyxValue(std::monostate{});
    }
    const std::string& str = std::get<std::string>(args[0].data);
    return NyxValue(Common::trim(str));
}

NyxValue native_string_toLowerCase(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<std::string>(args[0].data)) {
        std::cerr << "[Nyx C++] Error: 'string.toLowerCase' expects one string argument." << std::endl;
        return NyxValue(std::monostate{});
    }
    std::string str = std::get<std::string>(args[0].data);
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return NyxValue(str);
}

NyxValue native_string_toUpperCase(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<std::string>(args[0].data)) {
        std::cerr << "[Nyx C++] Error: 'string.toUpperCase' expects one string argument." << std::endl;
        return NyxValue(std::monostate{});
    }
    std::string str = std::get<std::string>(args[0].data);
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c){ return std::toupper(c); });
    return NyxValue(str);
}

NyxValue native_string_contains(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<std::string>(args[0].data) || !std::holds_alternative<std::string>(args[1].data)) {
        std::cerr << "[Nyx C++] Error: 'string.contains' expects two string arguments (mainString, subString)." << std::endl;
        return NyxValue(false);
    }
    const std::string& main_str = std::get<std::string>(args[0].data);
    const std::string& sub_str = std::get<std::string>(args[1].data);
    return NyxValue(main_str.find(sub_str) != std::string::npos);
}

NyxValue native_string_startsWith(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<std::string>(args[0].data) || !std::holds_alternative<std::string>(args[1].data)) {
        std::cerr << "[Nyx C++] Error: 'string.startsWith' expects two string arguments (mainString, prefix)." << std::endl;
        return NyxValue(false);
    }
    const std::string& main_str = std::get<std::string>(args[0].data);
    const std::string& prefix = std::get<std::string>(args[1].data);
    if (prefix.length() > main_str.length()) {
        return NyxValue(false);
    }
    return NyxValue(main_str.rfind(prefix, 0) == 0);
}

NyxValue native_string_endsWith(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<std::string>(args[0].data) || !std::holds_alternative<std::string>(args[1].data)) {
        std::cerr << "[Nyx C++] Error: 'string.endsWith' expects two string arguments (mainString, suffix)." << std::endl;
        return NyxValue(false);
    }
    const std::string& main_str = std::get<std::string>(args[0].data);
    const std::string& suffix = std::get<std::string>(args[1].data);
    if (suffix.length() > main_str.length()) {
        return NyxValue(false);
    }
    return NyxValue(main_str.compare(main_str.length() - suffix.length(), suffix.length(), suffix) == 0);
}

NyxValue native_string_split(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<std::string>(args[0].data) || !std::holds_alternative<std::string>(args[1].data)) {
        std::cerr << "[Nyx C++] Error: 'string.split' expects two string arguments (text, delimiter)." << std::endl;
        return NyxValue(NyxList());
    }
    const std::string& text_original = std::get<std::string>(args[0].data);
    std::string delimiter_from_nyx = std::get<std::string>(args[1].data);
    
    std::string delimiter_processed = Common::process_escapes(delimiter_from_nyx);

    NyxList result_list;
    size_t start = 0;
    size_t end = 0;

    if (delimiter_processed.empty()) { 
        for (char ch : text_original) {
            result_list.push_back(NyxValue(std::string(1, ch)));
        }
        return NyxValue(result_list);
    }

    end = text_original.find(delimiter_processed);
    while (end != std::string::npos) {
        result_list.push_back(NyxValue(text_original.substr(start, end - start)));
        start = end + delimiter_processed.length();
        end = text_original.find(delimiter_processed, start);
    }
    result_list.push_back(NyxValue(text_original.substr(start)));
    return NyxValue(result_list);
}

NyxValue native_string_substring(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if ((args.size() != 2 && args.size() != 3) ||
        !std::holds_alternative<std::string>(args[0].data) ||
        !std::holds_alternative<double>(args[1].data) ||
        (args.size() == 3 && !std::holds_alternative<double>(args[2].data))) {
        std::cerr << "[Nyx C++] Error: 'string.substring' expects (string, startIndex) or (string, startIndex, length)." << std::endl;
        return NyxValue(std::string(""));
    }

    const std::string& str = std::get<std::string>(args[0].data);
    double start_double = std::get<double>(args[1].data);

    if (!interpreter.isDoubleInteger(start_double)) {
        std::cerr << "[Nyx C++] Error: Start index for 'string.substring' must be an integer." << std::endl;
        return NyxValue(std::string(""));
    }
    long long start_index = static_cast<long long>(start_double);
    long long len = static_cast<long long>(str.length());

    if (start_index < 0) start_index += len;
    if (start_index < 0) start_index = 0;
    if (start_index > len) start_index = len;
    
    size_t actual_start = static_cast<size_t>(start_index);

    if (args.size() == 2) {
        return NyxValue(str.substr(actual_start));
    } else {
        double length_double = std::get<double>(args[2].data);
        if (!interpreter.isDoubleInteger(length_double) || length_double < 0) {
            std::cerr << "[Nyx C++] Error: Length for 'string.substring' must be a non-negative integer." << std::endl;
            return NyxValue(std::string(""));
        }
        size_t count = static_cast<size_t>(length_double);
        return NyxValue(str.substr(actual_start, count));
    }
}

NyxValue native_string_replace(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 3 || 
        !std::holds_alternative<std::string>(args[0].data) || 
        !std::holds_alternative<std::string>(args[1].data) || 
        !std::holds_alternative<std::string>(args[2].data)) {
        std::cerr << "[Nyx C++] Error: 'string.replace' expects three string arguments (original, oldSub, newSub)." << std::endl;
        return NyxValue(std::monostate{});
    }

    std::string original = std::get<std::string>(args[0].data);
    const std::string& old_sub = std::get<std::string>(args[1].data);
    const std::string& new_sub = std::get<std::string>(args[2].data);

    if (old_sub.empty()) {
        return NyxValue(original);
    }

    size_t start_pos = 0;
    while((start_pos = original.find(old_sub, start_pos)) != std::string::npos) {
        original.replace(start_pos, old_sub.length(), new_sub);
        start_pos += new_sub.length(); 
    }
    return NyxValue(original);
}


void registerStdStringModule(Interpreter& interpreter) {
    Interpreter::NativeModuleBuilder builder = [&]() {
        auto module_env = std::make_shared<Environment>(interpreter.globals);

        module_env->define("toNumber", NyxValue(std::make_shared<NyxNativeFunction>("toNumber", native_string_toNumber, 1)));
        module_env->define("trim", NyxValue(std::make_shared<NyxNativeFunction>("trim", native_string_trim, 1)));
        module_env->define("toLowerCase", NyxValue(std::make_shared<NyxNativeFunction>("toLowerCase", native_string_toLowerCase, 1)));
        module_env->define("toUpperCase", NyxValue(std::make_shared<NyxNativeFunction>("toUpperCase", native_string_toUpperCase, 1)));
        module_env->define("contains", NyxValue(std::make_shared<NyxNativeFunction>("contains", native_string_contains, 2)));
        module_env->define("startsWith", NyxValue(std::make_shared<NyxNativeFunction>("startsWith", native_string_startsWith, 2)));
        module_env->define("endsWith", NyxValue(std::make_shared<NyxNativeFunction>("endsWith", native_string_endsWith, 2)));
        module_env->define("split", NyxValue(std::make_shared<NyxNativeFunction>("split", native_string_split, 2)));
        module_env->define("substring", NyxValue(std::make_shared<NyxNativeFunction>("substring", native_string_substring, -1)));
        module_env->define("replace", NyxValue(std::make_shared<NyxNativeFunction>("replace", native_string_replace, 3)));
        
        return module_env;
    };
    interpreter.registerNativeModule("std:string", builder);
}

}
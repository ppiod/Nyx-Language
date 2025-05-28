#include "./list_module.h"
#include "../interpreter/Interpreter.h" 
#include "../interpreter/Environment.h" 
#include "../common/Utils.h"         
#include <string>
#include <sstream>

namespace Nyx {

NyxValue native_list_append(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2) {
        throw Common::NyxRuntimeException("'list.append' expects two arguments (list, item).", 0);
    }
    if (!std::holds_alternative<NyxList>(args[0].data)) {
        throw Common::NyxRuntimeException("First argument to 'list.append' must be a list.", 0);
    }
    NyxList original_list = std::get<NyxList>(args[0].data); 
    const NyxValue& item_to_append = args[1];
    
    original_list.push_back(item_to_append);
    return NyxValue(original_list);
}

NyxValue native_list_prepend(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2) {
        throw Common::NyxRuntimeException("'list.prepend' expects two arguments (list, item).", 0);
    }
    if (!std::holds_alternative<NyxList>(args[0].data)) {
        throw Common::NyxRuntimeException("First argument to 'list.prepend' must be a list.", 0);
    }
    NyxList original_list = std::get<NyxList>(args[0].data); 
    const NyxValue& item_to_prepend = args[1];
    
    NyxList new_list;
    new_list.reserve(original_list.size() + 1);
    new_list.push_back(item_to_prepend);
    new_list.insert(new_list.end(), original_list.begin(), original_list.end());
    
    return NyxValue(new_list);
}

NyxValue native_list_is_empty(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1) {
        throw Common::NyxRuntimeException("'list.is_empty' expects one argument (list).", 0);
    }
    if (!std::holds_alternative<NyxList>(args[0].data)) {
        throw Common::NyxRuntimeException("Argument to 'list.is_empty' must be a list.", 0);
    }
    const auto& list = std::get<NyxList>(args[0].data);
    return NyxValue(list.empty());
}

NyxValue native_list_slice(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() < 2 || args.size() > 3) {
        throw Common::NyxRuntimeException("'list.slice' expects 2 or 3 arguments (list, start_index, [end_index]).", 0);
    }
    if (!std::holds_alternative<NyxList>(args[0].data)) {
        throw Common::NyxRuntimeException("First argument to 'list.slice' must be a list.", 0);
    }
    if (!std::holds_alternative<double>(args[1].data)) {
        throw Common::NyxRuntimeException("Second argument (start_index) to 'list.slice' must be a number.", 0);
    }

    const auto& original_list = std::get<NyxList>(args[0].data);
    double start_index_double = std::get<double>(args[1].data);

    if (!interpreter.isDoubleInteger(start_index_double)) {
        throw Common::NyxRuntimeException("Start index for 'list.slice' must be an integer.", 0);
    }
    long long list_len = static_cast<long long>(original_list.size());
    long long start_index = static_cast<long long>(start_index_double);

    if (start_index < 0) start_index += list_len;
    if (start_index < 0) start_index = 0;
    if (start_index > list_len) start_index = list_len;

    long long end_index = list_len;
    if (args.size() == 3) {
        if (!std::holds_alternative<double>(args[2].data)) {
            throw Common::NyxRuntimeException("Third argument (end_index) to 'list.slice' must be a number.", 0);
        }
        double end_index_double = std::get<double>(args[2].data);
        if (!interpreter.isDoubleInteger(end_index_double)) {
            throw Common::NyxRuntimeException("End index for 'list.slice' must be an integer.", 0);
        }
        end_index = static_cast<long long>(end_index_double);
        if (end_index < 0) end_index += list_len;
        if (end_index < 0) end_index = 0;
        if (end_index > list_len) end_index = list_len;
    }
    
    NyxList new_slice;
    if (start_index < end_index && start_index < list_len) {
        new_slice.reserve(static_cast<size_t>(end_index - start_index));
        for (long long i = start_index; i < end_index; ++i) {
            new_slice.push_back(original_list[static_cast<size_t>(i)]);
        }
    }
    return NyxValue(new_slice);
}

NyxValue native_list_join(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() < 1 || args.size() > 2) {
        throw Common::NyxRuntimeException("'list.join' expects 1 or 2 arguments (list, [separator]).", 0);
    }
    if (!std::holds_alternative<NyxList>(args[0].data)) {
        throw Common::NyxRuntimeException("First argument to 'list.join' must be a list.", 0);
    }
    
    const auto& list = std::get<NyxList>(args[0].data);
    std::string separator = "";
    if (args.size() == 2) {
        if (!std::holds_alternative<std::string>(args[1].data)) {
            throw Common::NyxRuntimeException("Second argument (separator) to 'list.join' must be a string.", 0);
        }
        separator = std::get<std::string>(args[1].data);
    }

    std::stringstream ss;
    for (size_t i = 0; i < list.size(); ++i) {
        if (std::holds_alternative<std::string>(list[i].data)) {
            ss << std::get<std::string>(list[i].data);
        } else {
            ss << nyxValueToString(list[i]);
        }
        if (i < list.size() - 1) {
            ss << separator;
        }
    }
    return NyxValue(ss.str());
}

NyxValue native_list_each(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2) {
        throw Common::NyxRuntimeException("'list.each' expects two arguments (list, callback_function).", 0);
    }
    if (!std::holds_alternative<NyxList>(args[0].data)) {
        throw Common::NyxRuntimeException("First argument to 'list.each' must be a list.", 0);
    }
    const NyxValue& callback_value = args[1];
    if (!std::holds_alternative<UserDefinedFunctionPtr>(callback_value.data) && 
        !std::holds_alternative<NativeFunctionPtr>(callback_value.data)) {
        throw Common::NyxRuntimeException("Second argument to 'list.each' must be a function.", 0);
    }

    const auto& list = std::get<NyxList>(args[0].data);
    std::vector<NyxValue> callback_args(1); 

    for (const auto& element : list) {
        callback_args[0] = element;
        if (std::holds_alternative<UserDefinedFunctionPtr>(callback_value.data)) {
            auto func_ptr = std::get<UserDefinedFunctionPtr>(callback_value.data);
            if (!func_ptr) throw Common::NyxRuntimeException("Callback function is null.", 0);
            if (func_ptr->arity() != 1) {
                 throw Common::NyxRuntimeException("Callback function for 'list.each' must accept 1 argument (element).", 0);
            }
            interpreter.executeFunctionBody(*func_ptr, callback_args);
        } else if (std::holds_alternative<NativeFunctionPtr>(callback_value.data)) {
            auto native_func_ptr = std::get<NativeFunctionPtr>(callback_value.data);
            if (!native_func_ptr) throw Common::NyxRuntimeException("Native callback function is null.", 0);
            if (native_func_ptr->arity != 1 && native_func_ptr->arity != -1) {
                 throw Common::NyxRuntimeException("Native callback function for 'list.each' must accept 1 argument (element) or be variadic.", 0);
            }
            native_func_ptr->callback(interpreter, callback_args);
        }
    }
    return NyxValue(std::monostate{}); 
}


void registerStdListModule(Interpreter& interpreter) {
    Interpreter::NativeModuleBuilder builder = [&]() {
        auto module_env = std::make_shared<Environment>(interpreter.globals); 
        
        module_env->define("append", NyxValue(std::make_shared<NyxNativeFunction>("append", native_list_append, 2)));
        module_env->define("prepend", NyxValue(std::make_shared<NyxNativeFunction>("prepend", native_list_prepend, 2)));
        module_env->define("is_empty", NyxValue(std::make_shared<NyxNativeFunction>("is_empty", native_list_is_empty, 1)));
        module_env->define("slice", NyxValue(std::make_shared<NyxNativeFunction>("slice", native_list_slice, -1)));
        module_env->define("join", NyxValue(std::make_shared<NyxNativeFunction>("join", native_list_join, -1)));
        module_env->define("each", NyxValue(std::make_shared<NyxNativeFunction>("each", native_list_each, 2)));
        
        return module_env;
    };
    interpreter.registerNativeModule("std:list", builder);
}

}

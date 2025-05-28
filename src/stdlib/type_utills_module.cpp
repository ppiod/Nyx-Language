#include "./type_utils_module.h"

#include "../interpreter/Interpreter.h"
#include "../interpreter/Environment.h"
#include "../common/Value.h"
#include "../common/Utils.h"


namespace Nyx {

NyxValue native_getType(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1) {
        throw Common::NyxRuntimeException("'getType' function expects exactly one argument.", 0); 
    }
    return NyxValue(nyxValueTypeToString(args[0]));
}

void registerStdTypeUtilsModule(Interpreter& interpreter) {
    Interpreter::NativeModuleBuilder builder = [&]() {
        auto module_env = std::make_shared<Environment>(interpreter.globals);
        module_env->define("getType", NyxValue(std::make_shared<NyxNativeFunction>("getType", native_getType, 1)));
        return module_env;
    };
    interpreter.registerNativeModule("std:type", builder);
}

}
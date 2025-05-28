#include "./time_module.h"
#include "../interpreter/Interpreter.h"
#include "../interpreter/Environment.h"
#include "../common/Utils.h"
#include <chrono>
#include <ctime>

namespace Nyx {

NyxValue native_time_clock(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (!args.empty()) {
        throw Common::NyxRuntimeException("'time.clock' function takes no arguments.", 0); 
    }
    double cpu_time = static_cast<double>(std::clock()) / CLOCKS_PER_SEC;
    return NyxValue(cpu_time);
}

NyxValue native_time_now(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (!args.empty()) {
        throw Common::NyxRuntimeException("'time.now' function takes no arguments.", 0);
    }
    auto now = std::chrono::system_clock::now();
    auto epoch_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return NyxValue(static_cast<double>(epoch_seconds));
}

void registerStdTimeModule(Interpreter& interpreter) {
    Interpreter::NativeModuleBuilder builder = [&]() {

        auto module_env = std::make_shared<Environment>(interpreter.globals);

        auto clock_func_obj = std::make_shared<NyxNativeFunction>("clock", native_time_clock, 0);
        module_env->define("clock", NyxValue(clock_func_obj));

        auto now_func_obj = std::make_shared<NyxNativeFunction>("now", native_time_now, 0);
        module_env->define("now", NyxValue(now_func_obj));

        return module_env;
    };
    interpreter.registerNativeModule("std:time", builder);
}

}
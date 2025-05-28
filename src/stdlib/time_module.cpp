#include "./time_module.h"
#include "../interpreter/Interpreter.h"
#include "../interpreter/Environment.h"
#include "../common/Utils.h"
#include "../common/Value.h"
#include <chrono>
#include <ctime>
#include <thread>
#include <iomanip>
#include <sstream>

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

NyxValue native_time_sleep(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'time.sleep' expects one number argument (seconds).", 0);
    }
    double seconds = std::get<double>(args[0].data);
    if (seconds < 0) {
        throw Common::NyxRuntimeException("'time.sleep' argument must be non-negative.", 0);
    }
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    return NyxValue(std::monostate{}); // Return null
}

NyxList tm_to_nyxlist(const std::tm* timeinfo) {
    NyxList time_list;
    if (timeinfo) {
        time_list.push_back(NyxValue(static_cast<double>(timeinfo->tm_year + 1900))); // Year
        time_list.push_back(NyxValue(static_cast<double>(timeinfo->tm_mon + 1)));    // Month (1-12)
        time_list.push_back(NyxValue(static_cast<double>(timeinfo->tm_mday)));       // Day (1-31)
        time_list.push_back(NyxValue(static_cast<double>(timeinfo->tm_hour)));       // Hour (0-23)
        time_list.push_back(NyxValue(static_cast<double>(timeinfo->tm_min)));        // Minute (0-59)
        time_list.push_back(NyxValue(static_cast<double>(timeinfo->tm_sec)));        // Second (0-59)
        time_list.push_back(NyxValue(static_cast<double>(timeinfo->tm_wday)));       // Weekday (0=Sunday, 6=Saturday)
        time_list.push_back(NyxValue(static_cast<double>(timeinfo->tm_yday)));       // Day of year (0-365)
    }
    return time_list;
}

NyxValue native_time_getLocalTime(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (!args.empty()) {
        throw Common::NyxRuntimeException("'time.getLocalTime' function takes no arguments.", 0);
    }
    auto now_chrono = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now_chrono);
    std::tm local_tm;
#ifdef _MSC_VER
    localtime_s(&local_tm, &now_time_t);
    return NyxValue(tm_to_nyxlist(&local_tm));
#else
    std::tm* local_tm_ptr = std::localtime(&now_time_t);
    if (local_tm_ptr) {
        return NyxValue(tm_to_nyxlist(local_tm_ptr));
    }
    return NyxValue(NyxList()); // Return empty list on error
#endif
}

NyxValue native_time_getUtcTime(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (!args.empty()) {
        throw Common::NyxRuntimeException("'time.getUtcTime' function takes no arguments.", 0);
    }
    auto now_chrono = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now_chrono);
    std::tm utc_tm;
#ifdef _MSC_VER
    gmtime_s(&utc_tm, &now_time_t);
    return NyxValue(tm_to_nyxlist(&utc_tm));
#else
    std::tm* utc_tm_ptr = std::gmtime(&now_time_t);
     if (utc_tm_ptr) {
        return NyxValue(tm_to_nyxlist(utc_tm_ptr));
    }
    return NyxValue(NyxList()); // Return empty list on error
#endif
}

NyxValue native_time_monotonic(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (!args.empty()) {
        throw Common::NyxRuntimeException("'time.monotonic' function takes no arguments.", 0);
    }
    auto monotonic_now = std::chrono::steady_clock::now();
    double seconds = std::chrono::duration<double>(monotonic_now.time_since_epoch()).count();
    return NyxValue(seconds);
}

NyxValue native_time_format(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.empty() || args.size() > 2) {
        throw Common::NyxRuntimeException("'time.format' expects 1 or 2 arguments: (format_string, [timestamp_seconds]).", 0);
    }
    if (!std::holds_alternative<std::string>(args[0].data)) {
        throw Common::NyxRuntimeException("First argument to 'time.format' (format_string) must be a string.", 0);
    }
    std::string format_str_nyx = std::get<std::string>(args[0].data);
    std::string format_str = Common::process_escapes(format_str_nyx);

    std::time_t time_to_format;
    if (args.size() == 2) {
        if (!std::holds_alternative<double>(args[1].data)) {
            throw Common::NyxRuntimeException("Second argument to 'time.format' (timestamp_seconds) must be a number.", 0);
        }
        time_to_format = static_cast<std::time_t>(std::get<double>(args[1].data));
    } else {
        time_to_format = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    std::tm timeinfo_tm;
#ifdef _MSC_VER
    localtime_s(&timeinfo_tm, &time_to_format);
    std::tm* timeinfo = &timeinfo_tm;
#else
    std::tm* timeinfo = std::localtime(&time_to_format);
#endif

    if (!timeinfo) {
         throw Common::NyxRuntimeException("Failed to convert timestamp to local time structure for formatting.", 0);
    }

    char buffer[128];
    if (std::strftime(buffer, sizeof(buffer), format_str.c_str(), timeinfo) == 0) {
        return NyxValue(std::string("")); 
    }
    return NyxValue(std::string(buffer));
}


void registerStdTimeModule(Interpreter& interpreter) {
    Interpreter::NativeModuleBuilder builder = [&]() {
        auto module_env = std::make_shared<Environment>(interpreter.globals);

        module_env->define("clock", NyxValue(std::make_shared<NyxNativeFunction>("clock", native_time_clock, 0)));
        module_env->define("now", NyxValue(std::make_shared<NyxNativeFunction>("now", native_time_now, 0)));

        module_env->define("sleep", NyxValue(std::make_shared<NyxNativeFunction>("sleep", native_time_sleep, 1)));
        module_env->define("getLocalTime", NyxValue(std::make_shared<NyxNativeFunction>("getLocalTime", native_time_getLocalTime, 0)));
        module_env->define("getUtcTime", NyxValue(std::make_shared<NyxNativeFunction>("getUtcTime", native_time_getUtcTime, 0)));
        module_env->define("monotonic", NyxValue(std::make_shared<NyxNativeFunction>("monotonic", native_time_monotonic, 0)));
        module_env->define("format", NyxValue(std::make_shared<NyxNativeFunction>("format", native_time_format, -1))); // Arity -1 for 1 or 2 args

        return module_env;
    };
    interpreter.registerNativeModule("std:time", builder);
}

}
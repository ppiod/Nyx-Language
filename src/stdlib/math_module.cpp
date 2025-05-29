#include "./math_module.h"
#include "../interpreter/Interpreter.h"
#include "../interpreter/Environment.h"
#include "../common/Utils.h"
#include "../common/Value.h"
#include <cmath>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <limits>

namespace Nyx {

static bool random_seeded = false;
void seed_random_if_needed() {
    if (!random_seeded) {
        std::srand(static_cast<unsigned int>(std::time(0)));
        random_seeded = true;
    }
}

const double PI_VALUE_FOR_CONVERSION = std::acos(-1.0);

NyxValue native_math_abs(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.abs' expects one number argument.", 0);
    }
    return NyxValue(std::abs(std::get<double>(args[0].data)));
}

NyxValue native_math_floor(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.floor' expects one number argument.", 0);
    }
    return NyxValue(std::floor(std::get<double>(args[0].data)));
}

NyxValue native_math_ceil(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.ceil' expects one number argument.", 0);
    }
    return NyxValue(std::ceil(std::get<double>(args[0].data)));
}

NyxValue native_math_round(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.round' expects one number argument.", 0);
    }
    return NyxValue(std::round(std::get<double>(args[0].data)));
}

NyxValue native_math_trunc(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.trunc' expects one number argument.", 0);
    }
    return NyxValue(std::trunc(std::get<double>(args[0].data)));
}

NyxValue native_math_sqrt(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.sqrt' expects one number argument.", 0);
    }
    double val = std::get<double>(args[0].data);
    if (val < 0) {
        throw Common::NyxRuntimeException("'math.sqrt' domain error (argument cannot be negative).", 0);
    }
    return NyxValue(std::sqrt(val));
}

NyxValue native_math_pow(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<double>(args[0].data) || !std::holds_alternative<double>(args[1].data)) {
        throw Common::NyxRuntimeException("'math.pow' expects two number arguments (base, exponent).", 0);
    }
    return NyxValue(std::pow(std::get<double>(args[0].data), std::get<double>(args[1].data)));
}

NyxValue native_math_sin(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.sin' expects one number argument (radians).", 0);
    }
    return NyxValue(std::sin(std::get<double>(args[0].data)));
}

NyxValue native_math_cos(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.cos' expects one number argument (radians).", 0);
    }
    return NyxValue(std::cos(std::get<double>(args[0].data)));
}

NyxValue native_math_tan(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.tan' expects one number argument (radians).", 0);
    }
    return NyxValue(std::tan(std::get<double>(args[0].data)));
}

NyxValue native_math_asin(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.asin' expects one number argument.", 0);
    }
    double val = std::get<double>(args[0].data);
    if (val < -1.0 || val > 1.0) {
        throw Common::NyxRuntimeException("'math.asin' domain error (argument must be between -1 and 1).", 0);
    }
    return NyxValue(std::asin(val));
}

NyxValue native_math_acos(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.acos' expects one number argument.", 0);
    }
    double val = std::get<double>(args[0].data);
    if (val < -1.0 || val > 1.0) {
        throw Common::NyxRuntimeException("'math.acos' domain error (argument must be between -1 and 1).", 0);
    }
    return NyxValue(std::acos(val));
}

NyxValue native_math_atan(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.atan' expects one number argument.", 0);
    }
    return NyxValue(std::atan(std::get<double>(args[0].data)));
}

NyxValue native_math_atan2(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<double>(args[0].data) || !std::holds_alternative<double>(args[1].data)) {
        throw Common::NyxRuntimeException("'math.atan2' expects two number arguments (y, x).", 0);
    }
    return NyxValue(std::atan2(std::get<double>(args[0].data), std::get<double>(args[1].data)));
}

NyxValue native_math_degrees(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.degrees' expects one number argument (radians).", 0);
    }
    return NyxValue(std::get<double>(args[0].data) * (180.0 / PI_VALUE_FOR_CONVERSION));
}

NyxValue native_math_radians(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.radians' expects one number argument (degrees).", 0);
    }
    return NyxValue(std::get<double>(args[0].data) * (PI_VALUE_FOR_CONVERSION / 180.0));
}

NyxValue native_math_log(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.log' (natural log) expects one number argument.", 0);
    }
    double val = std::get<double>(args[0].data);
    if (val <= 0) {
        throw Common::NyxRuntimeException("'math.log' domain error (argument must be positive).", 0);
    }
    return NyxValue(std::log(val));
}

NyxValue native_math_log10(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.log10' expects one number argument.", 0);
    }
    double val = std::get<double>(args[0].data);
    if (val <= 0) {
        throw Common::NyxRuntimeException("'math.log10' domain error (argument must be positive).", 0);
    }
    return NyxValue(std::log10(val));
}

NyxValue native_math_exp(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        throw Common::NyxRuntimeException("'math.exp' expects one number argument.", 0);
    }
    return NyxValue(std::exp(std::get<double>(args[0].data)));
}

NyxValue native_math_min(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<double>(args[0].data) || !std::holds_alternative<double>(args[1].data)) {
        throw Common::NyxRuntimeException("'math.min' expects two number arguments.", 0);
    }
    return NyxValue(std::min(std::get<double>(args[0].data), std::get<double>(args[1].data)));
}

NyxValue native_math_max(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<double>(args[0].data) || !std::holds_alternative<double>(args[1].data)) {
        throw Common::NyxRuntimeException("'math.max' expects two number arguments.", 0);
    }
    return NyxValue(std::max(std::get<double>(args[0].data), std::get<double>(args[1].data)));
}

NyxValue native_math_random(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (!args.empty()) {
        throw Common::NyxRuntimeException("'math.random' function takes no arguments.", 0);
    }
    seed_random_if_needed();
    return NyxValue(static_cast<double>(std::rand()) / (RAND_MAX + 1.0)); // 0.0 to <1.0
}

NyxValue native_math_randomInt(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<double>(args[0].data) || !std::holds_alternative<double>(args[1].data)) {
        throw Common::NyxRuntimeException("'math.randomInt' expects two number arguments (min, max).", 0);
    }
    seed_random_if_needed();
    double min_val_double = std::get<double>(args[0].data);
    double max_val_double = std::get<double>(args[1].data);

    if (!interpreter.isDoubleInteger(min_val_double) || !interpreter.isDoubleInteger(max_val_double)) {
        throw Common::NyxRuntimeException("Arguments for 'math.randomInt' must be integers.", 0);
    }
    long long min_val = static_cast<long long>(min_val_double);
    long long max_val = static_cast<long long>(max_val_double);

    if (min_val > max_val) {
        throw Common::NyxRuntimeException("'math.randomInt' min cannot be greater than max.", 0);
    }
    if (min_val == max_val) {
        return NyxValue(static_cast<double>(min_val));
    }
    long long range = max_val - min_val + 1;
    return NyxValue(static_cast<double>(min_val + (std::rand() % range)));
}


void registerStdMathModule(Interpreter& interpreter) {
    Interpreter::NativeModuleBuilder builder = [&]() {
        auto module_env = std::make_shared<Environment>(interpreter.globals);

        // Constants
        module_env->define("PI", NyxValue(PI_VALUE_FOR_CONVERSION));
        module_env->define("E", NyxValue(std::exp(1.0)));

        // Functions
        module_env->define("abs", NyxValue(std::make_shared<NyxNativeFunction>("abs", native_math_abs, 1)));
        module_env->define("floor", NyxValue(std::make_shared<NyxNativeFunction>("floor", native_math_floor, 1)));
        module_env->define("ceil", NyxValue(std::make_shared<NyxNativeFunction>("ceil", native_math_ceil, 1)));
        module_env->define("round", NyxValue(std::make_shared<NyxNativeFunction>("round", native_math_round, 1)));
        module_env->define("trunc", NyxValue(std::make_shared<NyxNativeFunction>("trunc", native_math_trunc, 1)));
        module_env->define("sqrt", NyxValue(std::make_shared<NyxNativeFunction>("sqrt", native_math_sqrt, 1)));
        module_env->define("pow", NyxValue(std::make_shared<NyxNativeFunction>("pow", native_math_pow, 2)));
        module_env->define("sin", NyxValue(std::make_shared<NyxNativeFunction>("sin", native_math_sin, 1)));
        module_env->define("cos", NyxValue(std::make_shared<NyxNativeFunction>("cos", native_math_cos, 1)));
        module_env->define("tan", NyxValue(std::make_shared<NyxNativeFunction>("tan", native_math_tan, 1)));
        module_env->define("asin", NyxValue(std::make_shared<NyxNativeFunction>("asin", native_math_asin, 1)));
        module_env->define("acos", NyxValue(std::make_shared<NyxNativeFunction>("acos", native_math_acos, 1)));
        module_env->define("atan", NyxValue(std::make_shared<NyxNativeFunction>("atan", native_math_atan, 1)));
        module_env->define("atan2", NyxValue(std::make_shared<NyxNativeFunction>("atan2", native_math_atan2, 2)));
        module_env->define("degrees", NyxValue(std::make_shared<NyxNativeFunction>("degrees", native_math_degrees, 1)));
        module_env->define("radians", NyxValue(std::make_shared<NyxNativeFunction>("radians", native_math_radians, 1)));
        module_env->define("log", NyxValue(std::make_shared<NyxNativeFunction>("log", native_math_log, 1)));
        module_env->define("log10", NyxValue(std::make_shared<NyxNativeFunction>("log10", native_math_log10, 1)));
        module_env->define("exp", NyxValue(std::make_shared<NyxNativeFunction>("exp", native_math_exp, 1)));
        module_env->define("min", NyxValue(std::make_shared<NyxNativeFunction>("min", native_math_min, 2)));
        module_env->define("max", NyxValue(std::make_shared<NyxNativeFunction>("max", native_math_max, 2)));
        module_env->define("random", NyxValue(std::make_shared<NyxNativeFunction>("random", native_math_random, 0)));
        module_env->define("randomInt", NyxValue(std::make_shared<NyxNativeFunction>("randomInt", native_math_randomInt, 2)));
        
        return module_env;
    };
    interpreter.registerNativeModule("std:math", builder);
}

}
#ifndef NYX_STDLIB_MATH_H
#define NYX_STDLIB_MATH_H

#include "../common/Value.h"
#include <vector>

namespace Nyx {

class Interpreter;

NyxValue native_math_abs(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_floor(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_ceil(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_round(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_trunc(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_sqrt(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_pow(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_sin(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_cos(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_tan(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_asin(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_acos(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_atan(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_atan2(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_degrees(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_radians(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_log(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_log10(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_exp(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_min(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_max(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_random(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_math_randomInt(Interpreter& interpreter, const std::vector<NyxValue>& args);

void registerStdMathModule(Interpreter& interpreter);

}

#endif
#ifndef NYX_STDLIB_STRING_H
#define NYX_STDLIB_STRING_H

#include "../common/Value.h"
#include <vector>

namespace Nyx {

class Interpreter;

NyxValue native_string_toNumber(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_string_trim(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_string_toLowerCase(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_string_toUpperCase(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_string_contains(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_string_startsWith(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_string_endsWith(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_string_split(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_string_substring(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_string_replace(Interpreter& interpreter, const std::vector<NyxValue>& args);

void registerStdStringModule(Interpreter& interpreter);

}

#endif
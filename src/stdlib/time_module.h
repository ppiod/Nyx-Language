#ifndef NYX_STDLIB_TIME_H
#define NYX_STDLIB_TIME_H

#include "../common/Value.h"
#include <vector>

namespace Nyx {

class Interpreter;

NyxValue native_time_clock(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_time_now(Interpreter& interpreter, const std::vector<NyxValue>& args);

void registerStdTimeModule(Interpreter& interpreter);

}

#endif

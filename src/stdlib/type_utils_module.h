#ifndef NYX_STDLIB_TYPE_UTILS_H
#define NYX_STDLIB_TYPE_UTILS_H

#include "../common/Value.h"
#include <vector>

namespace Nyx {

class Interpreter; 

NyxValue native_getType(Interpreter& interpreter, const std::vector<NyxValue>& args);
void registerStdTypeUtilsModule(Interpreter& interpreter);

}

#endif
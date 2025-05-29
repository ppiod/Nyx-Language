#include "./native_stdlib.h"
#include "../interpreter/Interpreter.h"

#include "./time_module.h" 
#include "./io_module.h"
#include "./list_module.h"
#include "./sdl_module.h"
#include "./string_module.h"
#include "./type_utils_module.h"
#include "./math_module.h"

namespace Nyx {

void registerNyxStandardLibrary(Interpreter& interpreter) {
    registerStdTimeModule(interpreter);
    registerStdIoModule(interpreter);
    registerStdListModule(interpreter);
    registerStdSdlModule(interpreter);
    registerStdStringModule(interpreter);
    registerStdTypeUtilsModule(interpreter);
    registerStdMathModule(interpreter);
}

}
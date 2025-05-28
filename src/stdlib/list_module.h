#ifndef NYX_STDLIB_LIST_H
#define NYX_STDLIB_LIST_H

#include "../common/Value.h" 
#include <vector>

namespace Nyx {

class Interpreter; 

NyxValue native_list_append(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_list_prepend(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_list_is_empty(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_list_slice(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_list_join(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_list_each(Interpreter& interpreter, const std::vector<NyxValue>& args);

void registerStdListModule(Interpreter& interpreter);

} 

#endif
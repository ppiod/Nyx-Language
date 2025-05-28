#ifndef NYX_STDLIB_IO_H
#define NYX_STDLIB_IO_H

#include "../common/Value.h" 
#include <vector>       

namespace Nyx {

class Interpreter; 

NyxValue native_io_input(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_io_print(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_io_readFile(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_io_writeFile(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_io_appendFile(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_io_fileExists(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_io_deleteFile(Interpreter& interpreter, const std::vector<NyxValue>& args);


void registerStdIoModule(Interpreter& interpreter);

} 

#endif
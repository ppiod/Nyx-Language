#include "./io_module.h"
#include "../interpreter/Interpreter.h" 
#include "../interpreter/Environment.h" 
#include "../common/Utils.h"         
#include <iostream> 
#include <string>
#include <sstream> 
#include <fstream>
#include <filesystem>

namespace Nyx {

NyxValue native_io_input(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() > 1) {
        throw Common::NyxRuntimeException("'io.input' function takes 0 or 1 argument (prompt).", 0); 
    }
    if (args.size() == 1) {
        if (!std::holds_alternative<std::string>(args[0].data)) {
            throw Common::NyxRuntimeException("Prompt for 'io.input' must be a string.", 0);
        }
        std::cout << std::get<std::string>(args[0].data);
        std::cout.flush(); 
    }
    std::string line;
    if (!std::getline(std::cin, line)) {
        if (std::cin.eof()) {
            return NyxValue(std::monostate{}); 
        }
        throw Common::NyxRuntimeException("Error reading input from console.", 0);
    }
    return NyxValue(line);
}

NyxValue native_io_print(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    std::stringstream ss;
    for (size_t i = 0; i < args.size(); ++i) {
        ss << nyxValueToString(args[i]);
        if (i < args.size() - 1) {
            ss << " ";
        }
    }
    std::cout << ss.str() << std::endl;
    return NyxValue(std::monostate{}); 
}

NyxValue native_io_readFile(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<std::string>(args[0].data)) {
        throw Common::NyxRuntimeException("'io.readFile' expects one string argument (filepath).", 0);
    }
    std::string filepath = std::get<std::string>(args[0].data);
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw Common::NyxRuntimeException("Could not open file '" + filepath + "' for reading.", 0);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return NyxValue(content);
}

NyxValue native_io_writeFile(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<std::string>(args[0].data) || !std::holds_alternative<std::string>(args[1].data)) {
        throw Common::NyxRuntimeException("'io.writeFile' expects two string arguments (filepath, content).", 0);
    }
    std::string filepath = std::get<std::string>(args[0].data);
    std::string content_from_nyx = std::get<std::string>(args[1].data);

    std::string content_processed = Common::process_escapes(content_from_nyx);

    std::ofstream file(filepath, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        throw Common::NyxRuntimeException("Could not open file '" + filepath + "' for writing.", 0);
    }
    file << content_processed;
    file.close();
    return NyxValue(true); 
}

NyxValue native_io_appendFile(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<std::string>(args[0].data) || !std::holds_alternative<std::string>(args[1].data)) {
        throw Common::NyxRuntimeException("'io.appendFile' expects two string arguments (filepath, content).", 0);
    }
    std::string filepath = std::get<std::string>(args[0].data);
    std::string content_from_nyx = std::get<std::string>(args[1].data);

    std::string content_processed = Common::process_escapes(content_from_nyx);

    std::ofstream file(filepath, std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        throw Common::NyxRuntimeException("Could not open file '" + filepath + "' for appending.", 0);
    }
    file << content_processed;
    file.close();
    return NyxValue(true);
}

NyxValue native_io_fileExists(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<std::string>(args[0].data)) {
        throw Common::NyxRuntimeException("'io.fileExists' expects one string argument (filepath).", 0);
    }
    std::string filepath = std::get<std::string>(args[0].data);
    return NyxValue(std::filesystem::exists(filepath));
}

NyxValue native_io_deleteFile(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<std::string>(args[0].data)) {
        throw Common::NyxRuntimeException("'io.deleteFile' expects one string argument (filepath).", 0);
    }
    std::string filepath = std::get<std::string>(args[0].data);
    try {
        if (std::filesystem::exists(filepath)) {
            return NyxValue(std::filesystem::remove(filepath));
        }
        return NyxValue(false);
    } catch (const std::filesystem::filesystem_error& e) {
        throw Common::NyxRuntimeException("Error deleting file '" + filepath + "': " + e.what(), 0);
    }
}


void registerStdIoModule(Interpreter& interpreter) {
    Interpreter::NativeModuleBuilder builder = [&]() {
        auto module_env = std::make_shared<Environment>(interpreter.globals); 
        
        module_env->define("input", NyxValue(std::make_shared<NyxNativeFunction>("input", native_io_input, -1)));
        module_env->define("print", NyxValue(std::make_shared<NyxNativeFunction>("print", native_io_print, -1)));
        module_env->define("readFile", NyxValue(std::make_shared<NyxNativeFunction>("readFile", native_io_readFile, 1)));
        module_env->define("writeFile", NyxValue(std::make_shared<NyxNativeFunction>("writeFile", native_io_writeFile, 2)));
        module_env->define("appendFile", NyxValue(std::make_shared<NyxNativeFunction>("appendFile", native_io_appendFile, 2)));
        module_env->define("fileExists", NyxValue(std::make_shared<NyxNativeFunction>("fileExists", native_io_fileExists, 1)));
        module_env->define("deleteFile", NyxValue(std::make_shared<NyxNativeFunction>("deleteFile", native_io_deleteFile, 1)));
        
        return module_env;
    };
    interpreter.registerNativeModule("std:io", builder);
}

}
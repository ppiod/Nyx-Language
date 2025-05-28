#include "./Nyx.h"
#include "./common/Utils.h"
#include "./interpreter/Interpreter.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace Nyx {

namespace {
    bool has_nyx_extension(const std::string &filename) {
        if (filename.length() >= 4) {
            return filename.substr(filename.length() - 4) == ".nyx";
        }
        return false;
    }
}

int runNyxScript(const std::string& script_path_arg, const std::vector<std::string>& script_args) {
    if (!has_nyx_extension(script_path_arg)) {
        std::cerr << "Error: Input file must have the .nyx extension. Provided: " << script_path_arg << std::endl;
        return 1;
    }

    std::filesystem::path script_file_fs_path(script_path_arg);
    std::string canonical_script_path_str;

    try {
        canonical_script_path_str = std::filesystem::canonical(std::filesystem::absolute(script_file_fs_path)).string();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: Cannot access script file '" << script_path_arg << "': " << e.what() << std::endl;
        return 1;
    }

    std::ifstream file(canonical_script_path_str);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << canonical_script_path_str << "'" << std::endl;
        return 1;
    }

    std::string source_code((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    file.close();
    
    Nyx::Interpreter lang_interpreter;
    try {
        lang_interpreter.interpret(source_code, canonical_script_path_str, script_args);
    } catch (const Nyx::Common::NyxException &e) {
        std::cerr << "Error (Nyx Framework): " << e.what() << std::endl;
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "Unexpected system error during script execution: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

}

#include "./Nyx.h"
#include <iostream>
#include <string>
#include <vector>

const std::string NIX_VERSION = "0.0.1";

void print_help() {
    std::cout << "Usage: nyx [option] <file.nyx>" << std::endl;
    std::cout << std::endl;
    std::cout << "Description:" << std::endl;
    std::cout << "  Executes a Nyx script file (<file.nyx>) or displays information using options." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  <file.nyx>      Path to the Nyx script file to execute." << std::endl;
    std::cout << "  --help, -h      Show this help message and exit." << std::endl;
    std::cout << "  --version, -v   Show version information and exit." << std::endl;
    std::cout << "  --about         Show information about the Nyx language and exit." << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nyx script.nyx" << std::endl;
    std::cout << "  nyx --help" << std::endl;
    std::cout << "  nyx --version" << std::endl;
}

void print_version() {
    std::cout << "Nyx Interpreter version " << NIX_VERSION << "." << std::endl;
}

void print_about() {
    std::cout << "Nyx Language" << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "Nyx is a lightweight, dynamically-typed scripting language designed for ease of use." << std::endl;
    std::cout << "It supports procedural programming with features including:" << std::endl;
    std::cout << "  - Variables (dynamic typing with 'auto')" << std::endl;
    std::cout << "  - Basic arithmetic and logical operations" << std::endl;
    std::cout << "  - Control flow (if/else, for loops, break, continue)" << std::endl;
    std::cout << "  - Lists (creation, indexing, len, concatenation, repetition)" << std::endl;
    std::cout << "  - Functions (definition, call, return, closures, imports)" << std::endl;
    std::cout << "  - String interpolation with #{expression}" << std::endl;
    std::cout << "  - Simple console output (output, put)" << std::endl;
    std::cout << std::endl;
    std::cout << "Developed as a learning and experimentation project." << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: nyx <file.nyx> [script_args...] or nyx [option]" << std::endl;
        std::cerr << "Try 'nyx --help' for more information." << std::endl;
        return 1;
    }

    std::string arg1 = argv[1];

    if (arg1 == "--help" || arg1 == "-h") {
        print_help();
        return 0;
    } else if (arg1 == "--version" || arg1 == "-v") {
        print_version();
        return 0;
    } else if (arg1 == "--about") {
        print_about();
        return 0;
    } else {
        std::vector<std::string> script_args;
        if (argc > 2) {
            for (int i = 2; i < argc; ++i) {
                script_args.push_back(argv[i]);
            }
        }
        
        if (arg1.rfind("--", 0) == 0 || (arg1.rfind("-", 0) == 0 && arg1.length() > 1 && arg1 != "-")) {
             std::cerr << "Error: Unknown option '" << arg1 << "' or script path expected." << std::endl;
             std::cerr << "Try 'nyx --help' for more information." << std::endl;
             return 1;
        }

        return Nyx::runNyxScript(arg1, script_args);
    }
    return 0;
}

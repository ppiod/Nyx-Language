#pragma once
#include <string>
#include <algorithm>
#include <vector>
#include <cctype>
#include <stdexcept>

namespace Nyx {
namespace Common {

std::string trim(const std::string& str);
std::string process_escapes(const std::string& input_str);

bool isValidIdentifierChar(char ch, bool is_start);
bool isValidIdentifier(const std::string& s);

bool isSimpleNumeric(const std::string& s);

class NyxException : public std::runtime_error {
public:
    NyxException(const std::string& message) : std::runtime_error(message) {}
};

class NyxParserException : public NyxException {
public:
    NyxParserException(const std::string& message, int line)
        : NyxException("Parser error at line " + std::to_string(line) + ": " + message), error_line(line) {}
    int getLine() const { return error_line; }
private:
    int error_line;
};

class NyxRuntimeException : public NyxException {
public:
    NyxRuntimeException(const std::string& message, int line)
        : NyxException("Runtime error at line " + std::to_string(line) + ": " + message), error_line(line) {}
    int getLine() const { return error_line; }
private:
    int error_line;
};

}
}

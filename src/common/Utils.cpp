#include "./Utils.h"
#include <sstream>

namespace Nyx {
namespace Common {

std::string trim(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

bool isValidIdentifierChar(char ch, bool is_start) {
    if (is_start) {
        return std::isalpha(static_cast<unsigned char>(ch)) || ch == '_';
    }
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
}

bool isValidIdentifier(const std::string& s) {
    if (s.empty() || !isValidIdentifierChar(s[0], true)) {
        return false;
    }
    for (size_t i = 1; i < s.length(); ++i) {
        if (!isValidIdentifierChar(s[i], false)) {
            return false;
        }
    }
    return true;
}

std::string process_escapes(const std::string& input_str) {
    std::stringstream ss;
    for (size_t i = 0; i < input_str.length(); ++i) {
        if (input_str[i] == '\\' && i + 1 < input_str.length()) {
            char next_char = input_str[i+1];
            switch (next_char) {
                case 'n': ss << '\n'; i++; break;
                case 'r': ss << '\r'; i++; break;
                case 't': ss << '\t'; i++; break;
                case 'e': ss << '\033'; i++; break;
                case '\\': ss << '\\'; i++; break;
                case '"': ss << '"'; i++; break;
                default:
                    ss << '\\' << next_char;
                    i++;
                    break;
            }
        } else {
            ss << input_str[i];
        }
    }
    return ss.str();
}

bool isSimpleNumeric(const std::string& s) {
    if (s.empty()) return false;
    return std::all_of(s.begin(), s.end(), [](char c){ return std::isdigit(static_cast<unsigned char>(c)); });
}

}
}

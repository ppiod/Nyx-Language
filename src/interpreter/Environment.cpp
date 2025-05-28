#include "./Environment.h"

namespace Nyx {

Environment::Environment() : enclosing(nullptr) {}

Environment::Environment(std::shared_ptr<Environment> enclosing_scope)
   : enclosing(std::move(enclosing_scope)) {}

void Environment::define(const std::string& name, const NyxValue& value) {
    values[name] = value;
}

std::optional<NyxValue> Environment::get(const std::string& name) const {
    auto it = values.find(name);
    if (it != values.end()) {
        return it->second;
    }
    if (enclosing) {
        return enclosing->get(name);
    }
    return std::nullopt;
}

bool Environment::assign(const std::string& name, const NyxValue& value) {
    auto it = values.find(name);
    if (it != values.end()) {
        values[name] = value;
        return true;
    }
    if (enclosing) {
        return enclosing->assign(name, value);
    }
    return false;
}


}

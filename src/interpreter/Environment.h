#pragma once
#include <string>
#include <map>
#include <optional>
#include <memory>
#include "../common/Value.h"

namespace Nyx {

class Environment {
public:
    Environment();
    explicit Environment(std::shared_ptr<Environment> enclosing_scope);

    void define(const std::string& name, const NyxValue& value);
    std::optional<NyxValue> get(const std::string& name) const;
    bool assign(const std::string& name, const NyxValue& value);


    std::shared_ptr<Environment> enclosing;
private:
    std::map<std::string, NyxValue> values;
};

}
#pragma once
#include <stdexcept>
#include <string>
#include "../common/Value.h"

namespace Nyx {
namespace Common {

class NyxControlFlowSignal : public std::runtime_error {
public:
    NyxControlFlowSignal(const std::string& type) : std::runtime_error(type) {}
};

class NyxBreakSignal : public NyxControlFlowSignal {
public:
    NyxBreakSignal() : NyxControlFlowSignal("break_signal") {}
};

class NyxContinueSignal : public NyxControlFlowSignal {
public:
    NyxContinueSignal() : NyxControlFlowSignal("continue_signal") {}
};

class NyxReturnSignal : public NyxControlFlowSignal {
public:
    NyxValue value;
    NyxReturnSignal(NyxValue val) 
        : NyxControlFlowSignal("return_signal"), value(std::move(val)) {}
};

} 
}

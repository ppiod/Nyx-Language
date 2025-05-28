#ifndef NYX_RUNNER_H
#define NYX_RUNNER_H

#include <string>
#include <vector>

namespace Nyx {
    int runNyxScript(const std::string& script_path_str, const std::vector<std::string>& script_args);
}

#endif
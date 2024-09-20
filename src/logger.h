#pragma once

#include <iostream>

#include "types.h"

namespace blaz {

struct Logger {
    template <typename... Args>
    void error(str arg, Args&&... args) {
        std::cout << "[ERROR] " << arg;
        ((std::cout << " " << std::forward<Args>(args)), ...);
        std::cout << std::endl;
    }

    void error(Error err) {
        std::cout << "[ERROR] " << err.message();
        std::cout << std::endl;
    }

    template <typename... Args>
    void info(str arg, Args&&... args) {
        std::cout << "[INFO] " << arg;
        ((std::cout << std::forward<Args>(args)), ...);
        std::cout << std::endl;
    }

    bool print_to_stdout = true;
};

extern Logger logger;

}  // namespace blaz
#pragma once

#include <iostream>
#include <string>

#include "error.h"
#include "types.h"

namespace blaz {

struct Logger {
    template <typename... Args>
    void error(str arg, Args&&... args) {
        std::cout << "\x1B[31m[ERROR]\x1B[0m " << arg;
        ((std::cout << " " << std::forward<Args>(args)), ...);
        std::cout << std::endl;
    }

    void error(Error err) {
        std::cout << "\x1B[31m[ERROR]\x1B[0m " << err.message();
        std::cout << std::endl;
    }

    template <typename... Args>
    void info(str arg, Args&&... args) {
        std::cout << "\x1B[33m[INFO]\x1B[0m " << arg;
        ((std::cout << std::forward<Args>(args)), ...);
        std::cout << std::endl;
    }

    bool print_to_stdout = true;
};

extern Logger logger;

}  // namespace blaz
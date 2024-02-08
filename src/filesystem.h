#pragma once

#include <functional>
#include <thread>

#include "error.h"
#include "types.h"

namespace blaz {

struct FileWatcher {
    Error init(str path, std::function<void(str)> callback);
    void stop();
    void* m_os_data = NULL;

    std::thread watch_thread;
};

pair<Error, str> read_whole_file(str path);

}  // namespace blaz
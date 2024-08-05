#pragma once

#include <functional>
#include <thread>

#include "error.h"
#include "types.h"

namespace blaz {

struct FileWatcher {
    Error init(const str& path, std::function<void(const str&)> callback);
    void stop();
    void* m_os_data = NULL;

    std::thread watch_thread;
};

pair<Error, str> read_whole_file(const str& path);
pair<Error, vec<u8>> read_whole_file_binary(const str& path);

}  // namespace blaz
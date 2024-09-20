#pragma once

#include <functional>
#include <thread>

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
Error write_to_file(const str& path, const void* buffer, size_t buffer_size);
str get_current_directory();

}  // namespace blaz
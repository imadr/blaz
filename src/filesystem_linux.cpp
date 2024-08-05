#include "filesystem.h"

namespace blaz {

Error FileWatcher::init(const str& path, std::function<void(str)> callback) {
    watch_thread = std::thread([this, callback]() {

    });

    return Error();
}

void FileWatcher::stop() {
    watch_thread.join();
}

}  // namespace blaz
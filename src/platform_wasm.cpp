#include <emscripten/html5.h>

#include "logger.h"
#include "platform.h"

namespace blaz {

Error Window::init(const str& title) {
    emscripten_set_resize_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 0,
        [](int even_type, const EmscriptenUiEvent* ui_event, void* user_data) -> EM_BOOL {
            Window* window = (Window*)user_data;
            window->m_resize_callback(window);
            return EM_TRUE;
        });
    return Error();
}

bool Window::event_loop() {
    return true;
}

void Window::close() {
}

void Window::screenshot(str filename) {
}

}  // namespace blaz
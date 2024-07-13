#include <emscripten/html5.h>

#include "logger.h"
#include "platform.h"

namespace blaz {

Error Window::init(const str& title) {
    emscripten_set_resize_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 0,
        [](int even_type, const EmscriptenUiEvent* ui_event, void* user_data) -> EM_BOOL {
            Window* window = (Window*)user_data;
            int width = ui_event->windowInnerWidth;
            int height = ui_event->windowInnerHeight;
            window->m_size.width = width;
            window->m_size.height = height;
            for (auto& callback : window->m_resize_callbacks) {
                callback(window);
            }
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
#pragma once

#include <functional>
#include <map>

#include "error.h"
#include "my_math.h"
#include "types.h"

namespace blaz {

enum class KeyState { RELEASED, PRESSED };

enum class ButtonState { RELEASED, PRESSED };

struct Window {
    Error init(const str& title);
    void close();
    bool event_loop();
    void screenshot(str filename);

    struct Size {
        u32 width;
        u32 height;
    };
    Size m_size = {.width = 800, .height = 600};

    void* m_os_data = NULL;
    void* m_d3d11_data = NULL;

    std::map<str, KeyState> m_keyboard;
    ButtonState m_left_mouse_button = ButtonState::RELEASED;
    ButtonState m_right_mouse_button = ButtonState::RELEASED;

    std::function<void(Window*)> m_resize_callback = NULL;
    std::function<void(Vec2I)> m_mouse_move_callback = NULL;
    std::function<void(ButtonState, ButtonState)> m_mouse_click_callback = NULL;
};

}  // namespace blaz
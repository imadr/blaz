#include "platform.h"
#include "utils_WIN32.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((unsigned short)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((unsigned short)0x02)
#endif

struct Window_WIN32 {
    HWND window_handle;
    HDC device_context;
};

static Window_WIN32* m_win32;

std::map<int, str> win32_keycodes = {{VK_BACK, "KEY_BACKSPACE"},
                                     {VK_TAB, "KEY_TAB"},
                                     {VK_CLEAR, "KEY_CLEAR"},
                                     {VK_RETURN, "KEY_RETURN"},
                                     {VK_LCONTROL, "KEY_LEFTCTRL"},
                                     {VK_RCONTROL, "KEY_RIGHTCTRL"},
                                     {VK_LSHIFT, "KEY_LEFTSHIFT"},
                                     {VK_RSHIFT, "KEY_RIGHTSHIFT"},
                                     {VK_MENU, "KEY_ALT"},
                                     {VK_PAUSE, "KEY_PAUSE"},
                                     {VK_CAPITAL, "KEY_CAPSLOCK"},
                                     {VK_ESCAPE, "KEY_ESCAPE"},
                                     {VK_SPACE, "KEY_SPACE"},
                                     {VK_PRIOR, "KEY_PAGEUP"},
                                     {VK_NEXT, "KEY_PAGEDOWN"},
                                     {VK_HOME, "KEY_HOME"},
                                     {VK_LEFT, "KEY_LEFT"},
                                     {VK_RIGHT, "KEY_RIGHT"},
                                     {VK_UP, "KEY_UP"},
                                     {VK_DOWN, "KEY_DOWN"},
                                     {VK_SELECT, "KEY_SELECT"},
                                     {VK_SNAPSHOT, "KEY_PRINTSCREEN"},
                                     {VK_INSERT, "KEY_INSERT"},
                                     {VK_DELETE, "KEY_DELETE"},
                                     {0x30, "KEY_0"},
                                     {0x31, "KEY_1"},
                                     {0x32, "KEY_2"},
                                     {0x33, "KEY_3"},
                                     {0x34, "KEY_4"},
                                     {0x35, "KEY_5"},
                                     {0x36, "KEY_6"},
                                     {0x37, "KEY_7"},
                                     {0x38, "KEY_8"},
                                     {0x39, "KEY_9"},
                                     {0x41, "KEY_A"},
                                     {0x42, "KEY_B"},
                                     {0x43, "KEY_C"},
                                     {0x44, "KEY_D"},
                                     {0x45, "KEY_E"},
                                     {0x46, "KEY_F"},
                                     {0x47, "KEY_G"},
                                     {0x48, "KEY_H"},
                                     {0x49, "KEY_I"},
                                     {0x4A, "KEY_J"},
                                     {0x4B, "KEY_K"},
                                     {0x4C, "KEY_L"},
                                     {0x4D, "KEY_M"},
                                     {0x4E, "KEY_N"},
                                     {0x4F, "KEY_O"},
                                     {0x50, "KEY_P"},
                                     {0x51, "KEY_Q"},
                                     {0x52, "KEY_R"},
                                     {0x53, "KEY_S"},
                                     {0x54, "KEY_T"},
                                     {0x55, "KEY_U"},
                                     {0x56, "KEY_V"},
                                     {0x57, "KEY_W"},
                                     {0x58, "KEY_X"},
                                     {0x59, "KEY_Y"},
                                     {0x5A, "KEY_Z"},
                                     {VK_F1, "KEY_F1"},
                                     {VK_F2, "KEY_F2"},
                                     {VK_F3, "KEY_F3"},
                                     {VK_F4, "KEY_F4"},
                                     {VK_F5, "KEY_F5"},
                                     {VK_F6, "KEY_F6"},
                                     {VK_F7, "KEY_F7"},
                                     {VK_F8, "KEY_F8"},
                                     {VK_F9, "KEY_F9"},
                                     {VK_F10, "KEY_F10"},
                                     {VK_F11, "KEY_F11"},
                                     {VK_F12, "KEY_F12"}};

namespace blaz {

static LRESULT CALLBACK window_procedure(HWND window_handle, UINT message, WPARAM w_param,
                                         LPARAM l_param) {
    if (message == WM_CREATE) {
        CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(l_param);
        int* user_data = reinterpret_cast<int*>(create_struct->lpCreateParams);
        SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)user_data);
        return 0;
    }

    LONG_PTR user_data = GetWindowLongPtrA(window_handle, GWLP_USERDATA);
    if (user_data != 0) {
        Window* window = reinterpret_cast<Window*>(user_data);
        switch (message) {
            case WM_SIZE: {
                if (window->m_resize_callback == NULL) break;
                window->m_size.width = LOWORD(l_param);
                window->m_size.height = HIWORD(l_param);
                window->m_resize_callback(window);
            } break;
            case WM_KEYDOWN: {
                str key = win32_keycodes[w_param];
                window->m_keyboard[key] = KeyState::PRESSED;
            } break;
            case WM_KEYUP: {
                str key = win32_keycodes[w_param];
                window->m_keyboard[key] = KeyState::RELEASED;
            } break;
            case WM_INPUT: {
                if (window->m_mouse_callback == NULL) break;

                UINT size = 0;

                GetRawInputData((HRAWINPUT)l_param, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
                LPBYTE buffer = new BYTE[size];

                if (GetRawInputData((HRAWINPUT)l_param, RID_INPUT, buffer, &size,
                                    sizeof(RAWINPUTHEADER)) == size) {
                    RAWINPUT* raw_input = (RAWINPUT*)buffer;

                    if (raw_input->header.dwType == RIM_TYPEMOUSE) {
                        window->m_mouse_callback(Vec2I((i32)raw_input->data.mouse.lLastX,
                                                       (i32)raw_input->data.mouse.lLastY));
                    }
                }
                delete[] buffer;
            } break;
            case WM_DESTROY: {
                PostQuitMessage(0);
            } break;
        }
    }

    return DefWindowProc(window_handle, message, w_param, l_param);
}

Error Window::init() {
    m_win32 = new Window_WIN32();
    m_os_data = m_win32;

    const char* window_class_name = "window_class";
    WNDCLASS window_class = {0};
    window_class.style = CS_OWNDC;
    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = GetModuleHandle(0);
    window_class.lpszClassName = window_class_name;

    if (!RegisterClass(&window_class)) {
        return Error("RegisterClass: " + win32_get_last_error());
    }

    DWORD windowed_style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    RECT window_rect = {0, 0, (LONG)m_size.width, (LONG)m_size.height};
    if (!AdjustWindowRect(&window_rect, windowed_style, 0)) {
        return Error("AdjustWindowRect: " + win32_get_last_error());
    }
    u32 adjusted_width = window_rect.right - window_rect.left;
    u32 adjusted_height = window_rect.bottom - window_rect.top;
    m_win32->window_handle =
        CreateWindowEx(0, window_class_name, "", windowed_style,
                       GetSystemMetrics(SM_CXSCREEN) / 2 - adjusted_width / 2,
                       GetSystemMetrics(SM_CYSCREEN) / 2 - adjusted_height / 2, adjusted_width,
                       adjusted_height, NULL, NULL, window_class.hInstance, this);
    if (!m_win32->window_handle) {
        return Error("CreateWindowEx: " + win32_get_last_error());
    }

    m_win32->device_context = GetDC(m_win32->window_handle);
    if (!m_win32->device_context) {
        return Error("GetDC: " + win32_get_last_error());
    }

    ShowWindow(m_win32->window_handle, 5);

    RAWINPUTDEVICE raw_input_device[1];
    raw_input_device[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    raw_input_device[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    raw_input_device[0].dwFlags = RIDEV_INPUTSINK;
    raw_input_device[0].hwndTarget = m_win32->window_handle;
    if (!RegisterRawInputDevices(raw_input_device, 1, sizeof(raw_input_device[0]))) {
        return Error("RegisterRawInputDevices: " + win32_get_last_error());
    }

    return Error();
}

bool Window::event_loop() {
    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
        if (message.message != WM_QUIT) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        } else {
            return false;
        }
    }
    return true;
}

void Window::close() {
}

}  // namespace blaz
#include "my_math.h"
#include "platform.h"
#include "utils_WIN32.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((unsigned short)0x01)
#endif
#include "types.h"
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((unsigned short)0x02)
#endif
#include <basetsd.h>
#include <d3d11.h>
#include <libloaderapi.h>

#include <map>

namespace blaz {

struct Window_WIN32 {
    HWND window_handle;
    HDC device_context;
};

static Window_WIN32* m_win32;

std::map<u64, str> win32_keycodes = {{VK_BACK, "KEY_BACKSPACE"},
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

struct D3D11 {
    ID3D11RenderTargetView* backbuffer;
    IDXGISwapChain* swapchain;
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;

    u32 swap_interval;
};

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
                if (window->m_resize_callbacks.size() == 0) break;
                window->m_size.width = LOWORD(l_param);
                window->m_size.height = HIWORD(l_param);

                D3D11* d3d11 = (D3D11*)window->m_d3d11_data;
                if (d3d11) {
                    d3d11->device_context->OMSetRenderTargets(0, 0, 0);
                    d3d11->backbuffer->Release();
                    HRESULT result =
                        d3d11->swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                    ID3D11Texture2D* buffer;
                    result =
                        d3d11->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);
                    result =
                        d3d11->device->CreateRenderTargetView(buffer, NULL, &d3d11->backbuffer);
                    buffer->Release();
                    d3d11->device_context->OMSetRenderTargets(1, &d3d11->backbuffer, NULL);
                    D3D11_VIEWPORT viewport;
                    viewport.Width = FLOAT(window->m_size.width);
                    viewport.Height = FLOAT(window->m_size.height);
                    viewport.TopLeftX = 0;
                    viewport.TopLeftY = 0;
                    d3d11->device_context->RSSetViewports(1, &viewport);
                }

                for (auto& callback : window->m_resize_callbacks) {
                    callback(window);
                }
            } break;
            case WM_KEYDOWN: {
                str key = win32_keycodes[w_param];
                window->m_keyboard[key] = KeyState::PRESSED;
            } break;
            case WM_KEYUP: {
                str key = win32_keycodes[w_param];
                window->m_keyboard[key] = KeyState::RELEASED;
            } break;
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP: {
                POINT mouse_pos;
                if (GetCursorPos(&mouse_pos)) {
                    ScreenToClient(window_handle, &mouse_pos);
                    if (message == WM_LBUTTONDOWN) {
                        window->m_left_mouse_button = ButtonState::PRESSED;
                    } else if (message == WM_LBUTTONUP) {
                        window->m_left_mouse_button = ButtonState::RELEASED;
                    } else if (message == WM_RBUTTONDOWN) {
                        window->m_right_mouse_button = ButtonState::PRESSED;
                    } else if (message == WM_RBUTTONUP) {
                        window->m_right_mouse_button = ButtonState::RELEASED;
                    }
                    if (window->m_mouse_click_callback) {
                        window->m_mouse_click_callback(Vec2I(mouse_pos.x, mouse_pos.y),
                                                       window->m_left_mouse_button,
                                                       window->m_right_mouse_button);
                    }
                }

            } break;
            case WM_MOUSEMOVE: {
                if (window->m_mouse_move_callback == NULL) break;
                window->m_mouse_move_callback(
                    Vec2I(i32(GET_X_LPARAM(l_param)), i32(GET_Y_LPARAM(l_param))));
            } break;
            case WM_MOUSEWHEEL: {
                if (window->m_mouse_wheel_callback == NULL) break;
                i16 delta = GET_WHEEL_DELTA_WPARAM(w_param);
                window->m_mouse_wheel_callback(delta);
            } break;
            case WM_INPUT: {
                if (window->m_mouse_move_raw_callback == NULL) break;

                UINT size = 0;

                GetRawInputData((HRAWINPUT)l_param, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
                LPBYTE buffer = new BYTE[size];

                if (GetRawInputData((HRAWINPUT)l_param, RID_INPUT, buffer, &size,
                                    sizeof(RAWINPUTHEADER)) == size) {
                    RAWINPUT* raw_input = (RAWINPUT*)buffer;

                    if (raw_input->header.dwType == RIM_TYPEMOUSE) {
                        window->m_mouse_move_raw_callback(Vec2I((i32)raw_input->data.mouse.lLastX,
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

Error Window::init(const str& title) {
    m_win32 = new Window_WIN32();
    m_os_data = m_win32;

    const char* window_class_name = "window_class";
    WNDCLASS window_class = {0};
    window_class.style = CS_OWNDC;
    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = GetModuleHandle(0);
    window_class.lpszClassName = window_class_name;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);

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
        CreateWindowEx(0, window_class_name, title.c_str(), windowed_style,
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

    RAWINPUTDEVICE raw_input_device[1]{};
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

void Window::screenshot(str filename) {
    RECT rect;
    GetClientRect(m_win32->window_handle, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    HDC screen_device_context = GetDC(NULL);
    HDC device_context = CreateCompatibleDC(screen_device_context);

    HBITMAP bitemap_handle = CreateCompatibleBitmap(screen_device_context, width, height);
    SelectObject(device_context, bitemap_handle);

    PrintWindow(m_win32->window_handle, device_context, PW_CLIENTONLY);

    BITMAPINFOHEADER bitmap_info;
    bitmap_info.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.biWidth = width;
    bitmap_info.biHeight = -height;
    bitmap_info.biPlanes = 1;
    bitmap_info.biBitCount = 24;
    bitmap_info.biCompression = BI_RGB;
    bitmap_info.biSizeImage = 0;

    BITMAPFILEHEADER bitmap_header;
    bitmap_header.bfType = 0x4D42;
    bitmap_header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * 3;
    bitmap_header.bfReserved1 = 0;
    bitmap_header.bfReserved2 = 0;
    bitmap_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    HANDLE file =
        CreateFileA(filename.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    WriteFile(file, &bitmap_header, sizeof(BITMAPFILEHEADER), 0, 0);
    WriteFile(file, &bitmap_info, sizeof(BITMAPINFOHEADER), 0, 0);
    BYTE* bits = new BYTE[width * height * 3];
    GetDIBits(device_context, bitemap_handle, 0, height, bits, (BITMAPINFO*)&bitmap_info,
              DIB_RGB_COLORS);
    WriteFile(file, bits, width * height * 3, 0, 0);

    CloseHandle(file);
    delete[] bits;
    DeleteObject(bitemap_handle);
    DeleteDC(device_context);
    ReleaseDC(NULL, screen_device_context);
}

}  // namespace blaz
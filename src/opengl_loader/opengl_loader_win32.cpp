// clang-format off
#include "opengl_loader/opengl_loader.h"
#include "utils_WIN32.h"
#include "types.h"

#include <sstream>
#include <windows.h>
#include <GL/gl.h>
// clang-format on

#define WGL_SAMPLES_ARB 0x2042
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_TYPE_RGBA_ARB 0x202B
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define ERROR_INVALID_VERSION_ARB 0x2095
#define ERROR_INVALID_PROFILE_ARB 0x2096

namespace blaz {

struct Window_WIN32_Opengl {
    HWND window_handle;
    HDC device_context;
};

static Window_WIN32_Opengl* m_win32_opengl;

typedef const char*(APIENTRY* wglGetExtensionsStringARB_TYPE)(HDC);
typedef HGLRC(APIENTRY* wglCreateContextAttribsARB_TYPE)(HDC, HGLRC, const int*);
typedef BOOL(APIENTRY* wglChoosePixelFormatARB_TYPE)(HDC, const int*, const FLOAT*, UINT, int*,
                                                     UINT*);
typedef BOOL(APIENTRY* wglSwapIntervalEXT_TYPE)(int);

wglSwapIntervalEXT_TYPE wglSwapIntervalEXT;

void OpenglLoader::set_swap_interval(u32 interval) {
    if (wglSwapIntervalEXT != NULL) {
        wglSwapIntervalEXT(interval);
    }
}

void OpenglLoader::swap_buffers(blaz::Window* window) {
    SwapBuffers(m_win32_opengl->device_context);
}

Error OpenglLoader::init(blaz::Window* window, bool debug_context) {
    m_win32_opengl = (Window_WIN32_Opengl*)window->m_os_data;

    HGLRC m_context_win32;

    if (m_win32_opengl->device_context == NULL) {
        return Error("OpenglLoader::init: A window is needed for opengl context creation");
    }

    WNDCLASS dummy_class = {0};
    dummy_class.style = CS_OWNDC;
    dummy_class.lpfnWndProc = DefWindowProc;
    dummy_class.hInstance = GetModuleHandle(0);
    dummy_class.lpszClassName = "dummy_window_class";

    if (!RegisterClass(&dummy_class)) {
        return Error("RegisterClass: " + win32_get_last_error());
    }

    HWND dummy_window_handle = CreateWindowEx(0, "dummy_window_class", "", WS_OVERLAPPEDWINDOW, 0,
                                              0, 0, 0, NULL, NULL, dummy_class.hInstance, NULL);

    if (!dummy_window_handle) {
        return Error("CreateWindow: " + win32_get_last_error());
    }

    HDC dummy_device_context = GetDC(dummy_window_handle);

    PIXELFORMATDESCRIPTOR dummy_pixel_format = {0};
    dummy_pixel_format.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    dummy_pixel_format.nVersion = 1;
    dummy_pixel_format.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    dummy_pixel_format.iPixelType = PFD_TYPE_RGBA;
    dummy_pixel_format.cColorBits = 32;
    dummy_pixel_format.cDepthBits = 24;
    dummy_pixel_format.cStencilBits = 8;
    u32 dummy_pixel_format_id = ChoosePixelFormat(dummy_device_context, &dummy_pixel_format);

    if (dummy_pixel_format_id == 0) {
        return Error("ChoosePixelFormat: no suitable pixel format found, " +
                     win32_get_last_error());
    }

    PIXELFORMATDESCRIPTOR good_dummy_pixel_format;
    if (!DescribePixelFormat(dummy_device_context, dummy_pixel_format_id,
                             sizeof(PIXELFORMATDESCRIPTOR), &good_dummy_pixel_format)) {
        return Error("DescribePixelFormat: " + win32_get_last_error());
    }

    if (!SetPixelFormat(dummy_device_context, dummy_pixel_format_id, &good_dummy_pixel_format)) {
        return Error("SetPixelFormat: " + win32_get_last_error());
    }

    HGLRC dummy_opengl_context = wglCreateContext(dummy_device_context);
    if (dummy_opengl_context == NULL) {
        return Error("wglCreateContext: " + win32_get_last_error());
    }

    if (!wglMakeCurrent(dummy_device_context, dummy_opengl_context)) {
        return Error("wglMakeCurrent: " + win32_get_last_error());
    }

    wglGetExtensionsStringARB_TYPE wglGetExtensionsStringARB =
        (wglGetExtensionsStringARB_TYPE)wglGetProcAddress("wglGetExtensionsStringARB");
    if (wglGetExtensionsStringARB == NULL) {
        return Error("wglGetExtensionsStringARB is NULL");
    }

    str wgl_extensions_string = str(wglGetExtensionsStringARB(dummy_device_context));
    std::istringstream iss(wgl_extensions_string);
    vec<str> wgl_extensions;
    for (str s; iss >> s;) {
        wgl_extensions.push_back(s);
    }

    if (!contains(wgl_extensions, str("WGL_ARB_create_context"))) {
        return Error("WGL_ARB_create_context unavailable");
    }
    if (!contains(wgl_extensions, str("WGL_ARB_create_context"))) {
        return Error("WGL_ARB_create_context_profile unavailable");
    }
    if (!contains(wgl_extensions, str("WGL_EXT_swap_control"))) {
        return Error("WGL_EXT_swap_control unavailable");
    }
    if (!contains(wgl_extensions, str("WGL_ARB_pixel_format"))) {
        return Error("WGL_ARB_pixel_format unavailable");
    }

    wglCreateContextAttribsARB_TYPE wglCreateContextAttribsARB =
        (wglCreateContextAttribsARB_TYPE)wglGetProcAddress("wglCreateContextAttribsARB");
    if (wglCreateContextAttribsARB == NULL) {
        return Error("wglCreateContextAttribsARB unavailable");
    }

    wglChoosePixelFormatARB_TYPE wglChoosePixelFormatARB =
        (wglChoosePixelFormatARB_TYPE)wglGetProcAddress("wglChoosePixelFormatARB");
    if (wglChoosePixelFormatARB == NULL) {
        return Error("wglChoosePixelFormatARB unavailable");
    }

    wglDeleteContext(dummy_opengl_context);
    ReleaseDC(dummy_window_handle, dummy_device_context);
    DestroyWindow(dummy_window_handle);
    UnregisterClass("dummy_window_class", dummy_class.hInstance);

    i32 pixel_format_attributes[] = {
        WGL_DRAW_TO_WINDOW_ARB,
        GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,
        GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,
        GL_TRUE,
        WGL_PIXEL_TYPE_ARB,
        WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,
        32,
        WGL_DEPTH_BITS_ARB,
        24,
        WGL_STENCIL_BITS_ARB,
        8,
        WGL_SAMPLES_ARB,
        2,
        0,
    };

    i32 pixel_format;
    u32 num_formats;
    wglChoosePixelFormatARB(m_win32_opengl->device_context, pixel_format_attributes, NULL, 1,
                            &pixel_format, &num_formats);

    PIXELFORMATDESCRIPTOR good_pixel_format;
    if (!DescribePixelFormat(m_win32_opengl->device_context, pixel_format,
                             sizeof(PIXELFORMATDESCRIPTOR), &good_pixel_format)) {
        return Error("DescribePixelFormat: " + win32_get_last_error());
    }

    if (!SetPixelFormat(m_win32_opengl->device_context, pixel_format, &good_pixel_format)) {
        return Error("SetPixelFormat: " + win32_get_last_error());
    }

    int debug = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
    if (debug_context) debug |= WGL_CONTEXT_DEBUG_BIT_ARB;

    i32 context_attributes[] = {WGL_CONTEXT_MAJOR_VERSION_ARB,
                                4,
                                WGL_CONTEXT_MINOR_VERSION_ARB,
                                4,
                                WGL_CONTEXT_FLAGS_ARB,
                                debug,
                                WGL_CONTEXT_PROFILE_MASK_ARB,
                                WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                                0};

    m_context_win32 =
        wglCreateContextAttribsARB(m_win32_opengl->device_context, 0, context_attributes);

    if (m_context_win32 == NULL) {
        str error_message;
        switch (GetLastError() & 0x0000FFFF) {
            case ERROR_INVALID_VERSION_ARB:
                error_message = "Invalid or unsupported OpenGL version.";
                break;
            case ERROR_INVALID_PROFILE_ARB:
                error_message = "Invalid or unsupported OpenGL profile.";
                break;
            default:
                error_message = "Unknown error.";
                break;
        }

        return Error("wglMakeCurrent: " + error_message);
    }

    if (!wglMakeCurrent(m_win32_opengl->device_context, m_context_win32)) {
        return Error("wglMakeCurrent: " + win32_get_last_error());
    }

    wglSwapIntervalEXT = (wglSwapIntervalEXT_TYPE)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT == NULL) {
        return Error("wglSwapIntervalEXT unavailable");
    }

#define GL_FUNCTION(return_type, name, ...)                     \
    name = (name##Type*)wglGetProcAddress(#name);               \
    if (!name) {                                                \
        return Error("OpenglLoader::init : Can't load " #name); \
    }

    GL_FUNCTIONS_LIST
#undef GL_FUNCTION

#define GL_FUNCTION(return_type, name, ...) name = ::name;

    GL_OLD_FUNCTIONS_LIST
#undef GL_FUNCTION

    m_context = m_context_win32;

    i32 major;
    i32 minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    GLint profile;
    str gl_profile;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);

    if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
        gl_profile = "Core";
    } else {
        gl_profile = "Compatibility";
    }

    char* renderer = (char*)glGetString(GL_RENDERER);
    logger.info(str(renderer) + ", OpenGL " + std::to_string(major) + "." + std::to_string(minor) +
                " " + gl_profile);

    return Error();
}

}  // namespace blaz
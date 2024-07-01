// clang-format off
#include "opengl_loader/opengl_loader.h"

#include <dlfcn.h>
#include <cstdio>
#include <GL/gl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
// clang-format on

typedef struct __GLXcontextRec* GLXContext;
typedef XID GLXPixmap;
typedef XID GLXDrawable;
typedef struct __GLXFBConfigRec* GLXFBConfig;
typedef XID GLXFBConfigID;
typedef XID GLXContextID;
typedef XID GLXWindow;
typedef XID GLXPbuffer;
#define GLX_X_VISUAL_TYPE 0x22
#define GLX_RED_SIZE 8
#define GLX_GREEN_SIZE 9
#define GLX_BLUE_SIZE 10
#define GLX_ALPHA_SIZE 11
#define GLX_DEPTH_SIZE 12
#define GLX_STENCIL_SIZE 13
#define GLX_X_RENDERABLE 0x8012
#define GLX_DRAWABLE_TYPE 0x8010
#define GLX_DOUBLEBUFFER 5
#define GLX_WINDOW_BIT 0x00000001
#define GLX_RENDER_TYPE 0x8011
#define GLX_SAMPLES 0x186a1
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#define GLX_CONTEXT_PROFILE_MASK_ARB 0x9126
#define GLX_RGBA_BIT 0x00000001
#define GLX_TRUE_COLOR 0x8002
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define GLX_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define GLX_CONTEXT_FLAGS_ARB 0x2094
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002

struct Window_X11 {
    ::Display* display;
    ::Screen* screen;
    int screen_num;
    ::Window window;
};

static Window_X11* m_x11;

namespace blaz {

typedef GLXContext (*glXCreateContextAttribsARBType)(::Display*, GLXFBConfig, GLXContext, Bool,
                                                     const int*);
typedef Bool (*glXMakeCurrentType)(::Display*, GLXDrawable, GLXContext);
typedef GLXFBConfig* (*glXChooseFBConfigType)(::Display*, int, const int*, int*);
typedef void (*glXSwapBuffersType)(::Display*, GLXDrawable);

glXSwapBuffersType glXSwapBuffers;

void OpenglLoader::swap_buffers(blaz::Window* window) {
    glXSwapBuffers(m_x11->display, m_x11->window);
}

Error OpenglLoader::init(blaz::Window* window, bool debug_context) {
    GLXContext m_context_linux;
    m_x11 = (Window_X11*)window->m_os_data;

    void* libgl_handle = dlopen("libGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (!libgl_handle) {
        libgl_handle = dlopen("libGL.so", RTLD_LAZY | RTLD_LOCAL);
        if (!libgl_handle) {
            return Error("OpenglLoader::init: " + str(dlerror()));
        }
    }

    glXChooseFBConfigType glXChooseFBConfig =
        (glXChooseFBConfigType)dlsym(libgl_handle, "glXChooseFBConfig");
    glXCreateContextAttribsARBType glXCreateContextAttribsARB =
        (glXCreateContextAttribsARBType)dlsym(libgl_handle, "glXCreateContextAttribsARB");
    glXMakeCurrentType glXMakeCurrent = (glXMakeCurrentType)dlsym(libgl_handle, "glXMakeCurrent");
    glXSwapBuffers = (glXSwapBuffersType)dlsym(libgl_handle, "glXSwapBuffers");

    i32 visual_attributes[] = {GLX_X_RENDERABLE,
                               1,
                               GLX_DRAWABLE_TYPE,
                               GLX_WINDOW_BIT,
                               GLX_RENDER_TYPE,
                               GLX_RGBA_BIT,
                               GLX_X_VISUAL_TYPE,
                               GLX_TRUE_COLOR,
                               GLX_DOUBLEBUFFER,
                               1,
                               GLX_RED_SIZE,
                               1,
                               GLX_GREEN_SIZE,
                               1,
                               GLX_BLUE_SIZE,
                               1,
                               GLX_ALPHA_SIZE,
                               8,
                               GLX_DEPTH_SIZE,
                               24,
                               GLX_SAMPLES,
                               2,
                               0};

    i32 num_framebuffer_config = 0;
    GLXFBConfig* framebuffer_config = glXChooseFBConfig(m_x11->display, m_x11->screen_num,
                                                        visual_attributes, &num_framebuffer_config);
    if (!framebuffer_config) {
        return Error("OpenglLoader::init : glXChooseFBConfig : Failed to get framebuffer config");
    }

    int debug = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
    if (debug_context) debug |= GLX_CONTEXT_DEBUG_BIT_ARB;

    int context_attributes[] = {GLX_CONTEXT_MAJOR_VERSION_ARB,
                                4,
                                GLX_CONTEXT_MINOR_VERSION_ARB,
                                5,
                                GLX_CONTEXT_FLAGS_ARB,
                                debug,
                                GLX_CONTEXT_PROFILE_MASK_ARB,
                                GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                                None};
    m_context_linux =
        glXCreateContextAttribsARB(m_x11->display, framebuffer_config[0], 0, 1, context_attributes);
    XFree(framebuffer_config);

    glXMakeCurrent(m_x11->display, m_x11->window, m_context_linux);

#define GL_FUNCTION(return_type, name, ...)                     \
    name = (name##Type*)dlsym(libgl_handle, #name);             \
    if (!name) {                                                \
        return Error("OpenglLoader::init : " + str(dlerror())); \
    }

    GL_FUNCTIONS_LIST
#undef GL_FUNCTION

#define GL_FUNCTION(return_type, name, ...)                     \
    name = (name##Type*)dlsym(libgl_handle, #name);             \
    if (!name) {                                                \
        return Error("OpenglLoader::init : " + str(dlerror())); \
    }

    GL_OLD_FUNCTIONS_LIST
#undef GL_FUNCTION

    i32 major;
    i32 minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    logger.info("Loaded OpenGL context ", major, ".", minor);

    m_context = m_context_linux;

    return Error();
}

}  // namespace blaz
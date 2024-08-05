#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>

#include <cstring>

#include "platform.h"

namespace blaz {

struct Window_X11 {
    Display* display;
    Screen* screen;
    int screen_num;
    Window window;
};

static Window_X11* m_x11;

std::map<int, str> x11_keycodes = {
    {22, "KEY_BACKSPACE"},  {23, "KEY_TAB"},       {36, "KEY_RETURN"},     {37, "KEY_LEFTCTRL"},
    {105, "KEY_RIGHTCTRL"}, {50, "KEY_LEFTSHIFT"}, {62, "KEY_RIGHTSHIFT"}, {64, "KEY_ALT"},
    {66, "KEY_CAPSLOCK"},   {9, "KEY_ESCAPE"},     {65, "KEY_SPACE"},      {112, "KEY_PAGEUP"},
    {117, "KEY_PAGEDOWN"},  {110, "KEY_HOME"},     {113, "KEY_LEFT"},      {114, "KEY_RIGHT"},
    {111, "KEY_UP"},        {116, "KEY_DOWN"},     {118, "KEY_INSERT"},    {119, "KEY_DELETE"},
    {19, "KEY_0"},          {10, "KEY_1"},         {11, "KEY_2"},          {12, "KEY_3"},
    {13, "KEY_4"},          {14, "KEY_5"},         {15, "KEY_6"},          {16, "KEY_7"},
    {17, "KEY_8"},          {18, "KEY_9"},         {38, "KEY_A"},          {56, "KEY_B"},
    {54, "KEY_C"},          {40, "KEY_D"},         {26, "KEY_E"},          {41, "KEY_F"},
    {42, "KEY_G"},          {43, "KEY_H"},         {31, "KEY_I"},          {44, "KEY_J"},
    {45, "KEY_K"},          {46, "KEY_L"},         {58, "KEY_M"},          {57, "KEY_N"},
    {32, "KEY_O"},          {33, "KEY_P"},         {24, "KEY_Q"},          {27, "KEY_R"},
    {39, "KEY_S"},          {28, "KEY_T"},         {30, "KEY_U"},          {55, "KEY_V"},
    {25, "KEY_W"},          {53, "KEY_X"},         {29, "KEY_Y"},          {52, "KEY_Z"},
    {67, "KEY_F1"},         {68, "KEY_F2"},        {69, "KEY_F3"},         {70, "KEY_F4"},
    {71, "KEY_F5"},         {72, "KEY_F6"},        {73, "KEY_F7"},         {74, "KEY_F8"},
    {75, "KEY_F9"},         {76, "KEY_F10"},       {95, "KEY_F11"},        {96, "KEY_F12"}};

Error Window::init() {
    m_x11 = new Window_X11();
    m_os_data = m_x11;
    m_x11->display = XOpenDisplay(0);
    if (m_x11->display == NULL) {
        return Error("XOpenDisplay: Failed to open display");
    }

    m_x11->screen = DefaultScreenOfDisplay(m_x11->display);
    m_x11->screen_num = DefaultScreen(m_x11->display);
    if (m_x11->screen == NULL) {
        return Error("DefaultScreenOfDisplay: Failed to get default screen");
    }

    m_x11->window = XCreateSimpleWindow(m_x11->display, RootWindowOfScreen(m_x11->screen), 0, 0,
                                        m_size.width, m_size.height, 0, 0, 0);

    XSelectInput(m_x11->display, m_x11->window, StructureNotifyMask);

    XClearWindow(m_x11->display, m_x11->window);
    XMapRaised(m_x11->display, m_x11->window);

    static int xinput2_opcode;
    int event, err;
    if (!XQueryExtension(m_x11->display, "XInputExtension", &xinput2_opcode, &event, &err)) {
        return Error("XQueryExtension: XInputExtension not supported " + err);
    }

    unsigned char mask[5] = {0};
    XIEventMask event_mask;
    event_mask.deviceid = XIAllMasterDevices;
    event_mask.mask_len = sizeof(mask);
    event_mask.mask = mask;
    XISetMask(mask, XI_RawMotion);
    XISetMask(mask, XI_RawButtonPress);
    XISetMask(mask, XI_RawButtonRelease);
    XISetMask(mask, XI_RawKeyPress);
    XISetMask(mask, XI_RawKeyRelease);

    if (XISelectEvents(m_x11->display, DefaultRootWindow(m_x11->display), &event_mask, 1) !=
        Success) {
        return Error("XISelectEvents: Can't select XInput events");
    }

    return Error();
}

bool Window::event_loop() {
    ::XEvent event;
    while (XPending(m_x11->display)) {
        XNextEvent(m_x11->display, &event);
        if (event.xcookie.type == GenericEvent) {
            if (XGetEventData(m_x11->display, &event.xcookie)) {
                XIRawEvent* raw_event = (XIRawEvent*)event.xcookie.data;
                if (event.xcookie.evtype == XI_RawMotion) {
                    if (m_mouse_callback != NULL) {
                        m_mouse_callback(
                            Vec2I((i32)raw_event->raw_values[0], (i32)raw_event->raw_values[1]));
                    }
                } else if (event.xcookie.evtype == XI_RawButtonPress) {
                    if (raw_event->detail == 1) {
                        m_left_mouse_button = ButtonState::PRESSED;
                    } else if (raw_event->detail == 3) {
                        m_right_mouse_button = ButtonState::PRESSED;
                    }
                } else if (event.xcookie.evtype == XI_RawButtonRelease) {
                    if (raw_event->detail == 1) {
                        m_left_mouse_button = ButtonState::RELEASED;
                    } else if (raw_event->detail == 3) {
                        m_right_mouse_button = ButtonState::RELEASED;
                    }
                } else if (event.xcookie.evtype == XI_RawKeyPress) {
                    str key = x11_keycodes[raw_event->detail];
                    m_keyboard[key] = KeyState::PRESSED;
                } else if (event.xcookie.evtype == XI_RawKeyRelease) {
                    str key = x11_keycodes[raw_event->detail];
                    m_keyboard[key] = KeyState::RELEASED;
                }
                XFreeEventData(m_x11->display, &event.xcookie);
            }
        } else if (event.type == DestroyNotify) {
            return false;
        } else if (event.type == ConfigureNotify) {
            m_size = {.width = u32(event.xconfigure.width), .height = u32(event.xconfigure.height)};
            if (m_resize_callback != NULL) {
                m_resize_callback(this);
            }
        }
    }
    return true;
}

void Window::close() {
    XDestroyWindow(m_x11->display, m_x11->window);
    XCloseDisplay(m_x11->display);
}

}  // namespace blaz
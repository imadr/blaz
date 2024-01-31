#include "utils_WIN32.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace blaz {

str win32_get_last_error() {
    DWORD error = GetLastError();
    if (error) {
        LPVOID message_buffer;
        DWORD buffer_len =
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                              FORMAT_MESSAGE_IGNORE_INSERTS,
                          NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPTSTR)&message_buffer, 0, NULL);
        if (buffer_len) {
            LPCSTR lpMsgStr = (LPCSTR)message_buffer;
            str result(lpMsgStr, lpMsgStr + buffer_len);
            LocalFree(message_buffer);
            return result;
        }
    }
    return str();
}

};  // namespace blaz
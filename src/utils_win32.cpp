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

std::string wide_to_narrow_str(const std::wstring& wide_str) {
    int required_size =
        WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(required_size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), -1, &result[0], required_size, nullptr,
                        nullptr);
    return result;
}

std::wstring narrow_to_wide_str(const std::string& narrow_str) {
    int required_size = MultiByteToWideChar(CP_UTF8, 0, narrow_str.c_str(), -1, nullptr, 0);
    std::wstring result(required_size - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, narrow_str.c_str(), -1, &result[0], required_size);
    return result;
}

};  // namespace blaz
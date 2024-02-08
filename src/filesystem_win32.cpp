#include <array>
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <locale>

#include "filesystem.h"
#include "utils_win32.h"

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

namespace blaz {

struct FileWatch_WIN32 {
    HANDLE directory_handle = {nullptr};
    HANDLE close_event = {nullptr};
    OVERLAPPED overlapped_buffer{0};
};

Error FileWatcher::init(str path, std::function<void(str)> callback) {
    FileWatch_WIN32* win32_file_watch = new FileWatch_WIN32();
    m_os_data = win32_file_watch;

    win32_file_watch->directory_handle = CreateFile(
        path.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    if (win32_file_watch->directory_handle == INVALID_HANDLE_VALUE) {
        return Error("FileWatcher::init: " + win32_get_last_error());
    }

    win32_file_watch->close_event = CreateEvent(NULL, TRUE, FALSE, NULL);

    watch_thread = std::thread([this, callback]() {
        FileWatch_WIN32* win32_file_watch = (FileWatch_WIN32*)m_os_data;

        std::vector<BYTE> buffer(1024 * 256);
        DWORD bytes_returned = 0;

        win32_file_watch->overlapped_buffer.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        std::array<HANDLE, 2> handles{win32_file_watch->close_event,
                                      win32_file_watch->overlapped_buffer.hEvent};
        bool exit = false;

        do {
            ReadDirectoryChangesW(win32_file_watch->directory_handle, buffer.data(),
                                  static_cast<DWORD>(buffer.size()), TRUE,
                                  FILE_NOTIFY_CHANGE_LAST_WRITE, &bytes_returned,
                                  &win32_file_watch->overlapped_buffer, NULL);

            DWORD waited_event = WaitForMultipleObjects(2, handles.data(), FALSE, INFINITE);

            switch (waited_event) {
                case WAIT_OBJECT_0: {
                    exit = true;
                } break;
                case WAIT_OBJECT_0 + 1: {
                    GetOverlappedResult(win32_file_watch->directory_handle,
                                        &win32_file_watch->overlapped_buffer, &bytes_returned,
                                        TRUE);
                    FILE_NOTIFY_INFORMATION* file_information =
                        reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[0]);

                    std::wstring file_name_wstr{
                        file_information->FileName,
                        file_information->FileNameLength / sizeof(file_information->FileName[0])};

                    std::string file_name = wide_to_narrow_str(file_name_wstr);

                    callback(file_name);
                } break;
            }
        } while (!exit);
    });

    return Error();
}

void FileWatcher::stop() {
    FileWatch_WIN32* win32_file_watch = (FileWatch_WIN32*)m_os_data;
    SetEvent(win32_file_watch->close_event);
    CancelIoEx(win32_file_watch->directory_handle, &win32_file_watch->overlapped_buffer);
    CloseHandle(win32_file_watch->directory_handle);
    watch_thread.join();
}

pair<Error, str> read_whole_file(str path) {
    std::wstring path_wstr = narrow_to_wide_str(path);
    HANDLE file_handle = CreateFileW(path_wstr.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_handle == INVALID_HANDLE_VALUE) {
        return std::make_pair(
            Error("Failed to open file '" + path + "' : " + win32_get_last_error()), "");
    }

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(file_handle, &file_size)) {
        CloseHandle(file_handle);
        return std::make_pair(
            Error("Failed to get file size '" + path + "' : " + win32_get_last_error()), "");
    }

    str file_content;
    file_content.resize(static_cast<size_t>(file_size.QuadPart));

    DWORD bytes_read;
    if (!ReadFile(file_handle, &file_content[0], static_cast<DWORD>(file_size.QuadPart),
                  &bytes_read, NULL)) {
        CloseHandle(file_handle);
        return std::make_pair(
            Error("Failed to read file '" + path + "' : " + win32_get_last_error()), "");
    }

    CloseHandle(file_handle);
    file_content.resize(static_cast<size_t>(bytes_read));

    return std::make_pair(Error(), file_content);
}

}  // namespace blaz
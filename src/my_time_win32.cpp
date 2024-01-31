#include <windows.h>

#include "my_time.h"

namespace blaz {

u64 get_timestamp_microsecond() {
    LARGE_INTEGER frequency, time;
    QueryPerformanceFrequency(&frequency);  // @note This value never changes so it should be cached
    QueryPerformanceCounter(&time);
    time.QuadPart *= 1000000;
    time.QuadPart /= frequency.QuadPart;
    return (u64)time.QuadPart;
}

void sleep_milisecond(u32 ms) {
    Sleep(ms);
}

}  // namespace blaz
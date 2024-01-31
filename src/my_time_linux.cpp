#include <time.h>

#include "my_time.h"

namespace blaz {

u64 get_timestamp_microsecond() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return (u64)((1000000000 * (time.tv_sec) + time.tv_nsec) / 1000);
}

void sleep_milisecond(u32 ms) {
    struct timespec time;
    time.tv_sec = ms / 1000;
    time.tv_nsec = (ms * 1000000) % 1000000000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &time, NULL);
}

}  // namespace blaz
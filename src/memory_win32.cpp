#include <windows.h>

#include "memory.h"

namespace blaz {

void* alloc(size_t size) {
    return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void dealloc(void* ptr) {
    VirtualFree(ptr, 0, MEM_RELEASE);
}

void memcopy(void* dst, const void* src, size_t count) {
    memcpy(dst, src, count);
}

}  // namespace blaz
#pragma once

#include "types.h"

namespace blaz {

void* alloc(size_t size);
void dealloc(void* ptr);
void memcopy(void* dst, const void* src, size_t count);

}  // namespace blaz
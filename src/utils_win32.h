#pragma once

#include "types.h"

namespace blaz {

str win32_get_last_error();
std::string wide_to_narrow_str(const std::wstring& wide_str);
std::wstring narrow_to_wide_str(const std::string& narrow_str);

};  // namespace blaz
#pragma once

#include "color.h"
#include "error.h"
#include "types.h"

namespace blaz {

struct Texture;
pair<Error, Texture> load_data_from_tga_file(str texture_path);

}  // namespace blaz
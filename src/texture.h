#pragma once

#include "color.h"
#include "types.h"

namespace blaz {

struct Texture;

Error load_texture_data_from_file(Texture* texture);
Error load_texture_data_from_tga_file(Texture* texture);

}  // namespace blaz
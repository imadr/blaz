#pragma once

#include "renderer.h"

namespace blaz {

Mesh make_cube();
Mesh make_uv_sphere(u32 slices, u32 stacks);

pair<Error, Mesh> load_from_obj_file(str mesh_path);

}  // namespace blaz
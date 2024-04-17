#pragma once

#include "renderer.h"

namespace blaz {

Mesh make_cube();
Mesh make_cube_wireframe();
Mesh make_plane();
Mesh make_uv_sphere(u32 slices, u32 stacks);
Mesh make_wireframe_sphere(u32 vertices);

pair<Error, Mesh> load_mesh_from_obj_file(str mesh_path);

}  // namespace blaz
#pragma once

#include "error.h"
#include "types.h"

namespace blaz {

struct Mesh;

Error make_cube(Mesh* mesh);
Error make_cube_wireframe(Mesh* mesh);
Error make_plane(Mesh* mesh);
Error make_uv_sphere(Mesh* mesh, u32 slices, u32 stacks);
Error make_wireframe_sphere(Mesh* mesh, u32 vertices);

Error load_mesh_from_obj_file(Mesh* mesh);
Error load_mesh_from_file(Mesh* mesh);
Error export_mesh_file(const str& path, Mesh* mesh);

}  // namespace blaz
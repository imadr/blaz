#pragma once

#include "error.h"
#include "types.h"

namespace blaz {

struct Mesh {
    str m_name;
    vec<f32> m_vertices;
    vec<u32> m_indices;
    vec<pair<str, u32>> m_attribs;

    void* m_api_data = NULL;

    Error load_from_obj_file(str mesh_path);
};

}  // namespace blaz
#pragma once

#include "types.h"
#include "error.h"

namespace blaz {

struct Shader {
    str m_name;
    str m_vertex_shader_source;
    str m_fragment_shader_source;
    str m_vertex_shader_path;
    str m_fragment_shader_path;
    void* m_api_data = NULL;

    bool m_should_reload = false;
    bool m_is_error = false;

    Error load_from_file(str vertex_shader_path, str fragment_shader_path);
};

}  // namespace blaz
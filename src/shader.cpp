#include "shader.h"
#include "error.h"
#include "types.h"
#include <fstream>
#include <sstream>
#include <iosfwd>

namespace blaz {

Error Shader::load_from_file(str vertex_shader_path, str fragment_shader_path) {
    m_vertex_shader_path = vertex_shader_path;
    m_fragment_shader_path = fragment_shader_path;
    std::ifstream vertex_shader_file(vertex_shader_path);
    if (!vertex_shader_file.is_open()) {
        return Error("Can't open file " + vertex_shader_path);
    }
    std::stringstream buffer;
    buffer << vertex_shader_file.rdbuf();
    m_vertex_shader_source = buffer.str();

    std::ifstream fragment_shader_file(fragment_shader_path);
    if (!fragment_shader_file.is_open()) {
        return Error("Can't open file " + fragment_shader_path);
    }
    buffer.str("");
    buffer << fragment_shader_file.rdbuf();
    m_fragment_shader_source = buffer.str();

    return Error();
}

}  // namespace blaz
#include "mesh.h"
#include <iosfwd>
#include "error.h"
#include "types.h"
#include <string>
#include <tuple>
#include <map>
#include <string>
#include <string_view>
#include <tuple>
#include <charconv>
#include <sstream>
#include <fstream>

namespace blaz {

vec<std::string_view> split_strview(std::string_view str, char delim) {
    vec<std::string_view> result;

    int index_comma_to_left_of_column = 0;
    int index_comma_to_right_of_column = -1;

    for (int i = 0; i < static_cast<int>(str.size()); i++) {
        if (str[i] == delim) {
            index_comma_to_left_of_column = index_comma_to_right_of_column;
            index_comma_to_right_of_column = i;
            int index = index_comma_to_left_of_column + 1;
            int length = index_comma_to_right_of_column - index;
            std::string_view column(str.data() + index, length);
            result.push_back(column);
        }
    }
    const std::string_view final_column(str.data() + index_comma_to_right_of_column + 1,
                                        str.size() - index_comma_to_right_of_column - 1);
    result.push_back(final_column);
    return result;
}

Error Mesh::load_from_obj_file(str mesh_path) {
    std::ifstream mesh_file(mesh_path);
    if (!mesh_file.is_open()) {
        return Error("Can't open file " + mesh_path);
    }
    std::stringstream buffer;
    buffer << mesh_file.rdbuf();
    str mesh_content = buffer.str();

    vec<std::string_view> lines = split_strview(mesh_content, '\n');

    struct Position {
        f32 x, y, z;
    };
    struct Normal {
        f32 x, y, z;
    };
    struct Tangent {
        f32 x, y, z;
    };
    struct Texcoord {
        f32 u, v;
    };

    vec<Position> positions;
    vec<Normal> normals;
    vec<Texcoord> texcoords;

    std::map<std::string_view, u32> unique_vertices;
    vec<std::tuple<u32, u32, u32>> vertex_indices;

    for (u32 i = 0; i < lines.size(); i++) {
        vec<std::string_view> line = split_strview(lines[i], ' ');
        if (line[0] == "#" || line[0] == "mtllib" || line[0] == "o" || line[0] == "s" ||
            line[0] == "usemtl") {
            continue;
        } else if (line[0] == "v") {
            f32 x, y, z;
            std::from_chars(line[1].data(), line[1].data() + line[1].size(), x);
            std::from_chars(line[2].data(), line[2].data() + line[2].size(), y);
            std::from_chars(line[3].data(), line[3].data() + line[3].size(), z);
            positions.push_back(Position(x, y, z));
        } else if (line[0] == "vn") {
            f32 x, y, z;
            std::from_chars(line[1].data(), line[1].data() + line[1].size(), x);
            std::from_chars(line[2].data(), line[2].data() + line[2].size(), y);
            std::from_chars(line[3].data(), line[3].data() + line[3].size(), z);
            normals.push_back(Normal(x, y, z));
        } else if (line[0] == "vt") {
            f32 u, v;
            std::from_chars(line[1].data(), line[1].data() + line[1].size(), u);
            std::from_chars(line[2].data(), line[2].data() + line[2].size(), v);
            texcoords.push_back(Texcoord(u, v));
        } else if (line[0] == "f") {
            for (u32 j = 1; j < 4; j++) {
                if (!unique_vertices.contains(line[j])) {
                    vec<std::string_view> face = split_strview(line[j], '/');
                    unique_vertices[line[j]] = (u32)vertex_indices.size();
                    u32 face_1, face_2, face_3;
                    std::from_chars(face[0].data(), face[0].data() + face[0].size(), face_1);
                    std::from_chars(face[1].data(), face[1].data() + face[1].size(), face_2);
                    std::from_chars(face[2].data(), face[2].data() + face[2].size(), face_3);
                    vertex_indices.push_back(std::make_tuple(face_1 - 1, face_2 - 1, face_3 - 1));
                }
                m_indices.push_back(unique_vertices[line[j]]);
            }
        }
    }

    m_vertices.reserve(unique_vertices.size() * 8);
    for (u32 i = 0; i < vertex_indices.size(); i++) {
        u32 position_index = std::get<0>(vertex_indices[i]);
        u32 texcoord_index = std::get<1>(vertex_indices[i]);
        u32 normal_index = std::get<2>(vertex_indices[i]);
        m_vertices.push_back(positions[position_index].x);
        m_vertices.push_back(positions[position_index].y);
        m_vertices.push_back(positions[position_index].z);
        m_vertices.push_back(normals[normal_index].x);
        m_vertices.push_back(normals[normal_index].y);
        m_vertices.push_back(normals[normal_index].z);
        m_vertices.push_back(texcoords[texcoord_index].u);
        m_vertices.push_back(texcoords[texcoord_index].v);
    }

    m_attribs = {
        {"position_attrib", 3},
        {"normal_attrib", 3},
        {"texcoord_attrib", 2},
    };

    return Error();
}

}  // namespace blaz
#include "mesh.h"

#include <charconv>
#include <fstream>
#include <sstream>
#include <string_view>

#include "my_math.h"

namespace blaz {

Mesh make_cube() {
    return Mesh{
        .m_name = "cube",
        .m_vertices =
            {
                -0.5, 0.5,  0.5,  0.0,  0.0,  1.0,  0.0,  0.0,  0.5,  0.5,  0.5,  0.0,  0.0,  1.0,
                1.0,  0.0,  0.5,  -0.5, 0.5,  0.0,  0.0,  1.0,  1.0,  1.0,  -0.5, -0.5, 0.5,  0.0,
                0.0,  1.0,  0.0,  1.0,  0.5,  0.5,  -0.5, 0.0,  0.0,  -1.0, 0.0,  0.0,  -0.5, 0.5,
                -0.5, 0.0,  0.0,  -1.0, 1.0,  0.0,  -0.5, -0.5, -0.5, 0.0,  0.0,  -1.0, 1.0,  1.0,
                0.5,  -0.5, -0.5, 0.0,  0.0,  -1.0, 0.0,  1.0,  0.5,  0.5,  0.5,  1.0,  0.0,  0.0,
                0.0,  0.0,  0.5,  0.5,  -0.5, 1.0,  0.0,  0.0,  1.0,  0.0,  0.5,  -0.5, -0.5, 1.0,
                0.0,  0.0,  1.0,  1.0,  0.5,  -0.5, 0.5,  1.0,  0.0,  0.0,  0.0,  1.0,  -0.5, 0.5,
                -0.5, -1.0, 0.0,  0.0,  0.0,  0.0,  -0.5, 0.5,  0.5,  -1.0, 0.0,  0.0,  1.0,  0.0,
                -0.5, -0.5, 0.5,  -1.0, 0.0,  0.0,  1.0,  1.0,  -0.5, -0.5, -0.5, -1.0, 0.0,  0.0,
                0.0,  1.0,  0.5,  0.5,  0.5,  0.0,  1.0,  0.0,  0.0,  0.0,  -0.5, 0.5,  0.5,  0.0,
                1.0,  0.0,  1.0,  0.0,  -0.5, 0.5,  -0.5, 0.0,  1.0,  0.0,  1.0,  1.0,  0.5,  0.5,
                -0.5, 0.0,  1.0,  0.0,  0.0,  1.0,  -0.5, -0.5, 0.5,  0.0,  -1.0, 0.0,  0.0,  0.0,
                0.5,  -0.5, 0.5,  0.0,  -1.0, 0.0,  1.0,  0.0,  0.5,  -0.5, -0.5, 0.0,  -1.0, 0.0,
                1.0,  1.0,  -0.5, -0.5, -0.5, 0.0,  -1.0, 0.0,  0.0,  1.0,
            },
        .m_indices =
            {
                0,  3,  2,  2,  1,  0,  4,  7,  6,  6,  5,  4,  8,  11, 10, 10, 9,  8,
                12, 15, 14, 14, 13, 12, 16, 19, 18, 18, 17, 16, 20, 23, 22, 22, 21, 20,
            },
        .m_attribs =
            {
                {"position", 3},
                {"normal", 3},
                {"uv", 2},
            },
        .m_primitive = MeshPrimitive::TRIANGLES,
    };
}

Mesh make_cube_wireframe() {
    return Mesh{
        .m_name = "cube",
        .m_vertices =
            {
                -0.5, 0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  -0.5, 0.5,  -0.5, -0.5, 0.5,
                -0.5, 0.5,  -0.5, 0.5,  0.5,  -0.5, 0.5,  -0.5, -0.5, -0.5, -0.5, -0.5,
                -0.5, 0.5,  0.5,  -0.5, 0.5,  -0.5, 0.5,  0.5,  0.5,  0.5,  0.5,  -0.5,
                0.5,  -0.5, 0.5,  0.5,  -0.5, -0.5, -0.5, -0.5, 0.5,  -0.5, -0.5, -0.5,
            },
        .m_indices =
            {
                0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7,
            },
        .m_attribs =
            {
                {"position", 3},
            },
        .m_primitive = MeshPrimitive::LINES,
    };
}

Mesh make_plane() {
    return Mesh{
        .m_name = "plane",
        .m_vertices =
            {
                -0.5, 0.0, 0.5,  0.0, 1.0, 0.0, 0.0, 0.0, 0.5,  0.0, 0.5,  0.0, 1.0, 0.0, 1.0, 0.0,
                0.5,  0.0, -0.5, 0.0, 1.0, 0.0, 1.0, 1.0, -0.5, 0.0, -0.5, 0.0, 1.0, 0.0, 0.0, 1.0,
            },
        .m_indices = {0, 1, 2, 2, 3, 0},
        .m_attribs =
            {
                {"position", 3},
                {"normal", 3},
                {"uv", 2},
            },
        .m_primitive = MeshPrimitive::TRIANGLES,
    };
}

Mesh make_uv_sphere(u32 slices, u32 stacks) {
    Mesh mesh = Mesh{
        .m_name = "sphere",
        .m_attribs =
            {
                {"position", 3},
                {"normal", 3},
                {"uv", 2},
            },
        .m_primitive = MeshPrimitive::TRIANGLES,
    };

    mesh.m_vertices = {0, 1, 0, 0, 1, 0, 0, 0};

    for (u32 i = 0; i < stacks; i++) {
        f32 phi = f32(PI) * f32(i + 1) / f32(stacks);
        for (u32 j = 0; j < slices; j++) {
            f32 theta = 2.0f * f32(PI) * (f32(j) + 0.5f) / f32(slices);
            if (slices % 2 != 0) theta += f32(PI) / 2 / f32(slices);
            f32 x = sin(phi) * cos(theta);
            f32 y = cos(phi);
            f32 z = sin(phi) * sin(theta);
            Vec3 normal = Vec3(x, y, z).normalize();
            mesh.m_vertices.insert(mesh.m_vertices.end(), {x, y, z});
            mesh.m_vertices.insert(mesh.m_vertices.end(), {normal.x(), normal.y(), normal.z()});
            mesh.m_vertices.insert(mesh.m_vertices.end(), {0, 0});
        }
    }
    mesh.m_vertices.insert(mesh.m_vertices.end(), {0, -1, 0, 0, -1, 0, 0, 0});

    for (u32 i = 0; i < slices; i++) {
        u32 i0 = i + 1;
        u32 i1 = (i + 1) % slices + 1;
        mesh.m_indices.insert(mesh.m_indices.end(), {0, i1, i0});
        i0 = i + slices * (stacks - 2) + 1;
        i1 = (i + 1) % slices + slices * (stacks - 2) + 1;
        mesh.m_indices.insert(mesh.m_indices.end(), {i0, i1, u32(mesh.m_vertices.size() / 8) - 1});
    }

    for (u32 j = 0; j < stacks - 2; j++) {
        auto j0 = j * slices + 1;
        auto j1 = (j + 1) * slices + 1;
        for (u32 i = 0; i < slices; i++) {
            auto i0 = j0 + i;
            auto i1 = j0 + (i + 1) % slices;
            auto i2 = j1 + (i + 1) % slices;
            auto i3 = j1 + i;
            mesh.m_indices.insert(mesh.m_indices.end(), {i0, i1, i3});
            mesh.m_indices.insert(mesh.m_indices.end(), {i1, i2, i3});
        }
    }

    return mesh;
}

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

pair<Error, Mesh> load_from_obj_file(str mesh_path) {
    // std::ifstream mesh_file(mesh_path);
    // if (!mesh_file.is_open()) {
    //     return {Error("Can't open file " + mesh_path), Mesh()};
    // }
    // std::stringstream buffer;
    // buffer << mesh_file.rdbuf();
    // str mesh_content = buffer.str();

    // vec<std::string_view> lines = split_strview(mesh_content, '\n');

    // struct Position {
    //     f32 x, y, z;
    // };
    // struct Normal {
    //     f32 x, y, z;
    // };
    // struct Tangent {
    //     f32 x, y, z;
    // };
    // struct Texcoord {
    //     f32 u, v;
    // };

    // vec<Position> positions;
    // vec<Normal> normals;
    // vec<Texcoord> texcoords;

    // std::map<std::string_view, u32> unique_vertices;
    // vec<std::tuple<u32, u32, u32>> vertex_indices;

    // vec<f32> vertices;
    // vec<u32> indices;

    // for (u32 i = 0; i < lines.size(); i++) {
    //     vec<std::string_view> line = split_strview(lines[i], ' ');
    //     if (line[0] == "#" || line[0] == "mtllib" || line[0] == "o" || line[0] == "s" ||
    //         line[0] == "usemtl") {
    //         continue;
    //     } else if (line[0] == "v") {
    //         f32 x, y, z;
    //         std::from_chars(line[1].data(), line[1].data() + line[1].size(), x);
    //         std::from_chars(line[2].data(), line[2].data() + line[2].size(), y);
    //         std::from_chars(line[3].data(), line[3].data() + line[3].size(), z);
    //         positions.push_back(Position(x, y, z));
    //     } else if (line[0] == "vn") {
    //         f32 x, y, z;
    //         std::from_chars(line[1].data(), line[1].data() + line[1].size(), x);
    //         std::from_chars(line[2].data(), line[2].data() + line[2].size(), y);
    //         std::from_chars(line[3].data(), line[3].data() + line[3].size(), z);
    //         normals.push_back(Normal(x, y, z));
    //     } else if (line[0] == "vt") {
    //         f32 u, v;
    //         std::from_chars(line[1].data(), line[1].data() + line[1].size(), u);
    //         std::from_chars(line[2].data(), line[2].data() + line[2].size(), v);
    //         texcoords.push_back(Texcoord(u, v));
    //     } else if (line[0] == "f") {
    //         for (u32 j = 1; j < 4; j++) {
    //             if (!unique_vertices.contains(line[j])) {
    //                 vec<std::string_view> face = split_strview(line[j], '/');
    //                 unique_vertices[line[j]] = (u32)vertex_indices.size();
    //                 u32 face_1, face_2, face_3;
    //                 std::from_chars(face[0].data(), face[0].data() + face[0].size(), face_1);
    //                 std::from_chars(face[1].data(), face[1].data() + face[1].size(), face_2);
    //                 std::from_chars(face[2].data(), face[2].data() + face[2].size(), face_3);
    //                 vertex_indices.push_back(std::make_tuple(face_1 - 1, face_2 - 1, face_3 - 1));
    //             }
    //             indices.push_back(unique_vertices[line[j]]);
    //         }
    //     }
    // }

    // vertices.reserve(unique_vertices.size() * 8);
    // for (u32 i = 0; i < vertex_indices.size(); i++) {
    //     u32 position_index = std::get<0>(vertex_indices[i]);
    //     u32 texcoord_index = std::get<1>(vertex_indices[i]);
    //     u32 normal_index = std::get<2>(vertex_indices[i]);
    //     vertices.push_back(positions[position_index].x);
    //     vertices.push_back(positions[position_index].y);
    //     vertices.push_back(positions[position_index].z);
    //     vertices.push_back(normals[normal_index].x);
    //     vertices.push_back(normals[normal_index].y);
    //     vertices.push_back(normals[normal_index].z);
    //     vertices.push_back(texcoords[texcoord_index].u);
    //     vertices.push_back(texcoords[texcoord_index].v);
    // }

    // Mesh mesh = Mesh{
    //     .m_name = "sphere",
    //     .m_vertices = vertices,
    //     .m_indices = indices,
    //     .m_attribs =
    //         {
    //             {"position", 3},
    //             {"normal", 3},
    //             {"uv", 2},
    //         },
    //     .m_primitive = MeshPrimitive::TRIANGLES,
    // };

    // return {Error(), mesh};
    return {Error("not implemented"), Mesh()};
}

Mesh make_wireframe_sphere(u32 vertices) {
    Mesh mesh = Mesh{.m_name = "wireframe_sphere",
                     .m_attribs =
                         {
                             {"position", 3},
                         },
                     .m_primitive = MeshPrimitive::LINES};

    for (u32 side = 0; side < 3; side++) {
        for (u32 i = 0; i < vertices; i++) {
            f32 phi = f32(PI) * f32(i) / f32(vertices / 2.0);

            f32 x = cos(phi);
            f32 y = sin(phi);
            if (side == 0) mesh.m_vertices.insert(mesh.m_vertices.end(), {x, 0, y});
            if (side == 1) mesh.m_vertices.insert(mesh.m_vertices.end(), {0, x, y});
            if (side == 2) mesh.m_vertices.insert(mesh.m_vertices.end(), {x, y, 0});
        }
        u32 index = side * vertices;
        for (; index < (side + 1) * vertices - 1; index++) {
            mesh.m_indices.insert(mesh.m_indices.end(), {index, index + 1});
        }
        mesh.m_indices.insert(mesh.m_indices.end(), {index, side * vertices});
    }

    return mesh;
}

}  // namespace blaz
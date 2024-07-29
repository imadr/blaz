#include "mesh.h"

#include <logger.h>

#include "filesystem.h"
#include "my_math.h"
#include "renderer.h"

namespace blaz {

Error make_cube(Mesh* mesh) {
    mesh->m_vertices = {
        -0.5, 0.5,  0.5,  0.0,  0.0,  1.0,  0.0, 0.0, 0.5,  0.5,  0.5,  0.0,  0.0,  1.0,  1.0, 0.0,
        0.5,  -0.5, 0.5,  0.0,  0.0,  1.0,  1.0, 1.0, -0.5, -0.5, 0.5,  0.0,  0.0,  1.0,  0.0, 1.0,
        0.5,  0.5,  -0.5, 0.0,  0.0,  -1.0, 0.0, 0.0, -0.5, 0.5,  -0.5, 0.0,  0.0,  -1.0, 1.0, 0.0,
        -0.5, -0.5, -0.5, 0.0,  0.0,  -1.0, 1.0, 1.0, 0.5,  -0.5, -0.5, 0.0,  0.0,  -1.0, 0.0, 1.0,
        0.5,  0.5,  0.5,  1.0,  0.0,  0.0,  0.0, 0.0, 0.5,  0.5,  -0.5, 1.0,  0.0,  0.0,  1.0, 0.0,
        0.5,  -0.5, -0.5, 1.0,  0.0,  0.0,  1.0, 1.0, 0.5,  -0.5, 0.5,  1.0,  0.0,  0.0,  0.0, 1.0,
        -0.5, 0.5,  -0.5, -1.0, 0.0,  0.0,  0.0, 0.0, -0.5, 0.5,  0.5,  -1.0, 0.0,  0.0,  1.0, 0.0,
        -0.5, -0.5, 0.5,  -1.0, 0.0,  0.0,  1.0, 1.0, -0.5, -0.5, -0.5, -1.0, 0.0,  0.0,  0.0, 1.0,
        0.5,  0.5,  0.5,  0.0,  1.0,  0.0,  0.0, 0.0, -0.5, 0.5,  0.5,  0.0,  1.0,  0.0,  1.0, 0.0,
        -0.5, 0.5,  -0.5, 0.0,  1.0,  0.0,  1.0, 1.0, 0.5,  0.5,  -0.5, 0.0,  1.0,  0.0,  0.0, 1.0,
        -0.5, -0.5, 0.5,  0.0,  -1.0, 0.0,  0.0, 0.0, 0.5,  -0.5, 0.5,  0.0,  -1.0, 0.0,  1.0, 0.0,
        0.5,  -0.5, -0.5, 0.0,  -1.0, 0.0,  1.0, 1.0, -0.5, -0.5, -0.5, 0.0,  -1.0, 0.0,  0.0, 1.0,
    };
    mesh->m_indices = {
        0,  3,  2,  2,  1,  0,  4,  7,  6,  6,  5,  4,  8,  11, 10, 10, 9,  8,
        12, 15, 14, 14, 13, 12, 16, 19, 18, 18, 17, 16, 20, 23, 22, 22, 21, 20,
    };
    mesh->m_attribs = {
        {"position", 3},
        {"normal", 3},
        {"texcoord", 2},
    };
    mesh->m_vertex_stride = 8;
    mesh->m_primitive = MeshPrimitive::TRIANGLES;
    return Error();
}

Error make_cube_wireframe(Mesh* mesh) {
    mesh->m_vertices = {
        -0.5, 0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  -0.5, 0.5,  -0.5, -0.5, 0.5,
        -0.5, 0.5,  -0.5, 0.5,  0.5,  -0.5, 0.5,  -0.5, -0.5, -0.5, -0.5, -0.5,
        -0.5, 0.5,  0.5,  -0.5, 0.5,  -0.5, 0.5,  0.5,  0.5,  0.5,  0.5,  -0.5,
        0.5,  -0.5, 0.5,  0.5,  -0.5, -0.5, -0.5, -0.5, 0.5,  -0.5, -0.5, -0.5,
    };
    mesh->m_indices = {
        0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7,
    };
    mesh->m_attribs = {
        {"position", 3},
    };
    mesh->m_vertex_stride = 3;
    mesh->m_primitive = MeshPrimitive::LINES;
    return Error();
}

Error make_plane(Mesh* mesh) {
    mesh->m_vertices = {
        -0.5, 0.0, 0.5,  0.0, 1.0, 0.0, 0.0, 0.0, 0.5,  0.0, 0.5,  0.0, 1.0, 0.0, 1.0, 0.0,
        0.5,  0.0, -0.5, 0.0, 1.0, 0.0, 1.0, 1.0, -0.5, 0.0, -0.5, 0.0, 1.0, 0.0, 0.0, 1.0,
    };
    mesh->m_indices = {
        0, 1, 2, 2, 3, 0, 0, 3, 2, 2, 1, 0,
    };
    mesh->m_attribs = {
        {"position", 3},
        {"normal", 3},
        {"texcoord", 2},
    };
    mesh->m_vertex_stride = 8;
    mesh->m_primitive = MeshPrimitive::TRIANGLES;
    return Error();
}

Error generate_tangent(Mesh* mesh) {
    bool found = false;
    for (u32 i = 0; i < mesh->m_attribs.size(); i++) {
        if (mesh->m_attribs[i].first == "tangent") {
            found = true;
            break;
        }
    }
    if (!found) {
        return Error("Mesh doesn't have tangent attrib");
    }

    for (u32 i = 0; i < mesh->m_indices.size() / 3; i++) {
        u32 vertex_0 = mesh->m_indices[i * 3 + 0];
        u32 vertex_1 = mesh->m_indices[i * 3 + 1];
        u32 vertex_2 = mesh->m_indices[i * 3 + 2];

        Vec3 pos_0 = Vec3(mesh->m_vertices[vertex_0 * mesh->m_vertex_stride + 0],
                          mesh->m_vertices[vertex_0 * mesh->m_vertex_stride + 1],
                          mesh->m_vertices[vertex_0 * mesh->m_vertex_stride + 2]);
        Vec3 pos_1 = Vec3(mesh->m_vertices[vertex_1 * mesh->m_vertex_stride + 0],
                          mesh->m_vertices[vertex_1 * mesh->m_vertex_stride + 1],
                          mesh->m_vertices[vertex_1 * mesh->m_vertex_stride + 2]);
        Vec3 pos_2 = Vec3(mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 0],
                          mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 1],
                          mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 2]);

        Vec2 texcoord_0 = Vec2(mesh->m_vertices[vertex_0 * mesh->m_vertex_stride + 9],
                               mesh->m_vertices[vertex_0 * mesh->m_vertex_stride + 10]);
        Vec2 texcoord_1 = Vec2(mesh->m_vertices[vertex_1 * mesh->m_vertex_stride + 9],
                               mesh->m_vertices[vertex_1 * mesh->m_vertex_stride + 10]);
        Vec2 texcoord_2 = Vec2(mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 9],
                               mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 10]);

        Vec3 edge_1 = pos_1 - pos_0;
        Vec3 edge_2 = pos_2 - pos_0;

        f32 delta_u_1 = texcoord_1.x() - texcoord_0.x();
        f32 delta_v_1 = texcoord_1.y() - texcoord_0.y();
        f32 delta_u_2 = texcoord_2.x() - texcoord_0.x();
        f32 delta_v_2 = texcoord_2.y() - texcoord_0.y();

        f32 f = 1.0f / (delta_u_1 * delta_v_2 - delta_u_2 * delta_v_1);

        f32 tangent_x = f * (delta_v_2 * edge_1.x() - delta_v_1 * edge_2.x());
        f32 tangent_y = f * (delta_v_2 * edge_1.y() - delta_v_1 * edge_2.y());
        f32 tangent_z = f * (delta_v_2 * edge_1.z() - delta_v_1 * edge_2.z());

        mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 6] += tangent_x;
        mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 7] += tangent_y;
        mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 8] += tangent_z;
    }

    for (u32 i = 0; i < mesh->m_indices.size() / 3; i++) {
        u32 vertex_0 = mesh->m_indices[i * 3 + 0];
        u32 vertex_1 = mesh->m_indices[i * 3 + 1];
        u32 vertex_2 = mesh->m_indices[i * 3 + 2];

        Vec3 tangent = Vec3(mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 6],
                            mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 7],
                            mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 8]);
        tangent = tangent.normalize();
        mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 6] = tangent.x();
        mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 7] = tangent.y();
        mesh->m_vertices[vertex_2 * mesh->m_vertex_stride + 8] = tangent.z();
    }
    return Error();
}

Error make_uv_sphere(Mesh* mesh, u32 slices, u32 stacks) {
    mesh->m_attribs = {
        {"position", 3},
        {"normal", 3},
        {"tangent", 3},
        {"texcoord", 2},
    };
    mesh->m_vertex_stride = 11;
    mesh->m_primitive = MeshPrimitive::TRIANGLES;

    mesh->m_vertices = {0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0};

    for (u32 i = 0; i < stacks; i++) {
        f32 phi = f32(PI) * f32(i + 1) / f32(stacks);
        for (u32 j = 0; j < slices; j++) {
            f32 theta = 2.0f * f32(PI) * (f32(j) + 0.5f) / f32(slices);
            if (slices % 2 != 0) theta += f32(PI) / 2 / f32(slices);
            f32 x = sin(phi) * cos(theta);
            f32 y = cos(phi);
            f32 z = sin(phi) * sin(theta);
            Vec3 normal = Vec3(x, y, z).normalize();
            mesh->m_vertices.insert(mesh->m_vertices.end(), {x, y, z});
            mesh->m_vertices.insert(mesh->m_vertices.end(), {normal.x(), normal.y(), normal.z()});
            mesh->m_vertices.insert(mesh->m_vertices.end(), {1, 1, 1});
            mesh->m_vertices.insert(mesh->m_vertices.end(),
                                    {float(i) / float(stacks), float(j) / float(slices)});
        }
    }
    mesh->m_vertices.insert(mesh->m_vertices.end(), {0, -1, 0, 0, -1, 0, 1, 1, 1, 0, 0});

    for (u32 i = 0; i < slices; i++) {
        u32 i0 = i + 1;
        u32 i1 = (i + 1) % slices + 1;
        mesh->m_indices.insert(mesh->m_indices.end(), {0, i1, i0});
        i0 = i + slices * (stacks - 2) + 1;
        i1 = (i + 1) % slices + slices * (stacks - 2) + 1;
        mesh->m_indices.insert(mesh->m_indices.end(),
                               {i0, i1, u32(mesh->m_vertices.size() / mesh->m_vertex_stride) - 1});
    }

    for (u32 j = 0; j < stacks - 2; j++) {
        auto j0 = j * slices + 1;
        auto j1 = (j + 1) * slices + 1;
        for (u32 i = 0; i < slices; i++) {
            auto i0 = j0 + i;
            auto i1 = j0 + (i + 1) % slices;
            auto i2 = j1 + (i + 1) % slices;
            auto i3 = j1 + i;
            mesh->m_indices.insert(mesh->m_indices.end(), {i0, i1, i3});
            mesh->m_indices.insert(mesh->m_indices.end(), {i1, i2, i3});
        }
    }

    return generate_tangent(mesh);
}

Error make_wireframe_sphere(Mesh* mesh, u32 vertices) {
    mesh->m_attribs = {
        {"position", 3},
    };
    mesh->m_vertex_stride = 3;
    mesh->m_primitive = MeshPrimitive::LINES;

    for (u32 side = 0; side < 3; side++) {
        for (u32 i = 0; i < vertices; i++) {
            f32 phi = f32(PI) * f32(i) / f32(vertices / 2.0);

            f32 x = cos(phi);
            f32 y = sin(phi);
            if (side == 0) mesh->m_vertices.insert(mesh->m_vertices.end(), {x, 0, y});
            if (side == 1) mesh->m_vertices.insert(mesh->m_vertices.end(), {0, x, y});
            if (side == 2) mesh->m_vertices.insert(mesh->m_vertices.end(), {x, y, 0});
        }
        u32 index = side * vertices;
        for (; index < (side + 1) * vertices - 1; index++) {
            mesh->m_indices.insert(mesh->m_indices.end(), {index, index + 1});
        }
        mesh->m_indices.insert(mesh->m_indices.end(), {index, side * vertices});
    }

    return Error();
}

Error load_mesh_from_obj_file(Mesh* mesh) {
    pair<Error, str> file_content = read_whole_file(mesh->m_path);
    if (file_content.first) {
        return file_content.first;
    }

    vec<Vec3> positions;
    vec<Vec3> normals;
    vec<Vec2> texcoords;

    std::map<std::tuple<u32, u32, u32>, u32> unique_faces;
    std::vector<std::tuple<u32, u32, u32>> vertex_indices;

    str& obj = file_content.second;
    str token;
    u32 token_n = 0;

    Vec3 vec3_buffer;
    Vec2 vec2_buffer;
    u32 faces_buffer[9];

    enum struct State {
        DEFAULT,
        SKIP_UNTIL_NEW_LINE,
        FILL_POSITION,
        FILL_NORMALS,
        FILL_TEXCOORD,
        FILL_FACES,
    };
    State state = State::DEFAULT;

    obj += '\0';
    for (u32 i = 0; i < obj.size(); i++) {
        if (state == State::SKIP_UNTIL_NEW_LINE) {
            if (obj[i] == '\n' || obj[i] == '\0') {
                state = State::DEFAULT;
            } else {
                continue;
            }
        }

        bool is_spacer = obj[i] == ' ' || obj[i] == '\0' || obj[i] == '\n' || obj[i] == '/';
        if (is_spacer) {
            if (token_n == 0) {
                if ((token == "#" || token == "o" || token == "s" || token == "usemtl" ||
                     token == "mtllib")) {
                    state = State::SKIP_UNTIL_NEW_LINE;
                } else if (token == "v") {
                    state = State::FILL_POSITION;
                } else if (token == "vn") {
                    state = State::FILL_NORMALS;
                } else if (token == "vt") {
                    state = State::FILL_TEXCOORD;
                } else if (token == "f") {
                    state = State::FILL_FACES;
                }
            } else {
                if (state == State::FILL_POSITION || state == State::FILL_NORMALS) {
                    vec3_buffer[token_n - 1] = std::stof(token);
                } else if (state == State::FILL_TEXCOORD) {
                    vec2_buffer[token_n - 1] = std::stof(token);
                } else if (state == State::FILL_FACES) {
                    faces_buffer[token_n - 1] = std::stoi(token);
                }
            }

            if (token_n == 3 && state == State::FILL_POSITION) {
                positions.push_back(vec3_buffer);
            } else if (token_n == 3 && state == State::FILL_NORMALS) {
                normals.push_back(vec3_buffer);
            } else if (token_n == 2 && state == State::FILL_TEXCOORD) {
                texcoords.push_back(vec2_buffer);
            } else if (token_n == 9 && state == State::FILL_FACES) {
                for (u32 j = 0; j <= 2; j++) {
                    std::tuple<u32, u32, u32> face = {faces_buffer[j * 3], faces_buffer[j * 3 + 1],
                                                      faces_buffer[j * 3 + 2]};
                    if (!unique_faces.contains(face)) {
                        unique_faces[face] = u32(vertex_indices.size());
                        vertex_indices.push_back(std::make_tuple(faces_buffer[j * 3] - 1,
                                                                 faces_buffer[j * 3 + 1] - 1,
                                                                 faces_buffer[j * 3 + 2] - 1));
                    }
                    mesh->m_indices.push_back(unique_faces[face]);
                }
            }

            token_n++;
            token = "";

            if (obj[i] == '\0') {
                break;
            } else if (obj[i] == '\n') {
                token_n = 0;
                state = State::DEFAULT;
            }
            continue;
        }

        token.push_back(obj[i]);
    }

    mesh->m_vertices.reserve(unique_faces.size() * 8);
    for (u32 i = 0; i < vertex_indices.size(); i++) {
        u32 position_index = std::get<0>(vertex_indices[i]);
        u32 texcoord_index = std::get<1>(vertex_indices[i]);
        u32 normal_index = std::get<2>(vertex_indices[i]);
        mesh->m_vertices.push_back(positions[position_index].x());
        mesh->m_vertices.push_back(positions[position_index].y());
        mesh->m_vertices.push_back(positions[position_index].z());
        mesh->m_vertices.push_back(normals[normal_index].x());
        mesh->m_vertices.push_back(normals[normal_index].y());
        mesh->m_vertices.push_back(normals[normal_index].z());
        mesh->m_vertices.push_back(1);
        mesh->m_vertices.push_back(1);
        mesh->m_vertices.push_back(1);
        mesh->m_vertices.push_back(texcoords[texcoord_index].x());
        mesh->m_vertices.push_back(texcoords[texcoord_index].y());
    }

    mesh->m_attribs = {
        {"position", 3},
        {"normal", 3},
        {"tangent", 3},
        {"texcoord", 2},
    };
    mesh->m_vertex_stride = 11;
    mesh->m_primitive = MeshPrimitive::TRIANGLES;

    generate_tangent(mesh);

    return Error();
}

}  // namespace blaz
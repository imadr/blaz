#include "mesh.h"

#include <logger.h>

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
                {"texcoord", 2},
            },
        .m_vertex_stride = 8,
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
        .m_vertex_stride = 3,
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
                {"texcoord", 2},
            },
        .m_vertex_stride = 8,
        .m_primitive = MeshPrimitive::TRIANGLES,
    };
}

void generate_tangent(Mesh* mesh) {
    bool found = false;
    for (u32 i = 0; i < mesh->m_attribs.size(); i++) {
        if (mesh->m_attribs[i].first == "tangent") {
            found = true;
            break;
        }
    }
    if (!found) {
        logger.error("Mesh doesn't have tangent attrib");
        return;
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
}

Mesh make_uv_sphere(u32 slices, u32 stacks) {
    Mesh mesh = Mesh{
        .m_name = "sphere",
        .m_attribs =
            {
                {"position", 3},
                {"normal", 3},
                {"tangent", 3},
                {"texcoord", 2},
            },
        .m_vertex_stride = 11,
        .m_primitive = MeshPrimitive::TRIANGLES,
    };

    mesh.m_vertices = {0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0};

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
            mesh.m_vertices.insert(mesh.m_vertices.end(), {1, 1, 1});
            mesh.m_vertices.insert(mesh.m_vertices.end(),
                                   {float(i) / float(stacks), float(j) / float(slices)});
        }
    }
    mesh.m_vertices.insert(mesh.m_vertices.end(), {0, -1, 0, 0, -1, 0, 1, 1, 1, 0, 0});

    for (u32 i = 0; i < slices; i++) {
        u32 i0 = i + 1;
        u32 i1 = (i + 1) % slices + 1;
        mesh.m_indices.insert(mesh.m_indices.end(), {0, i1, i0});
        i0 = i + slices * (stacks - 2) + 1;
        i1 = (i + 1) % slices + slices * (stacks - 2) + 1;
        mesh.m_indices.insert(mesh.m_indices.end(),
                              {i0, i1, u32(mesh.m_vertices.size() / mesh.m_vertex_stride) - 1});
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

    generate_tangent(&mesh);

    return mesh;
}

Mesh make_wireframe_sphere(u32 vertices) {
    Mesh mesh = Mesh{.m_name = "wireframe_sphere",
                     .m_attribs =
                         {
                             {"position", 3},
                         },
                     .m_vertex_stride = 3,
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
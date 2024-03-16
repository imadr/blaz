#include "mesh.h"

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
        .m_attribs = {
            {"position", 3},
            {"normal", 3},
            {"uv", 2},
        }};
}

Mesh make_uv_sphere(u32 slices, u32 stacks) {
    Mesh mesh = Mesh{.m_name = "cube",
                     .m_attribs = {
                         {"position", 3},
                         // {"normal", 3},
                         // {"uv", 2},
                     }};

    mesh.m_vertices = {0, 1, 0};
    for (u32 i = 0; i < stacks; i++) {
        f32 phi = f32(PI) * f32(i + 1) / f32(stacks);
        for (u32 j = 0; j < slices; j++) {
            f32 theta = 2.0f * f32(PI) * f32(j) / f32(slices);
            f32 x = sin(phi) * cos(theta);
            f32 y = cos(phi);
            f32 z = sin(phi) * sin(theta);
            mesh.m_vertices.insert(mesh.m_vertices.end(), {x, y, z});
        }
    }
    mesh.m_vertices.insert(mesh.m_vertices.end(), {0, -1, 0});

    for (u32 i = 0; i < slices; i++) {
        u32 i0 = i + 1;
        u32 i1 = (i + 1) % slices + 1;
        mesh.m_indices.insert(mesh.m_indices.end(), {i0, i1, 0});
        i0 = i + slices * (stacks - 2) + 1;
        i1 = (i + 1) % slices + slices * (stacks - 2) + 1;
        mesh.m_indices.insert(mesh.m_indices.end(), {i1, i0, u32(mesh.m_vertices.size() / 3) - 1});
    }

    for (u32 j = 0; j < stacks - 2; j++) {
        auto j0 = j * slices + 1;
        auto j1 = (j + 1) * slices + 1;
        for (u32 i = 0; i < slices; i++) {
            auto i0 = j0 + i;
            auto i1 = j0 + (i + 1) % slices;
            auto i2 = j1 + (i + 1) % slices;
            auto i3 = j1 + i;
            mesh.m_indices.insert(mesh.m_indices.end(), {i0, i3, i1});
            mesh.m_indices.insert(mesh.m_indices.end(), {i3, i2, i1});
        }
    }

    return mesh;
}

}  // namespace blaz
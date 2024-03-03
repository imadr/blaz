#include "mesh.h"

namespace blaz {

Mesh make_cube() {
    return Mesh{.m_name = "cube",
                .m_vertices =
                    {
                        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
                        0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,  0.5f,  0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,
                        1.0f,  1.0f,  -0.5f, 0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,  1.0f,  -0.5f,
                        -0.5f, 0.5f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,
                        0.0f,  1.0f,  1.0f,  0.0f, 0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f,  1.0f,
                        1.0f,  -0.5f, 0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f,  1.0f,
                    },
                .m_indices =
                    {
                        0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 0, 3, 7, 7, 4, 0,
                        1, 2, 6, 6, 5, 1, 3, 2, 6, 6, 7, 3, 0, 1, 5, 5, 4, 0,
                    },
                .m_attribs = {
                    {"position", 3},
                    {"normal", 3},
                    {"uv", 2},
                }};
}

}  // namespace blaz
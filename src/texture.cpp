#include "texture.h"

#include <cstdio>
#include <fstream>
#include <iosfwd>
#include "types.h"
#include "error.h"
#include "renderer.h"

namespace blaz {

pair<Error, Texture> load_data_from_tga_file(str texture_path) {
    std::ifstream texture_file(texture_path, std::ios::binary);
    if (!texture_file.is_open()) {
        return std::make_pair(Error("Can't open file " + texture_path), Texture());
    }

    u8 header[18];
    texture_file.read((char*)header, 18);

    Texture texture;
    texture.m_width = header[13] << 8 | header[12];
    texture.m_height = header[15] << 8 | header[14];
    texture.m_depth = header[16];

    if (texture.m_depth == 24) {
        texture.m_channels = 3;
    } else if (texture.m_depth == 32) {
        texture.m_channels = 4;
    }

    texture.m_data.resize(texture.m_width * texture.m_height * texture.m_channels);

    for (u32 i = 0; i < texture.m_width * texture.m_height * texture.m_channels; i += texture.m_channels) {
        u8 b, g, r, a;
        texture_file.read((char*)&b, 1);
        texture_file.read((char*)&g, 1);
        texture_file.read((char*)&r, 1);
        if (texture.m_channels == 4) {
            texture_file.read((char*)&a, 1);
            (texture.m_data)[i + 0] = r;
            (texture.m_data)[i + 1] = g;
            (texture.m_data)[i + 2] = b;
            (texture.m_data)[i + 3] = a;
        } else if (texture.m_channels == 3) {
            (texture.m_data)[i + 0] = r;
            (texture.m_data)[i + 1] = g;
            (texture.m_data)[i + 2] = b;
        }
    }

    return std::make_pair(Error(), texture);
}

}  // namespace blaz
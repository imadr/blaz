#include "texture.h"

#include <cstdio>
#include <fstream>
#include <iosfwd>

#include "error.h"
#include "filesystem.h"
#include "renderer.h"
#include "types.h"

namespace blaz {

Error load_texture(Texture* texture) {
    // @todo file format check here
    return load_texture_from_tga_file(texture);
}

Error load_texture_from_tga_file(Texture* texture) {
    pair<Error, vec<u8>> file_content = read_whole_file_binary(texture->m_path);
    if (file_content.first) {
        return file_content.first;
    }

    vec<u8> data = file_content.second;

    if (data.size() < 18) {
        return Error("Invalid TGA file format, file is too small");
    }

    texture->m_width = data[13] << 8 | data[12];
    texture->m_height = data[15] << 8 | data[14];
    texture->m_depth = data[16];

    if (texture->m_depth == 24) {
        texture->m_channels = 3;
    } else if (texture->m_depth == 32) {
        texture->m_channels = 4;
    }

    texture->m_data.resize(texture->m_width * texture->m_height * texture->m_channels);

    size_t pixel_data_offset = 18;
    for (size_t i = 0; i < texture->m_width * texture->m_height * texture->m_channels;
         i += texture->m_channels) {
        u8 b, g, r, a;
        b = data[pixel_data_offset++];
        g = data[pixel_data_offset++];
        r = data[pixel_data_offset++];
        if (texture->m_channels == 4) {
            a = data[pixel_data_offset++];
            (texture->m_data)[i + 0] = r;
            (texture->m_data)[i + 1] = g;
            (texture->m_data)[i + 2] = b;
            (texture->m_data)[i + 3] = a;
        } else if (texture->m_channels == 3) {
            (texture->m_data)[i + 0] = r;
            (texture->m_data)[i + 1] = g;
            (texture->m_data)[i + 2] = b;
        }
    }
    return Error();
}

}  // namespace blaz
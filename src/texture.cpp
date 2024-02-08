#include "texture.h"

#include <cstdio>
#include <fstream>
#include <iosfwd>
#include "types.h"
#include "error.h"

namespace blaz {

//@note remove this file read and use memory instead
Error Texture::load_data_from_tga_file(str texture_path) {
    std::ifstream texture_file(texture_path, std::ios::binary);
    if (!texture_file.is_open()) {
        return Error("Can't open file " + texture_path);
    }

    u8 header[18];
    texture_file.read((char*)header, 18);

    m_width = header[13] << 8 | header[12];
    m_height = header[15] << 8 | header[14];
    m_depth = header[16];

    if (m_depth == 24) {
        m_channels = 3;
    } else if (m_depth == 32) {
        m_channels = 4;
    }

    m_data = new vec<u8>;
    m_data->resize(m_width * m_height * m_channels);

    for (u32 i = 0; i < m_width * m_height * m_channels; i += m_channels) {
        u8 b, g, r, a;
        texture_file.read((char*)&b, 1);
        texture_file.read((char*)&g, 1);
        texture_file.read((char*)&r, 1);
        if (m_channels == 4) {
            texture_file.read((char*)&a, 1);
            (*m_data)[i + 0] = r;
            (*m_data)[i + 1] = g;
            (*m_data)[i + 2] = b;
            (*m_data)[i + 3] = a;
        } else if (m_channels == 3) {
            (*m_data)[i + 0] = r;
            (*m_data)[i + 1] = g;
            (*m_data)[i + 2] = b;
        }
    }

    return Error();
}

Error Texture::free_data() {
    delete m_data;
    return Error();
}

}  // namespace blaz
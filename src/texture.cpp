#include "texture.h"

#include "error.h"
#include "filesystem.h"
#include "renderer.h"
#include "types.h"

namespace blaz {

Error load_texture_data_from_file(Texture* texture) {
    // @todo file format check here
    return load_texture_data_from_tga_file(texture);
}

Error load_texture_data_from_tga_file(Texture* texture) {
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
    texture->m_channels = data[16] / 8;

    texture->m_data.resize(texture->m_width * texture->m_height * texture->m_channels);

    size_t pixel_data_offset = 18;
    for (size_t i = 0; i < texture->m_width * texture->m_height * texture->m_channels;
         i += texture->m_channels) {
        if (texture->m_channels == 1) {
            u8 r = data[pixel_data_offset++];
            (texture->m_data)[i + 0] = r;
        } else if (texture->m_channels == 2) {
            u8 g = data[pixel_data_offset++];
            u8 r = data[pixel_data_offset++];
            (texture->m_data)[i + 0] = r;
            (texture->m_data)[i + 1] = g;

        } else if (texture->m_channels == 3) {
            u8 b = data[pixel_data_offset++];
            u8 g = data[pixel_data_offset++];
            u8 r = data[pixel_data_offset++];
            (texture->m_data)[i + 0] = r;
            (texture->m_data)[i + 1] = g;
            (texture->m_data)[i + 2] = b;

        } else if (texture->m_channels == 4) {
            u8 b = data[pixel_data_offset++];
            u8 g = data[pixel_data_offset++];
            u8 r = data[pixel_data_offset++];
            u8 a = data[pixel_data_offset++];
            (texture->m_data)[i + 0] = r;
            (texture->m_data)[i + 1] = g;
            (texture->m_data)[i + 2] = b;
            (texture->m_data)[i + 3] = a;
        }
    }
    return Error();
}

Error load_texture_data_from_hdr_file(Texture* texture) {
    std::pair<Error, vec<u8>> file_content = read_whole_file_binary(texture->m_path);
    if (file_content.first) {
        return file_content.first;
    }
    vec<u8> data = file_content.second;

    if (memcmp(data.data(), "#?RADIANCE", 10) != 0) {
        return Error("Not a Radiance HDR file");
    }

    size_t res_line_start = 0;
    for (size_t i = 10; i < data.size() - 10; ++i) {
        if (data[i] == '-' && data[i+1] == 'Y' &&
            strstr(reinterpret_cast<const char*>(&data[i]), "+X")) {
            res_line_start = i;
            break;
        }
    }

    if (res_line_start == 0) {
        return Error("Could not find resolution line");
    }

    sscanf(reinterpret_cast<const char*>(&data[res_line_start]),
           "-Y %u +X %u", &texture->m_height, &texture->m_width);

    texture->m_channels = 3;
    texture->m_depth = 32;
    texture->m_data.resize(texture->m_width * texture->m_height * texture->m_channels);

    size_t pixel_data_start = res_line_start;
    while (pixel_data_start < data.size() && data[pixel_data_start] != '\n') {
        ++pixel_data_start;
    }
    ++pixel_data_start;

    for (u32 i = 0; i < texture->m_width * texture->m_height; ++i) {
        u8* rgbe = const_cast<u8*>(&data[pixel_data_start + i * 4]);
        u8* rgb = &texture->m_data[i * 3];

        if (rgbe[3]) {
            float scale = std::ldexp(1.0, rgbe[3] - 128);
            rgb[0] = static_cast<u8>(rgbe[0] * scale);
            rgb[1] = static_cast<u8>(rgbe[1] * scale);
            rgb[2] = static_cast<u8>(rgbe[2] * scale);
        } else {
            rgb[0] = rgb[1] = rgb[2] = 0;
        }
    }

    return Error();
}

}  // namespace blaz
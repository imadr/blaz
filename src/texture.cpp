#include "texture.h"

#include "error.h"
#include "filesystem.h"
#include "renderer.h"
#include "types.h"

namespace blaz {

Error load_texture_data_from_file(Texture* texture) {
    size_t dot_pos = texture->m_path.find_last_of('.');
    str extension = texture->m_path.substr(dot_pos + 1);
    if (extension == "tga") {
        return load_texture_data_from_tga_file(texture);
    } else if (extension == "hdr") {
        return load_texture_data_from_hdr_file(texture);
    } else {
        return Error("Unsupported texture file format ." + extension);
    }
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

    vec<str> header = {""};
    u32 blank_line = 0;
    size_t pointer = 0;
    u32 line = 0;
    while (blank_line < 2) {
        u8 c = data[pointer];
        pointer++;
        if (c == '\n') {
            if (blank_line) {
                break;
            } else if (data[pointer] == '\n') {
                blank_line++;
                pointer++;
            }
            line++;
            header.push_back("");
        } else {
            header[line] += c;
        }
    }

    if (header[0] != "#?RADIANCE") {
        return Error("Not a Radiance HDR file");
    }

    std::unordered_map<str, str> header_params;

    for (const str& line : header) {
        size_t equals_Pos = line.find('=');
        if (equals_Pos != str::npos) {
            str key = line.substr(0, equals_Pos);
            str value = line.substr(equals_Pos + 1);
            header_params[key] = value;
        }
    }

    if (header_params["FORMAT"] != "32-bit_rle_rgbe") {
        return Error("Unknown format");
    }

    texture->m_channels = 3;
    texture->m_depth = 32;

    sscanf(header.back().c_str(), "-Y %u +X %u", &texture->m_height, &texture->m_width);
    texture->m_float_data.resize(texture->m_width * texture->m_height * texture->m_channels);

    for (size_t y = 0; y < texture->m_height; y++) {
        u8 byte_0 = data[pointer++];
        u8 byte_1 = data[pointer++];
        u8 byte_2 = data[pointer++];

        if (byte_0 == 2 && byte_1 == 2 && (byte_2 & 0x80) == 0) {
            u8 byte_3 = data[pointer++];
            u32 run_length = byte_2 << 8 | byte_3;

            if (run_length != texture->m_width) {
                return Error("Invalid run length");
            }

            vec<u8> scanline(4 * texture->m_width);
            for (size_t channel = 0; channel < 4; ++channel) {
                size_t i = 0;
                while (i < texture->m_width) {
                    u8 count = data[pointer++];
                    if (count > 128) {
                        count -= 128;
                        u8 value = data[pointer++];
                        if (count == 0 || count > (texture->m_width - i)) {
                            return Error("Corrupted RLE data");
                        }

                        for (size_t z = 0; z < count; ++z) {
                            scanline[i * 4 + channel] = value;
                            i++;
                        }
                    } else {
                        if (count == 0 || count > (texture->m_width - i)) {
                            return Error("Corrupted RLE data");
                        }

                        for (size_t z = 0; z < count; ++z) {
                            scanline[i * 4 + channel] = data[pointer++];
                            i++;
                        }
                    }
                }
            }

            for (size_t x = 0; x < texture->m_width; x++) {
                u8 r = scanline[x * 4 + 0];
                u8 g = scanline[x * 4 + 1];
                u8 b = scanline[x * 4 + 2];
                u8 e = scanline[x * 4 + 3];

                size_t pixel_index = (y * texture->m_width + x) * 3;
                f32 scale = ldexpf(1.0f, e - 128.0f);

                (texture->m_float_data)[pixel_index + 0] = (r / 255.0f) * scale;
                (texture->m_float_data)[pixel_index + 1] = (g / 255.0f) * scale;
                (texture->m_float_data)[pixel_index + 2] = (b / 255.0f) * scale;
            }
        } else {
            pointer -= 3;
            for (size_t x = 0; x < texture->m_width; x++) {
                u8 byte_0 = data[pointer++];
                u8 byte_1 = data[pointer++];
                u8 byte_2 = data[pointer++];
                u8 byte_3 = data[pointer++];
                f32 r = byte_0;
                f32 g = byte_1;
                f32 b = byte_2;
                f32 e = byte_3;
                size_t pixel_index = (y * texture->m_width + x) * 3;
                f32 scale = ldexpf(1.0f, e - 128.0f);
                (texture->m_float_data)[pixel_index + 0] = (r / 255.0f) * scale;
                (texture->m_float_data)[pixel_index + 1] = (g / 255.0f) * scale;
                (texture->m_float_data)[pixel_index + 2] = (b / 255.0f) * scale;
            }
        }
    }

    return Error();
}

}  // namespace blaz

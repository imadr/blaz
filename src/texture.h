#pragma once

#include "color.h"
#include "error.h"
#include "types.h"

namespace blaz {

enum class TextureFormat {
    RGB8,
    RGBA8,
    DEPTH32,
    DEPTH32F,
};

enum class TextureWrapMode {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

enum class TextureFilteringMode {
    NEAREST,
    LINEAR,
};

struct TextureParams {
    TextureFormat m_format = TextureFormat::RGBA8;
    TextureWrapMode m_wrap_mode_s = TextureWrapMode::REPEAT;
    TextureWrapMode m_wrap_mode_t = TextureWrapMode::REPEAT;
    RGBA m_clamp_to_border_color = RGBA(0, 0, 0, 0);
    TextureFilteringMode m_filter_mode_min = TextureFilteringMode::LINEAR;
    TextureFilteringMode m_filter_mode_mag = TextureFilteringMode::LINEAR;
};

struct Texture {
    str m_name;
    vec<u8>* m_data = NULL;
    u32 m_width = 0;
    u32 m_height = 0;
    u8 m_depth = 0;
    u8 m_channels = 0;
    TextureParams m_texture_params;

    void* m_api_data = NULL;

    Error load_data_from_tga_file(str texture_path);
    Error free_data();
};

}  // namespace blaz
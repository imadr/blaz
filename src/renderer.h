#pragma once

#include "camera.h"
#include "color.h"
#include "error.h"
#include "mesh.h"
#include "platform.h"
#include "texture.h"
#include "types.h"

namespace blaz {

enum Clear {
    NONE = 0,
    COLOR = 1 << 0,
    DEPTH = 1 << 1,
};

static std::unordered_map<str, Clear> ClearStr = {
    {"NONE", Clear::NONE},
    {"COLOR", Clear::COLOR},
    {"DEPTH", Clear::DEPTH},
};

enum class CullingMode {
    BACK,
    FRONT,
};

static std::unordered_map<str, CullingMode> CullingModeStr = {
    {"BACK", CullingMode::BACK},
    {"FRONT", CullingMode::FRONT},
};

enum class CullingOrder {
    CCW,
    CW,
};

static std::unordered_map<str, CullingOrder> CullingOrderStr = {
    {"CCW", CullingOrder::CCW},
    {"CW", CullingOrder::CW},
};

enum class MeshPrimitive {
    TRIANGLES,
    LINES,
};

static std::unordered_map<str, MeshPrimitive> MeshPrimitiveStr = {
    {"TRIANGLES", MeshPrimitive::TRIANGLES},
    {"LINES", MeshPrimitive::LINES},
};

enum class TextureFormat {
    R8,
    RG8,
    RGB8,
    RGBA8,
    RGB32F,
    RGBA32F,
    DEPTH32,
    DEPTH32F,
};

static std::unordered_map<str, TextureFormat> TextureFormatStr = {
    {"R8", TextureFormat::R8},           {"RG8", TextureFormat::RG8},
    {"RGB8", TextureFormat::RGB8},       {"RGBA8", TextureFormat::RGBA8},
    {"RGB32F", TextureFormat::RGB32F},   {"RGBA32F", TextureFormat::RGBA32F},
    {"DEPTH32", TextureFormat::DEPTH32}, {"DEPTH32F", TextureFormat::DEPTH32F},
};

enum class TextureWrapMode {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
};

static std::unordered_map<str, TextureWrapMode> TextureWrapModeStr = {
    {"REPEAT", TextureWrapMode::REPEAT},
    {"MIRRORED_REPEAT", TextureWrapMode::MIRRORED_REPEAT},
    {"CLAMP_TO_EDGE", TextureWrapMode::CLAMP_TO_EDGE},
};

enum class TextureFilteringMode {
    NEAREST,
    LINEAR,
};

static std::unordered_map<str, TextureFilteringMode> TextureFilteringModeStr = {
    {"NEAREST", TextureFilteringMode::NEAREST},
    {"LINEAR", TextureFilteringMode::LINEAR},
};

enum UniformType {
    UNIFORM_MAT4,
    UNIFORM_VEC4,
    UNIFORM_VEC3,
    UNIFORM_VEC2,
    UNIFORM_FLOAT,
    UNIFORM_BOOL,
    UNIFORM_UINT,
    UNIFORM_INT,
};

static std::unordered_map<UniformType, u32> UniformTypeAlignment = {
    {UNIFORM_MAT4, 16}, {UNIFORM_VEC4, 16}, {UNIFORM_VEC3, 16}, {UNIFORM_VEC2, 8},
    {UNIFORM_FLOAT, 4}, {UNIFORM_BOOL, 4},  {UNIFORM_UINT, 4},  {UNIFORM_INT, 4},
};

static std::unordered_map<UniformType, u32> UniformTypeSize = {
    {UNIFORM_MAT4, 64}, {UNIFORM_VEC4, 16}, {UNIFORM_VEC3, 12}, {UNIFORM_VEC2, 8},
    {UNIFORM_FLOAT, 4}, {UNIFORM_BOOL, 4},  {UNIFORM_UINT, 4},  {UNIFORM_INT, 4},
};

struct TextureParams {
    TextureFormat m_format = TextureFormat::RGBA8;
    TextureWrapMode m_wrap_mode_s = TextureWrapMode::REPEAT;
    TextureWrapMode m_wrap_mode_t = TextureWrapMode::REPEAT;
    TextureFilteringMode m_filter_mode_min = TextureFilteringMode::LINEAR;
    TextureFilteringMode m_filter_mode_mag = TextureFilteringMode::LINEAR;
};

struct Framebuffer {
    str m_name;
    str m_color_attachment_texture;
    str m_depth_attachment_texture;
    str m_stencil_attachment_texture;
    void* m_api_data = NULL;
};

enum class ShaderType { VERTEX_FRAGMENT, COMPUTE };

enum class UniformBindingType { BLOCK, SAMPLER, IMAGE };

struct UniformBinding {
    u32 m_binding_point;
    UniformBindingType m_type;
};

struct Shader {
    str m_name;
    ShaderType m_type = ShaderType::VERTEX_FRAGMENT;
    str m_vertex_shader_source;
    str m_fragment_shader_source;
    str m_compute_shader_source;
    str m_vertex_shader_path;
    str m_fragment_shader_path;
    str m_compute_shader_path;
    bool m_is_error = false;
    std::unordered_map<str, UniformBinding> m_uniform_bindings;
    void* m_api_data = NULL;
    bool m_should_reload = true;
};

struct Uniform {
    str m_name;
    UniformType m_type;
    u32 m_offset;
    u32 m_size;
};

struct UniformBuffer {
    str m_name;
    u32 m_size;
    vec<Uniform> m_uniforms;
    std::unordered_map<str, u32> m_uniforms_ids;
    void* m_api_data = NULL;
    bool m_should_reload = true;
};

struct Mesh {
    str m_name;
    str m_path;
    vec<f32> m_vertices;
    vec<u32> m_indices;
    vec<pair<str, u32>> m_attribs;
    MeshPrimitive m_primitive = MeshPrimitive::TRIANGLES;
    void* m_api_data = NULL;
    bool m_should_reload = true;
};

struct Texture {
    str m_name;
    str m_path;
    vec<u8> m_data;
    u32 m_width = 0;
    u32 m_height = 0;
    u8 m_depth = 0;
    u8 m_channels = 0;
    TextureParams m_texture_params;
    void* m_api_data = NULL;
    bool m_should_reload = true;
    bool m_resize_to_viewport = false;
};

struct Renderable {
    str m_name;
    vec<str> m_tags;
    str m_material;
    str m_mesh;
    str m_node;
};

enum class PassType { RENDER, COPY, COMPUTE };

static std::unordered_map<str, PassType> PassTypeStr = {
    {"RENDER", PassType::RENDER},
    {"COPY", PassType::COPY},
    {"COMPUTE", PassType::COMPUTE},
};

using IntOrStr = std::variant<u32, str>;

enum class AccessType { READ_ONLY, WRITE_ONLY, READ_WRITE };

static std::unordered_map<str, AccessType> AccessTypeStr = {
    {"READ_ONLY", AccessType::READ_ONLY},
    {"WRITE_ONLY", AccessType::WRITE_ONLY},
    {"READ_WRITE", AccessType::READ_WRITE},
};

struct Pass {
    str m_name;
    PassType m_type;
    u32 m_clear_flag = 0;
    RGBA m_clear_color = RGBA(0, 0, 0, 1);
    float m_clear_depth = 1.0;
    str m_shader;
    vec<str> m_tags;
    str m_framebuffer;
    str m_camera;
    bool m_enabled = true;
    bool m_use_default_framebuffer = true;
    bool m_enable_depth_test = true;
    bool m_enable_face_culling = true;
    CullingMode m_culling_mode = CullingMode::BACK;
    CullingOrder m_culling_order = CullingOrder::CCW;
    bool m_bufferless_draw = false;
    u32 m_bufferless_draw_count = 0;
    std::unordered_map<str, str> m_sampler_uniforms_bindings;
    std::unordered_map<str, pair<str, AccessType>> m_image_uniforms_bindings;
    IntOrStr m_compute_work_groups[3];
    str m_copy_src_texture;
    str m_copy_dst_texture;
};

using UniformValue = std::variant<Mat4, Vec4, Vec3, Vec2, f32, bool, u32, i32>;

enum class ImDrawCommandType { RECT };

struct {
    ImDrawCommandType m_type;
    Vec2 m_pos;
    Vec2 m_size;
    RGB m_col;
};

struct ImDraw {
    vec<ImDrawCommand> m_commands;

    void start() {
        m_commands.clear();
    }

    void draw_rect(Vec2 pos, Vec2 size, RGB col) {
        m_commands.push_back(ImDrawCommand(.m_type = ImDrawCommandType::RECT, .m_pos = pos,
                                           .m_size = size, .m_col = col));
    }

    void end() {
        u32 counter = 0;
        for (auto& command : m_commands) {
            if (command.m_type == ImDrawCommandType::RECT) {
                Mesh mesh;
                float x = cmd.m_pos.x;
                float y = cmd.m_pos.y;
                float w = cmd.m_size.x;
                float h = cmd.m_size.y;

                mesh->m_vertices.insert(mesh->m_vertices.end(),
                                        {{x, y, cmd.m_col.r, cmd.m_col.g, cmd.m_col.b},
                                         {x + w, y, cmd.m_col.r, cmd.m_col.g, cmd.m_col.b},
                                         {x + w, y + h, cmd.m_col.r, cmd.m_col.g, cmd.m_col.b},
                                         {x, y + h, cmd.m_col.r, cmd.m_col.g, cmd.m_col.b}});

                mesh->m_indices.insert(mesh->m_indices.end(),
                                       {index, index + 1, index + 2, index, index + 2, index + 3});

                mesh->m_attribs = {
                    {"position", 2},
                    {"color", 3},
                };
                mesh->m_primitive = MeshPrimitive::TRIANGLES;
                mesh->m_name = "IMDRAW_RECT_" + str(counter) + "_MESH";

                Node node;
                node.m_name = "IMDRAW_RECT_" + str(counter) + "_NODE";
                node.m_parent = "root_node";

                Renderable renderable;
                renderable.m_name = "IMDRAW_RECT_" + str(counter) + "_RENDERABLE";
                renderable.m_tag = "imdraw";
                renderable.m_mesh = mesh->m_name;
                renderable.m_node = node->m_name;
            }
        }
    }
};

struct Renderer {
    Window* m_window = NULL;
    Scene* m_current_scene = NULL;

    ImDraw imdraw;

    u32 m_frame_number = 1;

    Error init(Window* window);
    Error init_api();
    void update();
    void clear(u32 clear_flag, RGBA clear_color, float clear_depth);
    void present();
    void draw(MeshPrimitive primitive, size_t count);
    void draw_indexed(MeshPrimitive primitive, size_t count);
    void dispatch_compute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z);
    void set_swap_interval(u32 interval);
    void copy_texture(str src, str dst, Vec3I src_pos, Vec2I src_size, Vec3I dst_pos);

    void debug_marker_start(str name);
    void debug_marker_end();

    void set_viewport(u32 x, u32 y, u32 width, u32 height);
    void set_depth_test(bool enabled);
    void set_face_culling(bool enabled, CullingMode mode, CullingOrder order);

    vec<Pass> m_passes;

    ArrayMap<Shader> m_shaders;
    Error create_shader(Shader shader);
    Error create_shader_api(str shader_id);
    Error reload_shader(str shader_id);
    Error reload_shader_api(str shader_id);
    void set_current_shader(str shader_id);

    ArrayMap<Mesh> m_meshes;
    Error create_mesh(Mesh mesh);
    Error create_mesh_api(str mesh_id);
    Error reload_mesh(str mesh_id);
    Error reload_mesh_api(str mesh_id);
    void set_current_mesh(str mesh_id);
    void set_bufferless_mesh();

    ArrayMap<Framebuffer> m_framebuffers;
    Error create_framebuffer(Framebuffer framebuffer);
    Error create_framebuffer_api(str framebuffer_id);
    void set_current_framebuffer(str framebuffer_id);
    void set_default_framebuffer();
    Error attach_texture_to_framebuffer(str framebuffer_id);

    ArrayMap<Texture> m_textures;
    Error create_texture(Texture texture);
    Error create_texture_api(str texture_id);
    Error reload_texture(str texture_id);
    Error reload_texture_api(str texture_id);

    ArrayMap<Camera> m_cameras;
    Error create_camera(Camera camera);

    vec<Renderable> m_renderables;
    std::unordered_map<str, vec<u32>> m_tagged_renderables;
    void create_renderable(Renderable renderable);

    ArrayMap<UniformBuffer> m_uniform_buffers;
    Error create_uniform_buffer(UniformBuffer uniform_buffer);
    Error create_uniform_buffer_api(str uniform_buffer_id);
    Error set_uniform_buffer_data(str uniform_buffer_id,
                                  vec<pair<str, UniformValue>> uniform_values);

    void bind_uniforms(Pass* pass, str shader_id);

    u32 special_value(const str& name);
};

}  // namespace blaz
#pragma once

#include <unordered_map>
#include <variant>

#include "camera.h"
#include "color.h"
#include "error.h"
#include "platform.h"
#include "texture.h"
#include "types.h"

namespace blaz {

struct Game;

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

struct Framebuffer {
    str m_name;
    u32 m_width;
    u32 m_height;
    void* m_api_data = NULL;
    TextureParams m_texture_params;
};

struct Shader {
    str m_name;
    str m_vertex_shader_source;
    str m_fragment_shader_source;
    str m_vertex_shader_path;
    str m_fragment_shader_path;
    void* m_api_data = NULL;
    bool m_should_reload = true;
    bool m_is_error = false;
};

enum UniformType {
    UNIFORM_MAT4,
    UNIFORM_VEC4,
    UNIFORM_VEC3,
    UNIFORM_FLOAT,
    UNIFORM_BOOL,
};

static std::unordered_map<UniformType, u32> UniformTypeAlignment = {
    {UNIFORM_MAT4, 64}, {UNIFORM_VEC4, 16}, {UNIFORM_VEC3, 16},
    {UNIFORM_FLOAT, 4}, {UNIFORM_BOOL, 4},
};

static std::unordered_map<UniformType, u32> UniformTypeSize = {
    {UNIFORM_MAT4, 64}, {UNIFORM_VEC4, 16}, {UNIFORM_VEC3, 16},
    {UNIFORM_FLOAT, 4}, {UNIFORM_BOOL, 4},
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
    u32 m_binding_point;
    vec<Uniform> m_uniforms;
    std::unordered_map<str, u32> m_uniforms_ids;
    bool m_should_reload = true;
    void* m_api_data = NULL;
};

struct Mesh {
    str m_name;
    vec<f32> m_vertices;
    vec<u32> m_indices;
    vec<pair<str, u32>> m_attribs;
    void* m_api_data = NULL;
    bool m_should_reload = true;
};

struct Material {
    //     str m_shader;
    //     //@note add some uniforms values here and textures
};

struct Renderable {
    str m_name;
    vec<str> m_tags;
    u32 m_material;
    u32 m_mesh;
    u32 m_node;
};

enum class PassType { VERT_FRAG, BLITTING, COMPUTE };

static std::unordered_map<str, PassType> PassTypeStr = {
    {"VERT_FRAG", PassType::VERT_FRAG},
    {"BLITTING", PassType::BLITTING},
    {"COMPUTE", PassType::COMPUTE},
};

struct Pass {
    str m_name;
    PassType m_type;
    u32 m_clear_flag;
    RGBA m_clear_color = RGBA(0, 0, 0, 1);
    float m_clear_depth = 1.0;
    u32 m_shader;
    vec<str> m_tags;
    u32 m_framebuffer;
    u32 m_camera;
    bool m_enabled = true;
    bool m_use_default_framebuffer = true;
    bool m_enable_depth_test = true;
    bool m_enable_face_culling = true;
    CullingMode m_culling_mode = CullingMode::BACK;
    CullingOrder m_culling_order = CullingOrder::CCW;
};

struct Pipeline {
    str m_name;
    vec<Pass> m_passes;
    vec<Framebuffer> m_framebuffers;
};

struct Game;

using UniformValue = std::variant<Mat4, Vec4, Vec3, f32, bool>;

struct Renderer {
    Game* m_game;
    Scene* m_current_scene;

    Error init(Game* game);
    Error init_api();
    void draw();
    void draw_pass(u32 pass);

    vec<Pipeline> m_pipelines;
    u32 m_current_pipeline;
    std::unordered_map<str, u32> m_pipelines_ids;
    vec<Shader> m_shaders;
    std::unordered_map<str, u32> m_shaders_ids;
    Shader m_error_shader;
    vec<Mesh> m_meshes;
    std::unordered_map<str, u32> m_meshes_ids;
    vec<Camera> m_cameras;
    std::unordered_map<str, u32> m_cameras_ids;
    vec<UniformBuffer> m_uniform_buffers;
    std::unordered_map<str, u32> m_uniform_buffers_ids;

    vec<Renderable> m_renderables;
    std::unordered_map<str, vec<u32>> m_tagged_renderables;

    void clear(u32 clear_flag, RGBA clear_color, float clear_depth);
    void present();
    void set_swap_interval(u32 interval);

    Error reload_shader(Shader* shader);
    Error compile_shader(Shader* shader);
    void bind_shader(Shader* shader);
    Error upload_mesh(Mesh* mesh);
    void debug_marker_start(str name);
    void debug_marker_end();
    void bind_default_framebuffer();
    void set_viewport(u32 x, u32 y, u32 width, u32 height);
    void bind_framebuffer(Framebuffer* framebuffer);
    void set_depth_test(bool enabled);
    void set_face_culling(bool enabled, CullingMode mode, CullingOrder order);
    void draw_vertex_array(Mesh* mesh);
    Error set_uniform_buffer_data(UniformBuffer* uniform_buffer, str uniform_name,
                                  UniformValue value);
    Error init_uniform_buffer(UniformBuffer* uniform_buffer);

    void add_mesh(Mesh mesh);
    void add_renderable(Renderable renderable);
    void add_camera(Camera camera);
    void add_uniform_buffer(UniformBuffer buffer);
};

}  // namespace blaz
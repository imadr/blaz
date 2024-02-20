#pragma once

#include "camera.h"
#include "color.h"
#include "error.h"
#include "mesh.h"
#include "platform.h"
#include "shader.h"
#include "texture.h"
#include "types.h"

#include <unordered_map>

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

struct Framebuffer {
    str m_name;
    u32 m_width;
    u32 m_height;
    void* m_api_data = NULL;
    TextureParams m_texture_params;
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
    { "VERT_FRAG", PassType::VERT_FRAG },
    { "BLITTING", PassType::BLITTING },
    { "COMPUTE", PassType::COMPUTE },
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
    // std::unordered_map<str, str> m_textures_bindings;
};

struct Pipeline {
    vec<u32> m_passes;
    vec<u32> m_framebuffers;
};

struct Game;

struct Renderer {
    Window* m_window;

    Error init(Window* window);
    void draw();
    void draw_pass(u32 pass);

    Pipeline m_pipelines[10];
    u32 n_pipelines = 0;
    Shader m_shaders[10];
    Shader m_error_shader;
    Texture m_textures[10];
    Material m_materials[10];
    Mesh m_meshes[10];
    Framebuffer m_framebuffers[10];
    Pass m_passes[10];
    Camera m_cameras[10];

    Pipeline* m_current_pipeline = NULL;

    // Error compile_shader(u32 shader);

    // Error set_shader_uniform_Mat4(Shader* shader, str uniform_name, Mat4 value);
    // Error set_shader_uniform_i32(Shader* shader, str uniform_name, i32 value);
    // Error set_shader_uniform_Vec2(Shader* shader, str uniform_name, Vec2 value);
    // Error set_shader_uniform_Vec3(Shader* shader, str uniform_name, Vec3 value);

    // Error upload_mesh(Mesh* mesh);
    // Error reupload_mesh_buffers(Mesh* mesh);
    // // Error delete_mesh(Mesh* mesh); @note add this

    // Error create_texture(Texture* texture);
    // Error upload_texture_data(Texture* texture);
    // Error delete_texture(Texture* texture);

    // Error create_framebuffer(Framebuffer* framebuffer);
    // Error delete_framebuffer(Framebuffer* framebuffer);

    void clear(u32 clear_flag, RGBA clear_color, float clear_depth);
    void present();

    void bind_shader(Shader& shader);
    void debug_marker_start(str name);
    void debug_marker_end();
    void bind_default_framebuffer();
    void set_viewport(u32 x, u32 y, u32 width, u32 height);
    void bind_framebuffer(u32 framebuffer);
    void set_depth_test(bool enabled);
    void set_face_culling(bool enabled);
    void set_face_culling_mode(CullingMode mode);
    // void bind_textures(Pass* pass, Shader* shader);
    // void draw_vertex_array(Mesh* mesh);
    // void draw_pass(Pass* pass);
};

}  // namespace blaz
#include <GLES3/gl3.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl2ext.h>
#include <emscripten/html5.h>

#include <unordered_map>
#include <utility>

#include "color.h"
#include "error.h"
#include "game.h"
#include "platform.h"
#include "renderer.h"
#include "texture.h"
#include "types.h"

namespace blaz {

struct Mesh_OPENGL {
    GLuint m_vbo, m_vao, m_ebo;
};

struct UniformBuffer_OPENGL {
    GLuint m_ubo;
};

struct Shader_OPENGL {
    GLuint m_program = 0;
    GLuint m_vertex_shader = 0;
    GLuint m_fragment_shader = 0;
};

struct Texture_OPENGL {
    u32 m_texture_name;
    u32 m_format;
};

struct Framebuffer_OPENGL {
    GLuint m_fbo;
};

static std::unordered_map<TextureFormat, std::tuple<GLint, GLenum, GLenum>> opengl_texture_formats =
    {
        {TextureFormat::R8, {GL_R8, GL_RED, GL_UNSIGNED_BYTE}},
        {TextureFormat::RG8, {GL_RG8, GL_RG, GL_UNSIGNED_BYTE}},
        {TextureFormat::RGB8, {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE}},
        {TextureFormat::RGBA8, {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE}},
        {TextureFormat::RGB32F, {GL_RGB32F, GL_RGB, GL_FLOAT}},
        {TextureFormat::RGBA32F, {GL_RGBA32F, GL_RGBA, GL_FLOAT}},
        {TextureFormat::DEPTH32, {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE}},
        {TextureFormat::DEPTH32F, {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE}},
};

static std::unordered_map<TextureWrapMode, GLenum> opengl_texture_wrap_modes = {
    {TextureWrapMode::REPEAT, GL_REPEAT},
    {TextureWrapMode::MIRRORED_REPEAT, GL_MIRRORED_REPEAT},
    {TextureWrapMode::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE},
};

static std::unordered_map<AttachementPoint, GLenum> opengl_attachment_point = {
    {AttachementPoint::COLOR_ATTACHMENT, GL_COLOR_ATTACHMENT0},
    {AttachementPoint::DEPTH_ATTACHMENT, GL_DEPTH_ATTACHMENT},
    {AttachementPoint::STENCIL_ATTACHMENT, GL_STENCIL_ATTACHMENT},
};

static std::unordered_map<TextureFilteringMode, GLenum> opengl_texture_filtering_modes = {
    {TextureFilteringMode::NEAREST, GL_NEAREST},
    {TextureFilteringMode::LINEAR, GL_LINEAR},
};

static std::unordered_map<MeshPrimitive, GLenum> opengl_mesh_primitive_types = {
    {MeshPrimitive::TRIANGLES, GL_TRIANGLES},
    {MeshPrimitive::LINES, GL_LINES},
};

GLuint dummy_vao;

Error Renderer::init_api() {
    EmscriptenWebGLContextAttributes attributes;
    emscripten_webgl_init_context_attributes(&attributes);
    attributes.alpha = false;
    attributes.depth = true;
    attributes.stencil = true;
    attributes.antialias = true;
    attributes.premultipliedAlpha = false;
    attributes.preserveDrawingBuffer = false;
    attributes.powerPreference = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
    attributes.failIfMajorPerformanceCaveat = false;
    attributes.majorVersion = 2;
    attributes.minorVersion = 0;
    attributes.enableExtensionsByDefault = true;

    int context = emscripten_webgl_create_context("#canvas", &attributes);
    if (!context) {
        return Error("Webgl context could not be created");
    }

    emscripten_webgl_make_context_current(context);

    glGenVertexArrays(1, &dummy_vao);

    return Error();
}

void Renderer::clear(u32 clear_flag, RGBA clear_color, float clear_depth) {
    if (clear_flag != Clear::NONE) {
        GLbitfield gl_clear_flag = 0;
        if (clear_flag & Clear::COLOR) {
            gl_clear_flag |= GL_COLOR_BUFFER_BIT;
            glClearColor(clear_color.r(), clear_color.g(), clear_color.b(), clear_color.a());
        }
        if (clear_flag & Clear::DEPTH) {
            gl_clear_flag |= GL_DEPTH_BUFFER_BIT;
            glClearDepthf(clear_depth);
        }
        glClear(gl_clear_flag);
    }
}

void Renderer::present() {
}

void Renderer::set_swap_interval(u32 interval) {
}

Error Renderer::create_shader_api(str shader_id) {
    Shader* shader = &m_shaders[shader_id];

    if (shader->m_type == ShaderType::COMPUTE) {
        return Error("Compute shaders not available in WebGL");
    }

    GLuint shader_program;
    shader_program = glCreateProgram();
    Shader_OPENGL* api_shader = new Shader_OPENGL;
    api_shader->m_program = shader_program;

    GLuint vertex_shader;
    GLuint fragment_shader;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    api_shader->m_vertex_shader = vertex_shader;
    api_shader->m_fragment_shader = fragment_shader;

    shader->m_api_data = api_shader;

    return Error();
}

Error Renderer::reload_shader_api(str shader_id) {
    Shader* shader = &m_shaders[shader_id];
    if (shader->m_type == ShaderType::COMPUTE) {
        return Error("Compute shaders not available in WebGL");
    }

    Shader_OPENGL* api_shader = (Shader_OPENGL*)shader->m_api_data;
    int success;
    char info[512];

    const char* c_str = shader->m_vertex_shader_source.c_str();
    glShaderSource(api_shader->m_vertex_shader, 1, &c_str, NULL);
    glCompileShader(api_shader->m_vertex_shader);

    glGetShaderiv(api_shader->m_vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(api_shader->m_vertex_shader, 512, NULL, info);
        glDeleteShader(api_shader->m_vertex_shader);
        return Error("Renderer::compile_shader: Failed to compile vertex shader \"" +
                     shader->m_name + "\" : " + str(info));
    }

    c_str = shader->m_fragment_shader_source.c_str();
    glShaderSource(api_shader->m_fragment_shader, 1, &c_str, NULL);
    glCompileShader(api_shader->m_fragment_shader);
    glGetShaderiv(api_shader->m_fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(api_shader->m_fragment_shader, 512, NULL, info);
        glDeleteShader(api_shader->m_fragment_shader);
        return Error("Renderer::compile_shader: Failed to compile fragment shader \"" +
                     shader->m_name + "\" : " + str(info));
    }

    glLinkProgram(api_shader->m_program);
    glGetProgramiv(api_shader->m_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(api_shader->m_program, 512, NULL, info);
        glDeleteProgram(api_shader->m_program);
        return Error("Renderer::compile_shader: Failed to link shader program \"" + shader->m_name +
                     "\" : " + str(info));
    }

    GLint n_uniforms, max_len;
    glGetProgramiv(api_shader->m_program, GL_ACTIVE_UNIFORMS, &n_uniforms);
    glGetProgramiv(api_shader->m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_len);
    GLchar* uniform_name = (GLchar*)malloc(max_len);

    glUseProgram(api_shader->m_program);

    if (uniform_name != NULL) {
        for (i32 i = 0; i < n_uniforms; i++) {
            GLint size = -1;
            GLenum type = -1;
            glGetActiveUniform(api_shader->m_program, i, max_len, NULL, &size, &type, uniform_name);
            if (type == GL_SAMPLER_2D) {
                GLint location = glGetUniformLocation(api_shader->m_program, uniform_name);
                GLint binding_point;
                glGetUniformiv(api_shader->m_program, location, &binding_point);
                shader->m_sampler_binding_points[uniform_name] = binding_point;
            }
        }
    }

    shader->m_should_reload = false;
    return Error();
}

void Renderer::set_current_shader(str shader_id) {
    Shader* shader = &m_shaders[shader_id];
    glUseProgram(((Shader_OPENGL*)shader->m_api_data)->m_program);
}

void Renderer::set_bufferless_mesh() {
    glBindVertexArray(dummy_vao);
}

Error Renderer::create_mesh_api(str mesh_id) {
    Mesh* mesh = &m_meshes[mesh_id];

    u32 vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    Mesh_OPENGL* api_mesh = new Mesh_OPENGL;
    api_mesh->m_vbo = vbo;
    api_mesh->m_ebo = ebo;
    api_mesh->m_vao = vao;
    mesh->m_api_data = api_mesh;

    return Error();
}

Error Renderer::reload_mesh_api(str mesh_id) {
    Mesh* mesh = &m_meshes[mesh_id];
    Mesh_OPENGL* api_mesh = (Mesh_OPENGL*)mesh->m_api_data;

    glBindVertexArray(api_mesh->m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, api_mesh->m_vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(f32), mesh->m_vertices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, api_mesh->m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(u32),
                 mesh->m_indices.data(), GL_STATIC_DRAW);

    u32 attribs_stride = 0;
    for (i32 i = 0; i < mesh->m_attribs.size(); i++) {
        attribs_stride += mesh->m_attribs[i].second;
    }
    u32 attribs_offset = 0;
    for (i32 i = 0; i < mesh->m_attribs.size(); i++) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, mesh->m_attribs[i].second, GL_FLOAT, GL_FALSE,
                              attribs_stride * sizeof(f32), (void*)(attribs_offset * sizeof(f32)));
        attribs_offset += mesh->m_attribs[i].second;
    }

    mesh->m_should_reload = false;

    return Error();
}

void Renderer::set_current_mesh(str mesh_id) {
    Mesh* mesh = &m_meshes[mesh_id];
    glBindVertexArray(((Mesh_OPENGL*)mesh->m_api_data)->m_vao);
}

Error Renderer::create_framebuffer_api(str framebuffer_id) {
    Framebuffer* framebuffer = &m_framebuffers[framebuffer_id];

    u32 fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    Framebuffer_OPENGL* api_framebuffer = new Framebuffer_OPENGL;
    api_framebuffer->m_fbo = fbo;
    framebuffer->m_api_data = api_framebuffer;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Error();
}

Error Renderer::attach_texture_to_framebuffer(str framebuffer_id, str texture_id,
                                              AttachementPoint attachment_point) {
    Texture* texture = &m_textures[texture_id];
    Texture_OPENGL* api_texture = (Texture_OPENGL*)texture->m_api_data;

    Framebuffer* framebuffer = &m_framebuffers[framebuffer_id];
    Framebuffer_OPENGL* api_framebuffer = (Framebuffer_OPENGL*)framebuffer->m_api_data;

    glBindFramebuffer(GL_FRAMEBUFFER, api_framebuffer->m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, opengl_attachment_point[attachment_point], GL_TEXTURE_2D,
                           ((Texture_OPENGL*)texture->m_api_data)->m_texture_name, 0);

    if (attachment_point == AttachementPoint::COLOR_ATTACHMENT) {
        framebuffer->m_color_attachment_texture = texture_id;
    } else if (attachment_point == AttachementPoint::DEPTH_ATTACHMENT) {
        framebuffer->m_depth_attachment_texture = texture_id;
    } else if (attachment_point == AttachementPoint::STENCIL_ATTACHMENT) {
        framebuffer->m_stencil_attachment_texture = texture_id;
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return Error("Framebuffer not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Error();
}

void Renderer::set_current_framebuffer(str framebuffer_id) {
    Framebuffer* framebuffer = &m_framebuffers[framebuffer_id];
    glBindFramebuffer(GL_FRAMEBUFFER, ((Framebuffer_OPENGL*)framebuffer->m_api_data)->m_fbo);
}

void Renderer::set_default_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Error Renderer::create_uniform_buffer_api(str uniform_buffer_id) {
    UniformBuffer* uniform_buffer = &m_uniform_buffers[uniform_buffer_id];
    GLuint ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, uniform_buffer->m_size, NULL, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, uniform_buffer->m_binding_point, ubo, 0,
                      uniform_buffer->m_size);

    UniformBuffer_OPENGL* api_uniform_buffer = new UniformBuffer_OPENGL;
    api_uniform_buffer->m_ubo = ubo;
    uniform_buffer->m_api_data = api_uniform_buffer;

    return Error();
}

Error Renderer::set_uniform_buffer_data(str uniform_buffer_id, str uniform_name,
                                        UniformValue value) {
    UniformBuffer* uniform_buffer = &m_uniform_buffers[uniform_buffer_id];
    glBindBuffer(GL_UNIFORM_BUFFER, ((UniformBuffer_OPENGL*)uniform_buffer->m_api_data)->m_ubo);
    Uniform& uniform = uniform_buffer->m_uniforms[uniform_buffer->m_uniforms_ids[uniform_name]];
    glBufferSubData(GL_UNIFORM_BUFFER, uniform.m_offset, uniform.m_size, &value);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return Error();
}

Error Renderer::create_texture_api(str texture_id) {
    Texture* texture = &m_textures[texture_id];

    u32 texture_name;
    glGenTextures(1, &texture_name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    opengl_texture_wrap_modes[texture->m_texture_params.m_wrap_mode_s]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    opengl_texture_wrap_modes[texture->m_texture_params.m_wrap_mode_t]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    opengl_texture_filtering_modes[texture->m_texture_params.m_filter_mode_min]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    opengl_texture_filtering_modes[texture->m_texture_params.m_filter_mode_mag]);

    glBindTexture(GL_TEXTURE_2D, texture_name);
    auto& texture_type = opengl_texture_formats[texture->m_texture_params.m_format];
    glTexImage2D(GL_TEXTURE_2D, 0, get<0>(texture_type), texture->m_width, texture->m_height, 0,
                 get<1>(texture_type), get<2>(texture_type), NULL);

    Texture_OPENGL* api_texture = new Texture_OPENGL;
    api_texture->m_texture_name = texture_name;
    texture->m_api_data = api_texture;

    return Error();
}

Error Renderer::reload_texture_api(str texture_id) {
    Texture* texture = &m_textures[texture_id];
    Texture_OPENGL* api_texture = (Texture_OPENGL*)texture->m_api_data;

    glBindTexture(GL_TEXTURE_2D, ((Texture_OPENGL*)texture->m_api_data)->m_texture_name);
    auto& texture_type = opengl_texture_formats[texture->m_texture_params.m_format];
    glTexImage2D(GL_TEXTURE_2D, 0, get<0>(texture_type), texture->m_width, texture->m_height, 0,
                 get<1>(texture_type), GL_UNSIGNED_BYTE, texture->m_data.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    texture->m_should_reload = false;

    return Error();
}

void Renderer::set_samplers_bindings(Pass* pass, Shader* shader) {
    for (const auto& sampler_uniform : pass->m_sampler_uniforms_bindings) {
        GLint sampler_binding_point = shader->m_sampler_binding_points[sampler_uniform.first];
        if (sampler_binding_point == 0) {
            continue;
        }
        glActiveTexture(GL_TEXTURE0 + sampler_binding_point);
        Texture* texture = &m_textures[sampler_uniform.second];
        glBindTexture(GL_TEXTURE_2D, ((Texture_OPENGL*)texture->m_api_data)->m_texture_name);
    }
}

void Renderer::set_images_bindings(Pass* pass, Shader* shader) {
    logger.error("Compute shaders not available in WebGL");
}

void Renderer::debug_marker_start(str name) {
}

void Renderer::debug_marker_end() {
}

void Renderer::set_viewport(u32 x, u32 y, u32 width, u32 height) {
    glViewport(x, y, width, height);
}

void Renderer::set_depth_test(bool enabled) {
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void Renderer::set_face_culling(bool enabled, CullingMode mode, CullingOrder order) {
    if (enabled) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    if (mode == CullingMode::BACK) {
        glCullFace(GL_BACK);
    } else if (mode == CullingMode::FRONT) {
        glCullFace(GL_FRONT);
    }

    if (order == CullingOrder::CCW) {
        glFrontFace(GL_CCW);
    } else if (order == CullingOrder::CW) {
        glFrontFace(GL_CW);
    }
}

void Renderer::draw(MeshPrimitive primitive, size_t count) {
    glDrawArrays(opengl_mesh_primitive_types[primitive], 0, GLsizei(count));
}

void Renderer::draw_indexed(MeshPrimitive primitive, size_t count) {
    glDrawElements(opengl_mesh_primitive_types[primitive], GLsizei(count), GL_UNSIGNED_INT, 0);
}

void Renderer::dispatch_compute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z) {
    logger.error("Compute shaders not available in WebGL");
}

}  // namespace blaz
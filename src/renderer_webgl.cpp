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
};

struct Texture_OPENGL {
    u32 m_texture_name;
    u32 m_format;
};

struct Framebuffer_OPENGL {
    GLuint m_fbo;
};

static std::unordered_map<TextureFormat, std::pair<GLint, GLenum>> opengl_texture_formats = {
    {TextureFormat::RGB8, {GL_RGB, GL_RGB}},
    {TextureFormat::RGBA8, {GL_RGBA, GL_RGBA}},
    {TextureFormat::DEPTH32, {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT}},
};

static std::unordered_map<TextureWrapMode, GLenum> opengl_texture_wrap_modes = {
    {TextureWrapMode::REPEAT, GL_REPEAT},
    {TextureWrapMode::MIRRORED_REPEAT, GL_MIRRORED_REPEAT},
    {TextureWrapMode::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE},
};

static std::unordered_map<TextureFilteringMode, GLenum> opengl_texture_filtering_modes = {
    {TextureFilteringMode::NEAREST, GL_NEAREST},
    {TextureFilteringMode::LINEAR, GL_LINEAR},
};

static std::unordered_map<MeshPrimitive, GLenum> opengl_mesh_primitive_types = {
    {MeshPrimitive::TRIANGLES, GL_TRIANGLES},
    {MeshPrimitive::LINES, GL_LINES},
};

Error Renderer::init_api() {
    EmscriptenWebGLContextAttributes attributes;
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

Error Renderer::compile_shader(Shader* shader) {
    GLuint vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const char* c_str = shader->m_vertex_shader_source.c_str();
    glShaderSource(vertex_shader, 1, &c_str, NULL);
    glCompileShader(vertex_shader);

    int success;
    char info[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info);
        glDeleteShader(vertex_shader);
        return Error("Renderer::compile_shader: Failed to compile vertex shader \"" +
                     shader->m_name + "\" : " + str(info));
    }

    GLuint fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    c_str = shader->m_fragment_shader_source.c_str();
    glShaderSource(fragment_shader, 1, &c_str, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info);
        glDeleteShader(fragment_shader);
        return Error("Renderer::compile_shader: Failed to compile fragment shader \"" +
                     shader->m_name + "\" : " + str(info));
    }

    GLuint shader_program;
    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info);
        glDeleteProgram(shader_program);
        return Error("Renderer::compile_shader: Failed to link shader program \"" + shader->m_name +
                     "\" : " + str(info));
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    Shader_OPENGL* api_shader = new Shader_OPENGL;
    api_shader->m_program = shader_program;

    shader->m_api_data = api_shader;

    GLint n_uniforms, max_len;
    glGetProgramiv(shader_program, GL_ACTIVE_UNIFORMS, &n_uniforms);
    glGetProgramiv(shader_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_len);
    GLchar* uniform_name = (GLchar*)malloc(max_len);

    glUseProgram(shader_program);

    struct Shader_Uniform_OPENGL {
        GLint m_size = -1;
        GLint m_location = -1;
        GLenum m_type = -1;
        u32 m_texture_unit = -1;
    };

    u32 texture_unit_counter = 0;
    if (uniform_name != NULL) {
        for (i32 i = 0; i < n_uniforms; i++) {
            Shader_Uniform_OPENGL uniform;
            glGetActiveUniform(shader_program, i, max_len, NULL, &uniform.m_size, &uniform.m_type,
                               uniform_name);
            uniform.m_location = glGetUniformLocation(shader_program, uniform_name);

            if (uniform.m_type == GL_SAMPLER_2D) {
                glUniform1i(uniform.m_location, texture_unit_counter);
                uniform.m_texture_unit = texture_unit_counter;
                texture_unit_counter++;
            }
            // api_shader->m_uniforms[str(uniform_name)] = uniform;
        }
    }

    return Error();
}

Error Renderer::upload_mesh(Mesh* mesh) {
    u32 vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(f32), mesh->m_vertices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
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

    Mesh_OPENGL* api_mesh = new Mesh_OPENGL;
    api_mesh->m_vbo = vbo;
    api_mesh->m_ebo = ebo;
    api_mesh->m_vao = vao;
    mesh->m_api_data = api_mesh;

    return Error();
}

Error Renderer::init_uniform_buffer(UniformBuffer* uniform_buffer) {
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

Error Renderer::set_uniform_buffer_data(UniformBuffer* uniform_buffer, str uniform_name,
                                        UniformValue value) {
    glBindBuffer(GL_UNIFORM_BUFFER, ((UniformBuffer_OPENGL*)uniform_buffer->m_api_data)->m_ubo);
    Uniform& uniform = uniform_buffer->m_uniforms[uniform_buffer->m_uniforms_ids[uniform_name]];
    glBufferSubData(GL_UNIFORM_BUFFER, uniform.m_offset, uniform.m_size, &value);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return Error();
}

void Renderer::set_shader(Shader* shader) {
    glUseProgram(((Shader_OPENGL*)shader->m_api_data)->m_program);
}

Error Renderer::upload_texture(Texture* texture) {
    u32 texture_name;
    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexImage2D(GL_TEXTURE_2D, 0, opengl_texture_formats[texture->m_texture_params.m_format].first,
                 texture->m_width, texture->m_height, 0,
                 opengl_texture_formats[texture->m_texture_params.m_format].second,
                 GL_UNSIGNED_BYTE, texture->m_data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    opengl_texture_wrap_modes[texture->m_texture_params.m_wrap_mode_s]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    opengl_texture_wrap_modes[texture->m_texture_params.m_wrap_mode_t]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    opengl_texture_filtering_modes[texture->m_texture_params.m_filter_mode_min]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    opengl_texture_filtering_modes[texture->m_texture_params.m_filter_mode_mag]);
    glGenerateMipmap(GL_TEXTURE_2D);

    Texture_OPENGL* api_texture = new Texture_OPENGL;
    api_texture->m_texture_name = texture_name;
    texture->m_api_data = api_texture;

    return Error();
}

void Renderer::set_texture(Pass* pass, Shader* shader) {
    // Shader_OPENGL* shader_opengl = ((Shader_OPENGL*)shader->m_api_data);

    // for (const auto& texture_binding : pass->m_textures_bindings) {
    //     u32 texture_unit = shader_openm_uniforms[texture_binding.first].m_texture_unit;
    //     glActiveTexture(GL_TEXTURE0 + texture_unit);

    //     str texture_name = texture_binding.second;
    //     if (m_texture_manager->m_textures.count(texture_name)) {
    //         Texture* texture = &m_texture_manager->m_textures[texture_name];
    //         glBindTexture(GL_TEXTURE_2D,
    //                           ((Texture_OPENGL*)texture->m_api_data)->m_texture_name);
    //     } else {
    //         logger.error(Error("Texture with name \"" + texture_name + "\" not found"));
    //     }
    // }
}

// Error Renderer::create_framebuffer(Framebuffer* framebuffer) {
//     u32 fbo;
//     glGenFramebuffers(1, &fbo);
//     glBindFramebuffer(GL_FRAMEBUFFER, fbo);

//     Texture texture = {
//         .m_width = framebuffer->m_width,
//         .m_height = framebuffer->m_height,
//         .m_texture_params = framebuffer->m_texture_params,
//     };
//     Error err = create_texture(&texture);
//     if (err) {
//         return err;
//     }

//     // @note change this to framebuffer type?
//     if (framebuffer->m_texture_params.m_format == TextureFormat::DEPTH32F) {
//         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
//                                    ((Texture_OPENGL*)texture.m_api_data)->m_texture_name, 0);
//         glDrawBuffer(GL_NONE);
//         glReadBuffer(GL_NONE);
//     } else {
//         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
//                                    ((Texture_OPENGL*)texture.m_api_data)->m_texture_name, 0);
//     }

//     Framebuffer_OPENGL* api_framebuffer = new Framebuffer_OPENGL;
//     api_framebuffer->m_fbo = fbo;
//     framebuffer->m_api_data = api_framebuffer;

//     glBindFramebuffer(GL_FRAMEBUFFER, 0);

//     m_texture_manager->m_textures[framebuffer->m_name] = std::move(texture);

//     return Error();
// }

void Renderer::debug_marker_start(str name) {
}

void Renderer::debug_marker_end() {
}

void Renderer::set_default_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::set_viewport(u32 x, u32 y, u32 width, u32 height) {
    glViewport(x, y, width, height);
}

void Renderer::set_framebuffer(Framebuffer* framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, ((Framebuffer_OPENGL*)framebuffer->m_api_data)->m_fbo);
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

void Renderer::draw_mesh(Mesh* mesh) {
    glBindVertexArray(((Mesh_OPENGL*)mesh->m_api_data)->m_vao);
    glDrawElements(opengl_mesh_primitive_types[mesh->m_primitive], GLsizei(mesh->m_indices.size()),
                   GL_UNSIGNED_INT, 0);
}
}  // namespace blaz
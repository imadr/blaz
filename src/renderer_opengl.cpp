#include <unordered_map>
#include <utility>

#include "color.h"
#include "error.h"
#include "game.h"
#include "opengl.h"
#include "platform.h"
#include "renderer.h"
#include "texture.h"
#include "types.h"

namespace blaz {

struct Mesh_OPENGL {
    u32 m_vbo, m_vao, m_ebo;
};

struct Shader_Uniform_OPENGL {
    GLint m_size = -1;
    GLint m_location = -1;
    GLenum m_type = -1;
    u32 m_texture_unit = -1;
};

struct Shader_OPENGL {
    GLuint m_program = 0;
    std::map<str, Shader_Uniform_OPENGL> m_uniforms;
};

struct Texture_OPENGL {
    u32 m_texture_name;
    u32 m_format;
};

struct Framebuffer_OPENGL {
    u32 m_fbo;
};

static std::unordered_map<TextureFormat, std::pair<GLint, GLenum>> opengl_texture_formats = {
    {TextureFormat::RGB8, {GL_RGB8, GL_RGB}},
    {TextureFormat::RGBA8, {GL_RGBA8, GL_RGBA}},
    {TextureFormat::DEPTH32, {GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT}},
    {TextureFormat::DEPTH32F, {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT}},
};

static std::unordered_map<TextureWrapMode, GLenum> opengl_texture_wrap_modes = {
    {TextureWrapMode::REPEAT, GL_REPEAT},
    {TextureWrapMode::MIRRORED_REPEAT, GL_MIRRORED_REPEAT},
    {TextureWrapMode::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE},
    {TextureWrapMode::CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
};

static std::unordered_map<TextureFilteringMode, GLenum> opengl_texture_filtering_modes = {
    {TextureFilteringMode::NEAREST, GL_NEAREST},
    {TextureFilteringMode::LINEAR, GL_LINEAR},
};

Opengl* gl;

Error Renderer::init_api() {
    gl = new Opengl();
    bool debug = false;
#ifdef DEBUG_RENDERER
    debug = true;
#endif
    Error err = gl->init(&m_game->m_window, debug);
    if (err) {
        return Error("Renderer::init ->\n" + err.message());
    }

    return Error();
}

void Renderer::clear(u32 clear_flag, RGBA clear_color, float clear_depth) {
    if (clear_flag != Clear::NONE) {
        GLbitfield gl_clear_flag = 0;
        if (clear_flag & Clear::COLOR) {
            gl_clear_flag |= GL_COLOR_BUFFER_BIT;
            gl->glClearColor(clear_color.r(), clear_color.g(), clear_color.b(), clear_color.a());
        }
        if (clear_flag & Clear::DEPTH) {
            gl_clear_flag |= GL_DEPTH_BUFFER_BIT;
            gl->glClearDepth(clear_depth);
        }
        gl->glClear(gl_clear_flag);
    }
}

void Renderer::present() {
    gl->swap_buffers(&m_game->m_window);
}

Error Renderer::compile_shader(Shader* shader) {
    GLuint vertex_shader;
    vertex_shader = gl->glCreateShader(GL_VERTEX_SHADER);

    const char* c_str = shader->m_vertex_shader_source.c_str();
    gl->glShaderSource(vertex_shader, 1, &c_str, NULL);
    gl->glCompileShader(vertex_shader);

    int success;
    char info[512];
    gl->glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        gl->glGetShaderInfoLog(vertex_shader, 512, NULL, info);
        gl->glDeleteShader(vertex_shader);
        return Error("Renderer::compile_shader: Failed to compile vertex shader \"" +
                     shader->m_name + "\" : " + str(info));
    }

    GLuint fragment_shader;
    fragment_shader = gl->glCreateShader(GL_FRAGMENT_SHADER);
    c_str = shader->m_fragment_shader_source.c_str();
    gl->glShaderSource(fragment_shader, 1, &c_str, NULL);
    gl->glCompileShader(fragment_shader);
    gl->glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        gl->glGetShaderInfoLog(fragment_shader, 512, NULL, info);
        gl->glDeleteShader(fragment_shader);
        return Error("Renderer::compile_shader: Failed to compile fragment shader \"" +
                     shader->m_name + "\" : " + str(info));
    }

    GLuint shader_program;
    shader_program = gl->glCreateProgram();
    gl->glAttachShader(shader_program, vertex_shader);
    gl->glAttachShader(shader_program, fragment_shader);
    gl->glLinkProgram(shader_program);
    gl->glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        gl->glGetProgramInfoLog(shader_program, 512, NULL, info);
        return Error("Renderer::compile_shader: Failed to link shader program \"" + shader->m_name +
                     "\" : " + str(info));
    }

    gl->glDeleteShader(vertex_shader);
    gl->glDeleteShader(fragment_shader);

    Shader_OPENGL* api_shader = new Shader_OPENGL;
    api_shader->m_program = shader_program;

    GLint n_uniforms, max_len;
    gl->glGetProgramiv(shader_program, GL_ACTIVE_UNIFORMS, &n_uniforms);
    gl->glGetProgramiv(shader_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_len);
    GLchar* uniform_name = (GLchar*)malloc(max_len);

    gl->glUseProgram(shader_program);

    u32 texture_unit_counter = 0;
    if (uniform_name != NULL) {
        for (i32 i = 0; i < n_uniforms; i++) {
            Shader_Uniform_OPENGL uniform;
            gl->glGetActiveUniform(shader_program, i, max_len, NULL, &uniform.m_size,
                                   &uniform.m_type, uniform_name);
            uniform.m_location = gl->glGetUniformLocation(shader_program, uniform_name);

            if (uniform.m_type == GL_SAMPLER_2D) {
                gl->glUniform1i(uniform.m_location, texture_unit_counter);
                uniform.m_texture_unit = texture_unit_counter;
                texture_unit_counter++;
            }
            api_shader->m_uniforms[str(uniform_name)] = uniform;
        }
    }
    free(uniform_name);

    shader->m_api_data = api_shader;

    return Error();
}

Error Renderer::upload_mesh(Mesh* mesh) {
    u32 vbo, vao, ebo;
    gl->glGenVertexArrays(1, &vao);
    gl->glGenBuffers(1, &vbo);
    gl->glGenBuffers(1, &ebo);
    gl->glBindVertexArray(vao);

    gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl->glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(f32),
                     mesh->m_vertices.data(), GL_STATIC_DRAW);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(u32),
                     mesh->m_indices.data(), GL_STATIC_DRAW);

    u32 attribs_stride = 0;
    for (i32 i = 0; i < mesh->m_attribs.size(); i++) {
        attribs_stride += mesh->m_attribs[i].second;
    }
    u32 attribs_offset = 0;
    for (i32 i = 0; i < mesh->m_attribs.size(); i++) {
        gl->glEnableVertexAttribArray(i);
        gl->glVertexAttribPointer(i, mesh->m_attribs[i].second, GL_FLOAT, GL_FALSE,
                                  attribs_stride * sizeof(f32),
                                  (void*)(attribs_offset * sizeof(f32)));
        attribs_offset += mesh->m_attribs[i].second;
    }

    Mesh_OPENGL* api_mesh = new Mesh_OPENGL;
    api_mesh->m_vbo = vbo;
    api_mesh->m_ebo = ebo;
    api_mesh->m_vao = vao;
    mesh->m_api_data = api_mesh;

    return Error();
}

// Error Renderer::reupload_mesh_buffers(Mesh* mesh) {
//     Mesh_OPENGL* api_mesh = ((Mesh_OPENGL*)mesh->m_api_data);
//     gl->glBindBuffer(GL_ARRAY_BUFFER, api_mesh->m_vbo);
//     gl->glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(f32),
//                      mesh->m_vertices.data(), GL_STATIC_DRAW);
//     gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, api_mesh->m_ebo);
//     gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(u32),
//                      mesh->m_indices.data(), GL_STATIC_DRAW);
//     return Error();
// }

// Error Renderer::upload_texture_data(Texture* texture) {
//     u32 format = ((Texture_OPENGL*)texture->m_api_data)->m_format;
//     gl->glBindTexture(GL_TEXTURE_2D, ((Texture_OPENGL*)texture->m_api_data)->m_texture_name);
//     gl->glTexImage2D(GL_TEXTURE_2D, 0, format, texture->m_width, texture->m_height, 0, format,
//                      GL_UNSIGNED_BYTE, texture->m_data->data());
//     gl->glGenerateMipmap(GL_TEXTURE_2D);
//     texture->free_data();

//     return Error();
// }

// Error Renderer::set_shader_uniform_Mat4(Shader* shader, str uniform_name, Mat4 value) {
//     Shader_OPENGL* shader_opengl = ((Shader_OPENGL*)shader->m_api_data);
//     gl->glUniformMatrix4fv(shader_opengl->m_uniforms[uniform_name].m_location, 1, GL_FALSE,
//                            value.m);
//     return Error();
// }

// Error Renderer::set_shader_uniform_i32(Shader* shader, str uniform_name, i32 value) {
//     Shader_OPENGL* shader_opengl = ((Shader_OPENGL*)shader->m_api_data);
//     gl->glUniform1i(shader_opengl->m_uniforms[uniform_name].m_location, value);
//     return Error();
// }

// Error Renderer::set_shader_uniform_Vec2(Shader* shader, str uniform_name, Vec2 value) {
//     Shader_OPENGL* shader_opengl = ((Shader_OPENGL*)shader->m_api_data);
//     gl->glUniform2f(shader_opengl->m_uniforms[uniform_name].m_location, value.x(), value.y());
//     return Error();
// }

// Error Renderer::set_shader_uniform_Vec3(Shader* shader, str uniform_name, Vec3 value) {
//     Shader_OPENGL* shader_opengl = ((Shader_OPENGL*)shader->m_api_data);
//     gl->glUniform3f(shader_opengl->m_uniforms[uniform_name].m_location, value.x(), value.y(),
//                     value.z());
//     return Error();
// }

void Renderer::bind_shader(Shader& shader) {
    gl->glUseProgram(((Shader_OPENGL*)shader.m_api_data)->m_program);
}

// Error Renderer::create_texture(Texture* texture) {
//     u32 texture_name;
//     gl->glGenTextures(1, &texture_name);
//     gl->glBindTexture(GL_TEXTURE_2D, texture_name);
//     gl->glTexImage2D(
//         GL_TEXTURE_2D, 0, opengl_texture_formats[texture->m_texture_params.m_format].first,
//         texture->m_width, texture->m_height, 0,
//         opengl_texture_formats[texture->m_texture_params.m_format].second, GL_UNSIGNED_BYTE,
//         NULL);
//     gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
//                         opengl_texture_wrap_modes[texture->m_texture_params.m_wrap_mode_s]);
//     gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
//                         opengl_texture_wrap_modes[texture->m_texture_params.m_wrap_mode_t]);
//     if (texture->m_texture_params.m_wrap_mode_s == TextureWrapMode::CLAMP_TO_BORDER ||
//         texture->m_texture_params.m_wrap_mode_t == TextureWrapMode::CLAMP_TO_BORDER) {
//         gl->glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
//                              texture->m_texture_params.m_clamp_to_border_color.values);
//     }
//     gl->glTexParameteri(
//         GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
//         opengl_texture_filtering_modes[texture->m_texture_params.m_filter_mode_min]);
//     gl->glTexParameteri(
//         GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
//         opengl_texture_filtering_modes[texture->m_texture_params.m_filter_mode_mag]);
//     gl->glGenerateMipmap(GL_TEXTURE_2D);

//     Texture_OPENGL* api_texture = new Texture_OPENGL;
//     api_texture->m_texture_name = texture_name;
//     texture->m_api_data = api_texture;

//     return Error();
// }

// Error Renderer::create_framebuffer(Framebuffer* framebuffer) {
//     u32 fbo;
//     gl->glGenFramebuffers(1, &fbo);
//     gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo);

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
//         gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
//                                    ((Texture_OPENGL*)texture.m_api_data)->m_texture_name, 0);
//         gl->glDrawBuffer(GL_NONE);
//         gl->glReadBuffer(GL_NONE);
//     } else {
//         gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
//                                    ((Texture_OPENGL*)texture.m_api_data)->m_texture_name, 0);
//     }

//     Framebuffer_OPENGL* api_framebuffer = new Framebuffer_OPENGL;
//     api_framebuffer->m_fbo = fbo;
//     framebuffer->m_api_data = api_framebuffer;

//     gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

//     m_texture_manager->m_textures[framebuffer->m_name] = std::move(texture);

//     return Error();
// }

void Renderer::debug_marker_start(str name) {
    gl->glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name.c_str());
}

void Renderer::debug_marker_end() {
    gl->glPopDebugGroup();
}

void Renderer::bind_default_framebuffer() {
    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::set_viewport(u32 x, u32 y, u32 width, u32 height) {
    gl->glViewport(x, y, width, height);
}

void Renderer::bind_framebuffer(Framebuffer& framebuffer) {
    gl->glBindFramebuffer(GL_FRAMEBUFFER, ((Framebuffer_OPENGL*)framebuffer.m_api_data)->m_fbo);
}

void Renderer::set_depth_test(bool enabled) {
    if (enabled) {
        gl->glEnable(GL_DEPTH_TEST);
    } else {
        gl->glDisable(GL_DEPTH_TEST);
    }
}

void Renderer::set_face_culling(bool enabled) {
    if (enabled) {
        gl->glEnable(GL_CULL_FACE);
    } else {
        gl->glDisable(GL_CULL_FACE);
    }
}

void Renderer::set_face_culling_mode(CullingMode mode) {
    if (mode == CullingMode::BACK) {
        gl->glCullFace(GL_BACK);
    } else if (mode == CullingMode::FRONT) {
        gl->glCullFace(GL_FRONT);
    }
}

// void Renderer::bind_textures(Pass* pass, Shader* shader) {
//     Shader_OPENGL* shader_opengl = ((Shader_OPENGL*)shader->m_api_data);

//     for (const auto& texture_binding : pass->m_textures_bindings) {
//         u32 texture_unit = shader_opengl->m_uniforms[texture_binding.first].m_texture_unit;
//         gl->glActiveTexture(GL_TEXTURE0 + texture_unit);

//         str texture_name = texture_binding.second;
//         if (m_texture_manager->m_textures.count(texture_name)) {
//             Texture* texture = &m_texture_manager->m_textures[texture_name];
//             gl->glBindTexture(GL_TEXTURE_2D,
//                               ((Texture_OPENGL*)texture->m_api_data)->m_texture_name);
//         } else {
//             logger.error(Error("Texture with name \"" + texture_name + "\" not found"));
//         }
//     }
// }

void Renderer::draw_vertex_array(Mesh* mesh) {
    gl->glBindVertexArray(((Mesh_OPENGL*)mesh->m_api_data)->m_vao);
    gl->glDrawElements(GL_TRIANGLES, (GLsizei)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
}

}  // namespace blaz
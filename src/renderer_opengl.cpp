#include <unordered_map>
#include <utility>

#include "color.h"
#include "error.h"
#include "game.h"
#include "opengl_loader/opengl_loader.h"
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
    GLuint m_compute_shader = 0;
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
        {TextureFormat::DEPTH32, {GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE}},
        {TextureFormat::DEPTH32F, {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE}},
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

static std::unordered_map<AccessType, GLenum> opengl_access_types = {
    {AccessType::READ_ONLY, GL_READ_ONLY},
    {AccessType::WRITE_ONLY, GL_WRITE_ONLY},
    {AccessType::READ_WRITE, GL_READ_WRITE},
};

OpenglLoader* gl;
GLuint dummy_vao;

void gl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                       const GLchar* message, const void* userParam) {
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }

    str _source;
    str _type;
    str _severity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            _source = "API";
            break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            _source = "WINDOW SYSTEM";
            break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            _source = "SHADER COMPILER";
            break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
            _source = "THIRD PARTY";
            break;

        case GL_DEBUG_SOURCE_APPLICATION:
            _source = "APPLICATION";
            break;

        case GL_DEBUG_SOURCE_OTHER:
            _source = "UNKNOWN";
            break;

        default:
            _source = "UNKNOWN";
            break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            _type = "ERROR";
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            _type = "DEPRECATED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            _type = "UNDEFINED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_PORTABILITY:
            _type = "PORTABILITY";
            break;

        case GL_DEBUG_TYPE_PERFORMANCE:
            _type = "PERFORMANCE";
            break;

        case GL_DEBUG_TYPE_OTHER:
            _type = "OTHER";
            break;

        case GL_DEBUG_TYPE_MARKER:
            _type = "MARKER";
            break;

        default:
            _type = "UNKNOWN";
            break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            _severity = "HIGH";
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            _severity = "MEDIUM";
            break;

        case GL_DEBUG_SEVERITY_LOW:
            _severity = "LOW";
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            _severity = "NOTIFICATION";
            break;

        default:
            _severity = "UNKNOWN";
            break;
    }

    str message_str(message);

    if (message_str.find("object is not successfully linked, or is not a program object") !=
        std::string::npos) {
        return;
    }
    if (message_str.find("has not been linked, or is not a program object.") != std::string::npos) {
        return;
    }
    if (message_str.find("No active compute shader.") != std::string::npos) {
        return;
    }

    logger.error("OpenGL error id: " + std::to_string(id) + " type: " + _type +
                 " severity: " + _severity + " source: " + _source + " message: " + message);
}

Error Renderer::init_api() {
    gl = new OpenglLoader();
    bool debug = false;
#ifdef DEBUG_RENDERER
    debug = true;
#endif
    Error err = gl->init(m_window, debug);
    if (err) {
        return err;
    }

#ifdef DEBUG_RENDERER
    gl->glEnable(GL_DEBUG_OUTPUT);
    gl->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    gl->glDebugMessageCallback(gl_error_callback, 0);
#endif

    gl->glGenVertexArrays(1, &dummy_vao);

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
    gl->swap_buffers(m_window);
}

void Renderer::set_swap_interval(u32 interval) {
    gl->set_swap_interval(interval);
}

Error Renderer::create_shader_api(str shader_id) {
    Shader* shader = &m_shaders[shader_id];
    Shader_OPENGL* api_shader = new Shader_OPENGL;
    shader->m_api_data = api_shader;

    return Error();
}

Error Renderer::reload_shader_api(str shader_id) {
    Shader* shader = &m_shaders[shader_id];
    Shader_OPENGL* api_shader = (Shader_OPENGL*)shader->m_api_data;
    int success;
    char info[512];

    if (api_shader->m_vertex_shader) {
        gl->glDeleteShader(api_shader->m_vertex_shader);
    }
    if (api_shader->m_fragment_shader) {
        gl->glDeleteShader(api_shader->m_fragment_shader);
    }
    if (api_shader->m_compute_shader) {
        gl->glDeleteShader(api_shader->m_compute_shader);
    }
    if (api_shader->m_program) {
        gl->glDeleteProgram(api_shader->m_program);
    }

    api_shader->m_program = gl->glCreateProgram();

    if (shader->m_type == ShaderType::VERTEX_FRAGMENT) {
        api_shader->m_vertex_shader = gl->glCreateShader(GL_VERTEX_SHADER);
        api_shader->m_fragment_shader = gl->glCreateShader(GL_FRAGMENT_SHADER);
        gl->glAttachShader(api_shader->m_program, api_shader->m_vertex_shader);
        gl->glAttachShader(api_shader->m_program, api_shader->m_fragment_shader);
    } else if (shader->m_type == ShaderType::COMPUTE) {
        api_shader->m_compute_shader = gl->glCreateShader(GL_COMPUTE_SHADER);
        gl->glAttachShader(api_shader->m_program, api_shader->m_compute_shader);
    }

#ifdef DEBUG_RENDERER
    gl->glObjectLabel(GL_PROGRAM, api_shader->m_program, -1, (shader->m_name + "_program").c_str());

    if (shader->m_type == ShaderType::VERTEX_FRAGMENT) {
        gl->glObjectLabel(GL_SHADER, api_shader->m_vertex_shader, -1,
                          (shader->m_name + "_vertex").c_str());
        gl->glObjectLabel(GL_SHADER, api_shader->m_fragment_shader, -1,
                          (shader->m_name + "_fragment").c_str());
    } else if (shader->m_type == ShaderType::COMPUTE) {
        gl->glObjectLabel(GL_SHADER, api_shader->m_compute_shader, -1,
                          (shader->m_name + "_compute").c_str());
    }
#endif

    if (shader->m_type == ShaderType::VERTEX_FRAGMENT) {
        const char* c_str = shader->m_vertex_shader_source.c_str();
        gl->glShaderSource(api_shader->m_vertex_shader, 1, &c_str, NULL);
        gl->glCompileShader(api_shader->m_vertex_shader);

        gl->glGetShaderiv(api_shader->m_vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            gl->glGetShaderInfoLog(api_shader->m_vertex_shader, 512, NULL, info);
            gl->glDeleteShader(api_shader->m_vertex_shader);
            return Error("Renderer::compile_shader: Failed to compile vertex shader \"" +
                         shader->m_name + "\" : " + str(info));
        }

        c_str = shader->m_fragment_shader_source.c_str();
        gl->glShaderSource(api_shader->m_fragment_shader, 1, &c_str, NULL);
        gl->glCompileShader(api_shader->m_fragment_shader);
        gl->glGetShaderiv(api_shader->m_fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            gl->glGetShaderInfoLog(api_shader->m_fragment_shader, 512, NULL, info);
            gl->glDeleteShader(api_shader->m_fragment_shader);
            return Error("Renderer::compile_shader: Failed to compile fragment shader \"" +
                         shader->m_name + "\" : " + str(info));
        }
    } else if (shader->m_type == ShaderType::COMPUTE) {
        const char* c_str = shader->m_compute_shader_source.c_str();
        gl->glShaderSource(api_shader->m_compute_shader, 1, &c_str, NULL);
        gl->glCompileShader(api_shader->m_compute_shader);

        gl->glGetShaderiv(api_shader->m_compute_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            gl->glGetShaderInfoLog(api_shader->m_compute_shader, 512, NULL, info);
            gl->glDeleteShader(api_shader->m_compute_shader);
            return Error("Renderer::compile_shader: Failed to compile compute shader \"" +
                         shader->m_name + "\" : " + str(info));
        }
    }

    gl->glLinkProgram(api_shader->m_program);
    gl->glGetProgramiv(api_shader->m_program, GL_LINK_STATUS, &success);
    if (!success) {
        gl->glGetProgramInfoLog(api_shader->m_program, 512, NULL, info);
        gl->glDeleteProgram(api_shader->m_program);
        return Error("Renderer::compile_shader: Failed to link shader program \"" + shader->m_name +
                     "\" : " + str(info));
    }

    gl->glUseProgram(api_shader->m_program);

    GLint n_uniforms, max_len;
    gl->glGetProgramiv(api_shader->m_program, GL_ACTIVE_UNIFORMS, &n_uniforms);
    gl->glGetProgramiv(api_shader->m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_len);
    GLchar* uniform_name = (GLchar*)malloc(max_len);

    if (uniform_name == NULL) {
        return Error("Renderer::compile_shader: Failed to allocate memory for uniform name");
    }

    for (GLint i = 0; i < n_uniforms; i++) {
        GLint size = -1;
        GLenum type = -1;
        gl->glGetActiveUniform(api_shader->m_program, i, max_len, NULL, &size, &type, uniform_name);

        GLint binding_point;
        if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE || type == GL_SAMPLER_2D_ARRAY ||
            type == GL_SAMPLER_3D) {
            GLint location = gl->glGetUniformLocation(api_shader->m_program, uniform_name);
            gl->glGetUniformiv(api_shader->m_program, location, &binding_point);
            shader->m_uniform_bindings[uniform_name] =
                UniformBinding(binding_point, UniformBindingType::SAMPLER);
        } else if (type == GL_IMAGE_2D) {
            GLint location = gl->glGetUniformLocation(api_shader->m_program, uniform_name);
            gl->glGetUniformiv(api_shader->m_program, location, &binding_point);
            shader->m_uniform_bindings[uniform_name] =
                UniformBinding(binding_point, UniformBindingType::IMAGE);
        }
    }

    free(uniform_name);

    GLint n_uniform_blocks;
    gl->glGetProgramiv(api_shader->m_program, GL_ACTIVE_UNIFORM_BLOCKS, &n_uniform_blocks);
    gl->glGetProgramiv(api_shader->m_program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_len);
    GLchar* uniform_block_name = (GLchar*)malloc(max_len);

    if (uniform_block_name == NULL) {
        return Error("Renderer::compile_shader: Failed to allocate memory for uniform block name");
    }

    for (GLint i = 0; i < n_uniform_blocks; i++) {
        GLint name_len;
        GLint binding_point;
        gl->glGetActiveUniformBlockName(api_shader->m_program, i, max_len, &name_len,
                                        uniform_block_name);
        uniform_block_name[name_len] = '\0';

        gl->glGetActiveUniformBlockiv(api_shader->m_program, i, GL_UNIFORM_BLOCK_BINDING,
                                      &binding_point);

        shader->m_uniform_bindings[str(uniform_block_name)] =
            UniformBinding(binding_point, UniformBindingType::BLOCK);
    }

    free(uniform_block_name);

    shader->m_should_reload = false;
    return Error();
}

void Renderer::set_current_shader(str shader_id) {
    Shader* shader = &m_shaders[shader_id];
    gl->glUseProgram(((Shader_OPENGL*)shader->m_api_data)->m_program);
}

void Renderer::set_bufferless_mesh() {
    gl->glBindVertexArray(dummy_vao);
}

Error Renderer::create_mesh_api(str mesh_id) {
    Mesh* mesh = &m_meshes[mesh_id];

    u32 vbo, vao, ebo;
    gl->glGenVertexArrays(1, &vao);
    gl->glBindVertexArray(vao);

    gl->glGenBuffers(1, &vbo);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);

    gl->glGenBuffers(1, &ebo);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    gl->glBindVertexArray(0);

    Mesh_OPENGL* api_mesh = new Mesh_OPENGL;
    api_mesh->m_vbo = vbo;
    api_mesh->m_ebo = ebo;
    api_mesh->m_vao = vao;
    mesh->m_api_data = api_mesh;

#ifdef DEBUG_RENDERER
    gl->glObjectLabel(GL_VERTEX_ARRAY, vao, -1, (mesh->m_name + "_vao").c_str());
    gl->glObjectLabel(GL_BUFFER, vbo, -1, (mesh->m_name + "_vbo").c_str());
    gl->glObjectLabel(GL_BUFFER, ebo, -1, (mesh->m_name + "_ebo").c_str());
#endif

    return Error();
}

Error Renderer::reload_mesh_api(str mesh_id) {
    Mesh* mesh = &m_meshes[mesh_id];
    Mesh_OPENGL* api_mesh = (Mesh_OPENGL*)mesh->m_api_data;

    gl->glBindVertexArray(api_mesh->m_vao);
    gl->glBindBuffer(GL_ARRAY_BUFFER, api_mesh->m_vbo);
    gl->glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(f32),
                     mesh->m_vertices.data(), GL_STATIC_DRAW);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, api_mesh->m_ebo);
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

    mesh->m_should_reload = false;

    return Error();
}

void Renderer::set_current_mesh(str mesh_id) {
    Mesh* mesh = &m_meshes[mesh_id];
    gl->glBindVertexArray(((Mesh_OPENGL*)mesh->m_api_data)->m_vao);
}

Error Renderer::create_framebuffer_api(str framebuffer_id) {
    Framebuffer* framebuffer = &m_framebuffers[framebuffer_id];

    u32 fbo;
    gl->glGenFramebuffers(1, &fbo);
    gl->glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    Framebuffer_OPENGL* api_framebuffer = new Framebuffer_OPENGL;
    api_framebuffer->m_fbo = fbo;
    framebuffer->m_api_data = api_framebuffer;

#ifdef DEBUG_RENDERER
    gl->glObjectLabel(GL_FRAMEBUFFER, fbo, -1, (framebuffer->m_name + "_framebuffer").c_str());
#endif

    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Error();
}

Error Renderer::attach_texture_to_framebuffer(str framebuffer_id) {
    Framebuffer* framebuffer = &m_framebuffers[framebuffer_id];
    Framebuffer_OPENGL* api_framebuffer = (Framebuffer_OPENGL*)framebuffer->m_api_data;

    gl->glBindFramebuffer(GL_FRAMEBUFFER, api_framebuffer->m_fbo);

    Texture* texture;

    bool no_color_attachment = true;

    if (framebuffer->m_color_attachment_texture != "") {
        texture = &m_textures[framebuffer->m_color_attachment_texture];
        gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   ((Texture_OPENGL*)texture->m_api_data)->m_texture_name, 0);
        no_color_attachment = false;
    }

    if (framebuffer->m_depth_attachment_texture != "") {
        texture = &m_textures[framebuffer->m_depth_attachment_texture];
        gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                                   ((Texture_OPENGL*)texture->m_api_data)->m_texture_name, 0);
    }

    if (framebuffer->m_stencil_attachment_texture != "") {
        texture = &m_textures[framebuffer->m_stencil_attachment_texture];
        gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                                   ((Texture_OPENGL*)texture->m_api_data)->m_texture_name, 0);
    }

    if (no_color_attachment) {
        gl->glDrawBuffer(GL_NONE);
        gl->glReadBuffer(GL_NONE);
    }

    if (gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return Error("Framebuffer not complete");
    }

    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Error();
}

void Renderer::set_current_framebuffer(str framebuffer_id) {
    Framebuffer* framebuffer = &m_framebuffers[framebuffer_id];
    gl->glBindFramebuffer(GL_FRAMEBUFFER, ((Framebuffer_OPENGL*)framebuffer->m_api_data)->m_fbo);
}

void Renderer::set_default_framebuffer() {
    gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Error Renderer::create_uniform_buffer_api(str uniform_buffer_id) {
    UniformBuffer* uniform_buffer = &m_uniform_buffers[uniform_buffer_id];
    GLuint ubo;
    gl->glGenBuffers(1, &ubo);
    gl->glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    gl->glBufferData(GL_UNIFORM_BUFFER, uniform_buffer->m_size, NULL, GL_STATIC_DRAW);

    UniformBuffer_OPENGL* api_uniform_buffer = new UniformBuffer_OPENGL;
    api_uniform_buffer->m_ubo = ubo;
    uniform_buffer->m_api_data = api_uniform_buffer;

#ifdef DEBUG_RENDERER
    gl->glObjectLabel(GL_BUFFER, ubo, -1, (uniform_buffer->m_name + "_uniform_buffer").c_str());
#endif

    return Error();
}

Error Renderer::set_uniform_buffer_data(str uniform_buffer_id,
                                        vec<pair<str, UniformValue>> uniform_values) {
    UniformBuffer* uniform_buffer = &m_uniform_buffers[uniform_buffer_id];
    gl->glBindBuffer(GL_UNIFORM_BUFFER, ((UniformBuffer_OPENGL*)uniform_buffer->m_api_data)->m_ubo);
    for (auto& uniform_value : uniform_values) {
        Uniform& uniform =
            uniform_buffer->m_uniforms[uniform_buffer->m_uniforms_ids[uniform_value.first]];
        gl->glBufferSubData(GL_UNIFORM_BUFFER, uniform.m_offset, uniform.m_size,
                            &uniform_value.second);
    }
    return Error();
}

Error Renderer::create_texture_api(str texture_id) {
    Texture* texture = &m_textures[texture_id];

    u32 texture_name;
    gl->glGenTextures(1, &texture_name);
    gl->glBindTexture(GL_TEXTURE_2D, texture_name);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        opengl_texture_wrap_modes[texture->m_texture_params.m_wrap_mode_s]);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        opengl_texture_wrap_modes[texture->m_texture_params.m_wrap_mode_t]);
    gl->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        opengl_texture_filtering_modes[texture->m_texture_params.m_filter_mode_min]);
    gl->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        opengl_texture_filtering_modes[texture->m_texture_params.m_filter_mode_mag]);

    auto& texture_type = opengl_texture_formats[texture->m_texture_params.m_format];
    gl->glTexImage2D(GL_TEXTURE_2D, 0, get<0>(texture_type), texture->m_width, texture->m_height, 0,
                     get<1>(texture_type), get<2>(texture_type), NULL);
    gl->glGenerateMipmap(GL_TEXTURE_2D);

    Texture_OPENGL* api_texture = new Texture_OPENGL;
    api_texture->m_texture_name = texture_name;
    texture->m_api_data = api_texture;

#ifdef DEBUG_RENDERER
    gl->glObjectLabel(GL_TEXTURE, texture_name, -1, (texture->m_name + "_texture").c_str());
#endif

    return Error();
}

Error Renderer::reload_texture_api(str texture_id) {
    Texture* texture = &m_textures[texture_id];
    Texture_OPENGL* api_texture = (Texture_OPENGL*)texture->m_api_data;

    gl->glBindTexture(GL_TEXTURE_2D, ((Texture_OPENGL*)texture->m_api_data)->m_texture_name);
    auto& texture_type = opengl_texture_formats[texture->m_texture_params.m_format];
    gl->glTexImage2D(GL_TEXTURE_2D, 0, get<0>(texture_type), texture->m_width, texture->m_height, 0,
                     get<1>(texture_type), get<2>(texture_type), texture->m_data.data());
    gl->glGenerateMipmap(GL_TEXTURE_2D);

    texture->m_should_reload = false;

    return Error();
}

void Renderer::bind_uniforms(Pass* pass, str shader_id) {
    for (const auto& uniform_binding : m_shaders[shader_id].m_uniform_bindings) {
        u32 binding_point = uniform_binding.second.m_binding_point;
        if (uniform_binding.second.m_type == UniformBindingType::BLOCK) {
            UniformBuffer* uniform_buffer = &m_uniform_buffers[uniform_binding.first];
            gl->glBindBufferRange(GL_UNIFORM_BUFFER, binding_point,
                                  ((UniformBuffer_OPENGL*)uniform_buffer->m_api_data)->m_ubo, 0,
                                  uniform_buffer->m_size);
        } else if (uniform_binding.second.m_type == UniformBindingType::SAMPLER) {
            str texture_id = pass->m_sampler_uniforms_bindings.at(uniform_binding.first);
            gl->glActiveTexture(GL_TEXTURE0 + binding_point);
            Texture* texture = &m_textures[texture_id];
            gl->glBindTexture(GL_TEXTURE_2D,
                              ((Texture_OPENGL*)texture->m_api_data)->m_texture_name);

        } else if (uniform_binding.second.m_type == UniformBindingType::IMAGE) {
            pair<str, AccessType> image_uniform_binding =
                pass->m_image_uniforms_bindings.at(uniform_binding.first);
            str texture_id = image_uniform_binding.first;
            Texture* texture = &m_textures[texture_id];
            gl->glBindImageTexture(
                binding_point, ((Texture_OPENGL*)texture->m_api_data)->m_texture_name, 0, GL_FALSE,
                0, opengl_access_types[image_uniform_binding.second], GL_RGBA32F);
        }
    }
}

void Renderer::debug_marker_start(str name) {
    gl->glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name.c_str());
}

void Renderer::debug_marker_end() {
    gl->glPopDebugGroup();
}

void Renderer::set_viewport(u32 x, u32 y, u32 width, u32 height) {
    gl->glViewport(x, y, width, height);
}

void Renderer::set_depth_test(bool enabled) {
    if (enabled) {
        gl->glEnable(GL_DEPTH_TEST);
    } else {
        gl->glDisable(GL_DEPTH_TEST);
    }
}

void Renderer::set_face_culling(bool enabled, CullingMode mode, CullingOrder order) {
    if (enabled) {
        gl->glEnable(GL_CULL_FACE);
    } else {
        gl->glDisable(GL_CULL_FACE);
    }

    if (mode == CullingMode::BACK) {
        gl->glCullFace(GL_BACK);
    } else if (mode == CullingMode::FRONT) {
        gl->glCullFace(GL_FRONT);
    }

    if (order == CullingOrder::CCW) {
        gl->glFrontFace(GL_CCW);
    } else if (order == CullingOrder::CW) {
        gl->glFrontFace(GL_CW);
    }
}

void Renderer::draw(MeshPrimitive primitive, size_t count) {
    gl->glDrawArrays(opengl_mesh_primitive_types[primitive], 0, GLsizei(count));
}

void Renderer::draw_indexed(MeshPrimitive primitive, size_t count) {
    gl->glDrawElements(opengl_mesh_primitive_types[primitive], GLsizei(count), GL_UNSIGNED_INT, 0);
}

void Renderer::dispatch_compute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z) {
    gl->glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    gl->glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
}

void Renderer::copy_texture(str src, str dst, Vec3I src_pos, Vec2I src_size, Vec3I dst_pos) {
    Texture* src_texture = &m_textures[src];
    Texture_OPENGL* src_api_texture = (Texture_OPENGL*)src_texture->m_api_data;

    Texture* dst_texture = &m_textures[dst];
    Texture_OPENGL* dst_api_texture = (Texture_OPENGL*)dst_texture->m_api_data;

    gl->glCopyImageSubData(src_api_texture->m_texture_name, GL_TEXTURE_2D, 0, src_pos.x(),
                           src_pos.y(), src_pos.z(), dst_api_texture->m_texture_name, GL_TEXTURE_2D,
                           0, dst_pos.x(), dst_pos.y(), dst_pos.z(), src_size.x(), src_size.y(), 1);
}

}  // namespace blaz
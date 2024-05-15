#pragma once

#include "error.h"
#include "logger.h"
#include "platform.h"
#include "types.h"

#if defined(_WIN64)
typedef signed long long int khronos_ssize_t;
typedef unsigned long long int khronos_usize_t;
#else
typedef signed long int khronos_ssize_t;
typedef unsigned long int khronos_usize_t;
#endif

#ifdef KHRONOS_USE_INTPTR_T
typedef intptr_t khronos_intptr_t;
typedef uintptr_t khronos_uintptr_t;
#elif defined(_WIN64)
typedef signed long long int khronos_intptr_t;
typedef unsigned long long int khronos_uintptr_t;
#else
typedef signed long int khronos_intptr_t;
typedef unsigned long int khronos_uintptr_t;
#endif

typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
typedef char GLchar;
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;

#define GL_NONE 0
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_QUADS 0x0007
#define GL_QUAD_STRIP 0x0008
#define GL_POLYGON 0x0009
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_DEPTH_TEST 0x0B71
#define GL_DEPTH_BITS 0x0D56
#define GL_DEPTH_CLEAR_VALUE 0x0B73
#define GL_DEPTH_FUNC 0x0B74
#define GL_DEPTH_RANGE 0x0B70
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DEPTH_COMPONENT 0x1902
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_2_BYTES 0x1407
#define GL_3_BYTES 0x1408
#define GL_4_BYTES 0x1409
#define GL_DOUBLE 0x140A
#define GL_BUFFER_SIZE 0x8764
#define GL_BUFFER_USAGE 0x8765
#define GL_QUERY_COUNTER_BITS 0x8864
#define GL_CURRENT_QUERY 0x8865
#define GL_QUERY_RESULT 0x8866
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_READ_WRITE 0x88BA
#define GL_BUFFER_ACCESS 0x88BB
#define GL_BUFFER_MAPPED 0x88BC
#define GL_BUFFER_MAP_POINTER 0x88BD
#define GL_STREAM_DRAW 0x88E0
#define GL_STREAM_READ 0x88E1
#define GL_STREAM_COPY 0x88E2
#define GL_STATIC_DRAW 0x88E4
#define GL_STATIC_READ 0x88E5
#define GL_STATIC_COPY 0x88E6
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_DYNAMIC_COPY 0x88EA
#define GL_SAMPLES_PASSED 0x8914
#define GL_SRC1_ALPHA 0x8589
#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_FRAMEBUFFER 0x8D40
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_TEXTURE_2D 0x0DE1
#define GL_STEREO 0x0C33
#define GL_BITMAP 0x1A00
#define GL_COLOR 0x1800
#define GL_DEPTH 0x1801
#define GL_STENCIL 0x1802
#define GL_DITHER 0x0BD0
#define GL_R8 0x8229
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_LINEAR 0x2601
#define GL_NEAREST 0x2600
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_MIRRORED_REPEAT 0x8370
#define GL_TEXTURE_WIDTH 0x1000
#define GL_TEXTURE_HEIGHT 0x1001
#define GL_TEXTURE_INTERNAL_FORMAT 0x1003
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_TEXTURE_BORDER 0x1005
#define GL_CLAMP 0x2900
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_RGB4 0x804F
#define GL_RGB5 0x8050
#define GL_RGB8 0x8051
#define GL_RGB10 0x8052
#define GL_RGB12 0x8053
#define GL_RGB16 0x8054
#define GL_RGBA2 0x8055
#define GL_RGBA4 0x8056
#define GL_RGB5_A1 0x8057
#define GL_RGBA8 0x8058
#define GL_RGB10_A2 0x8059
#define GL_RGBA12 0x805A
#define GL_RGBA16 0x805B
#define GL_COLOR_INDEX 0x1900
#define GL_STENCIL_INDEX 0x1901
#define GL_DEPTH_COMPONENT 0x1902
#define GL_RED 0x1903
#define GL_GREEN 0x1904
#define GL_BLUE 0x1905
#define GL_ALPHA 0x1906
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_DEPTH_COMPONENT32 0x81A7
#define GL_DEPTH_COMPONENT32F 0x8CAC
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_RENDERBUFFER 0x8D41
#define GL_SAMPLER_2D 0x8B5E
#define GL_CULL_FACE 0x0B44
#define GL_CULL_FACE_MODE 0x0B45
#define GL_FRONT_FACE 0x0B46
#define GL_POINT 0x1B00
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_CW 0x0900
#define GL_CCW 0x0901
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_POLYGON_MODE 0x0B40
#define GL_POLYGON_SMOOTH 0x0B41
#define GL_POLYGON_STIPPLE 0x0B42
#define GL_EDGE_FLAG 0x0B43
#define GL_CULL_FACE 0x0B44
#define GL_CULL_FACE_MODE 0x0B45
#define GL_FRONT_FACE 0x0B46
#define GL_POLYGON_OFFSET_FACTOR 0x8038
#define GL_POLYGON_OFFSET_UNITS 0x2A00
#define GL_POLYGON_OFFSET_POINT 0x2A01
#define GL_POLYGON_OFFSET_LINE 0x2A02
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_ACTIVE_UNIFORM_BLOCKS 0x8A36
#define GL_UNIFORM_BLOCK_DATA_SIZE 0x8A40
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS 0x8A42
#define GL_UNIFORM_BLOCK_BINDING 0x8A3F
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_UNIFORM_SIZE 0x8A38
#define GL_UNIFORM_TYPE 0x8A37
#define GL_UNIFORM_OFFSET 0x8A3B
#define GL_BUFFER 0x82E0
#define GL_VERTEX_ARRAY 0x8074
#define GL_SHADER 0x82E1
#define GL_PROGRAM 0x82E2
#define GL_FRONT_AND_BACK 0x0408
#define GL_RED 0x1903
#define GL_RG8 0x822B
#define GL_RG 0x8227

#ifdef OS_WIN32

#define CALLING_CONVENTION WINAPI

#elif OS_LINUX

#define CALLING_CONVENTION

#endif

#define GL_OLD_FUNCTIONS_LIST                                                               \
    GL_FUNCTION(void, glGetIntegerv, GLenum, GLint*)                                        \
    GL_FUNCTION(void, glClear, GLbitfield)                                                  \
    GL_FUNCTION(void, glClearColor, GLclampf, GLclampf, GLclampf, GLclampf)                 \
    GL_FUNCTION(void, glClearDepth, GLdouble depth)                                         \
    GL_FUNCTION(void, glViewport, GLint, GLint, GLsizei, GLsizei)                           \
    GL_FUNCTION(void, glTexParameteri, GLenum target, GLenum pname, GLint param)            \
    GL_FUNCTION(void, glTexParameterf, GLenum target, GLenum pname, GLfloat param)          \
    GL_FUNCTION(void, glTexParameterfv, GLenum target, GLenum pname, const GLfloat* params) \
    GL_FUNCTION(void, glTexImage2D, GLenum target, GLint level, GLint internalFormat,       \
                GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,    \
                const GLvoid* data)                                                         \
    GL_FUNCTION(void, glBindTexture, GLenum target, GLuint texture)                         \
    GL_FUNCTION(void, glGenTextures, GLsizei n, GLuint* textures)                           \
    GL_FUNCTION(void, glEnable, GLenum cap)                                                 \
    GL_FUNCTION(void, glDisable, GLenum cap)                                                \
    GL_FUNCTION(void, glDrawElements, GLenum mode, GLsizei count, GLenum type,              \
                const GLvoid* indices)                                                      \
    GL_FUNCTION(void, glDrawArrays, GLenum mode, GLint first, GLsizei count)                \
    GL_FUNCTION(void, glDrawBuffer, GLenum mode)                                            \
    GL_FUNCTION(void, glReadBuffer, GLenum mode)                                            \
    GL_FUNCTION(void, glFrontFace, GLenum mode)                                             \
    GL_FUNCTION(void, glPolygonMode, GLenum face, GLenum mode)                              \
    GL_FUNCTION(void, glCullFace, GLenum mode)

#define GL_FUNCTION(return_type, name, ...) typedef return_type name##Type(__VA_ARGS__);

GL_OLD_FUNCTIONS_LIST
#undef GL_FUNCTION

#define GL_FUNCTIONS_LIST                                                                         \
    GL_FUNCTION(void, glActiveTexture, GLenum texture)                                            \
    GL_FUNCTION(void, glAttachShader, GLuint program, GLuint shader)                              \
    GL_FUNCTION(void, glGenFramebuffers, GLsizei n, GLuint* ids)                                  \
    GL_FUNCTION(void, glBindBuffer, GLenum target, GLuint buffer)                                 \
    GL_FUNCTION(GLenum, glCheckFramebufferStatus, GLenum target)                                  \
    GL_FUNCTION(void, glBindFramebuffer, GLenum target, GLuint framebuffer)                       \
    GL_FUNCTION(void, glFramebufferTexture2D, GLenum target, GLenum attachment, GLenum textarget, \
                GLuint texture, GLint level)                                                      \
    GL_FUNCTION(GLuint, glCreateShader, GLenum shaderType)                                        \
    GL_FUNCTION(void, glShaderSource, GLuint shader, GLsizei count, const GLchar** string,        \
                const GLint* length)                                                              \
    GL_FUNCTION(void, glCompileShader, GLuint shader)                                             \
    GL_FUNCTION(void, glGetShaderiv, GLuint shader, GLenum pname, GLint* params)                  \
    GL_FUNCTION(void, glGetShaderInfoLog, GLuint shader, GLsizei maxLength, GLsizei* length,      \
                GLchar* infoLog)                                                                  \
    GL_FUNCTION(void, glDeleteShader, GLuint shader)                                              \
    GL_FUNCTION(void, glGetProgramInfoLog, GLuint program, GLsizei maxLength, GLsizei* length,    \
                GLchar* infoLog)                                                                  \
    GL_FUNCTION(GLuint, glCreateProgram, void)                                                    \
    GL_FUNCTION(void, glLinkProgram, GLuint program)                                              \
    GL_FUNCTION(void, glGetProgramiv, GLuint program, GLenum pname, GLint* params)                \
    GL_FUNCTION(void, glVertexAttribPointer, GLuint index, GLint size, GLenum type,               \
                GLboolean normalized, GLsizei stride, const GLvoid* pointer)                      \
    GL_FUNCTION(void, glEnableVertexAttribArray, GLuint index)                                    \
    GL_FUNCTION(void, glDisableVertexAttribArray, GLuint index)                                   \
    GL_FUNCTION(void, glGenVertexArrays, GLsizei n, GLuint* arrays)                               \
    GL_FUNCTION(void, glGenBuffers, GLsizei n, GLuint* buffers)                                   \
    GL_FUNCTION(void, glBindVertexArray, GLuint array)                                            \
    GL_FUNCTION(void, glBufferData, GLenum target, GLsizeiptr size, const GLvoid* data,           \
                GLenum usage)                                                                     \
    GL_FUNCTION(void, glUseProgram, GLuint program)                                               \
    GL_FUNCTION(GLint, glGetUniformLocation, GLuint program, const GLchar* name)                  \
    GL_FUNCTION(void, glGenerateMipmap, GLenum target)                                            \
    GL_FUNCTION(void, glGetActiveUniform, GLuint program, GLuint index, GLsizei bufSize,          \
                GLsizei* length, GLint* size, GLenum* type, GLchar* name)                         \
    GL_FUNCTION(void, glUniformMatrix4fv, GLint location, GLsizei count, GLboolean transpose,     \
                const GLfloat* value)                                                             \
    GL_FUNCTION(void, glUniform1i, GLint location, GLint v0)                                      \
    GL_FUNCTION(void, glUniform2f, GLint location, GLfloat v0, GLfloat v1)                        \
    GL_FUNCTION(void, glUniform3f, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)            \
    GL_FUNCTION(void, glPushDebugGroup, GLenum source, GLuint id, GLsizei length,                 \
                const char* message)                                                              \
    GL_FUNCTION(void, glPopDebugGroup)                                                            \
    GL_FUNCTION(void, glGetActiveUniformBlockName, GLuint program, GLuint uniformBlockIndex,      \
                GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName)                       \
    GL_FUNCTION(void, glGetActiveUniformBlockiv, GLuint program, GLuint uniformBlockIndex,        \
                GLenum pname, GLint* params)                                                      \
    GL_FUNCTION(void, glGetActiveUniformsiv, GLuint program, GLsizei uniformCount,                \
                const GLuint* uniformIndices, GLenum pname, GLint* params)                        \
    GL_FUNCTION(void, glGetActiveUniformName, GLuint program, GLuint uniformIndex,                \
                GLsizei bufSize, GLsizei* length, GLchar* uniformName)                            \
    GL_FUNCTION(void, glBufferSubData, GLenum target, GLintptr offset, GLsizeiptr size,           \
                const GLvoid* data)                                                               \
    GL_FUNCTION(void, glObjectLabel, GLenum identifier, GLuint name, GLsizei length,              \
                const GLchar* label)                                                              \
    GL_FUNCTION(void, glDeleteProgram, GLuint program)                                            \
    GL_FUNCTION(void, glGetUniformiv, GLuint program, GLint location, GLint* params)              \
    GL_FUNCTION(void, glBindBufferRange, GLenum, GLuint, GLuint, GLintptr, GLsizeiptr)

#define GL_FUNCTION(return_type, name, ...) typedef return_type name##Type(__VA_ARGS__);

GL_FUNCTIONS_LIST
#undef GL_FUNCTION

namespace blaz {

struct Opengl {
    Error init(Window* window, bool debug_context);
    void swap_buffers(Window* window);
    void set_swap_interval(u32 interval);

    void* m_context;

#define GL_FUNCTION(return_type, name, ...) name##Type* name;

    GL_FUNCTIONS_LIST
#undef GL_FUNCTION

#define GL_FUNCTION(return_type, name, ...) name##Type* name;

    GL_OLD_FUNCTIONS_LIST
#undef GL_FUNCTION
};

}  // namespace blaz
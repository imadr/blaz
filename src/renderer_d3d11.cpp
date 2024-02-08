#ifdef RENDERER_D3D11

#ifdef _MSC_VER
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#endif

#include <d3d11.h>
#include <d3dcompiler.h>
#include <windows.h>
#include <windowsx.h>

#include "renderer.h"
#include "shader.h"

namespace blaz {

// void resize_callback(Window* window) {
// }

// struct Window_WIN32 {
//     HWND window_handle;
//     HDC device_context;
// };

// struct D3D11 {
//     ID3D11RenderTargetView* backbuffer;
//     IDXGISwapChain* swapchain;
//     ID3D11Device* device;
//     ID3D11DeviceContext* device_context;
// };

// static D3D11* m_d3d11;

// Error Renderer::init(Window* window) {
//     m_window = window;
//     Window_WIN32* m_win32 = (Window_WIN32*)window->m_os_data;
//     m_d3d11 = new D3D11();

//     DXGI_SWAP_CHAIN_DESC swapchain_desc;

//     ZeroMemory(&swapchain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

//     swapchain_desc.BufferCount = 1;
//     swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//     swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//     swapchain_desc.OutputWindow = m_win32->window_handle;
//     swapchain_desc.SampleDesc.Count = 4;
//     swapchain_desc.Windowed = TRUE;

//     D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
//                                   D3D11_SDK_VERSION, &swapchain_desc, &m_d3d11->swapchain,
//                                   &m_d3d11->device, NULL, &m_d3d11->device_context);

//     ID3D11Texture2D* backbuffer_pointer;
//     m_d3d11->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer_pointer);
//     m_d3d11->device->CreateRenderTargetView(backbuffer_pointer, NULL, &m_d3d11->backbuffer);
//     backbuffer_pointer->Release();
//     m_d3d11->device_context->OMSetRenderTargets(1, &m_d3d11->backbuffer, NULL);

//     bool debug = false;
// #ifdef DEBUG_RENDERER
//     debug = true;
// #endif
//     Error err;
//     if (err) {
//         return Error("Renderer::init ->\n" + err.message());
//     }

//     window->m_resize_callback = resize_callback;
//     return Error();
// }

// void Renderer::clear(u32 clear_flag, RGBA clear_color, float clear_depth) {
//     m_d3d11->device_context->ClearRenderTargetView(m_d3d11->backbuffer, clear_color.values);
// }

// void Renderer::present() {
//     m_d3d11->swapchain->Present(0, 0);
// }

// struct Shader_D3D11 {
//     ID3D11VertexShader* m_vertex_shader;
//     ID3D11PixelShader* m_pixel_shader;
// };

// Error Renderer::compile_shader(Shader* shader) {
//     Shader_D3D11* api_shader = new Shader_D3D11;

//     const char* vertex_entry_point = "VS";
//     const char* vertex_shader_model = "vs_5_0";
//     const char* pixel_entry_point = "PS";
//     const char* pixel_shader_model = "ps_5_0";

//     ID3DBlob* vertex_shader_blob = nullptr;
//     ID3DBlob* vertex_error_blob = nullptr;
//     ID3DBlob* pixel_shader_blob = nullptr;
//     ID3DBlob* pixel_error_blob = nullptr;

//     HRESULT vertex_result = D3DCompile(shader->m_vertex_shader_source.c_str(),
//                                        strlen(shader->m_vertex_shader_source.c_str()), nullptr,
//                                        nullptr, nullptr, vertex_entry_point, vertex_shader_model,
//                                        0, 0, &vertex_shader_blob, &vertex_error_blob);

//     if (FAILED(vertex_result)) {
//         if (vertex_error_blob) {
//             const char* error_str = (const char*)vertex_error_blob->GetBufferPointer();
//             size_t error_str_length = vertex_error_blob->GetBufferSize();
//             std::string error(error_str, error_str_length);
//             vertex_error_blob->Release();
//             return Error("Renderer::compile_shader: Failed to compile vertex shader, " + error);
//         }
//     }

//     HRESULT pixel_result = D3DCompile(shader->m_fragment_shader_source.c_str(),
//                                       strlen(shader->m_fragment_shader_source.c_str()), nullptr,
//                                       nullptr, nullptr, pixel_entry_point, pixel_shader_model, 0,
//                                       0, &pixel_shader_blob, &pixel_error_blob);

//     if (FAILED(pixel_result)) {
//         const char* error_str = (const char*)pixel_error_blob->GetBufferPointer();
//         size_t error_str_length = pixel_error_blob->GetBufferSize();
//         std::string error(error_str, error_str_length);
//         pixel_error_blob->Release();
//         return Error("Renderer::compile_shader: Failed to compile vertex shader, " + error);
//     }

//     ID3D11VertexShader* vertex_shader;
//     ID3D11PixelShader* pixel_shader;

//     m_d3d11->device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(),
//                                         vertex_shader_blob->GetBufferSize(), NULL,
//                                         &vertex_shader);
//     m_d3d11->device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(),
//                                        pixel_shader_blob->GetBufferSize(), NULL, &pixel_shader);

//     api_shader->m_vertex_shader = vertex_shader;
//     api_shader->m_pixel_shader = pixel_shader;
//     shader->m_api_data = api_shader;

//     return Error();
// }

// Error Renderer::upload_mesh(Mesh* mesh) {
//     return Error();
// }

// Error Renderer::reupload_mesh_buffers(Mesh* mesh) {
//     return Error();
// }

// Error Renderer::upload_texture_data(Texture* texture) {
//     return Error();
// }

// Error Renderer::set_shader_uniform_Mat4(Shader* shader, str uniform_name, Mat4 value) {
//     return Error();
// }

// Error Renderer::set_shader_uniform_i32(Shader* shader, str uniform_name, i32 value) {
//     return Error();
// }

// Error Renderer::set_shader_uniform_Vec2(Shader* shader, str uniform_name, Vec2 value) {
//     return Error();
// }

// Error Renderer::set_shader_uniform_Vec3(Shader* shader, str uniform_name, Vec3 value) {
//     return Error();
// }

// void Renderer::set_current_shader(Shader* shader) {
// }

// Error Renderer::create_texture(Texture* texture) {
//     return Error();
// }

// Error Renderer::create_framebuffer(Framebuffer* framebuffer) {
//     return Error();
// }

// void Renderer::debug_marker_start(str name) {
// }

// void Renderer::debug_marker_end() {
// }

// void Renderer::bind_default_framebuffer() {
// }

// void Renderer::set_viewport(f32 x, f32 y, f32 width, f32 height) {
//     D3D11_VIEWPORT viewport;
//     ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
//     viewport.TopLeftX = x;
//     viewport.TopLeftY = y;
//     viewport.Width = width;
//     viewport.Height = height;
//     m_d3d11->device_context->RSSetViewports(1, &viewport);
// }

// void Renderer::bind_framebuffer(Framebuffer* framebuffer) {
// }

// void Renderer::set_depth_test(bool enabled) {
// }

// void Renderer::set_face_culling(bool enabled) {
// }

// void Renderer::set_face_culling_mode(CullingMode mode) {
// }

// void Renderer::bind_textures(Pass* pass, Shader* shader) {
// }

// void Renderer::draw_vertex_array(Mesh* mesh) {
// }

}  // namespace blaz

#endif
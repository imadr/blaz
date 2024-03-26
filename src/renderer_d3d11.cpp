#ifdef _MSC_VER
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "D3DCompiler.lib")
#endif

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <windows.h>
#include <windowsx.h>

#include "game.h"
#include "logger.h"
#include "renderer.h"
#include "utils_win32.h"

namespace blaz {

struct Window_WIN32 {
    HWND window_handle;
    HDC device_context;
};

struct D3D11 {
    ID3D11RenderTargetView* backbuffer;
    IDXGISwapChain* swapchain;
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    ID3DUserDefinedAnnotation* annotation = nullptr;

    u32 swap_interval;
};

struct Shader_D3D11 {
    ID3D11VertexShader* m_vertex_shader;
    ID3D11PixelShader* m_pixel_shader;
};

struct Mesh_D3D11 {
    ID3D11Buffer* m_vertex_buffer;
    ID3D11Buffer* m_index_buffer;
    vec<D3D11_INPUT_ELEMENT_DESC> m_input_layout_desc;
};

D3D11* d3d11;

Error Renderer::init_api() {
    Window_WIN32* m_win32 = (Window_WIN32*)m_game->m_window.m_os_data;
    d3d11 = new D3D11();

    DXGI_SWAP_CHAIN_DESC swapchain_desc;

    ZeroMemory(&swapchain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapchain_desc.BufferCount = 1;
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.OutputWindow = m_win32->window_handle;
    swapchain_desc.SampleDesc.Count = 4;
    swapchain_desc.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
                                  D3D11_SDK_VERSION, &swapchain_desc, &d3d11->swapchain,
                                  &d3d11->device, NULL, &d3d11->device_context);

    ID3D11Texture2D* backbuffer_pointer;
    d3d11->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer_pointer);
    d3d11->device->CreateRenderTargetView(backbuffer_pointer, NULL, &d3d11->backbuffer);
    backbuffer_pointer->Release();
    d3d11->device_context->OMSetRenderTargets(1, &d3d11->backbuffer, NULL);

    bool debug = false;
#ifdef DEBUG_RENDERER
    debug = true;
#endif
    Error err;
    if (err) {
        return Error("Renderer::init ->\n" + err.message());
    }

    m_game->m_window.m_d3d11_data = d3d11;

    HRESULT res = d3d11->device_context->QueryInterface(__uuidof(ID3DUserDefinedAnnotation),
                                                        (void**)&d3d11->annotation);

    if (FAILED(res)) {
        return Error(win32_get_last_error());
    }

    return Error();
}

void Renderer::clear(u32 clear_flag, RGBA clear_color, float clear_depth) {
    d3d11->device_context->ClearRenderTargetView(d3d11->backbuffer, clear_color.values);
}

void Renderer::present() {
    d3d11->swapchain->Present(d3d11->swap_interval, 0);
}

static ID3DBlob* TMP__vshader_blob = NULL;

Error Renderer::compile_shader(Shader* shader) {
    Shader_D3D11* api_shader = new Shader_D3D11;

    const char* vertex_entry_point = "main";
    const char* vertex_shader_model = "vs_5_0";
    const char* pixel_entry_point = "main";
    const char* pixel_shader_model = "ps_5_0";

    ID3DBlob* vertex_shader_blob = nullptr;
    ID3DBlob* vertex_error_blob = nullptr;
    ID3DBlob* pixel_shader_blob = nullptr;
    ID3DBlob* pixel_error_blob = nullptr;

    HRESULT vertex_result = D3DCompile(shader->m_vertex_shader_source.c_str(),
                                       strlen(shader->m_vertex_shader_source.c_str()), nullptr,
                                       nullptr, nullptr, vertex_entry_point, vertex_shader_model, 0,
                                       0, &vertex_shader_blob, &vertex_error_blob);

    if (FAILED(vertex_result)) {
        if (vertex_error_blob) {
            const char* error_str = (const char*)vertex_error_blob->GetBufferPointer();
            size_t error_str_length = vertex_error_blob->GetBufferSize();
            std::string error(error_str, error_str_length);
            vertex_error_blob->Release();
            return Error("Renderer::compile_shader: Failed to compile vertex shader, " + error);
        }
    }
    TMP__vshader_blob = vertex_shader_blob;
    HRESULT pixel_result = D3DCompile(shader->m_fragment_shader_source.c_str(),
                                      strlen(shader->m_fragment_shader_source.c_str()), nullptr,
                                      nullptr, nullptr, pixel_entry_point, pixel_shader_model, 0, 0,
                                      &pixel_shader_blob, &pixel_error_blob);

    if (FAILED(pixel_result)) {
        const char* error_str = (const char*)pixel_error_blob->GetBufferPointer();
        size_t error_str_length = pixel_error_blob->GetBufferSize();
        std::string error(error_str, error_str_length);
        pixel_error_blob->Release();
        return Error("Renderer::compile_shader: Failed to compile vertex shader, " + error);
    }

    ID3D11VertexShader* vertex_shader;
    ID3D11PixelShader* pixel_shader;

    d3d11->device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(),
                                      vertex_shader_blob->GetBufferSize(), NULL, &vertex_shader);
    d3d11->device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(),
                                     pixel_shader_blob->GetBufferSize(), NULL, &pixel_shader);

    api_shader->m_vertex_shader = vertex_shader;
    api_shader->m_pixel_shader = pixel_shader;
    shader->m_api_data = api_shader;

    return Error();
}

Error Renderer::init_uniform_buffer(UniformBuffer* uniform_buffer) {
    return Error();
}

Error Renderer::set_uniform_buffer_data(UniformBuffer* uniform_buffer, str uniform_name,
                                        UniformValue value) {
    return Error();
}

void Renderer::bind_shader(Shader* shader) {
    Shader_D3D11* api_shader = (Shader_D3D11*)shader->m_api_data;
    d3d11->device_context->VSSetShader(api_shader->m_vertex_shader, 0, 0);
    d3d11->device_context->PSSetShader(api_shader->m_pixel_shader, 0, 0);
}

void Renderer::debug_marker_start(str name) {
    d3d11->annotation->BeginEvent(narrow_to_wide_str(name).c_str());
}

void Renderer::debug_marker_end() {
    d3d11->annotation->EndEvent();
}

void Renderer::bind_default_framebuffer() {
}

void Renderer::bind_framebuffer(Framebuffer* framebuffer) {
}

void Renderer::set_depth_test(bool enabled) {
}

void Renderer::set_face_culling(bool enabled, CullingMode mode, CullingOrder order) {
    D3D11_RASTERIZER_DESC rasterizer_desc = {};
    rasterizer_desc.FillMode = D3D11_FILL_SOLID;
    if (enabled) {
        rasterizer_desc.CullMode = mode == CullingMode::BACK ? D3D11_CULL_BACK : D3D11_CULL_FRONT;
    } else {
        rasterizer_desc.CullMode = D3D11_CULL_NONE;
    }
    rasterizer_desc.FrontCounterClockwise = order == CullingOrder::CCW ? TRUE : FALSE;
    ID3D11RasterizerState* rasterizer_state;
    d3d11->device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
    d3d11->device_context->RSSetState(rasterizer_state);
}

struct TMP_VERTEX_STRUCT {
    FLOAT X, Y, Z, R, G, B;
};

void Renderer::draw_vertex_array(Mesh* mesh) {
    Mesh_D3D11* api_mesh = (Mesh_D3D11*)mesh->m_api_data;

    ID3D11InputLayout* input_layout;
    d3d11->device->CreateInputLayout(api_mesh->m_input_layout_desc.data(),
                                     UINT(api_mesh->m_input_layout_desc.size()),
                                     TMP__vshader_blob->GetBufferPointer(),  // ??????????? Wtf
                                     TMP__vshader_blob->GetBufferSize(), &input_layout);
    d3d11->device_context->IASetInputLayout(input_layout);

    UINT stride = 0;
    for (i32 i = 0; i < mesh->m_attribs.size(); i++) {
        stride += mesh->m_attribs[i].second * sizeof(f32);
    }
    UINT offset = 0;

    d3d11->device_context->IASetVertexBuffers(0, 1, &api_mesh->m_vertex_buffer, &stride, &offset);
    d3d11->device_context->IASetIndexBuffer(api_mesh->m_index_buffer, DXGI_FORMAT_R32_UINT, 0);
    d3d11->device_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    d3d11->device_context->DrawIndexed(UINT(mesh->m_indices.size()), 0, 0);
}

Error Renderer::upload_mesh(Mesh* mesh) {
    ID3D11Buffer* vertex_buffer;
    D3D11_BUFFER_DESC vertex_buffer_desc;
    ZeroMemory(&vertex_buffer_desc, sizeof(vertex_buffer_desc));

    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(f32) * UINT(mesh->m_vertices.size());
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA vertex_buffer_data;
    ZeroMemory(&vertex_buffer_data, sizeof(vertex_buffer_data));
    vertex_buffer_data.pSysMem = mesh->m_vertices.data();

    d3d11->device->CreateBuffer(&vertex_buffer_desc, &vertex_buffer_data, &vertex_buffer);

    ID3D11Buffer* index_buffer;
    D3D11_BUFFER_DESC index_buffer_desc;
    ZeroMemory(&index_buffer_desc, sizeof(index_buffer_desc));
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = sizeof(u32) * UINT(mesh->m_indices.size());
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA index_buffer_data;
    ZeroMemory(&index_buffer_data, sizeof(index_buffer_data));
    index_buffer_data.pSysMem = mesh->m_indices.data();

    d3d11->device->CreateBuffer(&index_buffer_desc, &index_buffer_data, &index_buffer);

    vec<D3D11_INPUT_ELEMENT_DESC> input_layout_desc;

    u32 attribs_offset = 0;
    for (i32 i = 0; i < mesh->m_attribs.size(); i++) {
        auto attrib = mesh->m_attribs[i];
        str semantic;
        if (attrib.first.find("position") != std::string::npos) {
            semantic = "POSITION";
        } else if (attrib.first.find("normal") != std::string::npos) {
            semantic = "NORMAL";
        } else if (attrib.first.find("color") != std::string::npos) {
            semantic = "COLOR";
        }
        DXGI_FORMAT format = DXGI_FORMAT_R32G32B32_FLOAT;
        if (attrib.second == 2) {
            format = DXGI_FORMAT_R32G32_FLOAT;
        } else if (attrib.second == 3) {
            format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
        input_layout_desc.push_back(D3D11_INPUT_ELEMENT_DESC(semantic.c_str(), 0, format, 0,
                                                             attribs_offset * sizeof(f32),
                                                             D3D11_INPUT_PER_VERTEX_DATA, 0));
        attribs_offset += mesh->m_attribs[i].second;
    }

    Mesh_D3D11* api_mesh = new Mesh_D3D11;
    api_mesh->m_vertex_buffer = vertex_buffer;
    api_mesh->m_index_buffer = index_buffer;
    api_mesh->m_input_layout_desc = input_layout_desc;

    mesh->m_api_data = (void*)api_mesh;

    return Error();
}

void Renderer::set_viewport(u32 x, u32 y, u32 width, u32 height) {
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = (FLOAT)x;
    viewport.TopLeftY = (FLOAT)y;
    viewport.Width = (FLOAT)width;
    viewport.Height = (FLOAT)height;
    d3d11->device_context->RSSetViewports(1, &viewport);
}

void Renderer::set_swap_interval(u32 interval) {
    d3d11->swap_interval = interval;
}

}  // namespace blaz

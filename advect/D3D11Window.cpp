// Copyright (c) 2020 Toby Chen All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "App.h"
#include "BufferQueue.h"
#include "Camera.h"
#include "Renderer.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <glm\gtc\type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <windows.h>

// Predefined data
const float vertexData[] = {
    0.000000f,  0.866025f,  -0.500000f, 0.000000f,  -0.500000f, -0.866025f, -0.000000f, 1.000000f,  -0.000000f,
    0.353553f,  0.866025f,  -0.353553f, 0.612372f,  0.500000f,  -0.612373f, 0.707107f,  -0.000000f, -0.707107f,
    0.612372f,  -0.500000f, -0.612372f, 0.353553f,  -0.866025f, -0.353553f, 0.500000f,  0.866025f,  -0.000000f,
    0.866025f,  0.500000f,  -0.000000f, 1.000000f,  -0.000000f, -0.000000f, 0.866025f,  -0.500000f, -0.000000f,
    0.500000f,  -0.866025f, -0.000000f, 0.353553f,  0.866025f,  0.353553f,  0.612372f,  0.500000f,  0.612372f,
    0.707107f,  -0.000000f, 0.707106f,  0.612372f,  -0.500000f, 0.612372f,  0.353553f,  -0.866025f, 0.353553f,
    0.000000f,  -1.000000f, -0.000000f, -0.000000f, 0.866025f,  0.500000f,  0.000000f,  0.500000f,  0.866025f,
    0.000000f,  -0.000000f, 1.000000f,  0.000000f,  -0.500000f, 0.866025f,  -0.000000f, -0.866025f, 0.500000f,
    -0.353553f, 0.866025f,  0.353553f,  -0.612372f, 0.500000f,  0.612372f,  -0.707107f, -0.000000f, 0.707106f,
    -0.612372f, -0.500000f, 0.612372f,  -0.353553f, -0.866025f, 0.353553f,  -0.500000f, 0.866025f,  -0.000000f,
    -0.866025f, 0.500000f,  -0.000000f, -1.000000f, -0.000000f, -0.000000f, -0.866025f, -0.500000f, -0.000000f,
    -0.500000f, -0.866025f, -0.000000f, -0.353553f, 0.866025f,  -0.353553f, -0.612372f, 0.500000f,  -0.612372f,
    -0.707107f, -0.000000f, -0.707107f, -0.612372f, -0.500000f, -0.612372f, -0.353553f, -0.866025f, -0.353553f,
    -0.000000f, 0.500000f,  -0.866025f, 0.000000f,  -0.000000f, -1.000000f, -0.000000f, -0.866025f, -0.500000f};
const uint16_t indexData[] = {
    0,  4,  39, 41, 6,  7,  39, 5,  40, 0,  2,  3,  18, 41, 7,  40, 6,  1,  6,  12, 7,  4,  10, 5,  3,  2,  8,
    18, 7,  12, 5,  11, 6,  3,  9,  4,  11, 17, 12, 10, 14, 15, 8,  2,  13, 18, 12, 17, 10, 16, 11, 8,  14, 9,
    14, 21, 15, 13, 2,  19, 18, 17, 23, 15, 22, 16, 13, 20, 14, 16, 23, 17, 19, 2,  24, 18, 23, 28, 22, 26, 27,
    19, 25, 20, 22, 28, 23, 21, 25, 26, 18, 28, 33, 26, 32, 27, 24, 30, 25, 27, 33, 28, 25, 31, 26, 24, 2,  29,
    18, 33, 38, 31, 37, 32, 29, 35, 30, 32, 38, 33, 30, 36, 31, 29, 2,  34, 18, 38, 41, 36, 1,  37, 34, 39, 35,
    38, 1,  41, 35, 40, 36, 34, 2,  0,  0,  3,  4,  41, 1,  6,  39, 4,  5,  40, 5,  6,  6,  11, 12, 4,  9,  10,
    5,  10, 11, 3,  8,  9,  11, 16, 17, 10, 9,  14, 10, 15, 16, 8,  13, 14, 14, 20, 21, 15, 21, 22, 13, 19, 20,
    16, 22, 23, 22, 21, 26, 19, 24, 25, 22, 27, 28, 21, 20, 25, 26, 31, 32, 24, 29, 30, 27, 32, 33, 25, 30, 31,
    31, 36, 37, 29, 34, 35, 32, 37, 38, 30, 35, 36, 36, 40, 1,  34, 0,  39, 38, 37, 1,  35, 39, 40};
const uint32_t indexCount = 240;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;
D3D_DRIVER_TYPE g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device *g_pd3dDevice = nullptr;
ID3D11Device1 *g_pd3dDevice1 = nullptr;
ID3D11DeviceContext *g_pImmediateContext = nullptr;
ID3D11DeviceContext1 *g_pImmediateContext1 = nullptr;
IDXGISwapChain *g_pSwapChain = nullptr;
IDXGISwapChain1 *g_pSwapChain1 = nullptr;
ID3D11RenderTargetView *g_pRenderTargetView = nullptr;
ID3D11VertexShader *g_pVertexShader = nullptr;
ID3D11PixelShader *g_pPixelShader = nullptr;
ID3D11InputLayout *g_pVertexLayout = nullptr;
ID3D11Buffer *g_pVertexBuffer = nullptr;
ID3D11Buffer *g_pVertexBuffer2 = nullptr;
ID3D11Buffer *g_pVertexBuffer3 = nullptr;
ID3D11Buffer *g_pIndexBuffer = nullptr;
ID3D11RasterizerState *g_pRastState = nullptr;
ID3D11DepthStencilState *g_pDepthStencilState = nullptr;
ID3D11BlendState *g_pBlendState = nullptr;
Camera g_camera{{0.f, 0.f, 0.f}, {0.f, 0.f, 1.0f}};
uint32_t g_instanceCount = 0;

// Ideally we should generate this automatically from some reflection tool
class ParticleParameterSet
{
public:
    // Like a descriptor set, but more human friendly
    ParticleParameterSet()
    {
        const UINT reflectedGlobalCBSize = sizeof(float) * 16 * 2;
        D3D11_BUFFER_DESC desc = {
            reflectedGlobalCBSize, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0};
        g_pd3dDevice->CreateBuffer(&desc, nullptr, &globalCB);
    }

    ParticleParameterSet(const ParticleParameterSet &) = delete;
    ParticleParameterSet &operator=(const ParticleParameterSet &) = delete;
    ~ParticleParameterSet() { globalCB->Release(); }

    void bindAsOnlySet(ID3D11DeviceContext *ctx)
    {
        commit();
        ctx->VSSetConstantBuffers(0, 1, &globalCB);
    }

    void setModelMat(const void *data)
    {
        if (!globalCBMapped)
        {
            D3D11_MAPPED_SUBRESOURCE mapped;
            g_pImmediateContext->Map(globalCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
            globalCBMapped = mapped.pData;
        }
        auto *dest = static_cast<char *>(globalCBMapped);
        memcpy(dest, data, sizeof(float) * 16);
    }

    void setVpMat(const void *data)
    {
        if (!globalCBMapped)
        {
            D3D11_MAPPED_SUBRESOURCE mapped;
            g_pImmediateContext->Map(globalCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
            globalCBMapped = mapped.pData;
        }
        auto *dest = static_cast<char *>(globalCBMapped);
        memcpy(dest + sizeof(float) * 16, data, sizeof(float) * 16);
    }

    void commit()
    {
        if (globalCBMapped)
        {
            g_pImmediateContext->Unmap(globalCB, 0);
            globalCBMapped = false;
        }
    }

private:
    ID3D11Buffer *globalCB;
    void *globalCBMapped = nullptr;
};

ParticleParameterSet *g_pParticleParameterSet;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();

void Renderer::onAcquireBuffer(std::shared_ptr<const SimBuffer> buffer)
{
    printf("Begin renderer\n");
    while (!g_pd3dDevice1)
        ;
    UINT sz = sizeof(buffer->position);
    D3D11_BUFFER_DESC bufferDesc = {sz, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0};
    D3D11_SUBRESOURCE_DATA initialData = {buffer->position, 0, 0};
    auto *oldBuffer = g_pVertexBuffer3;
    g_pd3dDevice1->CreateBuffer(&bufferDesc, &initialData, &g_pVertexBuffer3);
    g_instanceCount = particleCount;
    if (oldBuffer)
        oldBuffer->Release();
    printf("End renderer\n");
    bufferQueue.releaseBuffer(buffer);
}

int Renderer::renderThreadLoop()
{
    auto profThread = globalProfiler.makeThreadData("window");
    profThread->beginEvent("init", ProfilerColor::DefaultBlue);
    if (FAILED(InitWindow(GetModuleHandle(NULL), SW_SHOWNORMAL)))
        return 0;

    if (FAILED(InitDevice()))
    {
        CleanupDevice();
        return 0;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pImmediateContext);
    profThread->endEvent("init");

    // Main message loop
    MSG msg = {0};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                app.notifyQuit();
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            profThread->beginEvent("draw", ProfilerColor::DefaultBlue);
            // Start the Dear ImGui frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGui::ShowDemoWindow();

            globalProfiler.drawImGui();

            ImGui::Render();
            Render();
            profThread->endEvent("draw");
            // Don't count vsync into draw
            // Present the information rendered to the back buffer to the front buffer (the screen)
            g_pSwapChain->Present(1, 0);
        }
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDevice();

    return (int)msg.wParam;
}

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = TEXT("MyWndCls");
    wcex.hIconSm = NULL;
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = {0, 0, 800, 600};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindow(TEXT("MyWndCls"), ("Simulation Monitor"),
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
                          rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
    if (!g_hWnd)
        return E_FAIL;

    ShowWindow(g_hWnd, nCmdShow);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const WCHAR *szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob **ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob *pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut,
                            &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char *>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob)
        pErrorBlob->Release();

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(g_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                               D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);

        if (hr == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1],
                                   numFeatureLevels - 1, D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel,
                                   &g_pImmediateContext);
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1 *dxgiFactory = nullptr;
    {
        IDXGIDevice *dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void **>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter *adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void **>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    // Create swap chain
    IDXGIFactory2 *dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void **>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void **>(&g_pd3dDevice1));
        if (SUCCEEDED(hr))
        {
            (void)g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1),
                                                      reinterpret_cast<void **>(&g_pImmediateContext1));
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void **>(&g_pSwapChain));
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D *pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    // Prepare shaders
    ID3DBlob *vsBlob, *psBlob;
    CompileShaderFromFile(L"Particle.hlsl", "vertexShader", "vs_5_0", &vsBlob);
    CompileShaderFromFile(L"Particle.hlsl", "pixelShader", "ps_5_0", &psBlob);
    g_pd3dDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &g_pVertexShader);
    g_pd3dDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &g_pPixelShader);

    // Create InputLayout
    D3D11_INPUT_ELEMENT_DESC desc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"INSTPOSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1}};
    g_pd3dDevice->CreateInputLayout(desc, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &g_pVertexLayout);

    // Create state blocks
    D3D11_RASTERIZER_DESC rasterizerDesc = {
        D3D11_FILL_SOLID, D3D11_CULL_NONE, TRUE, 0, 0.0f, 0.0f, FALSE, FALSE, FALSE, FALSE};
    g_pd3dDevice->CreateRasterizerState(&rasterizerDesc, &g_pRastState);

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = false;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    g_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &g_pDepthStencilState);

    // Create and manage shader parameters
    g_pParticleParameterSet = new ParticleParameterSet;

    // Buffers
    const float originData[] = {-3.141592653589793f, -1.2246467991473532e-16f};
    {
        D3D11_BUFFER_DESC bufferDesc = {sizeof(vertexData), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0};
        D3D11_SUBRESOURCE_DATA initialData = {vertexData, 0, 0};
        g_pd3dDevice1->CreateBuffer(&bufferDesc, &initialData, &g_pVertexBuffer);
        g_pVertexBuffer->AddRef();
        g_pVertexBuffer2 = g_pVertexBuffer;
    }
    {
        D3D11_BUFFER_DESC bufferDesc = {sizeof(indexData), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0};
        D3D11_SUBRESOURCE_DATA initialData = {indexData, 0, 0};
        g_pd3dDevice1->CreateBuffer(&bufferDesc, &initialData, &g_pIndexBuffer);
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if (g_pImmediateContext)
        g_pImmediateContext->ClearState();

    if (g_pParticleParameterSet)
        delete g_pParticleParameterSet;
    if (g_pRastState)
        g_pRastState->Release();
    if (g_pDepthStencilState)
        g_pDepthStencilState->Release();
    if (g_pBlendState)
        g_pBlendState->Release();
    if (g_pVertexBuffer)
        g_pVertexBuffer->Release();
    if (g_pVertexBuffer2)
        g_pVertexBuffer2->Release();
    if (g_pVertexBuffer3)
        g_pVertexBuffer3->Release();
    if (g_pIndexBuffer)
        g_pIndexBuffer->Release();
    if (g_pVertexLayout)
        g_pVertexLayout->Release();
    if (g_pVertexShader)
        g_pVertexShader->Release();
    if (g_pPixelShader)
        g_pPixelShader->Release();
    if (g_pRenderTargetView)
        g_pRenderTargetView->Release();
    if (g_pSwapChain1)
        g_pSwapChain1->Release();
    if (g_pSwapChain)
        g_pSwapChain->Release();
    if (g_pImmediateContext1)
        g_pImmediateContext1->Release();
    if (g_pImmediateContext)
        g_pImmediateContext->Release();
    if (g_pd3dDevice1)
        g_pd3dDevice1->Release();
    if (g_pd3dDevice)
        g_pd3dDevice->Release();
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    PAINTSTRUCT ps;
    HDC hdc;
    static bool orbitting = false;
    static short xPos, yPos;
    static short xPos2, yPos2;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_LBUTTONDOWN:
        orbitting = true;
        break;

    case WM_LBUTTONUP:
        orbitting = false;
        break;

    case WM_MOUSEMOVE:
        xPos2 = LOWORD(lParam);
        yPos2 = HIWORD(lParam);
        if (wParam & MK_LBUTTON)
        {
            g_camera.Orbit(Camera::EDir::Right, (xPos2 - xPos) * 0.01f);
            g_camera.Orbit(Camera::EDir::Down, (yPos2 - yPos) * 0.01f);
        }
        xPos = xPos2;
        yPos = yPos2;
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    // Clear the back buffer
    const float clearColor[] = {0.2f, 0.2f, 0.2f, 0.0f};
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);
    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

    // Render a triangle
    const float kScale = 1.0f / 256.f;
    const float scale[] = {kScale, 0.0f, 0.0f,   0.0f, 0.0f, kScale, 0.0f, 0.0f,
                           0.0f,   0.0f, kScale, 0.0f, 0.0f, 0.0f,   0.0f, 1.0f};

    auto test = g_camera.GetViewMatrix() * glm::vec4(2.0f, 0.0f, 0.0f, 1.0f);
    auto projMat = g_camera.MakeProjection(90.f / 3.141592654f * 180.f, 800.f / 600.f, 0.1f, 10.f);
    auto test2 = projMat * test;
    test2 /= test2.w;
    auto vpMat = projMat * g_camera.GetViewMatrix();
    g_pParticleParameterSet->setModelMat(scale);
    g_pParticleParameterSet->setVpMat(glm::value_ptr(vpMat));
    g_pParticleParameterSet->commit();

    ID3D11Buffer *const buffers[] = {g_pVertexBuffer, g_pVertexBuffer2, g_pVertexBuffer3};
    const UINT strides[] = {sizeof(float) * 3, sizeof(float) * 3, sizeof(float) * 2};
    const UINT offsets[] = {0, 0, 0};
    g_pImmediateContext->IASetVertexBuffers(0, 3, buffers, strides, offsets);
    g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    g_pImmediateContext->IASetInputLayout(g_pVertexLayout);
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
    g_pParticleParameterSet->bindAsOnlySet(g_pImmediateContext);
    g_pImmediateContext->RSSetState(g_pRastState);
    g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState, 0);
    g_pImmediateContext->OMSetBlendState(g_pBlendState, nullptr, 0xffffffff);
    g_pImmediateContext->DrawIndexedInstanced(indexCount, g_instanceCount, 0, 0, 0);

    // Render imgui
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

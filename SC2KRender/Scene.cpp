#include "Scene.h"
#include "MapSceneTile.h"
#include "AssetLoader.h"
#include "Sprite3D.h"
#include "Sprite2D.h"
#include "Model3D.h"
#include "ModifyModel.h"
#include "QuadSceneTile.h"
#include "SceneGenerator.h"
#include "SC2KRMath.h"

#define USING_SPRITES_3D FALSE
#define USING_SPRITES_2D FALSE
#define ARRAY_LENGTH TILES_DIMENSION * TILES_DIMENSION
#define USE_GRID FALSE
#define MAX_INSTANCES ARRAY_LENGTH * 16

typedef struct _D3DX11_TECHNIQUE_DESC {
  LPCSTR Name;
  UINT   Passes;
  UINT   Annotations;
} D3DX11_TECHNIQUE_DESC;

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Plane;

typedef SceneTile::VertexPos VPos;


void Scene::UpdateWindow(HWND hWnd)
{
  if (m_d3dDevice.Get() == nullptr)
    return; //Don't update window if not pre-initialized

  m_timer.ResetElapsedTime();

  RECT WindowRect;
  GetClientRect(hWnd, (LPRECT)&WindowRect);
  ClientToScreen(hWnd, (LPPOINT)&WindowRect.left);
  ClientToScreen(hWnd, (LPPOINT)&WindowRect.right);

  RECT ClientRect;
  GetClientRect(hWnd, &ClientRect);
  m_window_coords = ClientRect;

  float m_outputWidth_ = static_cast<float>(m_window_coords.right - m_window_coords.left);
  float m_outputHeight_ = static_cast<float>(m_window_coords.bottom - m_window_coords.top);
  bool window_resized = m_outputWidth_ != m_outputWidth || m_outputHeight_ != m_outputHeight;
  m_outputWidth = m_outputWidth_;
  m_outputHeight = m_outputHeight_;

  client_cx = static_cast<int>(m_outputWidth / 2);
  client_cy = static_cast<int>(m_outputHeight / 2);
  window_cx = static_cast<int>(WindowRect.left + client_cx);
  window_cy = static_cast<int>(WindowRect.top + client_cy);

  if (window_resized)
  {
    m_proj = Matrix::CreatePerspectiveFieldOfView(fov, m_outputWidth / m_outputHeight, .1f * scale, 256.f * scale);
    m_BasicEffect->SetProjection(m_proj);
    CreateResources();
  }
}

void Scene::PreInitialize(HWND window)
{
  m_window = window;
  CreateDevice();
  m_position = Vector3(96.f, 12.f, 96.f);
  m_world = Matrix::CreateScale(scale);
  m_view = Matrix::CreateLookAt(m_position, Vector3(0.f, 0.f, 0.f), Vector3::UnitY);
  m_BasicEffect->SetView(m_view);
  UpdateWindow(window);

#if USING_SPRITES_3D || USING_SPRITES_2D
  AssetLoader::LoadSprites(m_d3dDevice, L"assets/sprites");
#endif

  m_fxFactory = std::make_unique<DirectX::EffectFactory>(m_d3dDevice.Get());
  m_fxFactory->SetSharing(false);
  m_fxFactory->EnableNormalMapEffect(false); //(not required anymore)
  
  
  AssetLoader::LoadModels(m_d3dDevice, m_fxFactory, L"assets/models");
  AssetLoader::LoadCustomAssets(L"assets/assetmap.cfg");
  AssetLoader::LoadXBLDVisibilityParameters(L"assets/visiblemap.cfg");

  auto it = AssetLoader::mmodels->begin();
  for (; it != AssetLoader::mmodels->end(); ++it)
  {

    it->second->UpdateEffects([&](DirectX::IEffect* effect)
    { 
      if (auto nmap = dynamic_cast<DirectX::NormalMapEffect*>(effect))
       nmap->SetInstancingEnabled(true);
    });
    for (auto mit = it->second->meshes.begin(); mit != it->second->meshes.end(); ++mit)
    {
      auto mesh = mit->get();
      assert(mesh != 0);

      for (auto it = mesh->meshParts.begin(); it != mesh->meshParts.end(); ++it)
      {
        auto part = it->get();
        assert(part != 0);
        auto il = *part->vbDecl;
        il.push_back(NormalEffectInstance[3]);
        il.push_back(NormalEffectInstance[4]);
        il.push_back(NormalEffectInstance[5]);
        CreateInputLayoutFromEffect(m_d3dDevice.Get(), part->effect.get(),
            il.data(), il.size(),
            part->inputLayout.ReleaseAndGetAddressOf());
      }
    }
  }
}

void Scene::SetFOV(float value)
{
  fov = static_cast<float>(value * (M_PI / 180.f));
  m_proj = Matrix::CreatePerspectiveFieldOfView(fov, m_outputWidth / m_outputHeight, .1f * scale, 256.f * scale);
  m_BasicEffect->SetProjection(m_proj);
}

void Scene::SetScale(float value)
{
  float scale_clamp = std::clamp(value, 0.1f, 1.f);
  float scale_multiplier = value / scale;
  scale = scale_clamp;
  m_world = Matrix::CreateScale(scale);
  for (Model3D* model3d : v_model3d)
  {
    model3d->Update(this);
  }
  m_position *= scale_multiplier;
  m_proj = Matrix::CreatePerspectiveFieldOfView(fov, m_outputWidth / m_outputHeight, .1f * scale, 256.f * scale);
  m_BasicEffect->SetProjection(m_proj);
}

void Scene::SetEnableVSync(bool value)
{
  use_vsync = value;
}

void Scene::SetMSAA(unsigned int value)
{
  msaa_value = static_cast<unsigned int>(std::pow(2, value));
  CreateResources();
}

void Scene::Initialize(Map& map)
{
  render_scene = false;

  if (map_tiles != nullptr)
  {
    delete[] map_tiles;
    map_tiles = nullptr;
  }
  map_tiles = std::move(map.tiles);

  while (!v_sprite3d.empty())
  {
    delete v_sprite3d.at(0);
    v_sprite3d.erase(v_sprite3d.begin());
  }
  while (!v_sprite2d.empty())
  {
    delete v_sprite2d.at(0);
    v_sprite2d.erase(v_sprite2d.begin());
  }
  while (!v_model3d.empty())
  {
    delete v_model3d.at(0);
    v_model3d.erase(v_model3d.begin());
  }

  while (!fill_tiles.empty())
  {
    delete fill_tiles.at(0);
    fill_tiles.erase(fill_tiles.begin());
  }  

  if (tiles != nullptr)
  {
    for (size_t i = 0; i < ARRAY_LENGTH; ++i)
    {
      MapSceneTile* tile = tiles[i];
      if (tile != nullptr)
      {
        SceneTile* sea_tile = tile->sea_tile;
        if (sea_tile != nullptr)
        {
          delete sea_tile;
        }
        delete tile;
      }
    }
    delete[] tiles;
  }

  m_model3d.clear();

  tiles = new MapSceneTile*[ARRAY_LENGTH];

  ModifyModel::SetMapRotation(map.rotation);  
  SceneGenerator::Generate(map_tiles, tiles, v_model3d, fill_tiles, map.rotation);

  m_position = Vector3(96.f, 12.f, 96.f);
  m_position *= scale;
  SetScale(scale);

  printf("Map Rotation: %d\n", map.rotation);
  printf("Money: %d\n", map.money_supply);
  printf("City Age: %d\n", map.days_elapsed);
  printf("City Founded: %d\n", map.founding_year);
  printf("Rendering %d 3d models\n", v_model3d.size());
  
  for (size_t i = 0; i < v_model3d.size(); ++i)
  {  
    int32_t index = v_model3d[i]->model_id;
    m_model3d[index].push_back(v_model3d[i]);
  }
  

  for (auto m_model3d_it : m_model3d)
  {
    size_t instance_c = m_model3d_it.second.size();
    int32_t index = m_model3d_it.first;

    std::unique_ptr<DirectX::XMFLOAT3X4[]> instanceData;
    instanceData.reset(new DirectX::XMFLOAT3X4[instance_c]);
    for (size_t i = 0; i < instance_c; ++i)
    {
      Model3D* model3d = m_model3d_it.second[i];
      DirectX::XMFLOAT3X4 xm;
      DirectX::XMStoreFloat3x4(&xm, model3d->m_world);
      instanceData[i] = xm;
    }
    auto desc = CD3D11_BUFFER_DESC(
      instance_c * sizeof(DirectX::XMFLOAT3X4),
      D3D11_BIND_VERTEX_BUFFER,
      D3D11_USAGE_DYNAMIC,
      D3D11_CPU_ACCESS_WRITE, 0U,
      sizeof(DirectX::XMFLOAT3X4)
    );
    
    D3D11_SUBRESOURCE_DATA initData = { instanceData.get(), 0, 0 };
    HRESULT hr = m_d3dDevice->CreateBuffer(&desc, &initData, m_InstanceBuffer[index].ReleaseAndGetAddressOf());
    assert(SUCCEEDED(hr));
  } 
  
  render_scene = true;
}

bool Scene::SetFullScreen(BOOL value)
{ 
  return  m_swapChain->SetFullscreenState(value, NULL) == S_OK;
}

void Scene::CreateDevice()
{
  UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; //Direct2D needs this flag
  static const D3D_FEATURE_LEVEL featureLevels[] =
  {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1,
  };

  Microsoft::WRL::ComPtr<ID3D11Device> device;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

  HRESULT hResult =
    D3D11CreateDevice(
      nullptr,                            // specify nullptr to use the default adapter
      D3D_DRIVER_TYPE_HARDWARE,
      nullptr,
      creationFlags,
      featureLevels,
      _countof(featureLevels),
      D3D11_SDK_VERSION,
      device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
      &m_featureLevel,                    // returns feature level of device created
      context.ReleaseAndGetAddressOf()    // returns the device immediate context
    );

  //device->CreateDeferredContext(0, &pd3dDeferredContext);

  device.As(&m_d3dDevice);
  context.As(&m_d3dContext);

  m_world = DirectX::SimpleMath::Matrix::Identity;
  m_states = std::make_unique<DirectX::CommonStates>(m_d3dDevice.Get());

  
  m_BasicEffect = std::make_unique<DirectX::BasicEffect>(m_d3dDevice.Get());
  m_BasicEffect->SetVertexColorEnabled(true);
  {
    void const* shaderByteCode;
    size_t byteCodeLength;
    m_BasicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
    m_d3dDevice->CreateInputLayout(
      DirectX::VertexPositionColor::InputElements,
      DirectX::VertexPositionColor::InputElementCount,
      shaderByteCode, byteCodeLength,
      m_BasicInputLayout.ReleaseAndGetAddressOf());
  }
  
  m_NormalMapEffect = std::make_unique<DirectX::NormalMapEffect>(m_d3dDevice.Get());
  //m_NormalMapEffect->EnableDefaultLighting();
  m_NormalMapEffect->SetFogEnabled(true);
  //m_NormalMapEffect->SetVertexColorEnabled(false);
  m_NormalMapEffect->SetInstancingEnabled(true);
  {
    constexpr UINT NormalEffectInstanceCount = std::size(NormalEffectInstance);
    HRESULT hr = CreateInputLayoutFromEffect(
      m_d3dDevice.Get(),
      m_NormalMapEffect.get(),
      NormalEffectInstance, NormalEffectInstanceCount,
      m_NormalInputLayout.ReleaseAndGetAddressOf());
    assert(SUCCEEDED(hr));
  }


  //m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(m_d3dContext.Get());
  m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(m_d3dContext.Get());
  m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_d3dContext.Get());

  CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
    D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
  m_d3dDevice->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf());

}

void Scene::CreateResources()
{
  // Clear the previous window size specific context.
  ID3D11RenderTargetView* nullViews[] = { nullptr };
  m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
  //pd3dDeferredContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
  m_renderTargetView.Reset();
  m_depthStencilView.Reset();
  m_d3dContext->Flush();
  //pd3dDeferredContext->Flush();

  const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
  const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
  m_viewport = DirectX::SimpleMath::Viewport(0.0f, 0.0f, m_outputWidth, m_outputHeight);

  const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
  const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_D32_FLOAT_S8X24_UINT; 
  constexpr UINT backBufferCount = 2;

  if (max_supported_sample_count == 0)
  {
    max_supported_sample_count = 1;
    for (unsigned int s = 1; s < 5; ++s)
    {
      UINT num_qual_levels;
      unsigned int sample_count = static_cast<unsigned int>(std::pow(2, s));
      HRESULT multi_sample_hr = m_d3dDevice->CheckMultisampleQualityLevels(
        DXGI_FORMAT_B8G8R8A8_UNORM,
        sample_count, &num_qual_levels);

      if (multi_sample_hr != S_OK || num_qual_levels == 0)
      {
        break;
      }
      max_supported_sample_count = sample_count;
    }
    msaa_value = max_supported_sample_count;
    printf("Max MSAA Sample Count: %d\n", max_supported_sample_count);
  }

  // If the swap chain already exists, resize it, otherwise create one.
  //TODO This is supposed to work, but it doesn't
  if (m_swapChain)
  {
    //m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);
    m_swapChain.Reset();
  }
  //else
  {
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    m_d3dDevice.As(&dxgiDevice);
    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
    Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
    dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
    dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));

    // Create a descriptor for the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = backBufferWidth;
    swapChainDesc.Height = backBufferHeight;
    swapChainDesc.Format = backBufferFormat;
    //swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Count = msaa_value;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = backBufferCount;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
    fsSwapChainDesc.Windowed = TRUE;

    // Create a SwapChain from a Win32 window.
    HRESULT hr_dxgifactory =
      dxgiFactory->CreateSwapChainForHwnd(
        m_d3dDevice.Get(),
        m_window,
        &swapChainDesc,
        &fsSwapChainDesc,
        nullptr,
        m_swapChain.ReleaseAndGetAddressOf()
      );

    dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER);

    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &wfactory);
    wfactory->CreateTextFormat(L"Lucida Console", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0f, L"en-us", &format);

    Microsoft::WRL::ComPtr<ID2D1Factory1> factory;
    D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, factory.GetAddressOf());

    Microsoft::WRL::ComPtr<ID2D1Device> dev;
    factory->CreateDevice(dxgiDevice.Get(), dev.GetAddressOf());

    dev->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, device_context.GetAddressOf());

    D2D1_BITMAP_PROPERTIES1 bp;
    bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    bp.dpiX = 96.0f;
    bp.dpiY = 96.0f;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bp.colorContext = nullptr;

    Microsoft::WRL::ComPtr<IDXGISurface> dxgiBuffer;
    m_swapChain->GetBuffer(0, __uuidof(IDXGISurface), &dxgiBuffer);

    Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBitmap;
    device_context->CreateBitmapFromDxgiSurface(dxgiBuffer.Get(), &bp, &targetBitmap);
    device_context->SetTarget(targetBitmap.Get());
    device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush);
  }
  // Obtain the backbuffer for this window which will be the final 3D rendertarget.
  Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
  m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
  m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf());

  CD3D11_TEXTURE2D_DESC depthStencilDesc;
  CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  if (msaa_value)
  {
    depthStencilDesc = CD3D11_TEXTURE2D_DESC(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1,
      D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0, msaa_value, 0);
    depthStencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2DMS);
  }
  else
  {
    depthStencilDesc = CD3D11_TEXTURE2D_DESC(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);
    depthStencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D);
  }

  Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
  m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf());
  m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf());
}

void Scene::Clear()
{
  CD3D11_VIEWPORT viewport(0.0f, 0.0f, m_outputWidth, m_outputHeight);
  m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), DirectX::Colors::CornflowerBlue);
  m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
  m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
  m_d3dContext->RSSetViewports(1, &viewport);
}


void Scene::Tick()
{
  if (GetActiveWindow() != m_window)
  {
    Sleep(10);
    return;
  }

  if (focused)
  {
    SetCursorPos(window_cx, window_cy);
  }
  m_timer.Tick([&]()
  {
    Update(m_timer);
  });

  Render();
}
#define M_PI_3 1.04719755119659774615

void Scene::MouseLook(int x, int y)
{
  float dx = static_cast<float>(x - window_cx) * mouse_move_speed;
  float dy = static_cast<float>(y - window_cy) * mouse_move_speed;
  yaw = atan2(sin((yaw + dx) - (float)M_PI), cos((yaw + dx) - (float)M_PI)) + (float)M_PI; // wrap angle between 0 and 2pi
  pitch = std::clamp(pitch + dy, (float)-M_PI_3, (float)M_PI_3);
  m_view = Matrix::CreateTranslation(-m_position);
  m_view *= Matrix::CreateFromYawPitchRoll(yaw, 0, 0);
  m_view *= Matrix::CreateFromYawPitchRoll(0, pitch, 0);
}

unsigned int Scene::GetMaxSampleCount()
{
  return max_supported_sample_count;
}

void Scene::SetMovementSpeed(float value)
{
  move_speed = value;
}

void Scene::SetMouseSpeed(float value)
{
  mouse_move_speed = value;
}

void Scene::SetRenderDebugUI(bool value)
{
  render_debug_ui = value;
}

void Scene::SetAABBFrustumCulling(bool value)
{
  use_aabb_frustum_culling = value;
}

void Scene::Update(DX::StepTimer const& timer)
{
  float fps = static_cast<float>(timer.GetFramesPerSecond());
  float delta_fps = fps > 0.01f ? 100 / fps : 1;
  float fps_move_speed = move_speed * delta_fps;

  if (GetAsyncKeyState(0x57)) //W (move forward)
  {
    m_position.x += cos(yaw - (float)M_PI_2) * fps_move_speed;
    m_position.z += sin(yaw - (float)M_PI_2) * fps_move_speed;
    m_position.y -= sin(pitch) * fps_move_speed;
  }
  else if (GetAsyncKeyState(0x53)) //S (move backwards)
  {
    m_position.x -= cos(yaw - (float)M_PI_2) * fps_move_speed;
    m_position.z -= sin(yaw - (float)M_PI_2) * fps_move_speed;
    m_position.y += sin(pitch) * fps_move_speed;
  }

  if (GetAsyncKeyState(0x41)) //A (strafe left)
  {
    m_position.x -= cos(yaw) * fps_move_speed;
    m_position.z -= sin(yaw) * fps_move_speed;
  }
  else if (GetAsyncKeyState(0x44)) //D (strafe right)
  {
    m_position.x += cos(yaw) * fps_move_speed;
    m_position.z += sin(yaw) * fps_move_speed;
  }

  if (GetAsyncKeyState(0x52)) //R (strafe up)
    m_position.y += fps_move_speed;
  else if (GetAsyncKeyState(0x46)) //F (strafe down)
    m_position.y -= fps_move_speed;
}

void Scene::MouseClick()
{
  if (!render_debug_ui) return;
  Vector3 near_plane = m_viewport.Unproject(Vector3((float)client_cx, (float)client_cy, 0.f), m_proj, m_view, m_world);
  Vector3 far_plane = m_viewport.Unproject(Vector3((float)client_cx, (float)client_cy, 1.f), m_proj, m_view, m_world);
  Vector3 normalized = far_plane - near_plane;
  normalized.Normalize();
  DirectX::SimpleMath::Ray ray(near_plane, normalized);

  for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
  {
    for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
    {
      MapSceneTile* t = tiles[x + TILES_DIMENSION * y];
      float distance1, distance2;
      bool tri1 = ray.Intersects(t->v_pos[VPos::TOP_LEFT], t->v_pos[VPos::BOTTOM_LEFT], t->v_pos[VPos::TOP_RIGHT], distance1);
      bool tri2 = ray.Intersects(t->v_pos[VPos::BOTTOM_RIGHT], t->v_pos[VPos::BOTTOM_LEFT], t->v_pos[VPos::TOP_RIGHT], distance2);
      if (tri1 || tri2)
      {        
        t->ColorTile(DirectX::Colors::Crimson);
        printf("[Debug] Map Tile(%d, %d): Map Height: %d, XTER: %x, Water Height:%d, ALTM: %d, XBLD: %x, XZON: %x, XUND: %x, XBIT: %x\n",
          x, y, t->map_tile->height, t->map_tile->xter, t->map_tile->water_height, t->map_tile->altm, t->map_tile->xbld, t->map_tile->xzon,
          t->map_tile->xund, t->map_tile->xbit);
        return;
      }
    }
  }
}

void Scene::MultiplyMovementSpeed(float value)
{
  move_speed *= value;
}

void Scene::SetRenderModels(bool value)
{
  render_models = value;
}


void Scene::DrawInstancedModel(Model3D* model3d, bool alpha, size_t count)
{
  for (auto mit = model3d->model->meshes.cbegin(); mit != model3d->model->meshes.cend(); ++mit)
  {
    DirectX::ModelMesh* mesh = mit->get();
    mesh->PrepareForRendering(m_d3dContext.Get(), *m_states.get(), alpha);
    for (auto it = mesh->meshParts.cbegin(); it != mesh->meshParts.cend(); ++it)
    {
      auto part = it->get();
      if (part->isAlpha == alpha)
      {
        auto imatrices = dynamic_cast<DirectX::IEffectMatrices*>(part->effect.get());
        if (imatrices)
        {
          imatrices->SetMatrices(DirectX::SimpleMath::Matrix::Identity, m_view, m_proj);
        }
        part->DrawInstanced(m_d3dContext.Get(), part->effect.get(), part->inputLayout.Get(), count);
      }
    }
  }
}

void Scene::Render()
{

  CD3D11_VIEWPORT viewport(0.0f, 0.0f, m_outputWidth, m_outputHeight);
  m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), DirectX::Colors::CornflowerBlue);
  m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
  m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
  m_d3dContext->RSSetViewports(1, &viewport);

  if (render_scene)
  {
    m_d3dContext->OMSetBlendState(m_states->Opaque(), NULL, 0xFFFFFFFF);
    m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    m_d3dContext->RSSetState(m_states->CullNone());
    m_BasicEffect->SetWorld(m_world);
    m_BasicEffect->SetView(m_view);
    m_BasicEffect->Apply(m_d3dContext.Get());

    //auto sampler = m_states->LinearClamp();
    //m_d3dContext->PSSetSamplers(0, 1, &sampler);

    m_d3dContext->IASetInputLayout(m_BasicInputLayout.Get());
    m_batch->Begin();

    for (const QuadSceneTile* qst : fill_tiles)
    {
      m_batch->DrawQuad(qst->vpc[0], qst->vpc[1], qst->vpc[2], qst->vpc[3]);
    }       
   
    for (unsigned int i = 0; i < ARRAY_LENGTH; ++i)
    {
      const SceneTile* t = tiles[i]; //object slice is ok
      //t should never be null
      if (t->render == ALWAYS || t->render == MODEL_HIDDEN || !render_models)
      {
        
        m_batch->DrawTriangle(t->vpc_pos[VPos::TOP_LEFT], t->vpc_pos[VPos::BOTTOM_LEFT], t->vpc_pos[VPos::TOP_RIGHT]);
        m_batch->DrawTriangle(t->vpc_pos[VPos::BOTTOM_RIGHT], t->vpc_pos[VPos::BOTTOM_LEFT], t->vpc_pos[VPos::TOP_RIGHT]);
      }
    }

#if USE_GRID
    for (int x = 0; x < TILES_DIMENSION; x++)
      m_batch->DrawLine(DirectX::VertexPositionColor(Vector3(x, 4, 0), DirectX::Colors::Black),
        DirectX::VertexPositionColor(Vector3(x, 4, 128), DirectX::Colors::Black));
    for (int y = 0; y < TILES_DIMENSION; y++)
      m_batch->DrawLine(DirectX::VertexPositionColor(Vector3(0, 4, y), DirectX::Colors::Black),
        DirectX::VertexPositionColor(Vector3(128, 4, y), DirectX::Colors::Black));
#endif    

    m_batch->End();



    if (render_models)
    {
      UINT stride = sizeof(DirectX::XMFLOAT3X4);
      UINT offset = 0;
      m_d3dContext->IASetInputLayout(m_NormalInputLayout.Get());

      std::map<int32_t, std::vector<Model3D*>>::iterator it;
      for (it = m_model3d.begin(); it != m_model3d.end(); ++it)
      {
        int32_t index = it->first;
        size_t instance_c = it->second.size();
        Model3D* model3d = it->second.at(0);

        m_d3dContext->IASetVertexBuffers(1, 1, m_InstanceBuffer[index].GetAddressOf(), &stride, &offset);
        DrawInstancedModel(model3d, false, instance_c);
        if (model3d->alpha)
          DrawInstancedModel(model3d, true, instance_c);
      }
      m_d3dContext->IASetInputLayout(m_BasicInputLayout.Get());
      m_d3dContext->RSSetState(m_states->CullNone());
      m_BasicEffect->Apply(m_d3dContext.Get());
    }

    //Separating the water into its own loop afterwards prevents 'blinds effect'
    m_d3dContext->OMSetBlendState(m_states->AlphaBlend(), NULL, 0xFFFFFFFF);
    m_batch->Begin();
    for (unsigned int i = 0; i < ARRAY_LENGTH; ++i)
    {
      const SceneTile* w = tiles[i]->sea_tile;
      if (w == nullptr) continue;
      m_batch->DrawTriangle(w->vpc_pos[VPos::TOP_LEFT], w->vpc_pos[VPos::BOTTOM_LEFT], w->vpc_pos[VPos::TOP_RIGHT]);
      m_batch->DrawTriangle(w->vpc_pos[VPos::BOTTOM_RIGHT], w->vpc_pos[VPos::BOTTOM_LEFT], w->vpc_pos[VPos::TOP_RIGHT]);
    }
    for (const QuadSceneTile* qst : fill_tiles_alpha)
    {
      m_batch->DrawQuad(qst->vpc[0], qst->vpc[1], qst->vpc[2], qst->vpc[3]);
    }
    m_batch->End();



    if (render_debug_ui)
    {
      std::wstring translation = L"Yaw: " + std::to_wstring(yaw) + L", Pitch: " + std::to_wstring(pitch);
      std::wstring position = L"Position: <" + std::to_wstring(m_position.x) + L", " + std::to_wstring(m_position.y) + L", " + std::to_wstring(m_position.z) + L">";
      std::wstring extra = L"Scale: " + std::to_wstring(scale) + L", Speed: " + std::to_wstring(move_speed);
      std::wstring fps = L"FPS: " + std::to_wstring(m_timer.GetFramesPerSecond());
      Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout1, text_layout2, text_layout3, text_layout4, text_layout5;
      wfactory->CreateTextLayout(translation.c_str(), translation.size(), format, m_outputWidth, m_outputHeight, &text_layout1);
      wfactory->CreateTextLayout(position.c_str(), position.size(), format, m_outputWidth, m_outputHeight, &text_layout2);
      wfactory->CreateTextLayout(extra.c_str(), extra.size(), format, m_outputWidth, m_outputHeight, &text_layout3);
      wfactory->CreateTextLayout(L"+", 1, format, m_outputWidth, m_outputHeight, &text_layout4);
      wfactory->CreateTextLayout(fps.c_str(), fps.size(), format, m_outputWidth, m_outputHeight, &text_layout5);

      device_context->BeginDraw();
      device_context->DrawTextLayout(D2D1::Point2F(2.0f, 2.0f), text_layout1.Get(), whiteBrush.Get());
      device_context->DrawTextLayout(D2D1::Point2F(2.0f, 2.0f + 10.f), text_layout2.Get(), whiteBrush.Get());
      device_context->DrawTextLayout(D2D1::Point2F(2.0f, 2.0f + 20.f), text_layout3.Get(), whiteBrush.Get());
      device_context->DrawTextLayout(D2D1::Point2F(2.0f, 2.0f + 30.f), text_layout5.Get(), whiteBrush.Get());
      //Account for text size, currently set to size of 10.f
      device_context->DrawTextLayout(D2D1::Point2F(client_cx - 2.5f, client_cy - 5.f), text_layout4.Get(), whiteBrush.Get());

      //Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout6;
      //wfactory->CreateTextLayout(L"X", 1, format, m_outputWidth, m_outputHeight, &text_layout6);
      //Vector3 v = m_viewport.Project(v_model3d.at(0)->origin, m_proj, m_view, m_world);
      //Vector3 v1 = v_model3d.at(0)->model->meshes.at(0)->boundingSphere.Center + v_model3d.at(0)->origin;
      //Vector3 v = m_viewport.Project(v1, m_proj, m_view, m_world);
      //device_context->DrawTextLayout(D2D1::Point2F(v.x, v.y), text_layout6.Get(), whiteBrush.Get());

      device_context->EndDraw();
    }
  }

  HRESULT hr = m_swapChain->Present(use_vsync, 0);
}

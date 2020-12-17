#include "Scene.h"
#include "MapSceneTile.h"
#include "AssetLoader.h"
#include "Sprite3D.h"
#include "Sprite2D.h"
#include "Model3D.h"
#include "AssetMaps.h"

#define USING_SPRITES_3D FALSE
#define USING_SPRITES_2D FALSE
#define USING_MODELS TRUE

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Plane;
typedef SceneTile::VertexPos VPos;

void Scene::UpdateWindow(HWND hWnd)
{
  if (m_d3dDevice.Get() == nullptr)
    return; //Don't update window if not pre-initialized

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

  client_cx = m_outputWidth / 2;
  client_cy = m_outputHeight / 2;
  window_cx = static_cast<int>(WindowRect.left + client_cx);
  window_cy = static_cast<int>(WindowRect.top + client_cy);

  if (window_resized)
  {
    m_proj = Matrix::CreatePerspectiveFieldOfView(fov, m_outputWidth / m_outputHeight, .1f * scale, 256.f * scale);
    m_effect->SetProjection(m_proj);
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
  m_effect->SetView(m_view);
  SetRenderDistance(render_distance);
  UpdateWindow(window);

#if USING_SPRITES_3D || USING_SPRITES_2D
  AssetLoader::LoadSprites(m_d3dDevice, L"assets/sprites");
#endif

#if USING_MODELS
  m_fxFactory = std::make_unique<DirectX::EffectFactory>(m_d3dDevice.Get());
  AssetLoader::LoadModels(m_d3dDevice, m_fxFactory, L"assets/models");
  LoadCustomAssets(L"assets/assetmap.cfg");
#endif
}

void Scene::SetScale(float value)
{
  float scale_clamp = std::clamp(value, 0.1f, 1.f);
  float scale_multiplier = value / scale;
  scale = scale_clamp;
  move_speed = base_move_speed * scale;
  m_world = Matrix::CreateScale(scale);
  for (Model3D* model3d : v_model3d)
  {
    model3d->Update(this);
  }
  SetRenderDistance(render_distance);
  m_position *= scale_multiplier;
  m_proj = Matrix::CreatePerspectiveFieldOfView(fov, m_outputWidth / m_outputHeight, .1f * scale, 256.f * scale);
  m_effect->SetProjection(m_proj);
}

void Scene::SetEnableVSync(bool value)
{
  use_vsync = value;
}

void Scene::SetMSAA(bool value)
{
  use_4x_msaa = value;
  CreateResources();
}

void Scene::SetRenderDistance(float value)
{
  render_distance = value;
  use_render_distance = value > 0.f;
  scaled_render_distance = render_distance * scale;

  float scaled_render_start = scaled_render_distance - (16.f * scale);
  m_effect->SetFogEnabled(use_render_distance);
  m_effect->SetFogStart(scaled_render_start);
  m_effect->SetFogEnd(scaled_render_distance);
  m_effect->SetFogColor(DirectX::Colors::CornflowerBlue);

  for (Model3D* model3d : v_model3d)
  {
    model3d->model->UpdateEffects(
      [&](DirectX::IEffect* effect)
    {
      auto fog = dynamic_cast<DirectX::IEffectFog*>(effect);
      if (fog)
      {
        fog->SetFogEnabled(use_render_distance);
        fog->SetFogColor(DirectX::Colors::CornflowerBlue);
        fog->SetFogStart(scaled_render_start);
        fog->SetFogEnd(scaled_render_distance);
      }
    });
  }
}

void Scene::Initialize(MapTile* map_tiles)
{
  render_scene = false;
  delete[] tiles;
  delete[] sea_tiles;
  fill_tiles.clear();
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

  map_orientation = 0b0000;
  tiles = new MapSceneTile[TILES_DIMENSION * TILES_DIMENSION];
  sea_tiles = new SceneTile[TILES_DIMENSION * TILES_DIMENSION];

  //first = index to v_model3d
  //second = index to tiles
  ModelTileVector highway_model_vec;
  for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
  {
    for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
    {
      MapSceneTile& t = tiles[x + TILES_DIMENSION * y];
      t.SetOrigin(x, y);
      const MapTile* map_tile = &map_tiles[x + TILES_DIMENSION * y];
      t.FillAttributes(map_tile);
      //bool add_water = map_tile->height < map_tile->water_height || map_tile->type / 0x10 == 3;
      bool add_water = map_tile->xter / 0x10 > 0 && map_tile->xter != XTER_WATERFALL;
      if (add_water)
      {
        SceneTile& sea_tile = sea_tiles[x + TILES_DIMENSION * y];
        sea_tile.SetOrigin(x, y);
        sea_tile.SetHeight(map_tile->water_height);
        sea_tile.ColorTile(DirectX::Colors::SC2K_SEA_BLUE);
      }
#if USING_SPRITES_3D
      if (XBLD_IS_TREE(map_tile->xbld))
      {
        v_sprite3d.push_back(new Sprite3D(
          AssetLoader::mresources->at(L"tree"),
          t.v_pos[VPos::TOP_LEFT],
          Orientation::X));
      }
#endif
#if USING_SPRITES_2D
      if (XBLD_IS_TREE(map_tile->xbld))
      {
        v_sprite2d.push_back(new Sprite2D(
          AssetLoader::mresources->at(L"tree"),
          AssetLoader::mdescriptions->at(L"tree"),
          t.v_pos[VPos::TOP_LEFT]));
      }
#endif

      bool no_xzon = map_tile->xzon == 0;
      BYTE orientation = (map_tile->xzon >> 4);

      bool single_tile = orientation == 0b1111;
      //if (map_orientation == 0b0000 && !no_xzon && !single_tile)
      if (map_orientation == 0b0000 && !single_tile &&
        XBLD_IS_BUILDING(map_tile->xbld))
      {
        map_orientation = orientation;
      }

     

      bool render_tile = single_tile ||
        (!no_xzon && orientation == map_orientation) ||
        (no_xzon && (map_tile->xbit >> 4) == 0) ||
        (!XBLD_IS_BUILDING(map_tile->xbld));

      if (render_tile)
      {

        if (map_tile->xbld == 0x6A)
        {
          printf("highway bridge at %d, %d\n", x, y);
        }
        if (xbld_map.count(map_tile->xbld))
        {
          std::map<std::wstring, std::shared_ptr<DirectX::Model>>::iterator it;
          it = AssetLoader::mmodels->find(xbld_map.at(map_tile->xbld));
          if (it != AssetLoader::mmodels->end())
          {
            DirectX::SimpleMath::Vector3 position = t.v_pos[VPos::TOP_LEFT];
            Model3D* model = new Model3D(it->second, position);

            if (map_tile->xbit & 0b0100) //appears to be a "water tile"
            {
              model->origin.y = map_tile->water_height;
            }
            else if (map_tile->xter >= XTER_UNDERWATER_FLAT)
            {
              model->origin.y = map_tile->water_height;
            }
            //else if (map_tile->water_height == map_tile->height)           

            XBLDType xbld_value = map_tile->xbld;

            if (XBLD_IS_HIGHWAY_ONRAMP(map_tile->xbld))
            {
              TransformHighwayOnRamp(map_tile, model);
              if (map_tile->xbit & 0b0010) //perhaps some sort of rotation
              {
                model->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
                model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
              }
            }

            else if (XBLD_IS_BRIDGE(map_tile->xbld))
            {
              if (map_tile->xbit & 0b0010) //perhaps some sort of rotation
              {
                model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
                model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
              }
            }

            else if (XBLD_IS_TUNNEL(map_tile->xbld))
            {
              model->origin.y = t.height;
            }

            else if (XBLD_IS_TREES(map_tile->xbld))
            {
              if (map_tile->xter != XTER_FLAT)
              { //raise the trees on sloped terrains to add extended trunks
                model->origin.y = t.height + 1.f;
              }
              else
              {
                model->origin.y = t.height;
              }
            }

            else if (XBLD_IS_POWER_LINE(map_tile->xbld))
            {
              if (map_tile->xter != XTER_FLAT)
              { //raise the trees on sloped terrains to add extended trunks
                model->origin.y = t.height + HEIGHT_INCREMENT / 2.f; //midpoint
              }
              else
              {
                model->origin.y = t.height;
              }
            }

            else if (XBLD_IS_HYDROELECTRIC(map_tile->xbld))
            {
              if (map_tile->xter == XTER_WATERFALL)
              {
                t.SetHeight(map_tile->height);
              }
            }

            else if (XBLD_IS_HIGHWAY(map_tile->xbld))
            {
              model->origin.y += HIGHWAY_HEIGHT;
              if (XBLD_IS_HIGHWAY_BRIDGE(map_tile->xbld))
              {
                model->origin.y += HEIGHT_INCREMENT;
              }
              
              if (XBLD_IS_HIGHWAY_SLOPE(map_tile->xbld) &&
                  map_tile->xter == XTER_HIGHGROUND)
              {
                //XBLDType convert;
                switch (map_tile->xbld)
                {
                case XBLD_HIGHWAY_SLOPE_1:
                case XBLD_HIGHWAY_SLOPE_3:
                  xbld_value = XBLD_HIGHWAY_2;
                  break;
                case XBLD_HIGHWAY_SLOPE_2:
                case XBLD_HIGHWAY_SLOPE_4:
                  xbld_value = XBLD_HIGHWAY_1;
                  break;
                }
                std::map<std::wstring, std::shared_ptr<DirectX::Model>>::iterator it;
                it = AssetLoader::mmodels->find(xbld_map.at(xbld_value));
                model->model = it->second;
              }              
            }
            
            RotateModel(xbld_value, model);
            v_model3d.push_back(model);
            AddSecondaryModel(t, model, xbld_value);
            if (XBLD_IS_HIGHWAY_CORNER(map_tile->xbld))
            {
              highway_model_vec.push_back(std::make_pair(
                v_model3d.size() - 1, 
                x + TILES_DIMENSION * y));
            }
          }
        }
      }
      SetDrawTileWithModel(t);    
    }
  }

  printf("Map XZON Object Origin (bits 4 - 7): 0x%x\n", map_orientation);
  FillTileEdges();
  FillTunnels();
  std::vector<ModelTileVector> highway_clusters;
  ClusterTiles(highway_model_vec, 1.0f, highway_clusters);
  TransformHighwayCorners(highway_clusters);
  m_position *= scale;
  SetScale(scale);

  printf("Rendering %d 3d models\n", v_model3d.size());
  render_scene = true;
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
  m_effect = std::make_unique<DirectX::BasicEffect>(m_d3dDevice.Get());
  m_effect->SetVertexColorEnabled(true);

  void const* shaderByteCode;
  size_t byteCodeLength;
  m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

  m_d3dDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
    DirectX::VertexPositionColor::InputElementCount,
    shaderByteCode, byteCodeLength,
    m_inputLayout.ReleaseAndGetAddressOf());

  m_texbatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(m_d3dContext.Get());
  m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(m_d3dContext.Get());
  m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_d3dContext.Get());
  
  
  /*
  CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
    D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
    */
  CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
    D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
  /*
  CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
    D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
    */


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

  //TODO clean this up, multiple sample counts
  if (max_supported_sample_count == 0)
  {
    max_supported_sample_count = 1;
    for (unsigned int s = 1; s < 5; ++s)
    {
      UINT num_qual_levels;
      unsigned int sample_count = std::pow(2, s);
      HRESULT multi_sample_hr = m_d3dDevice->CheckMultisampleQualityLevels(
        DXGI_FORMAT_B8G8R8A8_UNORM,
        sample_count, &num_qual_levels);

      if (multi_sample_hr != S_OK || num_qual_levels == 0)
      {
        break;
      }
      max_supported_sample_count = sample_count;
    }    

    if (max_supported_sample_count < 4)
    {
      use_4x_msaa = false;
    }
    printf("Max MSAA Sample Count: %d. Using 4x MSAA: %d\n", max_supported_sample_count, use_4x_msaa);
  }



  // If the swap chain already exists, resize it, otherwise create one.
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
    swapChainDesc.SampleDesc.Count = use_4x_msaa ? 4 : 1;
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
  if (use_4x_msaa)
  {
    depthStencilDesc = CD3D11_TEXTURE2D_DESC(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, 
      D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0, 4, 0);
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

  //pd3dDeferredContext->ClearRenderTargetView(m_renderTargetView.Get(), DirectX::Colors::CornflowerBlue);
  //pd3dDeferredContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
  //pd3dDeferredContext->RSSetViewports(1, &viewport);
  //pd3dDeferredContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());   
  //pd3dDeferredContext->ClearState();
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
  base_move_speed = std::clamp(value, 0.025f, 0.4f);
  move_speed = base_move_speed * scale;
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
  float fps = timer.GetFramesPerSecond();
  float delta_fps = fps ? 100 / fps : 1;
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
  else if (GetAsyncKeyState(0x41)) //A (strafe left)
  {
    m_position.x -= cos(yaw) * fps_move_speed;
    m_position.z -= sin(yaw) * fps_move_speed;
  }
  else if (GetAsyncKeyState(0x44)) //D (strafe right)
  {
    m_position.x += cos(yaw) * fps_move_speed;
    m_position.z += sin(yaw) * fps_move_speed;
  }
  else if (GetAsyncKeyState(0x52)) //R (strafe up)
    m_position.y += fps_move_speed;
  else if (GetAsyncKeyState(0x46)) //F (strafe down)
    m_position.y -= fps_move_speed;
}

void Scene::MouseClick()
{
  Vector3 near_plane = m_viewport.Unproject(Vector3(client_cx, client_cy, 0.f), m_proj, m_view, m_world);
  Vector3 far_plane = m_viewport.Unproject(Vector3(client_cx, client_cy, 1.f), m_proj, m_view, m_world);
  Vector3 normalized = far_plane - near_plane;
  normalized.Normalize();
  DirectX::SimpleMath::Ray ray(near_plane, normalized);

  for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
  {
    for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
    {
      MapSceneTile& t = tiles[x + TILES_DIMENSION * y];
      float distance1, distance2;
      bool tri1 = ray.Intersects(t.v_pos[VPos::TOP_LEFT], t.v_pos[VPos::BOTTOM_LEFT], t.v_pos[VPos::TOP_RIGHT], distance1);
      bool tri2 = ray.Intersects(t.v_pos[VPos::BOTTOM_RIGHT], t.v_pos[VPos::BOTTOM_LEFT], t.v_pos[VPos::TOP_RIGHT], distance2);
      if (tri1 || tri2)
      {
        t.ColorTile(DirectX::Colors::Crimson);
        printf("[Debug] Map Tile(%d, %d): Map Height: %d, XTER: %x, Water Height:%d, ALTM: %d, XBLD: %x, XZON: %x, XUND: %x, XBIT: %x\n",
          x, y, t.map_tile->height, t.map_tile->xter, t.map_tile->water_height, t.map_tile->altm, t.map_tile->xbld, t.map_tile->xzon,
          t.map_tile->xund, t.map_tile->xbit);
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


void Scene::Render()
{

  CD3D11_VIEWPORT viewport(0.0f, 0.0f, m_outputWidth, m_outputHeight);
  m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), DirectX::Colors::CornflowerBlue);
  m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
  m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
  m_d3dContext->RSSetViewports(1, &viewport);

  if (render_scene)
  {

    if (render_models)
    {

      Matrix view_proj = m_view * m_proj;
      Plane frustum_planes[6];

      // left
      frustum_planes[0].x = view_proj.m[0][3] + view_proj.m[0][0];
      frustum_planes[0].y = view_proj.m[1][3] + view_proj.m[1][0];
      frustum_planes[0].z = view_proj.m[2][3] + view_proj.m[2][0];
      frustum_planes[0].w = view_proj.m[3][3] + view_proj.m[3][0];

      // right
      frustum_planes[1].x = view_proj.m[0][3] - view_proj.m[0][0];
      frustum_planes[1].y = view_proj.m[1][3] - view_proj.m[1][0];
      frustum_planes[1].z = view_proj.m[2][3] - view_proj.m[2][0];
      frustum_planes[1].w = view_proj.m[3][3] - view_proj.m[3][0];

      // bottom
      frustum_planes[2].x = view_proj.m[0][3] + view_proj.m[0][1];
      frustum_planes[2].y = view_proj.m[1][3] + view_proj.m[1][1];
      frustum_planes[2].z = view_proj.m[2][3] + view_proj.m[2][1];
      frustum_planes[2].w = view_proj.m[3][3] + view_proj.m[3][1];

      // top
      frustum_planes[3].x = view_proj.m[0][3] - view_proj.m[0][1];
      frustum_planes[3].y = view_proj.m[1][3] - view_proj.m[1][1];
      frustum_planes[3].z = view_proj.m[2][3] - view_proj.m[2][1];
      frustum_planes[3].w = view_proj.m[3][3] - view_proj.m[3][1];

      // near
      frustum_planes[4].x = view_proj.m[0][2];
      frustum_planes[4].y = view_proj.m[1][2];
      frustum_planes[4].z = view_proj.m[2][2];
      frustum_planes[4].w = view_proj.m[3][2];

      // far
      frustum_planes[5].x = view_proj.m[0][3] - view_proj.m[0][2];
      frustum_planes[5].y = view_proj.m[1][3] - view_proj.m[1][2];
      frustum_planes[5].z = view_proj.m[2][3] - view_proj.m[2][2];
      frustum_planes[5].w = view_proj.m[3][3] - view_proj.m[3][2];
      for (unsigned int i = 0; i < 6; ++i)
      {
        frustum_planes[i].Normalize();
      }

      for (Model3D* model3d : v_model3d)
      {
        if (!use_render_distance || Vector3::Distance(m_position, model3d->origin_scaled) < scaled_render_distance)
        {
          if (!use_aabb_frustum_culling)
          {
            model3d->model->Draw(m_d3dContext.Get(), *m_states, model3d->m_world, m_view, m_proj);
            continue;
          }
          bool frustum_test = true;
          auto mesh_it = model3d->model->meshes.cbegin();
          if (mesh_it == model3d->model->meshes.cend()) continue;
          Vector3 center = (Vector3(mesh_it->get()->boundingBox.Center) * scale) + model3d->origin_scaled;
          for (int i = 0; i < 6; ++i)
          {
            Vector3 n = Vector3(fabs(frustum_planes[i].x), fabs(frustum_planes[i].y), fabs(frustum_planes[i].z));
            DirectX::SimpleMath::Vector4 v4(center.x, center.y, center.z, 1.f);
            float e = n.Dot(mesh_it->get()->boundingBox.Extents);
            float s = frustum_planes[i].Dot(v4);
            if (s + e < 0.f)
            {
              frustum_test = false;
              break;
            }            
          }
          if (frustum_test)
            model3d->model->Draw(m_d3dContext.Get(), *m_states, model3d->m_world, m_view, m_proj);
        }
      }

    }

    m_d3dContext->RSSetState(m_states->CullNone());
    //m_d3dContext->RSSetState(m_raster.Get());
    m_effect->SetTextureEnabled(false);
    m_effect->SetWorld(m_world);
    m_effect->SetView(m_view);
    m_effect->Apply(m_d3dContext.Get());
    m_d3dContext->IASetInputLayout(m_inputLayout.Get());
    m_d3dContext->OMSetBlendState(m_states->Opaque(), NULL, 0xFFFFFFFF);
    m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    m_batch->Begin();
    for (unsigned int quad_ix = 0; quad_ix < fill_tiles.size(); quad_ix += 4)
    {
      m_batch->DrawQuad(fill_tiles[quad_ix], fill_tiles[quad_ix + 1], fill_tiles[quad_ix + 2], fill_tiles[quad_ix + 3]);
    }

    for (unsigned int i = 0; i < TILES_DIMENSION * TILES_DIMENSION; ++i)
    {
      const SceneTile& t = tiles[i]; //object slice is ok
      if (t.render || !render_models)
      {
        m_batch->DrawTriangle(t.vpc_pos[VPos::TOP_LEFT], t.vpc_pos[VPos::BOTTOM_LEFT], t.vpc_pos[VPos::TOP_RIGHT]);
        m_batch->DrawTriangle(t.vpc_pos[VPos::BOTTOM_RIGHT], t.vpc_pos[VPos::BOTTOM_LEFT], t.vpc_pos[VPos::TOP_RIGHT]);
      }
    }
    m_batch->End();

    //Separating the water into its own loop afterwards prevents 'blinds effect' artifacting
    m_d3dContext->OMSetBlendState(m_states->AlphaBlend(), NULL, 0xFFFFFFFF);
    m_batch->Begin();
    for (unsigned int i = 0; i < TILES_DIMENSION * TILES_DIMENSION; ++i)
    {
      const SceneTile& w = sea_tiles[i];
      if (w.height > -1.f && w.render)
      {
        m_batch->DrawTriangle(w.vpc_pos[VPos::TOP_LEFT], w.vpc_pos[VPos::BOTTOM_LEFT], w.vpc_pos[VPos::TOP_RIGHT]);
        m_batch->DrawTriangle(w.vpc_pos[VPos::BOTTOM_RIGHT], w.vpc_pos[VPos::BOTTOM_LEFT], w.vpc_pos[VPos::TOP_RIGHT]);
      }
    }
    m_batch->End();

#if USING_SPRITES_3D
    m_effect->SetVertexColorEnabled(false);
    m_effect->SetTextureEnabled(true);
    m_d3dContext->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);
    for (const Sprite3D* sprite3d : v_sprite3d)
    {
      m_effect->SetTexture(sprite3d->resource.Get());
      m_effect->Apply(m_d3dContext.Get());
      m_texbatch->Begin();
      m_texbatch->DrawQuad(sprite3d->v1, sprite3d->v2, sprite3d->v3, sprite3d->v4);
      m_texbatch->End();
    }
    m_effect->SetVertexColorEnabled(true);
    m_effect->SetTextureEnabled(false);
#endif

#if USING_SPRITES_2D
    m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, m_states->NonPremultiplied());
    for (Sprite2D* sprite2d : v_sprite2d)
    {
      sprite2d->Update(this);
      float distance = Vector3::Distance(sprite2d->position3d * scale, m_position);
      float dscale = 1.f / distance;
      m_spriteBatch->Draw(sprite2d->resource.Get(), sprite2d->position2d, nullptr, DirectX::Colors::White, 0.f,
        sprite2d->origin2d, dscale);
    }
    m_spriteBatch->End();
#endif  

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

  if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
  {
    //m_swapChain.Reset();
    //CreateDevice();
    //CreateResources();
  }

}

BOOL Scene::FillMapSceneTile(const MapSceneTile& a, const MapSceneTile& b, Edge edge)
{
  Vector3 a1, a2, b1, b2;
  switch (edge)
  {
  case LEFT:
    a1 = a.v_pos[VPos::TOP_LEFT];
    a2 = a.v_pos[VPos::BOTTOM_LEFT];
    b1 = b.v_pos[VPos::TOP_RIGHT];
    b2 = b.v_pos[VPos::BOTTOM_RIGHT];
    break;
  case RIGHT:
    a1 = a.v_pos[VPos::TOP_RIGHT];
    a2 = a.v_pos[VPos::BOTTOM_RIGHT];
    b1 = b.v_pos[VPos::TOP_LEFT];
    b2 = b.v_pos[VPos::BOTTOM_LEFT];
    break;
  case TOP:
    a1 = a.v_pos[VPos::TOP_LEFT];
    a2 = a.v_pos[VPos::TOP_RIGHT];
    b1 = b.v_pos[VPos::BOTTOM_LEFT];
    b2 = b.v_pos[VPos::BOTTOM_RIGHT];
    break;
  case BOTTOM:
    a1 = a.v_pos[VPos::BOTTOM_LEFT];
    a2 = a.v_pos[VPos::BOTTOM_RIGHT];
    b1 = b.v_pos[VPos::TOP_LEFT];
    b2 = b.v_pos[VPos::TOP_RIGHT];
    break;
  }

  float winner = (a1.y - b1.y) + (a2.y - b2.y);
  if (winner == 0.f)
    return FALSE;

  XTERType winner_type = winner > 0.f ? a.map_tile->xter : b.map_tile->xter;
  DirectX::XMVECTORF32 winner_color = winner_type == XTER_WATERFALL ?
    DirectX::Colors::SC2K_SEA_BLUE_STATIC_BRIGHT : DirectX::Colors::SC2K_DIRT_DARKEST;
  fill_tiles.push_back(DirectX::VertexPositionColor(a1, winner_color));
  fill_tiles.push_back(DirectX::VertexPositionColor(a2, winner_color));
  fill_tiles.push_back(DirectX::VertexPositionColor(b2, winner_color));
  fill_tiles.push_back(DirectX::VertexPositionColor(b1, winner_color));
  return TRUE;
}

BOOL Scene::FillEdgeSceneTile(unsigned int index, Edge edge)
{
  const MapSceneTile& a = tiles[index];
  Vector3 a1, a2, b1, b2;
  SceneTile::VertexPos pos1, pos2;
  switch (edge)
  {
  case LEFT:
    pos1 = VPos::TOP_LEFT;
    pos2 = VPos::BOTTOM_LEFT;
    break;
  case RIGHT:
    pos1 = VPos::TOP_RIGHT;
    pos2 = VPos::BOTTOM_RIGHT;
    break;
  case TOP:
    pos1 = VPos::TOP_LEFT;
    pos2 = VPos::TOP_RIGHT;
    break;
  case BOTTOM:
    pos1 = VPos::BOTTOM_LEFT;
    pos2 = VPos::BOTTOM_RIGHT;
    break;
  }
  a1 = a.v_pos[pos1];
  a2 = a.v_pos[pos2];
  b1 = Vector3(a1.x, 0.f, a1.z);
  b2 = Vector3(a2.x, 0.f, a2.z);

  fill_tiles.push_back(DirectX::VertexPositionColor(a1, DirectX::Colors::SC2K_DIRT_DARKEST));
  fill_tiles.push_back(DirectX::VertexPositionColor(a2, DirectX::Colors::SC2K_DIRT_DARKEST));
  fill_tiles.push_back(DirectX::VertexPositionColor(b2, DirectX::Colors::SC2K_DIRT_DARKEST));
  fill_tiles.push_back(DirectX::VertexPositionColor(b1, DirectX::Colors::SC2K_DIRT_DARKEST));
  const SceneTile& w = sea_tiles[index];
  if (w.height > -1)
  {
    fill_tiles.push_back(DirectX::VertexPositionColor(w.v_pos[pos1], DirectX::Colors::SC2K_SEA_BLUE));
    fill_tiles.push_back(DirectX::VertexPositionColor(w.v_pos[pos2], DirectX::Colors::SC2K_SEA_BLUE));
    fill_tiles.push_back(DirectX::VertexPositionColor(a2, DirectX::Colors::SC2K_SEA_BLUE));
    fill_tiles.push_back(DirectX::VertexPositionColor(a1, DirectX::Colors::SC2K_SEA_BLUE));
  }
  return TRUE;
}

void Scene::FillTileEdges()
{
  //Fill-in tile polygons using minimal vertices, compare using top and left tiles.
  for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
  {
    for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
    {
      unsigned int index = x + TILES_DIMENSION * y;
      const MapSceneTile& this_tile = tiles[index];
      if (x == 0)
      {
        FillEdgeSceneTile(index, Edge::LEFT);
      }
      else if (x > 0)
      {
        const MapSceneTile& cmp_left = tiles[(x - 1) + TILES_DIMENSION * y];
        FillMapSceneTile(this_tile, cmp_left, Edge::LEFT);
        if (x + 1 == TILES_DIMENSION)
          FillEdgeSceneTile(index, Edge::RIGHT);
      }

      if (y == 0)
      {
        FillEdgeSceneTile(index, Edge::TOP);
      }
      else if (y > 0)
      {
        const MapSceneTile& cmp_top = tiles[x + TILES_DIMENSION * (y - 1)];
        FillMapSceneTile(this_tile, cmp_top, Edge::TOP);
        if (y + 1 == TILES_DIMENSION)
          FillEdgeSceneTile(index, Edge::BOTTOM);
      }
    }
  }
}

void Scene::TransformHighwayOnRamp(const MapTile* map_tile, Model3D* model)
{
  switch (map_tile->xbld)
  {
  case XBLD_HIGHWAY_ONRAMP_1:
  case XBLD_HIGHWAY_ONRAMP_4:
    model->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
    break;
  case XBLD_HIGHWAY_ONRAMP_2:
  case XBLD_HIGHWAY_ONRAMP_3:
    model->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    break;
  }
}

void Scene::TransformHighwayCorners(const std::vector<ModelTileVector>& clusters)
{
  for (ModelTileVector mtv : clusters)
  {
    const MapTile* tile_cluster_TL = tiles[mtv.at(0).second].map_tile;
    Model3D* model_cluster_TL = v_model3d[mtv.at(0).first];
    Model3D* model_cluster_TR = v_model3d[mtv.at(1).first];
    Model3D* model_cluster_BL = v_model3d[mtv.at(2).first];
    Model3D* model_cluster_BR = v_model3d[mtv.at(3).first];
    switch (tile_cluster_TL->xbld)
    {
    case 0x65:
      model_cluster_BL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
      model_cluster_BL->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
      model_cluster_TR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      model_cluster_TR->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
      model_cluster_TL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      model_cluster_TL->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
      break;
    case 0x66:
      //model_cluster_TL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI);
      model_cluster_TL->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
      model_cluster_TR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI);
      model_cluster_TR->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
      model_cluster_BL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI);
      model_cluster_BL->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI);
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
      break;
    case 0x67:
     model_cluster_TR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
     model_cluster_TR->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
     model_cluster_BL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
     model_cluster_BL->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
     model_cluster_TL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
     model_cluster_TL->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
     model_cluster_BR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
     model_cluster_BR->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
      break;
    case 0x68:
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI);
      break;
    }
  }
}


void Scene::ClusterTiles(const ModelTileVector& vec, float dist, std::vector<ModelTileVector>& clusters)
{
  if (vec.empty()) return;

  if (vec.size() % 4)
  {
    printf("Error: Detected %d highway corners - cannot determine highway layout\n", vec.size());
    return;
  }

  printf("Received %d highway corners, should generate %d clusters\n", vec.size(), vec.size() / 4);

  //Step 1 - cluster together highway pieces that make up a turn
  ModelTileVector::const_iterator vec_it;
  for (vec_it = vec.begin(); vec_it != vec.end(); ++vec_it)
  {
    const MapTile* tile = tiles[vec_it->second].map_tile;
    int x1 = vec_it->second % 128;
    int y1 = vec_it->second / 128;
    
    bool clustered = false;
    std::vector<ModelTileVector>::iterator cluster_it;
    for (cluster_it = clusters.begin(); cluster_it != clusters.end(); ++cluster_it)
    {
      for (std::pair<size_t, size_t> cluster : *cluster_it)
      {
        const MapTile* tile_cluster = tiles[cluster.second].map_tile;
        int x2 = cluster.second % 128;
        int y2 = cluster.second / 128;   

        if (tile_cluster->xbld != tile->xbld) continue;
        
        float distance = std::sqrt(
          std::pow(x2 - x1, 2) + 
          std::pow(y2 - y1, 2));

        //printf("<%d, %d> -> <%d, %d> Distance: %f\n", x1, y1, x2, y2, distance);

        if (distance <= dist )
        {
          clustered = true;
          cluster_it->push_back(*vec_it);
          break;
        }
      }
      if (clustered) break;
    } 
    if (!clustered)
    {
      clusters.push_back({ *vec_it });
    }
  }

  if (clusters.empty())
  {
    printf("Generated no clusters, however highway corners exist\n");
    return;
  }

  printf("Generated %d highway corner clusters\n", clusters.size());

  if (clusters.size() != vec.size() / 4)
  {
    printf("Incorrect amount of highway clusters generated\n");
    clusters.clear();
    return;
  }

  std::vector<ModelTileVector>::iterator cluster_it;
  for (cluster_it = clusters.begin(); cluster_it != clusters.end(); ++cluster_it)
  {
    if (cluster_it->size() != 4)
    {
      printf("Error: Found a highway corner cluster with %d tiles\n", cluster_it->size());
      clusters.clear();
      return;
    }
  }
}

/*
* Instead of creating separate models for crossovers, we can overlap two
* existing models. Separate models may look better, or be more appropriate in
* the future.
*/
void Scene::AddSecondaryModel(const MapSceneTile& t, 
                              const Model3D* rmodel,
                              const XBLDType xbld)
{
  if (t.map_tile->xter == XTER_FLAT && XBLD_IS_TREE(xbld))
  { //only add trunks if on a slope
    return;
  }

  int32_t model_id = 0;
  bool repeat_y = false;
  //Do not use t.map_tile->xbld
  switch (xbld)
  {
  case XBLD_TREES_1:
    model_id = SceneryObject::TREE_TRUNKS_1;
    break;
  case XBLD_TREES_2:
    model_id = SceneryObject::TREE_TRUNKS_2;
    break;
  case XBLD_TREES_3:
    model_id = SceneryObject::TREE_TRUNKS_3;
    break;
  case XBLD_TREES_4:
    model_id = SceneryObject::TREE_TRUNKS_4;
    break;
  case XBLD_TREES_5:
    model_id = SceneryObject::TREE_TRUNKS_5;
    break;
  case XBLD_TREES_6:
    model_id = SceneryObject::TREE_TRUNKS_6;
    break;
  case XBLD_TREES_7:
    model_id = SceneryObject::TREE_TRUNKS_7;
    break;
  case XBLD_HIGHWAY_CROSSOVER_1:
    model_id = XBLD_ROAD_2;
    break;
  case XBLD_HIGHWAY_CROSSOVER_2:
    model_id = XBLD_ROAD_1;
    break;
  case XBLD_HIGHWAY_CROSSOVER_3:
    model_id = XBLD_RAIL_2;
    break;
  case XBLD_HIGHWAY_CROSSOVER_4:
    model_id = XBLD_RAIL_1;
    break;
  case XBLD_HIGHWAY_1:
  case XBLD_HIGHWAY_2:
  case XBLD_HIGHWAY_BRIDGE_1:
  case XBLD_HIGHWAY_BRIDGE_2:
    model_id = SceneryObject::PILLAR;
    repeat_y = true;
    break;
  case XBLD_HIGHWAY_CROSSOVER_5:
  case XBLD_HIGHWAY_CROSSOVER_6:
    return; //Do nothing for powerlines
  }
  if (model_id == 0) return;
  if (repeat_y && rmodel == nullptr) return;
  std::map<std::wstring, std::shared_ptr<DirectX::Model>>::iterator it;
  it = AssetLoader::mmodels->find(xbld_map.at(model_id));
  if (it == AssetLoader::mmodels->end())
  {
    return; //TODO this is an error
  }
  float height = t.map_tile->height * HEIGHT_INCREMENT;

add_model:
  Vector3 reference_tile(t.v_pos[VPos::TOP_LEFT]);
  reference_tile.y = height;
  Model3D* model = new Model3D(it->second, reference_tile);
  RotateModel(model_id, model);
  v_model3d.push_back(model);

  if (repeat_y && (height + HEIGHT_INCREMENT) <= rmodel->origin.y)
  {
    height += HEIGHT_INCREMENT;
    goto add_model;
  }
}

/*
 * SimCity 2000 maps do not have data for tunnel pieces besides the entry/exit. This
 * function fills in the missing data by finding an entry, then iterating until it finds
 * the exit - adding tunnel pieces in between.
 */
void Scene::FillTunnels()
{
  for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
  {
    for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
    {
      unsigned int start_index = x + TILES_DIMENSION * y;
      const MapSceneTile& start_tile = tiles[start_index];

      if (start_tile.map_tile->xbld == XBLD_TUNNEL_3)
      {
        std::map<std::wstring, std::shared_ptr<DirectX::Model>>::iterator it;
        it = AssetLoader::mmodels->find(xbld_map.at(start_tile.map_tile->xbld));
        if (it == AssetLoader::mmodels->end())
        {
          continue; //TODO - this should never happen
        }

        for (unsigned int x2 = x; x2 < TILES_DIMENSION; ++x2)
        {
          unsigned int end_index = x2 + TILES_DIMENSION * y;
          const MapSceneTile& end_tile = tiles[end_index];
          if (end_tile.map_tile->xbld == XBLD_TUNNEL_1)
          {
            break;
          }
          DirectX::SimpleMath::Vector3 position = end_tile.v_pos[VPos::TOP_LEFT];
          Model3D* model = new Model3D(it->second, position);
          model->origin.y = start_tile.height;
          RotateModel(start_tile.map_tile->xbld, model);
          v_model3d.push_back(model);
        }
      }
      else if (start_tile.map_tile->xbld == XBLD_TUNNEL_4)
      {
        std::map<std::wstring, std::shared_ptr<DirectX::Model>>::iterator it;
        it = AssetLoader::mmodels->find(xbld_map.at(start_tile.map_tile->xbld));
        if (it == AssetLoader::mmodels->end())
        {
          continue; //TODO - this should never happen
        }

        for (unsigned int y2 = y; y2 < TILES_DIMENSION; ++y2)
        {
          unsigned int end_index = x + TILES_DIMENSION * y2;
          const MapSceneTile& end_tile = tiles[end_index];
          if (end_tile.map_tile->xbld == XBLD_TUNNEL_2)
          {
            break;
          }
          DirectX::SimpleMath::Vector3 position = end_tile.v_pos[VPos::TOP_LEFT];
          Model3D* model = new Model3D(it->second, position);
          model->origin.y = start_tile.height;
          RotateModel(start_tile.map_tile->xbld, model);
          v_model3d.push_back(model);
        }
      }
    }
  }
}


void Scene::RotateModel(int32_t model_id, Model3D* model)
{
  switch (model_id)
  {
  case XBLD_TUNNEL_1:
  case XBLD_TUNNEL_3:
  case XBLD_ROAD_2:
  case XBLD_RAIL_2:
  case XBLD_CROSSOVER_ROAD_RAIL_1:
  case XBLD_CROSSOVER_RAIL_POWER_2:
  case XBLD_ROAD_9:
  case XBLD_RAIL_9:
  case XBLD_RAIL_SLOPE_1:
  case XBLD_RAIL_SLOPE_3:
  case XBLD_HIGHWAY_2:
  case XBLD_HIGHWAY_BRIDGE_2:
  case XBLD_HIGHWAY_CROSSOVER_2:
  case XBLD_HIGHWAY_CROSSOVER_4:
  case XBLD_POWER_LINE_2:
  //case XBLD_HIGHWAY_CORNER_3:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    break;
  case XBLD_ROAD_8:
  case XBLD_RAIL_8:
  //case XBLD_HIGHWAY_CORNER_2:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI);
    model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
    break;
  case XBLD_ROAD_7:  
  case XBLD_RAIL_7:
  //case XBLD_HIGHWAY_CORNER_1:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
    break;
  case XBLD_ROAD_3:
  case XBLD_RAIL_3:
  case XBLD_HIGHWAY_SLOPE_1:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitZ, -M_PI_4);
    model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    break;
  case XBLD_ROAD_4:
  case XBLD_RAIL_4:
  case XBLD_HIGHWAY_SLOPE_2:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitX, M_PI_4);
    break;
  case XBLD_ROAD_5:
  case XBLD_RAIL_5:
  case XBLD_HIGHWAY_SLOPE_3:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitZ, M_PI_4);
    model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    break;
  case XBLD_ROAD_6:
  case XBLD_RAIL_6:
  case XBLD_HIGHWAY_SLOPE_4:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitX, -M_PI_4);
    break;
  }
}

void Scene::SetDrawTileWithModel(MapSceneTile& tile)
{
  switch (tile.map_tile->xbld)
  {
  case XBLD_TUNNEL_1:
  case XBLD_TUNNEL_2:
  case XBLD_TUNNEL_3:
  case XBLD_TUNNEL_4:
  case XBLD_HYDROELECTRIC_1:
  case XBLD_HYDROELECTRIC_2:
  case XBLD_ZOO:
  //case XBLD_MARINA:
    tile.render = false;
    break;
  }
}
#include "Scene.h"
#include "MapSceneTile.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
typedef SceneTile::VertexPos VPos;

void Scene::Initialize(HWND window, MapTile* map_tiles, int width, int height)
{
  m_window = window;
  m_outputWidth = width;
  m_outputHeight = height;

  RECT rect;
  GetWindowRect(m_window, &rect);
  window_cx = static_cast<int>(rect.left + ((rect.right - rect.left) / 2));
  window_cy = static_cast<int>(rect.top + ((rect.bottom - rect.top) / 2));
  SetCursorPos(window_cx, window_cy);
  ShowCursor(FALSE);

  CreateDevice();
  CreateResources();  
  m_position = Vector3(8.f, 3.f, 8.f);
  m_world = Matrix::CreateScale(scale);
  m_view = Matrix::CreateLookAt(m_position, Vector3(0.f, 0.f, 0.f), Vector3::UnitY);
  m_proj = Matrix::CreatePerspectiveFieldOfView(DirectX::XM_PI / 4.f, float(width) / float(height), 0.01f, 40.f);

  m_effect->SetView(m_view);
  m_effect->SetProjection(m_proj);
  tiles = new MapSceneTile[TILES_DIMENSION * TILES_DIMENSION];
  sea_tiles = new SceneTile[TILES_DIMENSION * TILES_DIMENSION];
  for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
  {
    for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
    {
      MapSceneTile& t = tiles[x + TILES_DIMENSION * y];
      t.SetOrigin(x, y);
      const MapTile* map_tile = &map_tiles[x + TILES_DIMENSION * y];
      t.FillAttributes(map_tile);
      //bool add_water = map_tile->height < map_tile->water_height || map_tile->type / 0x10 == 3;
      bool add_water = map_tile->type / 0x10 > 0 && map_tile->type != ETT_WATERFALL;
      if (add_water)
      {
        SceneTile& sea_tile = sea_tiles[x + TILES_DIMENSION * y];
        sea_tile.SetOrigin(x, y);
        sea_tile.SetHeight(map_tile->water_height);
        sea_tile.ColorTile(DirectX::Colors::SC2K_SEA_BLUE);
      }

    }
  } 
  //TILE_DIMENSION * SIDES * LAYERS (GROUND/WATER)
  FillTileEdges();
  //FillMapEdges();
  render_scene = true;
}

void Scene::CreateDevice()
{
  UINT creationFlags = 0;
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

  m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(m_d3dContext.Get());
  m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_d3dContext.Get());
  m_font = std::make_unique<DirectX::SpriteFont>(m_d3dDevice.Get(), L"d3d.spritefont");
}

void Scene::CreateResources()
{
  // Clear the previous window size specific context.
  ID3D11RenderTargetView* nullViews[] = { nullptr };
  m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
  m_renderTargetView.Reset();
  m_depthStencilView.Reset();
  m_d3dContext->Flush();

  const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
  const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
  const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
  const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
  constexpr UINT backBufferCount = 2;

  // If the swap chain already exists, resize it, otherwise create one.
  if (m_swapChain)
  {
    m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);
  }
  else
  {
    Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice;
    m_d3dDevice.As(&dxgiDevice);
    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
    Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
    dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));

    // Create a descriptor for the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = backBufferWidth;
    swapChainDesc.Height = backBufferHeight;
    swapChainDesc.Format = backBufferFormat;
    swapChainDesc.SampleDesc.Count = 1;
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
    if (hr_dxgifactory != S_OK)
    {
      printf("Result: %x\n", hr_dxgifactory);
      ExitProcess(1);
    }
   dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER);
  }
  // Obtain the backbuffer for this window which will be the final 3D rendertarget.
  Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
  m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
  m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf());
  CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);
  Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
  m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf());
  CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
  m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf());
}

void Scene::Clear()
{
  // Clear the views.
  m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), DirectX::Colors::CornflowerBlue);
  m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

  // Set the viewport.
  CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
  m_d3dContext->RSSetViewports(1, &viewport);
}


void Scene::Tick()
{
  m_timer.Tick([&]()
  {
    Update(m_timer);
  });

  Render();
}

void Scene::MouseLook(int x, int z, int y)
{
  SetCursorPos(window_cx, window_cy);
  float dx = static_cast<float>(x - window_cx) * 0.005f;
  float dy = static_cast<float>(y - window_cy) * 0.003f;
  yaw = atan2(sin((yaw + dx) - (float)M_PI), cos((yaw + dx) - (float)M_PI)) + (float)M_PI; // wrap angle between 0 and 2pi
  pitch = std::clamp(pitch + dy,  (float)-M_PI_4, (float)M_PI_4);
  m_view = Matrix::CreateTranslation(-m_position);
  m_view *= Matrix::CreateFromYawPitchRoll(yaw, 0, 0); 
  m_view *= Matrix::CreateFromYawPitchRoll(0, pitch, 0);
}

void Scene::Update(DX::StepTimer const& timer)
{
  float elapsedTime = float(timer.GetElapsedSeconds());
  if (GetAsyncKeyState(VK_RIGHT))
    rotation_y += 0.01f;
  else if (GetAsyncKeyState(VK_LEFT))
    rotation_y -= 0.01f;
  else if (GetAsyncKeyState(VK_UP))
  {
    scale = std::clamp(scale + 0.0005f, 0.05f, 1.f);
    move_speed = 0.3f * scale;
    m_world = Matrix::CreateScale(scale);
  }
  else if (GetAsyncKeyState(VK_DOWN))
  {
    scale = std::clamp(scale - 0.0005f, 0.05f, 1.f);
    move_speed = 0.3f * scale;
    m_world = Matrix::CreateScale(scale);
  }
  else if (GetAsyncKeyState(0x57)) //W (move forward)
  {
    m_position.x += cos(yaw - (float)M_PI_2) * move_speed;
    m_position.z += sin(yaw - (float)M_PI_2) * move_speed;
    m_position.y -= sin(pitch) * move_speed;
  }
  else if (GetAsyncKeyState(0x53)) //S (move backwards)
  {
    m_position.x -= cos(yaw - (float)M_PI_2) * move_speed;
    m_position.z -= sin(yaw - (float)M_PI_2) * move_speed;
    m_position.y += sin(pitch) * move_speed;
  }
  else if (GetAsyncKeyState(0x41)) //A (strafe left)
  {
    m_position.x -= cos(yaw) * move_speed;
    m_position.z -= sin(yaw) * move_speed;
  }
  else if (GetAsyncKeyState(0x44)) //D (strafe right)
  {
    m_position.x += cos(yaw) * move_speed;
    m_position.z += sin(yaw) * move_speed;
  }
  else if (GetAsyncKeyState(0x52)) //R (strafe up)
    m_position.y += move_speed;
  else if (GetAsyncKeyState(0x46)) //F (strafe down)
    m_position.y -= move_speed;
}

void Scene::MouseClick()
{
  
  float ray_x = + cos(yaw - (float)M_PI_2);
  float ray_z = + sin(yaw - (float)M_PI_2);
  float ray_y = - sin(pitch); // TODO not accurate

  DirectX::SimpleMath::Ray ray(m_position, Vector3(ray_x, ray_y, ray_z));
  for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
  {
    for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
    {
      MapSceneTile& t = tiles[x + TILES_DIMENSION * y];
      float distance1, distance2;    
      bool tri1 = ray.Intersects(t.v_pos[VPos::TOP_LEFT] * scale, t.v_pos[VPos::BOTTOM_LEFT] * scale, t.v_pos[VPos::TOP_RIGHT] * scale, distance1);
      bool tri2 = ray.Intersects(t.v_pos[VPos::BOTTOM_RIGHT] * scale, t.v_pos[VPos::BOTTOM_LEFT] * scale, t.v_pos[VPos::TOP_RIGHT] * scale, distance2);
      if(tri1 || tri2)
      {
        t.ColorTile(DirectX::Colors::Crimson);
        printf("[Debug] Tile(%d, %d) Dist (Tri1 %f, Tri2 %f) : Map Height: %d, Map Type: %x, Water Height:%d, ALTM: %d\n", 
          x, y, distance1, distance2, t.map_tile->height, t.map_tile->type, t.map_tile->water_height, t.map_tile->altm);
        goto exit_loop;
      }
    }
  }
exit_loop:
  return;  
}

void Scene::MultiplyMovementSpeed(float value)
{
  move_speed *= value;
}

void Scene::Render()
{
  if (!render_scene)
  {
    return;
  }

  Clear();

  m_d3dContext->RSSetState(m_states->CullNone());

  m_effect->SetWorld(m_world);
  m_effect->SetView(m_view);
  m_effect->Apply(m_d3dContext.Get());
  m_d3dContext->IASetInputLayout(m_inputLayout.Get());


  m_d3dContext->OMSetBlendState(m_states->Opaque(), NULL, 0xFFFFFFFF);
  m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);
  m_batch->Begin();

  /*
  Vector3 xaxis((float)TILES_DIMENSION, 4.f, 0.f);
  Vector3 yaxis(0.f, 4.f, (float)TILES_DIMENSION);
  Vector3 origin = Vector3::Zero;
  size_t divisions = TILES_DIMENSION;

  
  for (size_t i = 0; i <= divisions; i++)
  {
    float fPercent = float(i) / float(divisions);
    fPercent = (fPercent * 1.0f) - 0.0f;

    Vector3 scale = yaxis * fPercent + origin;

    DirectX::VertexPositionColor v1(scale + Vector3(0, 2, 0), DirectX::Colors::Black);
    DirectX::VertexPositionColor v2(scale + xaxis, DirectX::Colors::Black);
    m_batch->DrawLine(v1, v2);
  }

  for (size_t i = 0; i <= divisions; ++i)
  {
    float fPercent = float(i) / float(divisions);
    fPercent = (fPercent * 1.0f) - 0.0f;

    Vector3 scale = xaxis * fPercent + origin;

    DirectX::VertexPositionColor v1(scale + Vector3(0, 2, 0), DirectX::Colors::Black);
    DirectX::VertexPositionColor v2(scale + yaxis, DirectX::Colors::Black);
    m_batch->DrawLine(v1, v2);
  }
  */


  for (unsigned int quad_ix = 0; quad_ix < fill_tiles.size(); quad_ix += 4)
  {
    m_batch->DrawQuad(fill_tiles[quad_ix], fill_tiles[quad_ix + 1], fill_tiles[quad_ix + 2], fill_tiles[quad_ix + 3]);
  }  
  
  for (unsigned int i = 0; i < TILES_DIMENSION * TILES_DIMENSION; ++i)
  {
    const SceneTile t = tiles[i]; //object slice is ok
    m_batch->DrawTriangle(t.vpc_pos[VPos::TOP_LEFT], t.vpc_pos[VPos::BOTTOM_LEFT], t.vpc_pos[VPos::TOP_RIGHT]);
    m_batch->DrawTriangle(t.vpc_pos[VPos::BOTTOM_RIGHT], t.vpc_pos[VPos::BOTTOM_LEFT], t.vpc_pos[VPos::TOP_RIGHT]);
  }
  m_batch->End();

  m_d3dContext->OMSetBlendState(m_states->AlphaBlend(), NULL, 0xFFFFFFFF);
  m_batch->Begin();
  for (unsigned int i = 0; i < TILES_DIMENSION * TILES_DIMENSION; ++i)
  {
    const SceneTile w = sea_tiles[i];
    if (w.height > -1.f)
    {
      m_batch->DrawTriangle(w.vpc_pos[VPos::TOP_LEFT], w.vpc_pos[VPos::BOTTOM_LEFT], w.vpc_pos[VPos::TOP_RIGHT]);
      m_batch->DrawTriangle(w.vpc_pos[VPos::BOTTOM_RIGHT], w.vpc_pos[VPos::BOTTOM_LEFT], w.vpc_pos[VPos::TOP_RIGHT]);
    }
  }
  m_batch->End();



  m_spriteBatch->Begin();
  std::wstring translation = L"Yaw: " + std::to_wstring(yaw) + L", Pitch: " + std::to_wstring(pitch);
  m_font->DrawString(m_spriteBatch.get(), translation.c_str(), DirectX::XMFLOAT2(5, 5), DirectX::Colors::White, 0.f, DirectX::XMFLOAT2(0, 0));
  std::wstring position = L"Position: <" + std::to_wstring(m_position.x) + L", " + std::to_wstring(m_position.y) + L", " + std::to_wstring(m_position.z) + L">";
  m_font->DrawString(m_spriteBatch.get(), position.c_str(), DirectX::XMFLOAT2(5, 5 + 12), DirectX::Colors::White, 0.f, DirectX::XMFLOAT2(0, 0));
  m_font->DrawString(m_spriteBatch.get(), L"+", DirectX::XMFLOAT2(window_cx, window_cy), DirectX::Colors::Black, 0.f, DirectX::XMFLOAT2(0, 0));
  m_spriteBatch->End();

  HRESULT hr = m_swapChain->Present(1, 0);
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

  TileType winner_type = winner > 0.f ? a.map_tile->type : b.map_tile->type;
  DirectX::XMVECTORF32 winner_color = winner_type == ETT_WATERFALL ?
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
        if(x + 1 == TILES_DIMENSION)
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
        if(y + 1 == TILES_DIMENSION)
          FillEdgeSceneTile(index, Edge::BOTTOM);
      }
    }    
  }
}

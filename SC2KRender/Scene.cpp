#include "Scene.h"
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

void Scene::Initialize(HWND window, int width, int height)
{
  m_window = window;
  m_outputWidth = width;
  m_outputHeight = height;

  RECT rect;
  GetWindowRect(m_window, &rect);
  window_cx = static_cast<int>(rect.left + ((rect.right - rect.left) / 2));
  window_cy = static_cast<int>(rect.top + ((rect.bottom - rect.top) / 2));
  SetCursorPos(window_cx, window_cy);
  //ShowCursor(FALSE);

  CreateDevice();
  CreateResources();

  m_view = Matrix::CreateLookAt(Vector3(3.f, 2.f, 3.f), Vector3(0.f, 0.f, 0.f), Vector3::UnitY);
  m_proj = Matrix::CreatePerspectiveFieldOfView(DirectX::XM_PI / 4.f, float(width) / float(height), 0.1f, 10.f);

  m_effect->SetView(m_view);
  m_effect->SetProjection(m_proj);
  tiles = new SceneTile[TILES_DIMENSION * TILES_DIMENSION];
  for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
  {
    for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
    {
      SceneTile& t = tiles[x + TILES_DIMENSION * y];
      float fx = static_cast<float>(x);
      float fy = static_cast<float>(y);
      t.v_topleft = Vector3(fx, 0, fy);
      t.v_bottomleft = Vector3(fx, 0, fy + 1);
      t.v_topright = Vector3(fx + 1, 0, fy);
      t.v_bottomright = Vector3(fx + 1, 0, fy + 1);
      //t.CreateVertexPositionColors();
    }
  }
  /*
  tiles[0 + 16 * 0].FillAttributes(TileType::ETT_CORNER_SW, 0);
  tiles[0 + 16 * 1].FillAttributes(TileType::ETT_SLOPE_W, 0);
  tiles[0 + 16 * 2].FillAttributes(TileType::ETT_CORNER_NW, 0);
  tiles[1 + 16 * 0].FillAttributes(TileType::ETT_SLOPE_S, 0);
  tiles[1 + 16 * 1].FillAttributes(TileType::ETT_FLAT, 1 * HEIGHT_INCREMENT);
  tiles[1 + 16 * 2].FillAttributes(TileType::ETT_SLOPE_N, 0);
  tiles[2 + 16 * 0].FillAttributes(TileType::ETT_CORNER_SE, 0);
  tiles[2 + 16 * 1].FillAttributes(TileType::ETT_SLOPE_E, 0);
  tiles[2 + 16 * 2].FillAttributes(TileType::ETT_CORNER_NE, 0);
  */

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
    dxgiFactory->CreateSwapChainForHwnd(
      m_d3dDevice.Get(),
      m_window,
      &swapChainDesc,
      &fsSwapChainDesc,
      nullptr,
      m_swapChain.ReleaseAndGetAddressOf()
    );
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

void Scene::MouseLook(float x, float z, float y)
{
  SetCursorPos(window_cx, window_cy);
  float dx = x - window_cx;
  float dy = y - window_cy;
  dx *= 0.001f;
  dy *= 0.001f;
  if (std::abs(dx) > 0.1 || std::abs(dy) > 0.1)
    return;
  look_x += dx;
  look_y -= dy;
  look_z -= dx;
}

void Scene::Update(DX::StepTimer const& timer)
{

  float elapsedTime = float(timer.GetElapsedSeconds());
  if (GetAsyncKeyState(VK_ESCAPE))
    PostQuitMessage(0);
  else if (GetAsyncKeyState(VK_RIGHT))
    rotation_y += 0.01f;
  else if (GetAsyncKeyState(VK_LEFT))
    rotation_y -= 0.01f;
  else if (GetAsyncKeyState(VK_UP))
    z += 0.001f;
  else if (GetAsyncKeyState(VK_DOWN))
    z -= 0.001f;

  else if (GetAsyncKeyState(0x57)) //W 
  {
    eye_x -= 0.01f;
    eye_z -= 0.01f;
  }
  else if (GetAsyncKeyState(0x53))
  {
    eye_x += 0.01f;
    eye_z += 0.01f;
  }
  else if (GetAsyncKeyState(0x41)) //A
  {
    eye_x -= 0.01f;
    eye_z += 0.01f;
  }
  else if (GetAsyncKeyState(0x44)) //D
  {
    eye_x += 0.01f;
    eye_z -= 0.01f;
  }

  else if (GetAsyncKeyState(0x52)) //R
    eye_y += 0.01f;
  else if (GetAsyncKeyState(0x46)) //F
    eye_y -= 0.01f;

  rotation_y = atan2(sin(rotation_y), cos(rotation_y));

  m_world = Matrix::Identity;
  m_world *= Matrix::CreateRotationY(rotation_y);
  m_world *= Matrix::CreateScale(z);
  m_view = Matrix::CreateLookAt(Vector3(eye_x, eye_y, eye_z), Vector3(look_x, look_y, look_z), Vector3::UnitY);


  elapsedTime;
}

void Scene::Render()
{
  // Don't try to render anything before the first Update.
  if (m_timer.GetFrameCount() == 0)
  {
    return;
  }

  Clear();


  m_d3dContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
  m_d3dContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
  m_d3dContext->RSSetState(m_states->CullNone());

  m_effect->SetWorld(m_world);
  m_effect->SetView(m_view);

  m_effect->Apply(m_d3dContext.Get());

  m_d3dContext->IASetInputLayout(m_inputLayout.Get());



  m_batch->Begin();

  Vector3 xaxis((float)TILES_DIMENSION, 0.f, 0.f);
  Vector3 yaxis(0.f, 0.f, (float)TILES_DIMENSION);
  Vector3 origin = Vector3::Zero;
  size_t divisions = TILES_DIMENSION;



  for (size_t i = 0; i <= divisions; i++)
  {
    float fPercent = float(i) / float(divisions);
    fPercent = (fPercent * 1.0f) - 0.0f;

    Vector3 scale = yaxis * fPercent + origin;

    DirectX::VertexPositionColor v1(scale + Vector3::Zero, DirectX::Colors::Black);
    DirectX::VertexPositionColor v2(scale + xaxis, DirectX::Colors::Black);
    m_batch->DrawLine(v1, v2);
  }

  for (size_t i = 0; i <= divisions; ++i)
  {
    float fPercent = float(i) / float(divisions);
    fPercent = (fPercent * 1.0f) - 0.0f;

    Vector3 scale = xaxis * fPercent + origin;

    DirectX::VertexPositionColor v1(scale + Vector3::Zero, DirectX::Colors::Black);
    DirectX::VertexPositionColor v2(scale + yaxis, DirectX::Colors::Black);
    m_batch->DrawLine(v1, v2);
  }

  for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
  {
    for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
    {
      const SceneTile t = tiles[x + TILES_DIMENSION * y];
      m_batch->DrawTriangle(t.vpc_topleft, t.vpc_bottomleft, t.vpc_topright);
      m_batch->DrawTriangle(t.vpc_bottomright, t.vpc_bottomleft, t.vpc_topright);
    }
  }
   m_batch->End();


  m_spriteBatch->Begin();
  std::wstring text = L"RotX: " + std::to_wstring(rotation_x) + L", RotY: " + std::to_wstring(rotation_y) + L", Scale: " + std::to_wstring(z);
  m_font->DrawString(m_spriteBatch.get(), text.c_str(), DirectX::XMFLOAT2(5, 5), DirectX::Colors::White, 0.f, DirectX::XMFLOAT2(0, 0));
  m_spriteBatch->End();

  m_swapChain->Present(1, 0);
}
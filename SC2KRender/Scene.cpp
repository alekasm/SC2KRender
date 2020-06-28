#include "Scene.h"

void Scene::Initialize(HWND window, int width, int height)
{
  m_window = window;
  m_outputWidth = width;
  m_outputHeight = height;

  RECT rect;
  GetWindowRect(m_window, &rect);
  window_cx = rect.left + ((rect.right - rect.left) / 2);
  window_cy = rect.top + ((rect.bottom - rect.top) / 2);
  SetCursorPos(window_cx, window_cy);
  //ShowCursor(FALSE);

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
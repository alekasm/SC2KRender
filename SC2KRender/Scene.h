#pragma once
#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#define WIN32_LEAN_AND_MEAN

#define HEIGHT_INCREMENT 0.5f

#include <Windows.h>
#include <memory>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>
#include "DirectXTK.h"

#pragma comment(lib, "D3D11")

#include "MapData.h"

namespace DirectX::Colors
{
  DirectX::XMFLOAT4 SC2K_DIRT_FLAT(0.6078431f, 0.5294117f, 0.2784313f, 1.0f);
  DirectX::XMFLOAT4 SC2K_DIRT_BRIGHT(0.6941176f, 0.6235294f, 0.3725490f, 1.0f);
  DirectX::XMFLOAT4 SC2K_DIRT_DARK(0.5294117f, 0.4196078f, 0.2f, 1.0f);
  DirectX::XMFLOAT4 SC2K_DIRT_DARKER(0.4509803f, 0.3254901f, 0.1372549f, 1.0f);
  DirectX::XMFLOAT4 SC2K_DIRT_DARKEST(0.3725490f, 0.2313725f, 0.0745098f, 1.0f);
};

struct SceneTile : public MapTile
{
  DirectX::XMFLOAT3 v_topleft, v_topright, v_bottomleft, v_bottomright;
  MapTile map_tile;
  SceneTile(const MapTile& map_tile)
  {
    this->map_tile = map_tile;
  }
};


class Scene
{
public:
  void Initialize(HWND window, int width, int height);
  void MouseLook(float x, float z, float y);
  void Tick();

private:
  HWND m_window;
  int m_outputWidth;
  int m_outputHeight;

  D3D_FEATURE_LEVEL m_featureLevel;
  Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext;
  Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

  std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
  std::unique_ptr<DirectX::SpriteFont> m_font;
  std::unique_ptr<DirectX::CommonStates> m_states;
  std::unique_ptr<DirectX::BasicEffect> m_effect;
  std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
  DirectX::SimpleMath::Matrix m_world;
  DirectX::SimpleMath::Matrix m_view;
  DirectX::SimpleMath::Matrix m_proj;

  float window_cx = 0, window_cy = 0;
  float look_x = 0, look_y = 0, look_z = 0;
  float eye_x = 3.f, eye_y = 2.f, eye_z = 3.f;

};
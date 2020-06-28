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
#define TILES_DIMENSION 16


#include <Windows.h>
#include <memory>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>
#pragma comment(lib, "D3D11")

#include "DirectXTK.h"
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
  DirectX::SimpleMath::Vector3 v_topleft, v_topright, v_bottomleft, v_bottomright;
  DirectX::VertexPositionColor vpc_topleft, vpc_topright, vpc_bottomleft, vpc_bottomright;
  DirectX::XMVECTORF32 c_topleft, c_topright, c_bottomleft, c_bottomright;
  MapTile map_tile;
};


class Scene
{
public:
  void Initialize(HWND window, int width, int height);
  void MouseLook(float x, float z, float y);
  void Tick();
  void Render();
  void Update(DX::StepTimer const& timer);

private:
  void CreateDevice();
  void CreateResources();
  void Clear();

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
  DX::StepTimer m_timer;
  float rotation_x = 0.0f;
  float rotation_y = 0.8f;
  float z = 0.1f;
  int window_cx = 0, window_cy = 0;//
  float look_x = 0.f, look_y = 0.f, look_z = 0.f;
  float eye_x = 3.f, eye_y = 2.f, eye_z = 3.f;
  SceneTile* tiles;

};
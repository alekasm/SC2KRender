#pragma once
#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#define WIN32_LEAN_AND_MEAN

#define HEIGHT_INCREMENT 1.f


#include <Windows.h>
#include <memory>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>
#pragma comment(lib, "D3D11")

#include "DirectXTK.h"
#include "MapData.h"

struct SceneTile;

namespace DX
{
  inline void ThrowIfFailed(HRESULT hr)
  {
    if (FAILED(hr))
    {
      // Set a breakpoint on this line to catch DirectX API errors
      throw std::exception();
    }
  }
}

class Scene
{
public:
  void Initialize(HWND window, MapTile* tiles, int width, int height);
  void MouseLook(int x, int z, int y);
  void MouseClick();
  void Tick();
  void Render();
  void Update(DX::StepTimer const& timer);
  void MultiplyMovementSpeed(float value);

private:
  void CreateDevice();
  void CreateResources();
  void Clear();

  HWND m_window;
  int m_outputWidth;
  int m_outputHeight;
  bool render_scene = false;

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
  float yaw = 0.f;
  float pitch = 0.f;
  float scale = 0.1f;
  float move_speed = 0.3f * scale;
  int window_cx = 0, window_cy = 0;
  SceneTile* tiles = nullptr;
  std::vector<DirectX::VertexPositionColor> fill_tiles;
  DirectX::SimpleMath::Vector3 m_position;
};
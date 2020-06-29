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
#pragma comment(lib, "D3D11")

#include "DirectXTK.h"
#include "MapData.h"

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


namespace DirectX::Colors
{
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_FLAT = { {{0.6078431f, 0.5294117f, 0.2784313f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_BRIGHT = { {{0.6941176f, 0.6235294f, 0.3725490f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_DARK = { {{0.5294117f, 0.4196078f, 0.2f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_DARKER = { {{0.4509803f, 0.3254901f, 0.1372549f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_DARKEST = { {{0.5529411f, 0.4f, 0.1725490f, 1.0f}} };
};

struct SceneTile
{
  DirectX::SimpleMath::Vector3 v_topleft, v_topright, v_bottomleft, v_bottomright;
  DirectX::VertexPositionColor vpc_topleft, vpc_topright, vpc_bottomleft, vpc_bottomright;
  DirectX::XMVECTORF32 c_topleft, c_topright, c_bottomleft, c_bottomright;
  float height;

  SceneTile()
  {
    c_topleft = DirectX::Colors::SC2K_DIRT_FLAT;
    c_topright = DirectX::Colors::SC2K_DIRT_FLAT;
    c_bottomleft = DirectX::Colors::SC2K_DIRT_FLAT;
    c_bottomright = DirectX::Colors::SC2K_DIRT_FLAT;
  }

  void CreateVertexPositionColors()
  {
    vpc_topleft = DirectX::VertexPositionColor(v_topleft, c_topleft);
    vpc_bottomleft = DirectX::VertexPositionColor(v_bottomleft, c_bottomleft);
    vpc_topright = DirectX::VertexPositionColor(v_topright, c_topright);
    vpc_bottomright = DirectX::VertexPositionColor(v_bottomright, c_bottomright);
  }

  void Rotate90Degrees()
  {
    DirectX::SimpleMath::Vector3 v_topleft_original = v_topleft;
    v_topleft = v_bottomleft;
    v_bottomleft = v_bottomright;
    v_bottomright = v_topright;
    v_topright = v_topleft_original;  

    DirectX::XMVECTORF32 c_topleft_original = c_topleft;
    c_topleft = c_bottomleft;
    c_bottomleft = c_bottomright;
    c_bottomright = c_topright;
    c_topright = c_topleft_original;
  }

  void ColorTile(DirectX::XMVECTORF32 color)
  {
    c_topleft = color;
    c_topright = color;
    c_bottomleft = color;
    c_bottomright = color;
    CreateVertexPositionColors();
  }

  void FillAttributes(const MapTile& tile)
  {
    this->height = static_cast<float>(tile.height * HEIGHT_INCREMENT);
    v_topleft.y = height;
    v_topright.y = height;
    v_bottomleft.y = height;
    v_bottomright.y = height;
    switch (tile.type)
    {
    case ETT_SLOPE_E:
      v_topleft.y += HEIGHT_INCREMENT;
      v_bottomleft.y += HEIGHT_INCREMENT;
      c_topleft = DirectX::Colors::SC2K_DIRT_BRIGHT;
      c_topright = DirectX::Colors::SC2K_DIRT_BRIGHT;
      c_bottomleft = DirectX::Colors::SC2K_DIRT_BRIGHT;
      c_bottomright = DirectX::Colors::SC2K_DIRT_BRIGHT;
      break;      
    case ETT_SLOPE_N:
      v_topleft.y += HEIGHT_INCREMENT;
      v_topright.y += HEIGHT_INCREMENT;
      c_topleft = DirectX::Colors::SC2K_DIRT_DARKER;
      c_topright = DirectX::Colors::SC2K_DIRT_DARKER;
      c_bottomleft = DirectX::Colors::SC2K_DIRT_DARKER;
      c_bottomright = DirectX::Colors::SC2K_DIRT_DARKER;
      break;
    case ETT_SLOPE_S:
      v_bottomleft.y += HEIGHT_INCREMENT;
      v_bottomright.y += HEIGHT_INCREMENT;
      c_topleft = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_topright = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_bottomleft = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_bottomright = DirectX::Colors::SC2K_DIRT_DARKEST;
      break;
    case ETT_SLOPE_W:
      v_topright.y += HEIGHT_INCREMENT;
      v_bottomright.y += HEIGHT_INCREMENT;
      c_topleft = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_topright = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_bottomleft = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_bottomright = DirectX::Colors::SC2K_DIRT_DARKEST;
      break;
    case ETT_SLOPE_NE:
      v_topleft.y += HEIGHT_INCREMENT;
      v_topright.y += HEIGHT_INCREMENT;
      v_bottomleft.y += HEIGHT_INCREMENT;
      c_bottomright = DirectX::Colors::SC2K_DIRT_DARKER;
      break;
    case ETT_SLOPE_NW:
      v_topleft.y += HEIGHT_INCREMENT;
      v_bottomright.y += HEIGHT_INCREMENT;
      v_topright.y += HEIGHT_INCREMENT;
      c_bottomleft = DirectX::Colors::SC2K_DIRT_DARKER;
      //v_bottomleft.y += HEIGHT_INCREMENT;
      //c_topright = DirectX::Colors::SC2K_DIRT_DARKER;
      break;
    case ETT_SLOPE_SE:
      v_topleft.y += HEIGHT_INCREMENT;
      v_bottomright.y += HEIGHT_INCREMENT;
      v_bottomleft.y += HEIGHT_INCREMENT;
      c_topright = DirectX::Colors::SC2K_DIRT_DARKER;
      break;
    case ETT_SLOPE_SW:
      v_topright.y += HEIGHT_INCREMENT;
      v_bottomright.y += HEIGHT_INCREMENT;
      v_bottomleft.y += HEIGHT_INCREMENT;
      c_topleft = DirectX::Colors::SC2K_DIRT_DARKER;
      break;
    case ETT_CORNER_NE:
      v_topleft.y += HEIGHT_INCREMENT;
      c_topleft = DirectX::Colors::SC2K_DIRT_BRIGHT;
      c_topright = DirectX::Colors::SC2K_DIRT_DARK;
      c_bottomleft = DirectX::Colors::SC2K_DIRT_DARK;
      break;
    case ETT_CORNER_SE:
      v_bottomleft.y += HEIGHT_INCREMENT;
      c_topleft = DirectX::Colors::SC2K_DIRT_DARK;
      c_bottomright = DirectX::Colors::SC2K_DIRT_DARK;
      Rotate90Degrees();
      break;
    case ETT_CORNER_SW:
      v_bottomright.y += HEIGHT_INCREMENT;
      c_bottomright = DirectX::Colors::SC2K_DIRT_BRIGHT;
      //c_bottomright = DirectX::Colors::SC2K_DIRT_DARKEST;
      break;
    case ETT_CORNER_NW:
      v_topright.y += HEIGHT_INCREMENT;      
      c_topright = DirectX::Colors::SC2K_DIRT_BRIGHT;
      c_topleft = DirectX::Colors::SC2K_DIRT_DARK;
      c_bottomright = DirectX::Colors::SC2K_DIRT_DARK;
      Rotate90Degrees();
      break;
    }
    CreateVertexPositionColors();
  }
};


class Scene
{
public:
  void Initialize(HWND window, MapTile* tiles, int width, int height);
  void MouseLook(int x, int z, int y);
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
  float z = 0.1f;
  int window_cx = 0, window_cy = 0;
  float look_x = 0.f, look_y = 0.f, look_z = 0.f;
  float eye_x = 3.f, eye_y = 2.f, eye_z = 3.f;
  SceneTile* tiles;

};
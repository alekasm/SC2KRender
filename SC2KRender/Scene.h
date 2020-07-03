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
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_EXPOSED = { {{0.3725490f, 0.2313725f, 0.0745098f, 1.0f}} };
};

struct SceneTile
{
  enum VertexPos {TOP_LEFT, BOTTOM_LEFT, TOP_RIGHT, BOTTOM_RIGHT};
  DirectX::SimpleMath::Vector3 v_pos[4];
  DirectX::XMVECTORF32 c_pos[4];
  DirectX::VertexPositionColor vpc_pos[4];

  float height;
 // m_batch->DrawTriangle(t.vpc_pos[TOP_LEFT], t.vpc_bottomleft, t.vpc_topright);
 // m_batch->DrawTriangle(t.vpc_bottomright, t.vpc_bottomleft, t.vpc_topright);
  SceneTile()
  {
    c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_FLAT;
    c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_FLAT;
    c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_FLAT;
    c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_FLAT;
  }

  void CreateVertexPositionColors()
  {
    vpc_pos[TOP_LEFT] = DirectX::VertexPositionColor(v_pos[TOP_LEFT], c_pos[TOP_LEFT]);
    vpc_pos[BOTTOM_LEFT] = DirectX::VertexPositionColor(v_pos[BOTTOM_LEFT], c_pos[BOTTOM_LEFT]);
    vpc_pos[TOP_RIGHT] = DirectX::VertexPositionColor(v_pos[TOP_RIGHT], c_pos[TOP_RIGHT]);
    vpc_pos[BOTTOM_RIGHT] = DirectX::VertexPositionColor(v_pos[BOTTOM_RIGHT], c_pos[BOTTOM_RIGHT]);
  }

  void Rotate90Degrees()
  {
    DirectX::VertexPositionColor original = vpc_pos[TOP_LEFT];
    vpc_pos[TOP_LEFT] = vpc_pos[BOTTOM_LEFT];
    vpc_pos[BOTTOM_LEFT] = vpc_pos[BOTTOM_RIGHT];
    vpc_pos[BOTTOM_RIGHT] = vpc_pos[TOP_RIGHT];
    vpc_pos[TOP_RIGHT] = original;
   // ColorTile(DirectX::Colors::Purple);
  }

  void ColorTile(DirectX::XMVECTORF32 color)
  {
    c_pos[TOP_LEFT] = color;
    c_pos[TOP_RIGHT] = color;
    c_pos[BOTTOM_LEFT] = color;
    c_pos[BOTTOM_RIGHT] = color;
    CreateVertexPositionColors();
  }

  void FillAttributes(const MapTile& tile)
  {
    this->height = static_cast<float>(tile.height * HEIGHT_INCREMENT);
    v_pos[TOP_LEFT].y = height;
    v_pos[TOP_RIGHT].y = height;
    v_pos[BOTTOM_LEFT].y = height;
    v_pos[BOTTOM_RIGHT].y = height;
    switch (tile.type)
    {
    case ETT_WATER_SUBMERGED_SLOPE_N:
    case ETT_UNDERWATER_SLOPE_N:
    case ETT_SLOPE_N:
    case ETT_SURFACE_WATER_SLOPE_N:
      v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
      v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
      c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_BRIGHT;
      c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_BRIGHT;
      c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_BRIGHT;
      c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_BRIGHT;
      //ColorTile(DirectX::Colors::Red);
      break;     
    case ETT_WATER_SUBMERGED_SLOPE_E:
    case ETT_UNDERWATER_SLOPE_E:
    case ETT_SLOPE_E:
    case ETT_SURFACE_WATER_SLOPE_E:
      v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
      v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
      c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_DARKER;
      c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKER;
      c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_DARKER;
      c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKER;
      break;
    case ETT_WATER_SUBMERGED_SLOPE_W:
    case ETT_SLOPE_W:
    case ETT_UNDERWATER_SLOPE_W:
    case ETT_SURFACE_WATER_SLOPE_W:
      v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
      v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
      c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKEST;
      break;
    case ETT_WATER_SUBMERGED_SLOPE_S:
    case ETT_UNDERWATER_SLOPE_S:
    case ETT_SLOPE_S:
    case ETT_SURFACE_WATER_SLOPE_S:
      v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
      v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
      c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_DARKEST;
      c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKEST;
      break;
    case ETT_WATER_SUBMERGED_SLOPE_NE:
    case ETT_UNDERWATER_SLOPE_NE:
    case ETT_SLOPE_NE:
    case ETT_SURFACE_WATER_SLOPE_NE:
      v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
      v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
      v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
      c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKER;
      break;
    case ETT_WATER_SUBMERGED_SLOPE_NW:
    case ETT_UNDERWATER_SLOPE_NW:
    case ETT_SLOPE_NW:
    case ETT_SURFACE_WATER_SLOPE_NW:
      v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
      v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
      v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
      c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_DARKER;
      //v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
      //c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKER;
      break;
    case ETT_WATER_SUBMERGED_SLOPE_SE:
    case ETT_UNDERWATER_SLOPE_SE:
    case ETT_SLOPE_SE:
    case ETT_SURFACE_WATER_SLOPE_SE:
      v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
      v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
      v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
      //v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
      //v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
      c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKER;
      break;
    case ETT_WATER_SUBMERGED_SLOPE_SW:
    case ETT_UNDERWATER_SLOPE_SW:
    case ETT_SLOPE_SW:
    case ETT_SURFACE_WATER_SLOPE_SW:
      v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
      v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
      v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
      c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_DARKER;
      break;
    case ETT_WATER_SUBMERGED_CORNER_NE:
    case ETT_UNDERWATER_CORNER_NE:
    case ETT_CORNER_NE:
    case ETT_SURFACE_WATER_CORNER_NE:
      v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
      c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_BRIGHT;
      c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_DARK;
      c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_DARK;
      break;
    case ETT_WATER_SUBMERGED_CORNER_SE:
    case ETT_UNDERWATER_CORNER_SE:
    case ETT_CORNER_SE:
    case ETT_SURFACE_WATER_CORNER_SE:
      v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
      c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_DARK;
      c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_DARK;
      break;
    case ETT_WATER_SUBMERGED_CORNER_SW:
    case ETT_UNDERWATER_CORNER_SW:
    case ETT_CORNER_SW:
    case ETT_SURFACE_WATER_CORNER_SW:
      v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
      c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_BRIGHT;
      //c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKEST;
      break;
    case ETT_WATER_SUBMERGED_CORNER_NW:
    case ETT_UNDERWATER_CORNER_NW:
    case ETT_CORNER_NW:
    case ETT_SURFACE_WATER_CORNER_NW:
      v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
      c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_BRIGHT;
      c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_DARK;
      c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_DARK;      
      break;
    case ETT_WATER_SUBMERGED_HIGHGROUND:
    case ETT_UNDERWATER_HIGHGROUND:
    case ETT_HIGHGROUND:
    case ETT_SURFACE_WATER_HIGHGROUND:
      //ColorTile(DirectX::Colors::Red);
      v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
      v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
      v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
      v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
      ColorTile(DirectX::Colors::SC2K_DIRT_EXPOSED);
      break;
    case ETT_FLAT:
      break;
    case ETT_WATER_SUBMERGED_FLAT:
    case ETT_UNDERWATER_FLAT:
      ColorTile(DirectX::Colors::DarkBlue);
      break;
    case ETT_SURFACE_WATER_CANAL_WE:
    case ETT_SURFACE_WATER_CANAL_NS:
    case ETT_SURFACE_WATER_BAY_OPEN_S:
    case ETT_SURFACE_WATER_BAY_OPEN_W:
    case ETT_SURFACE_WATER_BAY_OPEN_N:
    case ETT_SURFACE_WATER_BAY_OPEN_E:
    case ETT_SURFACE_WATER_FLAT:
    default:
      ColorTile(DirectX::Colors::Red);
      break;
    }

    CreateVertexPositionColors();

    switch (tile.type)
    {
    case ETT_CORNER_NW:
    case ETT_CORNER_SE:
      Rotate90Degrees();
      break;
    }
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
  std::vector<DirectX::VertexPositionColor> fill_tiles;
};
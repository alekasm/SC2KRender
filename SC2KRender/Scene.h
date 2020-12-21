#pragma once
#define HEIGHT_INCREMENT 1.f
#define HIGHWAY_HEIGHT 0.75f
#include <Windows.h>
#include <memory>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>
#include <shared_mutex>
#include <condition_variable>
#include <future>

#pragma comment(lib, "D3D11")

#include "DirectXTK.h"
#include "MapData.h"

#include <dwrite.h>
#include <d2d1_1.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

struct SceneTile;
struct MapSceneTile;
struct Sprite2D;
struct Sprite3D;
struct Model3D;
using DirectX::SimpleMath::Vector3;

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
enum Edge { LEFT, TOP, BOTTOM, RIGHT };
typedef std::vector<std::pair<size_t, size_t>> ModelTileVector;

class Scene
{
public:
  void Initialize(MapTile* tiles);
  void PreInitialize(HWND window);
  void UpdateWindow(HWND window);
  void MouseLook(int x, int y);
  void MouseClick();
  void Tick();
  void Render();
  void SetRenderModels(bool);
  void Update(DX::StepTimer const& timer);
  void MultiplyMovementSpeed(float value);
  void SetScale(float);
  void SetRenderDebugUI(bool);
  void SetAABBFrustumCulling(bool);
  void SetMovementSpeed(float);
  void SetMouseSpeed(float);
  void SetRenderDistance(float);
  void SetMSAA(bool);
  void SetEnableVSync(bool);
  unsigned int GetMaxSampleCount();

  DirectX::SimpleMath::Matrix GetViewMatrix()
  {
    return m_view;
  }
  DirectX::SimpleMath::Matrix GetProjectionMatrix()
  {
    return m_proj;
  }
  DirectX::SimpleMath::Matrix GetWorldMatrix()
  {
    return m_world;
  }
  DirectX::SimpleMath::Vector2 GetWindowDimensions()
  {
    return DirectX::SimpleMath::Vector2(m_outputWidth, m_outputHeight);
  }
  float GetScale()
  {
    return scale;
  }
  void SetFocus(bool value)
  {
    focused = render_scene ? value : false;
    ShowCursor(!focused);
    if (focused)
    {
      SetCursorPos(window_cx, window_cy);
    }
  }
  bool HasFocus()
  {
    return focused;
  }

private:
  void CreateDevice();
  void CreateResources();
  void Clear();  
  void FillTileEdges();
  void FillTunnels();
  void AddSecondaryModel(const MapSceneTile&, const Model3D*, const XBLDType);
  void TransformHighwayOnRamp(const MapTile*, Model3D*);
  void TransformHighwayCorners(const std::vector<ModelTileVector>&);
  void TransformHighwayBridge(const std::vector<ModelTileVector>&);
  void ClusterTiles(const ModelTileVector&, float, std::vector<ModelTileVector>&);
  void RotateModel(int32_t, Model3D*);
  void SetDrawTileWithModel(MapSceneTile&);  
  
  BOOL FillMapSceneTile(const MapSceneTile&, const MapSceneTile&, Edge);
  BOOL FillEdgeSceneTile(unsigned int, Edge);

  //void DrawModel(Model3D*);

  HWND m_window;
  RECT m_window_coords;
  float m_outputWidth;
  float m_outputHeight;
  bool render_scene = false;
  bool render_models = true;
  bool render_debug_ui = true;
  bool use_aabb_frustum_culling = true;
  bool use_vsync = true;
  bool use_render_distance;
  bool use_4x_msaa = true;
  bool focused = false;


  D3D_FEATURE_LEVEL m_featureLevel;
  Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext;
  Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
  Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;

  //Text Drawing
  Microsoft::WRL::ComPtr<ID2D1DeviceContext> device_context;
  Microsoft::WRL::ComPtr<IDWriteFactory> wfactory;
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> whiteBrush;
  IDWriteTextFormat* format = NULL;

  //Deferred Context
  ID3D11CommandList* pd3dCommandList = NULL;
  ID3D11DeviceContext* pd3dDeferredContext = NULL;

  std::unique_ptr<DirectX::CommonStates> m_states;
  std::unique_ptr<DirectX::BasicEffect> m_effect;
  std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_texbatch;
  std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
  std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

  std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
  //std::unique_ptr<DirectX::Model> m_model;

  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
  DirectX::SimpleMath::Matrix m_world;
  DirectX::SimpleMath::Matrix m_view;
  DirectX::SimpleMath::Matrix m_proj;
  DirectX::SimpleMath::Viewport m_viewport;
  DX::StepTimer m_timer;
  float rotation_x = 0.0f;
  float rotation_y = 0.8f;
  float yaw = 0.f;
  float pitch = 0.f;
  float scale = 0.1f;
  float base_move_speed = 0.2f;
  float mouse_move_speed = 0.003f;
  float move_speed = base_move_speed * scale;
  unsigned int max_supported_sample_count = 0;
  //float fov = DirectX::XM_PI / 4.f;
  float fov = DirectX::XM_PI / 3.f;
  int window_cx = 0, window_cy = 0;
  int client_cx = 0, client_cy = 0;
  float render_distance = 128.f;
  float scaled_render_distance = render_distance * scale;
  BYTE map_orientation = 0x0;
  MapSceneTile* tiles = nullptr;
  SceneTile* sea_tiles = nullptr;
  std::vector<DirectX::VertexPositionColor> fill_tiles;
  DirectX::SimpleMath::Vector3 m_position;
  std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_texture;
  std::vector<Sprite3D*> v_sprite3d;
  std::vector<Sprite2D*> v_sprite2d;
  std::vector<Model3D*> v_model3d;
  };

  
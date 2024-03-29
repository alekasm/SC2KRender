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
struct QuadSceneTile;
struct MapSceneTile;
struct Sprite2D;
struct Sprite3D;
struct Model3D;
using DirectX::SimpleMath::Vector3;

const D3D11_INPUT_ELEMENT_DESC NormalEffectInstance[] =
{
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
    { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
    { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
    { "InstMatrix",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "InstMatrix",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "InstMatrix",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
};

class Scene
{
public:
  void Initialize(Map&);
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
  void SetFOV(float);
  void SetRenderDebugUI(bool);
  void SetAABBFrustumCulling(bool);
  void SetMovementSpeed(float);
  void SetMouseSpeed(float);
  void SetMSAA(unsigned int);
  void SetEnableVSync(bool);
  bool SetFullScreen(BOOL);
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
  void SetFocus(bool value, HCURSOR cursor)
  {
    focused = render_scene ? value : false;
    SetCursor(focused ? NULL : cursor);
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
  void DrawInstancedModel(Model3D* model, bool alpha, size_t count);

  HWND m_window;
  RECT m_window_coords;
  float m_outputWidth;
  float m_outputHeight;
  bool render_scene = false;
  bool render_models = true;
  bool render_debug_ui = false;
  bool use_aabb_frustum_culling = true;
  bool use_vsync = true;
  unsigned int msaa_value = 0;
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

  //Used for instancing
  std::unique_ptr<DirectX::NormalMapEffect> m_NormalMapEffect;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_NormalInputLayout;
  std::map<int32_t, Microsoft::WRL::ComPtr<ID3D11Buffer>> m_InstanceBuffer;


  std::unique_ptr<DirectX::BasicEffect> m_BasicEffect;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_BasicInputLayout;

   std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
  std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
  std::unique_ptr<DirectX::EffectFactory> m_fxFactory;

  
  DirectX::SimpleMath::Matrix m_world;
  DirectX::SimpleMath::Matrix m_view;
  DirectX::SimpleMath::Matrix m_proj;
  DirectX::SimpleMath::Viewport m_viewport;
  DX::StepTimer m_timer;
  float rotation_x = 0.0f;
  float rotation_y = 0.8f;
  float yaw = 0.f;
  float pitch = 0.f;
  float scale = 1.f; //Dev-use only
  //float base_move_speed = 0.2f;
  float mouse_move_speed;
  float move_speed;
  unsigned int max_supported_sample_count = 0;
  float fov = 70.f * DirectX::XM_PI / 180.f;
  int window_cx = 0, window_cy = 0;
  int client_cx = 0, client_cy = 0;
  MapSceneTile** tiles = nullptr;
  MapTile* map_tiles = nullptr;
  const Map map;
  std::vector<QuadSceneTile*> fill_tiles;
  std::vector<QuadSceneTile*> fill_tiles_alpha;
  DirectX::SimpleMath::Vector3 m_position;
  std::vector<Sprite3D*> v_sprite3d;
  std::vector<Sprite2D*> v_sprite2d;
  std::vector<Model3D*> v_model3d;
  std::map<int32_t, std::vector<Model3D*>> m_model3d;

  };

  
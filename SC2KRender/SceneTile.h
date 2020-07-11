#pragma once
#include "Scene.h"
namespace DirectX::Colors
{
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_FLAT = { {{0.6078431f, 0.5294117f, 0.2784313f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_BRIGHT = { {{0.6941176f, 0.6235294f, 0.3725490f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_DARK = { {{0.5294117f, 0.4196078f, 0.2f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_DARKER = { {{0.4509803f, 0.3254901f, 0.1372549f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_DARKEST = { {{0.5529411f, 0.4f, 0.1725490f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_EXPOSED = { {{0.3725490f, 0.2313725f, 0.0745098f, 1.0f}} };
}

struct SceneTile
{  
  enum VertexPos { TOP_LEFT, BOTTOM_LEFT, TOP_RIGHT, BOTTOM_RIGHT };
  DirectX::SimpleMath::Vector3 v_pos[4];
  DirectX::XMVECTORF32 c_pos[4];
  DirectX::VertexPositionColor vpc_pos[4];
  float height;
  SceneTile();
  void CreateVertexPositionColors();
  void Rotate90Degrees();
  void ColorTile(DirectX::XMVECTORF32 color);
  void FillAttributes(const MapTile& tile); 
};

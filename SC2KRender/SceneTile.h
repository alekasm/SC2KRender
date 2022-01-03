#pragma once
#include "Scene.h"
#include "ColorConstants.h"

struct SceneTile
{  
  enum VertexPos { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };
  DirectX::SimpleMath::Vector3 v_pos[4];
  DirectX::XMVECTORF32 c_pos[4];
  DirectX::VertexPositionColor vpc_pos[4];
  //DirectX::VertexPositionTexture vpc_tex[4];
  //DirectX::VertexPositionNormalColorTexture vpc_pos[4];
  DirectX::XMVECTORF32 fill_color;
  float height = -1.f;
  TileRenderMode render = ALWAYS;
  SceneTile();
  
  void CreateVertexPositionColors();
  void Rotate90Degrees();
  void ColorTile(DirectX::XMVECTORF32 color, bool update = true);
  void SetHeight(int height);
  void SetOrigin(unsigned int x, unsigned int y);  
};

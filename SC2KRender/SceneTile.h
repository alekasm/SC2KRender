#pragma once
#include "Scene.h"
#include "ColorConstants.h"

enum TileRenderMode { ALWAYS, NEVER, MODEL_VISIBLE, MODEL_HIDDEN };

struct SceneTile
{  
  enum VertexPos { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };
  DirectX::SimpleMath::Vector3 v_pos[4];
  DirectX::XMVECTORF32 c_pos[4];
  DirectX::VertexPositionColor vpc_pos[4];
  DirectX::XMVECTORF32 fill_color;
  float height = -1.f;
  TileRenderMode render = ALWAYS;

  //bool render = true;
  SceneTile();
  
  void CreateVertexPositionColors();
  void Rotate90Degrees();
  void ColorTile(DirectX::XMVECTORF32 color, bool update = true);
  void SetHeight(int height);
  void SetOrigin(unsigned int x, unsigned int y);
  
};

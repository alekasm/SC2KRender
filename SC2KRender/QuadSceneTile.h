#pragma once
#include "Scene.h"
#include "ColorConstants.h"

struct QuadSceneTile
{
  DirectX::VertexPositionColor vpc[4];
  //DirectX::VertexPositionNormalColorTexture vpc[4];
  bool render_with_models = true;
};
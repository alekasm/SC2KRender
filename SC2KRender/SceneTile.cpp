#include "SceneTile.h"

SceneTile::SceneTile()
{
  c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_FLAT;
  c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_FLAT;
  c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_FLAT;
  c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_FLAT;
}

void SceneTile::CreateVertexPositionColors()
{
  vpc_pos[TOP_LEFT] = DirectX::VertexPositionColor(v_pos[TOP_LEFT], c_pos[TOP_LEFT]);
  vpc_pos[BOTTOM_LEFT] = DirectX::VertexPositionColor(v_pos[BOTTOM_LEFT], c_pos[BOTTOM_LEFT]);
  vpc_pos[TOP_RIGHT] = DirectX::VertexPositionColor(v_pos[TOP_RIGHT], c_pos[TOP_RIGHT]);
  vpc_pos[BOTTOM_RIGHT] = DirectX::VertexPositionColor(v_pos[BOTTOM_RIGHT], c_pos[BOTTOM_RIGHT]);
}

void SceneTile::Rotate90Degrees()
{
  DirectX::VertexPositionColor original = vpc_pos[TOP_LEFT];
  vpc_pos[TOP_LEFT] = vpc_pos[BOTTOM_LEFT];
  vpc_pos[BOTTOM_LEFT] = vpc_pos[BOTTOM_RIGHT];
  vpc_pos[BOTTOM_RIGHT] = vpc_pos[TOP_RIGHT];
  vpc_pos[TOP_RIGHT] = original;
  // ColorTile(DirectX::Colors::Purple);
}

void SceneTile::ColorTile(DirectX::XMVECTORF32 color)
{
  c_pos[TOP_LEFT] = color;
  c_pos[TOP_RIGHT] = color;
  c_pos[BOTTOM_LEFT] = color;
  c_pos[BOTTOM_RIGHT] = color;
  CreateVertexPositionColors();
}

void SceneTile::SetHeight(int height)
{
  this->height = static_cast<float>(height * HEIGHT_INCREMENT);
  v_pos[TOP_LEFT].y = height;
  v_pos[TOP_RIGHT].y = height;
  v_pos[BOTTOM_LEFT].y = height;
  v_pos[BOTTOM_RIGHT].y = height;
}

void SceneTile::SetOrigin(unsigned int x, unsigned int y)
{ //Sets origin using top-left
  float fx = static_cast<float>(x);
  float fy = static_cast<float>(y);
  v_pos[TOP_LEFT] = DirectX::SimpleMath::Vector3(fx, 0, fy);
  v_pos[BOTTOM_LEFT] = DirectX::SimpleMath::Vector3(fx, 0, fy + 1);
  v_pos[TOP_RIGHT] = DirectX::SimpleMath::Vector3(fx + 1, 0, fy);
  v_pos[BOTTOM_RIGHT] = DirectX::SimpleMath::Vector3(fx + 1, 0, fy + 1);
}
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

void SceneTile::FillAttributes(const MapTile& tile)
{
  map_tile = &tile;
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
    c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_DARKEST;
    c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKEST;
    c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_DARKEST;
    c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKEST;
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
    c_pos[TOP_LEFT] = DirectX::Colors::SC2K_DIRT_DARKER;
    c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKER;
    c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_DARKER;
    c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKER;
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
    c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKER;
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
    c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_DARKER;
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
    c_pos[BOTTOM_LEFT] = DirectX::Colors::SC2K_DIRT_DARK;
    c_pos[TOP_RIGHT] = DirectX::Colors::SC2K_DIRT_DARK;
    //c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_BRIGHT;
    //c_pos[BOTTOM_RIGHT] = DirectX::Colors::SC2K_DIRT_DARKEST;
    break;
  case ETT_WATER_SUBMERGED_CORNER_NW:
  case ETT_UNDERWATER_CORNER_NW:
  case ETT_CORNER_NW:
  case ETT_SURFACE_WATER_CORNER_NW:
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
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
    //ColorTile(DirectX::Colors::Red);
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
#include "MapSceneTile.h"

void MapSceneTile::FillAttributes(const MapTile* tile)
{
  map_tile = tile;
  if (tile->type / 0x10 == 3)
  {
    SetHeight(tile->height - 1);   
  }
  else
  {
    SetHeight(tile->height);
  }  
  const std::vector<DirectX::XMVECTORF32>& colors = color_palette[0];
  switch (tile->type)
  {
  case ETT_WATER_SUBMERGED_SLOPE_N:
  case ETT_UNDERWATER_SLOPE_N:
  case ETT_SLOPE_N:
  case ETT_SURFACE_WATER_SLOPE_N:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::BRIGHT];
    break;
  case ETT_WATER_SUBMERGED_SLOPE_E:
  case ETT_UNDERWATER_SLOPE_E:
  case ETT_SLOPE_E:
  case ETT_SURFACE_WATER_SLOPE_E:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_3];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_3];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_3];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_3];
    break;
  case ETT_WATER_SUBMERGED_SLOPE_W:
  case ETT_SLOPE_W:
  case ETT_UNDERWATER_SLOPE_W:
  case ETT_SURFACE_WATER_SLOPE_W:
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_3];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_3];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_3];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_3];
    break;
  case ETT_WATER_SUBMERGED_SLOPE_S:
  case ETT_UNDERWATER_SLOPE_S:
  case ETT_SLOPE_S:
  case ETT_SURFACE_WATER_SLOPE_S:
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_2];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_2];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_2];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_2];
    break;
  case ETT_WATER_SUBMERGED_SLOPE_NE:
  case ETT_UNDERWATER_SLOPE_NE:
  case ETT_SLOPE_NE:
  case ETT_SURFACE_WATER_SLOPE_NE:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_2];
    break;
  case ETT_WATER_SUBMERGED_SLOPE_NW:
  case ETT_UNDERWATER_SLOPE_NW:
  case ETT_SLOPE_NW:
  case ETT_SURFACE_WATER_SLOPE_NW:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_2];
    break;
  case ETT_WATER_SUBMERGED_SLOPE_SE:
  case ETT_UNDERWATER_SLOPE_SE:
  case ETT_SLOPE_SE:
  case ETT_SURFACE_WATER_SLOPE_SE:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_2];
    break;
  case ETT_WATER_SUBMERGED_SLOPE_SW:
  case ETT_UNDERWATER_SLOPE_SW:
  case ETT_SLOPE_SW:
  case ETT_SURFACE_WATER_SLOPE_SW:
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_2];
    break;
  case ETT_WATER_SUBMERGED_CORNER_NE:
  case ETT_UNDERWATER_CORNER_NE:
  case ETT_CORNER_NE:
  case ETT_SURFACE_WATER_CORNER_NE:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::NORMAL];
    break;
  case ETT_WATER_SUBMERGED_CORNER_SE:
  case ETT_UNDERWATER_CORNER_SE:
  case ETT_CORNER_SE:
  case ETT_SURFACE_WATER_CORNER_SE:
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::NORMAL];
    break;
  case ETT_WATER_SUBMERGED_CORNER_SW:
  case ETT_UNDERWATER_CORNER_SW:
  case ETT_CORNER_SW:
  case ETT_SURFACE_WATER_CORNER_SW:
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_1];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_1];
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::NORMAL];
    break;
  case ETT_WATER_SUBMERGED_CORNER_NW:
  case ETT_UNDERWATER_CORNER_NW:
  case ETT_CORNER_NW:
  case ETT_SURFACE_WATER_CORNER_NW:
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::NORMAL];
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::BRIGHT];
    break;
  case ETT_WATER_SUBMERGED_HIGHGROUND:
  case ETT_UNDERWATER_HIGHGROUND:
  case ETT_HIGHGROUND:
  case ETT_SURFACE_WATER_HIGHGROUND:
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    ColorTile(colors[SC2K_COLOR::DARKEST]);
    break;
  case ETT_WATERFALL:
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT * 2;
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT * 2;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT * 2;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT * 2;
    ColorTile(DirectX::Colors::SC2K_SEA_BLUE_STATIC);
    break;
  case ETT_FLAT:
  case ETT_WATER_SUBMERGED_FLAT:
  case ETT_UNDERWATER_FLAT:
    break;
  case ETT_SURFACE_WATER_BAY_OPEN_S: //These tiles contain semi-circle water sprites
  case ETT_SURFACE_WATER_BAY_OPEN_W: //no good way to handle them for now
  case ETT_SURFACE_WATER_BAY_OPEN_N:
  case ETT_SURFACE_WATER_BAY_OPEN_E:
  case ETT_SURFACE_WATER_FLAT:
    break;
  case ETT_SURFACE_WATER_CANAL_WE: //These tiles have have thin ground on
  case ETT_SURFACE_WATER_CANAL_NS: //each side of water, no good way to handle
    break;
  default:
    break;
  }

  CreateVertexPositionColors();

  switch (tile->type)
  {
  case ETT_WATER_SUBMERGED_CORNER_NW:
  case ETT_UNDERWATER_CORNER_NW:
  case ETT_CORNER_NW:
  case ETT_SURFACE_WATER_CORNER_NW:
  case ETT_WATER_SUBMERGED_CORNER_SE:
  case ETT_UNDERWATER_CORNER_SE:
  case ETT_CORNER_SE:
  case ETT_SURFACE_WATER_CORNER_SE:
    Rotate90Degrees();
    break;
  }
}
#include "MapSceneTile.h"

void MapSceneTile::FillAttributes(const MapTile* tile)
{
  map_tile = tile;
  bool is_sea_tile = map_tile->xter / 0x10 > 0 && map_tile->xter != XTER_WATERFALL;
  if (is_sea_tile)
  {
    sea_tile1 = new SceneTile();
    sea_tile1->SetHeight(map_tile->water_height);
    sea_tile1->ColorTile(DirectX::Colors::SC2K_SEA_BLUE);
    sea_tile1->fill_color = DirectX::Colors::SC2K_SEA_BLUE;
  }
  if (tile->xter / 0x10 == 3)
  {
    SetHeight(tile->height - 1);   
  }
  else
  {
    SetHeight(tile->height);
  }  
  const std::vector<DirectX::XMVECTORF32>& colors = color_palette[0];
  switch (tile->xter)
  {
  case XTER_WATER_SUBMERGED_SLOPE_N:
  case XTER_UNDERWATER_SLOPE_N:
  case XTER_SLOPE_N:
  case XTER_SURFACE_WATER_SLOPE_N:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::BRIGHT];
    break;
  case XTER_WATER_SUBMERGED_SLOPE_E:
  case XTER_UNDERWATER_SLOPE_E:
  case XTER_SLOPE_E:
  case XTER_SURFACE_WATER_SLOPE_E:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_3];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_3];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_3];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_3];
    break;
  case XTER_WATER_SUBMERGED_SLOPE_W:
  case XTER_SLOPE_W:
  case XTER_UNDERWATER_SLOPE_W:
  case XTER_SURFACE_WATER_SLOPE_W:
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_3];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_3];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_3];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_3];
    break;
  case XTER_WATER_SUBMERGED_SLOPE_S:
  case XTER_UNDERWATER_SLOPE_S:
  case XTER_SLOPE_S:
  case XTER_SURFACE_WATER_SLOPE_S:
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_2];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_2];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_2];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_2];
    break;
  case XTER_WATER_SUBMERGED_SLOPE_NE:
  case XTER_UNDERWATER_SLOPE_NE:
  case XTER_SLOPE_NE:
  case XTER_SURFACE_WATER_SLOPE_NE:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_2];
    break;
  case XTER_WATER_SUBMERGED_SLOPE_NW:
  case XTER_UNDERWATER_SLOPE_NW:
  case XTER_SLOPE_NW:
  case XTER_SURFACE_WATER_SLOPE_NW:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_2];
    break;
  case XTER_WATER_SUBMERGED_SLOPE_SE:
  case XTER_UNDERWATER_SLOPE_SE:
  case XTER_SLOPE_SE:
  case XTER_SURFACE_WATER_SLOPE_SE:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_2];
    break;
  case XTER_WATER_SUBMERGED_SLOPE_SW:
  case XTER_UNDERWATER_SLOPE_SW:
  case XTER_SLOPE_SW:
  case XTER_SURFACE_WATER_SLOPE_SW:
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_2];
    break;
  case XTER_WATER_SUBMERGED_CORNER_NE:
  case XTER_UNDERWATER_CORNER_NE:
  case XTER_CORNER_NE:
  case XTER_SURFACE_WATER_CORNER_NE:
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::NORMAL];
    break;
  case XTER_WATER_SUBMERGED_CORNER_SE:
  case XTER_UNDERWATER_CORNER_SE:
  case XTER_CORNER_SE:
  case XTER_SURFACE_WATER_CORNER_SE:
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::NORMAL];
    break;
  case XTER_WATER_SUBMERGED_CORNER_SW:
  case XTER_UNDERWATER_CORNER_SW:
  case XTER_CORNER_SW:
  case XTER_SURFACE_WATER_CORNER_SW:
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::BRIGHT];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::DARK_1];
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::DARK_1];
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::NORMAL];
    break;
  case XTER_WATER_SUBMERGED_CORNER_NW:
  case XTER_UNDERWATER_CORNER_NW:
  case XTER_CORNER_NW:
  case XTER_SURFACE_WATER_CORNER_NW:
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    c_pos[TOP_RIGHT] = colors[SC2K_COLOR::NORMAL];
    c_pos[TOP_LEFT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_RIGHT] = colors[SC2K_COLOR::DARK_1];
    c_pos[BOTTOM_LEFT] = colors[SC2K_COLOR::BRIGHT];
    break;
  case XTER_WATER_SUBMERGED_HIGHGROUND:
  case XTER_UNDERWATER_HIGHGROUND:
  case XTER_HIGHGROUND:
  case XTER_SURFACE_WATER_HIGHGROUND:
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT;
    ColorTile(colors[SC2K_COLOR::DARKEST]);
    break;
  case XTER_WATERFALL:
    v_pos[TOP_RIGHT].y += HEIGHT_INCREMENT * 2;
    v_pos[TOP_LEFT].y += HEIGHT_INCREMENT * 2;
    v_pos[BOTTOM_RIGHT].y += HEIGHT_INCREMENT * 2;
    v_pos[BOTTOM_LEFT].y += HEIGHT_INCREMENT * 2;
    ColorTile(DirectX::Colors::SC2K_SEA_BLUE_STATIC);
    fill_color = DirectX::Colors::SC2K_SEA_BLUE_STATIC_BRIGHT;
    break;
  case XTER_FLAT:
  case XTER_WATER_SUBMERGED_FLAT:
  case XTER_UNDERWATER_FLAT:
    break;
  case XTER_SURFACE_WATER_BAY_OPEN_S: //These tiles contain semi-circle water sprites
  case XTER_SURFACE_WATER_BAY_OPEN_W: //no good way to handle them for now
  case XTER_SURFACE_WATER_BAY_OPEN_N:
  case XTER_SURFACE_WATER_BAY_OPEN_E:
  case XTER_SURFACE_WATER_FLAT:
    break;
  case XTER_SURFACE_WATER_CANAL_WE: //These tiles have have thin ground on
  case XTER_SURFACE_WATER_CANAL_NS: //each side of water, no good way to handle
    break;
  default:
    break;
  }

  CreateVertexPositionColors();

  switch (tile->xter)
  {
  case XTER_WATER_SUBMERGED_CORNER_NW:
  case XTER_UNDERWATER_CORNER_NW:
  case XTER_CORNER_NW:
  case XTER_SURFACE_WATER_CORNER_NW:
  case XTER_WATER_SUBMERGED_CORNER_SE:
  case XTER_UNDERWATER_CORNER_SE:
  case XTER_CORNER_SE:
  case XTER_SURFACE_WATER_CORNER_SE:
    Rotate90Degrees();
    break;
  }
}
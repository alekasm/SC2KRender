#include "ModifyModel.h"
#include "AssetLoader.h"
#include "MapSceneTile.h"

enum Origin { MODEL, TILE };
struct SecondaryProps
{
  bool repeat = false;  
  Origin origin = Origin::TILE;
  SecondaryProps(Origin origin)
  {
    this->origin = origin;
  }
  SecondaryProps(bool repeat)
  {
    this->repeat = repeat;
    if (repeat) origin = Origin::MODEL;
  }
  SecondaryProps(){}
};

uint32_t ModifyModel::map_rotation = 0;
std::map<XBLDType, std::vector<std::pair<int32_t, SecondaryProps>>> ModifyModel::scenery_object_map =
{
  {XBLD_HIGHWAY_BRIDGE_1, {{SceneryObject::BRIDGE_REINFORCED, SecondaryProps(Origin::MODEL)},
                           {SceneryObject::PILLAR, SecondaryProps(true)}                           
                          } 
  },
  {XBLD_HIGHWAY_BRIDGE_2, {{SceneryObject::BRIDGE_REINFORCED, SecondaryProps(Origin::MODEL)}}},
  {XBLD_BRIDGE_SUSPENSION_PIECE_1, {{SceneryObject::BRIDGE_SUSPENSION_1, SecondaryProps(Origin::MODEL)}}},
  {XBLD_BRIDGE_SUSPENSION_PIECE_2, {{SceneryObject::BRIDGE_SUSPENSION_2, SecondaryProps(Origin::MODEL)}}},
  {XBLD_BRIDGE_SUSPENSION_PIECE_3, {{SceneryObject::BRIDGE_SUSPENSION_3, SecondaryProps(Origin::MODEL)},
                                    {SceneryObject::BRIDGE_SUSPENSION_PILLAR, SecondaryProps(true)}}},
  {XBLD_BRIDGE_SUSPENSION_PIECE_4, {{SceneryObject::BRIDGE_SUSPENSION_2, SecondaryProps(Origin::MODEL)}}},
  {XBLD_BRIDGE_SUSPENSION_PIECE_5, {{SceneryObject::BRIDGE_SUSPENSION_1, SecondaryProps(Origin::MODEL)}}},
  {XBLD_HIGHWAY_1, {{SceneryObject::PILLAR, SecondaryProps(true)}}},
  {XBLD_HIGHWAY_2, {{SceneryObject::PILLAR, SecondaryProps(true)}}},
  {XBLD_BRIDGE_RAIL_PIECE_1, {{SceneryObject::PILLAR, SecondaryProps(true)}}},
  {XBLD_HIGHWAY_CROSSOVER_4, {{XBLD_RAIL_1, SecondaryProps()}}},
  {XBLD_HIGHWAY_CROSSOVER_3, {{XBLD_RAIL_2, SecondaryProps()}}},
  {XBLD_HIGHWAY_CROSSOVER_2, {{XBLD_ROAD_1, SecondaryProps()}}},
  {XBLD_HIGHWAY_CROSSOVER_1, {{XBLD_ROAD_2, SecondaryProps()}}},
  {XBLD_HIGHWAY_CROSSOVER_1, {{XBLD_ROAD_2, SecondaryProps()}}},
  {XBLD_TREES_7, {{SceneryObject::TREE_TRUNKS_7, SecondaryProps()}}},
  {XBLD_TREES_6, {{SceneryObject::TREE_TRUNKS_6, SecondaryProps()}}},
  {XBLD_TREES_5, {{SceneryObject::TREE_TRUNKS_5, SecondaryProps()}}},
  {XBLD_TREES_4, {{SceneryObject::TREE_TRUNKS_4, SecondaryProps()}}},
  {XBLD_TREES_3, {{SceneryObject::TREE_TRUNKS_3, SecondaryProps()}}},
  {XBLD_TREES_2, {{SceneryObject::TREE_TRUNKS_2, SecondaryProps()}}},
  {XBLD_TREES_1, {{SceneryObject::TREE_TRUNKS_1, SecondaryProps()}}},
  {XBLD_BRIDGE_COMMON_PIECE_1, {{SceneryObject::BRIDGE_RAISED_PILLAR, SecondaryProps(true)},
                                {SceneryObject::BRIDGE_RAISED_ARC, SecondaryProps(Origin::MODEL)}                                
                              }
  },
  {XBLD_PIER, {{SceneryObject::PIER_PILLAR, SecondaryProps(true)}}}
};

void ModifyModel::SetMapRotation(uint32_t rotation)
{
  map_rotation = rotation;
}

void ModifyModel::AdjustHydroElectricSea(MapSceneTile** tiles)
{
  for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
    for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
    {
      MapSceneTile* tile = tiles[x + TILES_DIMENSION * y];
      if (!XBLD_IS_HYDROELECTRIC(tile->map_tile->xbld)) continue;
      if (tile->map_tile->xter != XTER_WATERFALL)
      { //Adjustments only need to be made for waterfalls
        continue;
      }

      tile->SetHeight(tile->map_tile->height);


      if (x > 0)
      {
        MapSceneTile* left_tile = tiles[(x - 1) + TILES_DIMENSION * y];
        if (left_tile->sea_tile != nullptr)
        {
          SceneTile* left_sea_tile = left_tile->sea_tile;

          if (tile->v_pos[SceneTile::VertexPos::TOP_LEFT].y <
            left_sea_tile->v_pos[SceneTile::VertexPos::TOP_RIGHT].y)
          left_sea_tile->v_pos[SceneTile::VertexPos::TOP_RIGHT] =
            left_tile->v_pos[SceneTile::VertexPos::TOP_RIGHT];

          if (tile->v_pos[SceneTile::VertexPos::BOTTOM_LEFT].y <
            left_sea_tile->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT].y)
          left_sea_tile->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT] =
            left_tile->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT];

          left_sea_tile->CreateVertexPositionColors();
        }
      }

      if (y > 0)
      {
        MapSceneTile* top_tile = tiles[x  + TILES_DIMENSION * (y - 1)];
        if (top_tile->sea_tile != nullptr)
        {
          SceneTile* top_sea_tile = top_tile->sea_tile;

          if (tile->v_pos[SceneTile::VertexPos::TOP_LEFT].y <
            top_sea_tile->v_pos[SceneTile::VertexPos::BOTTOM_LEFT].y)
          top_sea_tile->v_pos[SceneTile::VertexPos::BOTTOM_LEFT] =
            top_tile->v_pos[SceneTile::VertexPos::BOTTOM_LEFT];

          if (tile->v_pos[SceneTile::VertexPos::TOP_RIGHT].y <
            top_sea_tile->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT].y)
          top_sea_tile->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT] =
            top_tile->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT];

          top_sea_tile->CreateVertexPositionColors();
        }
      }

      if (y < TILES_DIMENSION - 1)
      {
        MapSceneTile* bottom_tile = tiles[x + TILES_DIMENSION * (y + 1)];
        if (bottom_tile->sea_tile != nullptr)
        {
          SceneTile* bottom_sea_tile = bottom_tile->sea_tile;          
          
          if(tile->v_pos[SceneTile::VertexPos::BOTTOM_LEFT].y <
            bottom_sea_tile->v_pos[SceneTile::VertexPos::TOP_LEFT].y)
          bottom_sea_tile->v_pos[SceneTile::VertexPos::TOP_LEFT] =
            bottom_tile->v_pos[SceneTile::VertexPos::TOP_LEFT];
          
          if (tile->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT].y <
            bottom_sea_tile->v_pos[SceneTile::VertexPos::TOP_RIGHT].y)
          bottom_sea_tile->v_pos[SceneTile::VertexPos::TOP_RIGHT] =
            bottom_tile->v_pos[SceneTile::VertexPos::TOP_RIGHT];          

          bottom_sea_tile->CreateVertexPositionColors();
        }  
      }

      if (x < TILES_DIMENSION - 1)
      {
        MapSceneTile* right_tile = tiles[(x + 1) + TILES_DIMENSION * y];
        if (right_tile->sea_tile != nullptr)
        {
          SceneTile* right_sea_tile = right_tile->sea_tile;

          if (tile->v_pos[SceneTile::VertexPos::TOP_RIGHT].y <
            right_sea_tile->v_pos[SceneTile::VertexPos::TOP_LEFT].y)
          right_sea_tile->v_pos[SceneTile::VertexPos::TOP_LEFT] =
            right_tile->v_pos[SceneTile::VertexPos::TOP_LEFT];

          if (tile->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT].y <
            right_sea_tile->v_pos[SceneTile::VertexPos::BOTTOM_LEFT].y)
          right_sea_tile->v_pos[SceneTile::VertexPos::BOTTOM_LEFT] =
            right_tile->v_pos[SceneTile::VertexPos::BOTTOM_LEFT];

          right_sea_tile->CreateVertexPositionColors();
        }
      }

    }
}

void ModifyModel::RotateModel(int32_t model_id, Model3D* model, const MapTile* tile)
{
  switch (model_id)
  {
  case XBLD_TUNNEL_1:
  case XBLD_TUNNEL_3:
  case XBLD_ROAD_2:
  case XBLD_RAIL_2:
  case XBLD_CROSSOVER_ROAD_RAIL_1:
  case XBLD_CROSSOVER_RAIL_POWER_2:
  case XBLD_ROAD_9:
  case XBLD_RAIL_9:
  case XBLD_RAIL_SLOPE_1:
  case XBLD_RAIL_SLOPE_3:
  case XBLD_HIGHWAY_2:
  case XBLD_HIGHWAY_CROSSOVER_2:
  case XBLD_HIGHWAY_CROSSOVER_4:
  case XBLD_POWER_LINE_2:
  case XBLD_HIGHWAY_CROSSOVER_6:
  case XBLD_SUB_RAIL_1:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    break;
  case XBLD_HYDROELECTRIC_1:
  case XBLD_HYDROELECTRIC_2:
    if (map_rotation == 1 || map_rotation == 3)
    {
      model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
      model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    }
    break;
  case XBLD_ROAD_8:
  case XBLD_RAIL_8:
  case XBLD_SUB_RAIL_4:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI);
    model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
    break;
  case XBLD_ROAD_7:
  case XBLD_RAIL_7:
  case XBLD_SUB_RAIL_3:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
    break;
  case XBLD_ROAD_3:
  case XBLD_RAIL_3:
  case XBLD_HIGHWAY_SLOPE_1:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitZ, -M_PI_4);
    model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    break;
  case XBLD_ROAD_4:
  case XBLD_RAIL_4:
  case XBLD_HIGHWAY_SLOPE_2:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitX, M_PI_4);
    break;
  case XBLD_ROAD_5:
  case XBLD_RAIL_5:
  case XBLD_HIGHWAY_SLOPE_3:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitZ, M_PI_4);
    model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    break;
  case XBLD_ROAD_6:
  case XBLD_RAIL_6:
  case XBLD_HIGHWAY_SLOPE_4:
    model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitX, -M_PI_4);
    break;  
  case SceneryObject::BRIDGE_RAISED_ARC:
  case SceneryObject::BRIDGE_RAISED_PILLAR: //bit 2 in xbit
    if ((tile->xbit & 0b00000010) == 0b00000010)
    {
      model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
      model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    }
    break;
  case SceneryObject::BRIDGE_SUSPENSION_1: 
  case SceneryObject::BRIDGE_SUSPENSION_2:
  case SceneryObject::BRIDGE_SUSPENSION_3:
  case SceneryObject::BRIDGE_SUSPENSION_PILLAR:
  {
    bool rotate = (tile->xbit & 0b00000010) == 0b00000010;
    if (rotate)
    {
      model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
      model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    }
    switch (tile->xbld)
    {
    case XBLD_BRIDGE_SUSPENSION_PIECE_1:
    case XBLD_BRIDGE_SUSPENSION_PIECE_2:
      if (rotate)
      {
        model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
        model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
      }
      else
      {
        model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI);
        model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
      }
      break;
    }
  }
  break;
  }
}



/*
* Instead of creating separate models for crossovers, we can overlap two
* existing models. Separate models may look better, or be more appropriate in
* the future.
*/
void ModifyModel::AddSecondaryModel(const MapSceneTile* t,
  const Model3D* rmodel,
  const XBLDType xbld,
  std::vector<Model3D*>* v_model3d)
{
  if (t->map_tile->xter == XTER_FLAT && XBLD_IS_TREE(xbld))
  { //only add trunks if on a slope
    return;
  }

  //Do not use t.map_tile->xbld
  auto it_lookup = scenery_object_map.find(xbld);
  if (it_lookup == scenery_object_map.end()) return;  
  for (const std::pair<uint32_t, SecondaryProps>& entry : it_lookup->second)
  {
    int32_t model_id = entry.first;
    bool repeat_y = entry.second.repeat;

    if (model_id == 0) return;
    if (repeat_y && rmodel == nullptr) return;
    std::map<std::wstring, std::shared_ptr<DirectX::Model>>::iterator it;
    it = AssetLoader::mmodels->find(AssetLoader::xbld_map.at(model_id));
    if (it == AssetLoader::mmodels->end())
    {
      return; //TODO this is an error
    }      
    float height_model = rmodel->origin.y;
    float height_tile = t->map_tile->height * HEIGHT_INCREMENT;

    //TODO Understand why
    if (repeat_y)
    {     
      height_model -= HEIGHT_INCREMENT;
      if (t->map_tile->height == t->map_tile->water_height)
      {
        height_tile -= HEIGHT_INCREMENT;
      }
    }

    float height = entry.second.origin == Origin::TILE ? height_tile : height_model;
  add_model:
    Vector3 reference_tile(t->v_pos[SceneTile::VertexPos::TOP_LEFT]);
    reference_tile.y = height;
    Model3D* model = new Model3D(model_id, it->second, reference_tile);
    ModifyModel::RotateModel(model_id, model, t->map_tile);
    v_model3d->push_back(model);  
    
    if (repeat_y && height >= height_tile)
    {        
      height -= HEIGHT_INCREMENT;
      goto add_model;
    }
  }
}

void ModifyModel::TransformHighwayOnRamp(const MapTile* map_tile, Model3D* model)
{
  switch (map_tile->xbld)
  {
  case XBLD_HIGHWAY_ONRAMP_1:
  case XBLD_HIGHWAY_ONRAMP_4:
    model->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
    break;
  case XBLD_HIGHWAY_ONRAMP_2:
  case XBLD_HIGHWAY_ONRAMP_3:
    model->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
    model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
    break;
  }
}

void ModifyModel::TransformHighwayCorners(MapSceneTile** tiles, 
  std::vector<Model3D*>* v_model3d, 
  const std::vector<ModelTileVector>& clusters)
{
  for (ModelTileVector mtv : clusters)
  {
    const MapTile* tile_cluster_TL = tiles[mtv.at(0).second]->map_tile;
    Model3D* model_cluster_TL = v_model3d->operator[](mtv.at(0).first);
    Model3D* model_cluster_TR = v_model3d->operator[](mtv.at(1).first);
    Model3D* model_cluster_BL = v_model3d->operator[](mtv.at(2).first);
    Model3D* model_cluster_BR = v_model3d->operator[](mtv.at(3).first);
    switch (tile_cluster_TL->xbld)
    {
    case XBLD_HIGHWAY_CORNER_1:
      model_cluster_BL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
      model_cluster_BL->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
      model_cluster_TR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      model_cluster_TR->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
      model_cluster_TL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      model_cluster_TL->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
      break;
    case XBLD_HIGHWAY_CORNER_2:
      //model_cluster_TL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI);
      model_cluster_TL->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
      model_cluster_TR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI);
      model_cluster_TR->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
      model_cluster_BL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI);
      model_cluster_BL->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI);
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 1.f);
      break;
    case XBLD_HIGHWAY_CORNER_3:
      model_cluster_TR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      model_cluster_TR->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
      model_cluster_BL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
      model_cluster_BL->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
      model_cluster_TL->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
      model_cluster_TL->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
      break;
    case XBLD_HIGHWAY_CORNER_4:
      model_cluster_BR->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI);
      break;
    }
  }
}

bool IsXZON10(const MapTile* tile)
{
  return tile->xzon == 0x10;
}

void ModifyModel::TransformHighwayBridge(MapSceneTile** tiles, std::vector<Model3D*>* v_model3d, 
  const std::vector<ModelTileVector>& clusters)
{
  for (ModelTileVector mtv : clusters)
  {
    const MapTile* tile_cluster_TL = tiles[mtv.at(0).second]->map_tile;
    const MapTile* tile_cluster_TR = tiles[mtv.at(1).second]->map_tile;
    const MapTile* tile_cluster_BL = tiles[mtv.at(2).second]->map_tile;
    const MapTile* tile_cluster_BR = tiles[mtv.at(3).second]->map_tile;
    const std::vector<const MapTile*> tile_mtv_vec =
    {
      tile_cluster_TL, tile_cluster_TR, tile_cluster_BL, tile_cluster_BR
    };

    std::vector<const MapTile*>::const_iterator it = std::find_if(tile_mtv_vec.begin(), tile_mtv_vec.end(), IsXZON10);
    if (it == tile_mtv_vec.end())
    {
      printf("Unable to generate Highway Bridge rotations\n");
      return;
    }
    size_t index = it - tile_mtv_vec.begin();
    BYTE rotation_mask = 0b00000010;
    bool should_rotate = (tile_mtv_vec.at(index)->xbit & rotation_mask) == rotation_mask;
    if (should_rotate)
    {
      /*
      * I'm making pretty dangerous assumptions regarding transforming mtv.at(i)->first + 1. The + 1 is assumed
      * to be the secondary model directly after the the road model. For the HIGHWAY_BRIDGE_1/2 this is the metal
      * cage model decoration.
      */
      v_model3d->operator[](mtv.at(0).first)->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      v_model3d->operator[](mtv.at(0).first)->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);

      v_model3d->operator[](mtv.at(0).first + 1)->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      v_model3d->operator[](mtv.at(0).first + 1)->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);

      v_model3d->operator[](mtv.at(1).first)->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      v_model3d->operator[](mtv.at(1).first)->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);

      v_model3d->operator[](mtv.at(1).first + 1)->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      v_model3d->operator[](mtv.at(1).first + 1)->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);

      v_model3d->operator[](mtv.at(2).first)->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      v_model3d->operator[](mtv.at(2).first)->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);

      v_model3d->operator[](mtv.at(2).first + 1)->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      v_model3d->operator[](mtv.at(2).first + 1)->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);

      v_model3d->operator[](mtv.at(3).first)->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      v_model3d->operator[](mtv.at(3).first)->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);

      v_model3d->operator[](mtv.at(3).first + 1)->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
      v_model3d->operator[](mtv.at(3).first + 1)->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
    }
  }

}

/*
 * SimCity 2000 maps do not have data for tunnel pieces besides the entry/exit. This
 * function fills in the missing data by finding an entry, then iterating until it finds
 * the exit - adding tunnel pieces in between.
 */
void ModifyModel::FillTunnels(MapSceneTile** tiles, std::vector<Model3D*>* v_model3d)
{
  for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
  {
    for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
    {
      unsigned int start_index = x + TILES_DIMENSION * y;
      const MapSceneTile* start_tile = tiles[start_index];

      if (start_tile->map_tile->xbld == XBLD_TUNNEL_3)
      {
        std::map<std::wstring, std::shared_ptr<DirectX::Model>>::iterator it;
        it = AssetLoader::mmodels->find(AssetLoader::xbld_map.at(start_tile->map_tile->xbld));
        if (it == AssetLoader::mmodels->end())
        {
          continue; //TODO - this should never happen
        }

        for (unsigned int x2 = x; x2 < TILES_DIMENSION; ++x2)
        {
          unsigned int end_index = x2 + TILES_DIMENSION * y;
          const MapSceneTile* end_tile = tiles[end_index];
          if (end_tile->map_tile->xbld == XBLD_TUNNEL_1)
          {
            break;
          }
          DirectX::SimpleMath::Vector3 position = end_tile->v_pos[SceneTile::VertexPos::TOP_LEFT];
          Model3D* model = new Model3D(end_tile->map_tile->xbld, it->second, position);
          model->origin.y = start_tile->height;
          ModifyModel::RotateModel(start_tile->map_tile->xbld, model, start_tile->map_tile);
          v_model3d->push_back(model);
        }
      }
      else if (start_tile->map_tile->xbld == XBLD_TUNNEL_4)
      {
        std::map<std::wstring, std::shared_ptr<DirectX::Model>>::iterator it;
        it = AssetLoader::mmodels->find(AssetLoader::xbld_map.at(start_tile->map_tile->xbld));
        if (it == AssetLoader::mmodels->end())
        {
          continue; //TODO - this should never happen
        }

        for (unsigned int y2 = y; y2 < TILES_DIMENSION; ++y2)
        {
          unsigned int end_index = x + TILES_DIMENSION * y2;
          const MapSceneTile* end_tile = tiles[end_index];
          if (end_tile->map_tile->xbld == XBLD_TUNNEL_2)
          {
            break;
          }
          DirectX::SimpleMath::Vector3 position = end_tile->v_pos[SceneTile::VertexPos::TOP_LEFT];
          Model3D* model = new Model3D(end_tile->map_tile->xbld, it->second, position);
          model->origin.y = start_tile->height;
          ModifyModel::RotateModel(start_tile->map_tile->xbld, model, start_tile->map_tile);
          v_model3d->push_back(model);
        }
      }
    }
  }
}
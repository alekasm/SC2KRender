#include "SceneGenerator.h"
#include "ModifyModel.h"
#include "AssetLoader.h"
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;

#define ARRAY_LENGTH TILES_DIMENSION * TILES_DIMENSION

enum Edge { LEFT, TOP, BOTTOM, RIGHT };

void SceneGenerator::Generate(MapTile* map_tiles, MapSceneTile** tiles, 
                              std::vector<Model3D*>& v_model3d,
                              std::vector<QuadSceneTile*>& fill_tiles,
                              uint32_t map_rotation)
{
  BYTE map_orientation = 1 << map_rotation;  

  //first = index to v_model3d
  //second = index to tiles
  ModelTileVector highway_model_vec;
  ModelTileVector highway_bridge_model_vec;

  for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
  {
    for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
    {
      unsigned int i_index = x + TILES_DIMENSION * y;

      MapSceneTile* t = new MapSceneTile();
      tiles[i_index] = t;

      const MapTile* map_tile = &map_tiles[i_index];
      t->SetOrigin(x, y);
      t->FillAttributes(map_tile);
      if (t->sea_tile != nullptr)
      {
        t->sea_tile->SetOrigin(x, y);
      }


#if USING_SPRITES_3D
      if (XBLD_IS_TREE(map_tile->xbld))
      {
        v_sprite3d.push_back(new Sprite3D(
          AssetLoader::mresources->at(L"tree"),
          t->v_pos[VPos::TOP_LEFT],
          Orientation::X));
      }
#endif
#if USING_SPRITES_2D
      if (XBLD_IS_TREE(map_tile->xbld))
      {
        v_sprite2d.push_back(new Sprite2D(
          AssetLoader::mresources->at(L"tree"),
          AssetLoader::mdescriptions->at(L"tree"),
          t->v_pos[VPos::TOP_LEFT]));
      }
#endif

      bool no_xzon = map_tile->xzon == 0;
      BYTE orientation = (map_tile->xzon >> 4);
      bool single_tile = orientation == 0b1111;

      bool render_tile = single_tile ||
        (!no_xzon && orientation == map_orientation) ||
        (no_xzon && (map_tile->xbit >> 4) == 0) ||
        (!XBLD_IS_BUILDING(map_tile->xbld));

      if (AssetLoader::xbld_map.count(map_tile->xbld))
      {
        auto it = AssetLoader::xbld_visible_map.find(t->map_tile->xbld);
        if (it != AssetLoader::xbld_visible_map.end())
        {
          t->render = it->second;
        }
        if (render_tile)
        {
          std::map<std::wstring, std::shared_ptr<DirectX::Model>>::iterator it;
          it = AssetLoader::mmodels->find(AssetLoader::xbld_map.at(map_tile->xbld));
          if (it != AssetLoader::mmodels->end())
          {
            DirectX::SimpleMath::Vector3 position = t->v_pos[SceneTile::VertexPos::TOP_LEFT];
            Model3D* model = new Model3D(map_tile->xbld, it->second, position);

            if (map_tile->xbit & 0b0100) //appears to be a "water tile"
            {
              model->origin.y = map_tile->water_height;
            }
            else if (map_tile->xter >= XTER_UNDERWATER_FLAT)
            {
              model->origin.y = map_tile->water_height;
            }
            //else if (map_tile->water_height == map_tile->height)           

            XBLDType xbld_value = map_tile->xbld;

            if (XBLD_IS_HIGHWAY_ONRAMP(map_tile->xbld))
            {
              ModifyModel::TransformHighwayOnRamp(map_tile, model);
              if (map_tile->xbit & 0b0010) //perhaps some sort of rotation
              {
                model->m_world_identity *= DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
                model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
              }
            }

            else if (XBLD_IS_BRIDGE(map_tile->xbld))
            {
              if (map_tile->xbit & 0b0010) //perhaps some sort of rotation
              {
                model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, -M_PI_2);
                model->m_world_identity *= DirectX::XMMatrixTranslation(1.f, 0.f, 0.f);
              }
              model->origin.y += HEIGHT_INCREMENT;
            }

            else if (XBLD_IS_TUNNEL(map_tile->xbld))
            {
              model->origin.y = t->height;
            }

            else if (XBLD_IS_TREES(map_tile->xbld))
            {
              if (map_tile->xter != XTER_FLAT)
              { //raise the trees on sloped terrains to add extended trunks
                model->origin.y = t->height + 1.f;
              }
              else
              {
                model->origin.y = t->height;
              }
            }

            else if (XBLD_IS_POWER_LINE(map_tile->xbld))
            {
              if (map_tile->xter != XTER_FLAT)
              { //raise the trees on sloped terrains to add extended trunks
                model->origin.y = t->height + HEIGHT_INCREMENT / 2.f; //midpoint
              }
              else
              {
                model->origin.y = t->height;
              }
            }

            else if (map_tile->xbld == XBLD_PIER || map_tile->xbld == XBLD_RUNWAY)
            {
              BYTE bit_check_mask = 0b00000010;
              bool toggled = (map_tile->xbit & bit_check_mask) == bit_check_mask;
              bool rotate;
              switch (map_rotation)
              {
              case 0:
              case 2:
                rotate = !toggled;
                break;
              case 1:
              case 3:
                rotate = toggled;
                break;
              default:
                rotate = false;
                break;
              }
              if (rotate)
              {
                model->m_world_identity = DirectX::XMMatrixRotationAxis(Vector3::UnitY, M_PI_2);
                model->m_world_identity *= DirectX::XMMatrixTranslation(0.f, 0.f, 1.f);
              }
            }

            else if (XBLD_IS_HIGHWAY(map_tile->xbld))
            {

              model->origin.y += HIGHWAY_HEIGHT;

              if (XBLD_IS_HIGHWAY_BRIDGE(map_tile->xbld))
              {
                model->origin.y += HEIGHT_INCREMENT;
              }

              else if (XBLD_IS_HIGHWAY_SLOPE(map_tile->xbld) &&
                map_tile->xter == XTER_HIGHGROUND)
              {
                switch (map_tile->xbld)
                {
                case XBLD_HIGHWAY_SLOPE_1:
                case XBLD_HIGHWAY_SLOPE_3:
                  xbld_value = XBLD_HIGHWAY_2;
                  break;
                case XBLD_HIGHWAY_SLOPE_2:
                case XBLD_HIGHWAY_SLOPE_4:
                  xbld_value = XBLD_HIGHWAY_1;
                  break;
                }
                std::map<std::wstring, std::shared_ptr<DirectX::Model>>::iterator it;
                it = AssetLoader::mmodels->find(AssetLoader::xbld_map.at(xbld_value));
                model->model = it->second;
              }

            } //end XBLD_IS_HIGHWAY   

            ModifyModel::RotateModel(xbld_value, model, map_tile);
            v_model3d.push_back(model);
            model->SetTileId(i_index);

            if (XBLD_IS_HIGHWAY_CORNER(map_tile->xbld))
            {
              highway_model_vec.push_back(std::make_pair(
                v_model3d.size() - 1,
                x + TILES_DIMENSION * y));
            }
            else if (XBLD_IS_HIGHWAY_BRIDGE(map_tile->xbld))
            {
              highway_bridge_model_vec.push_back(std::make_pair(
                v_model3d.size() - 1,
                x + TILES_DIMENSION * y));
            }

            ModifyModel::AddSecondaryModel(t, model, xbld_value, &v_model3d);

          }
        }
      }
    }
  }

  ModifyModel::AdjustHydroElectricSea(tiles);
  FillTileEdges(tiles, fill_tiles);

  ModifyModel::FillTunnels(tiles, &v_model3d);

  std::vector<ModelTileVector> highway_clusters;
  ClusterTiles(tiles, highway_model_vec, 1.0f, highway_clusters);
  ModifyModel::TransformHighwayCorners(tiles, &v_model3d, highway_clusters);

  std::vector<ModelTileVector> highway_bridge_clusters;
  ClusterTiles(tiles, highway_bridge_model_vec, 1.0f, highway_bridge_clusters);
  ModifyModel::TransformHighwayBridge(tiles, &v_model3d, highway_bridge_clusters);

}

void SceneGenerator::ClusterTiles(MapSceneTile** tiles, const ModelTileVector& vec,
                                  float dist, std::vector<ModelTileVector>& clusters)
{
  if (vec.empty()) return;

  if (vec.size() % 4)
  {
    printf("Error: Detected %d highway corners - cannot determine highway layout\n", vec.size());
    return;
  }

  printf("Received %d highway corners, should generate %d clusters\n", vec.size(), vec.size() / 4);

  ModelTileVector::const_iterator vec_it;
  for (vec_it = vec.begin(); vec_it != vec.end(); ++vec_it)
  {
    const MapTile* tile = tiles[vec_it->second]->map_tile;
    int x1 = vec_it->second % 128;
    int y1 = vec_it->second / 128;

    bool clustered = false;
    std::vector<ModelTileVector>::iterator cluster_it;
    for (cluster_it = clusters.begin(); cluster_it != clusters.end(); ++cluster_it)
    {
      for (std::pair<size_t, size_t> cluster : *cluster_it)
      {
        const MapTile* tile_cluster = tiles[cluster.second]->map_tile;
        int x2 = cluster.second % 128;
        int y2 = cluster.second / 128;

        if (tile_cluster->xbld != tile->xbld) continue;

        float distance = std::sqrt(
          std::pow(x2 - x1, 2) +
          std::pow(y2 - y1, 2));

        //printf("<%d, %d> -> <%d, %d> Distance: %f\n", x1, y1, x2, y2, distance);

        if (distance <= dist)
        {
          clustered = true;
          cluster_it->push_back(*vec_it);
          break;
        }
      }
      if (clustered) break;
    }
    if (!clustered)
    {
      clusters.push_back({ *vec_it });
    }
  }

  if (clusters.empty())
  {
    printf("Generated no clusters, however highway corners exist\n");
    return;
  }

  printf("Generated %d highway corner clusters\n", clusters.size());

  if (clusters.size() != vec.size() / 4)
  {
    printf("Incorrect amount of highway clusters generated\n");
    clusters.clear();
    return;
  }

  std::vector<ModelTileVector>::iterator cluster_it;
  for (cluster_it = clusters.begin(); cluster_it != clusters.end(); ++cluster_it)
  {
    if (cluster_it->size() != 4)
    {
      printf("Error: Found a highway corner cluster with %d tiles\n", cluster_it->size());
      clusters.clear();
      return;
    }
  }
}

void SceneGenerator::FillTileEdges(MapSceneTile** tiles, std::vector<QuadSceneTile*>& fill_tiles)
{
  //Fill-in tile polygons using minimal vertices, compare using top and left tiles.
  for (unsigned int y = 0; y < TILES_DIMENSION; ++y)
  {
    for (unsigned int x = 0; x < TILES_DIMENSION; ++x)
    {
      unsigned int index = x + TILES_DIMENSION * y;
      MapSceneTile* this_tile = tiles[index];

      if (x == 0)
      {
        FillEdgeSceneTile(tiles, fill_tiles, index, Edge::LEFT);
      }
      else if (x > 0)
      {
        unsigned int cmp_left_index = (x - 1) + TILES_DIMENSION * y;
        MapSceneTile* cmp_left = tiles[cmp_left_index];
        FillMapSceneTile(this_tile, fill_tiles, cmp_left, Edge::LEFT);
        if (x + 1 == TILES_DIMENSION)
          FillEdgeSceneTile(tiles, fill_tiles, index, Edge::RIGHT);
      }

      if (y == 0)
      {
        FillEdgeSceneTile(tiles, fill_tiles, index, Edge::TOP);
      }
      else if (y > 0)
      {
        unsigned int cmp_top_index = x + TILES_DIMENSION * (y - 1);
        MapSceneTile* cmp_top = tiles[cmp_top_index];
        FillMapSceneTile(this_tile, fill_tiles, cmp_top, Edge::TOP);
        if (y + 1 == TILES_DIMENSION)
          FillEdgeSceneTile(tiles, fill_tiles, index, Edge::BOTTOM);
      }
    }
  }
}


BOOL SceneGenerator::FillMapSceneTile(const MapSceneTile* a, std::vector<QuadSceneTile*>& fill_tiles, const MapSceneTile* b, Edge edge)
{
  Vector3 a1, a2, b1, b2;
  switch (edge)
  {
  case LEFT:
    a1 = a->v_pos[SceneTile::VertexPos::TOP_LEFT];
    a2 = a->v_pos[SceneTile::VertexPos::BOTTOM_LEFT];
    b1 = b->v_pos[SceneTile::VertexPos::TOP_RIGHT];
    b2 = b->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT];
    break;
  case RIGHT:
    a1 = a->v_pos[SceneTile::VertexPos::TOP_RIGHT];
    a2 = a->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT];
    b1 = b->v_pos[SceneTile::VertexPos::TOP_LEFT];
    b2 = b->v_pos[SceneTile::VertexPos::BOTTOM_LEFT];
    break;
  case TOP:
    a1 = a->v_pos[SceneTile::VertexPos::TOP_LEFT];
    a2 = a->v_pos[SceneTile::VertexPos::TOP_RIGHT];
    b1 = b->v_pos[SceneTile::VertexPos::BOTTOM_LEFT];
    b2 = b->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT];
    break;
  case BOTTOM:
    a1 = a->v_pos[SceneTile::VertexPos::BOTTOM_LEFT];
    a2 = a->v_pos[SceneTile::VertexPos::BOTTOM_RIGHT];
    b1 = b->v_pos[SceneTile::VertexPos::TOP_LEFT];
    b2 = b->v_pos[SceneTile::VertexPos::TOP_RIGHT];
    break;
  }

  // distance of 2 means the entire side is filled
  // distance of 1 means that a diagonal edge was filled
  // distance of 0 means that the edges are flush

  float winner = (a1.y - b1.y) + (a2.y - b2.y);
  QuadSceneTile* qst_terrain = nullptr;
  if (std::fabs(winner) > 0.f)
  {
    const MapSceneTile* winner_tile = winner > 0.f ? a : b;
    DirectX::XMVECTORF32 winner_color = winner_tile->fill_color;

    qst_terrain = new QuadSceneTile();
    qst_terrain->vpc[0] = DirectX::VertexPositionColor(a1, winner_color);
    qst_terrain->vpc[1] = DirectX::VertexPositionColor(a2, winner_color);
    qst_terrain->vpc[2] = DirectX::VertexPositionColor(b2, winner_color);
    qst_terrain->vpc[3] = DirectX::VertexPositionColor(b1, winner_color);
    
    if (winner_color.f[3] == 1.f)
      fill_tiles.push_back(qst_terrain);
    //else
    //  fill_tiles_alpha.push_back(qst_terrain);
  }
  return TRUE;


}

BOOL SceneGenerator::FillEdgeSceneTile(MapSceneTile** tiles, std::vector<QuadSceneTile*>& fill_tiles, unsigned int index, Edge edge)
{
  const MapSceneTile* a = tiles[index];
  Vector3 a1, a2, b1, b2;
  SceneTile::VertexPos pos1, pos2;
  switch (edge)
  {
  case LEFT:
    pos1 = SceneTile::VertexPos::TOP_LEFT;
    pos2 = SceneTile::VertexPos::BOTTOM_LEFT;
    break;
  case RIGHT:
    pos1 = SceneTile::VertexPos::TOP_RIGHT;
    pos2 = SceneTile::VertexPos::BOTTOM_RIGHT;
    break;
  case TOP:
    pos1 = SceneTile::VertexPos::TOP_LEFT;
    pos2 = SceneTile::VertexPos::TOP_RIGHT;
    break;
  case BOTTOM:
    pos1 = SceneTile::VertexPos::BOTTOM_LEFT;
    pos2 = SceneTile::VertexPos::BOTTOM_RIGHT;
    break;
  }
  a1 = a->v_pos[pos1];
  a2 = a->v_pos[pos2];
  b1 = Vector3(a1.x, 0.f, a1.z);
  b2 = Vector3(a2.x, 0.f, a2.z);

  QuadSceneTile* qst = new QuadSceneTile();

  
  qst->vpc[0] = DirectX::VertexPositionColor(a1, DirectX::Colors::SC2K_DIRT_DARKEST);
  qst->vpc[1] = DirectX::VertexPositionColor(a2, DirectX::Colors::SC2K_DIRT_DARKEST);
  qst->vpc[2] = DirectX::VertexPositionColor(b2, DirectX::Colors::SC2K_DIRT_DARKEST);
  qst->vpc[3] = DirectX::VertexPositionColor(b1, DirectX::Colors::SC2K_DIRT_DARKEST);
  
  fill_tiles.push_back(qst);
  const SceneTile* w = tiles[index]->sea_tile;
  if (w != nullptr)
  {
    QuadSceneTile* qst_sea = new QuadSceneTile();
    
    qst_sea->vpc[0] = DirectX::VertexPositionColor(w->v_pos[pos1], DirectX::Colors::SC2K_SEA_BLUE);
    qst_sea->vpc[1] = DirectX::VertexPositionColor(w->v_pos[pos2], DirectX::Colors::SC2K_SEA_BLUE);
    qst_sea->vpc[2] = DirectX::VertexPositionColor(a2, DirectX::Colors::SC2K_SEA_BLUE);
    qst_sea->vpc[3] = DirectX::VertexPositionColor(a1, DirectX::Colors::SC2K_SEA_BLUE);
    
    fill_tiles.push_back(qst_sea);
  }
  return TRUE;
}
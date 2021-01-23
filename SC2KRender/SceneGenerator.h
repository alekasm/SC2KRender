#pragma once
#include <vector>
#include "MapSceneTile.h"
#include "QuadSceneTile.h"
#include "ModifyModel.h"

enum Edge;
class SceneGenerator
{
public:
  static void Generate(MapTile*, MapSceneTile**, std::vector<Model3D*>&, std::vector<QuadSceneTile*>&, uint32_t);
private:
  static void ClusterTiles(MapSceneTile**, const ModelTileVector&, float, std::vector<ModelTileVector>&);
  static void FillTileEdges(MapSceneTile**, std::vector<QuadSceneTile*>&);
  static BOOL FillEdgeSceneTile(MapSceneTile** tiles, std::vector<QuadSceneTile*>&, unsigned int index, Edge edge);
  static BOOL FillMapSceneTile(const MapSceneTile* a, std::vector<QuadSceneTile*>&, const MapSceneTile* b, Edge edge);
};
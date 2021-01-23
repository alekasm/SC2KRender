#pragma once
#include "SceneTile.h"

struct MapSceneTile : public SceneTile
{
  void FillAttributes(const MapTile*);
  const MapTile* map_tile; 
  SceneTile* sea_tile = nullptr;
};
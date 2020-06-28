#pragma once
#include <string>
struct MapTile;
class MapLoader
{
public:
  static bool LoadMap(std::string filename, MapTile* &tiles);
};
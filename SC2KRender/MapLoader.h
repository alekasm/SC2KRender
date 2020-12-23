#pragma once
#include <string>
struct Map;
class MapLoader
{
public:
  static bool LoadMap(std::string filename, Map &out);
};
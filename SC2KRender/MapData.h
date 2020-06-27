#pragma once
#include <Windows.h>
#include <string>

#define FORM 0x4d524f46
#define ISCD 0x48444353 //Referred to as SCDH

enum TileType
{
  ETT_FLAT, // 0x00
  ETT_SLOPE_N, // 0x01
  ETT_SLOPE_E, // 0x02
  ETT_SLOPE_S, // 0x03
  ETT_SLOPE_W, // 0x04
  ETT_SLOPE_NE, // 0x05
  ETT_SLOPE_SE, // 0x06
  ETT_SLOPE_SW, // 0x07
  ETT_SLOPE_NW, // 0x08
  ETT_CORNER_NE, // 0x09
  ETT_CORNER_SE, // 0xA
  ETT_CORNER_SW, // 0xB
  ETT_CORNER_NW, // 0xC
  ETT_HIGHGROUND,
  ETT_UNUSED_0E,
  ETT_UNUSED_0F,
  ETT_UNDERWATER_FLAT,
  ETT_UNDERWATER_SLOPE_N,
  ETT_UNDERWATER_SLOPE_E,
  ETT_UNDERWATER_SLOPE_S,
  ETT_UNDERWATER_SLOPE_W,
  ETT_UNDERWATER_SLOPE_NE,
  ETT_UNDERWATER_SLOPE_SE,
  ETT_UNDERWATER_SLOPE_SW,
  ETT_UNDERWATER_SLOPE_NW,
  ETT_UNDERWATER_CORNER_NE,
  ETT_UNDERWATER_CORNER_SE,
  ETT_UNDERWATER_CORNER_SW,
  ETT_UNDERWATER_CORNER_NW,
  ETT_UNDERWATER_HIGHGROUND,
  ETT_UNUSED_1E,
  ETT_UNUSED_1F,
  ETT_WATER_SUBMERGED_FLAT,
  ETT_WATER_SUBMERGED_SLOPE_N,
  ETT_WATER_SUBMERGED_SLOPE_E,
  ETT_WATER_SUBMERGED_SLOPE_S,
  ETT_WATER_SUBMERGED_SLOPE_W,
  ETT_WATER_SUBMERGED_SLOPE_NE,
  ETT_WATER_SUBMERGED_SLOPE_SE,
  ETT_WATER_SUBMERGED_SLOPE_SW,
  ETT_WATER_SUBMERGED_SLOPE_NW,
  ETT_WATER_SUBMERGED_CORNER_NE,
  ETT_WATER_SUBMERGED_CORNER_SE,
  ETT_WATER_SUBMERGED_CORNER_SW,
  ETT_WATER_SUBMERGED_CORNER_NW,
  ETT_WATER_SUBMERGED_HIGHGROUND,
  ETT_UNUSED_2E,
  ETT_UNUSED_2F,
  ETT_SURFACE_WATER_FLAT,
  ETT_SURFACE_WATER_SLOPE_N,
  ETT_SURFACE_WATER_SLOPE_E,
  ETT_SURFACE_WATER_SLOPE_S,
  ETT_SURFACE_WATER_SLOPE_W,
  ETT_SURFACE_WATER_SLOPE_NE,
  ETT_SURFACE_WATER_SLOPE_SE,
  ETT_SURFACE_WATER_SLOPE_SW,
  ETT_SURFACE_WATER_SLOPE_NW,
  ETT_SURFACE_WATER_CORNER_NE,
  ETT_SURFACE_WATER_CORNER_SE,
  ETT_SURFACE_WATER_CORNER_SW,
  ETT_SURFACE_WATER_CORNER_NW,
  ETT_SURFACE_WATER_HIGHGROUND,
  ETT_WATERFALL,
  ETT_UNUSED_3F,
  ETT_SURFACE_WATER_CANAL_WE,
  ETT_SURFACE_WATER_CANAL_NS,
  ETT_SURFACE_WATER_BAY_OPEN_S,
  ETT_SURFACE_WATER_BAY_OPEN_W,
  ETT_SURFACE_WATER_BAY_OPEN_N,
  ETT_SURFACE_WATER_BAY_OPEN_E
};

struct MapTile
{
  int height;
  bool water;
  TileType type;
  int GetHeightValue()
  {
    return (height * 100) + 50;
  }
};

struct MapSegmentHeader
{
  char type[4];
  uint32_t totalbytes; 
};

struct MapHeader
{
  uint32_t iff_file;
  uint32_t totalbytes; //Stored in big-endian
  uint32_t file_type;
};

struct Map
{
  MapHeader header;
  MapTile* tiles;
  int sea_level;
  std::string city_name;
  //uint32_t founding_year;
  //uint32_t days_elapsed;
  //int32_t money_supply;
};
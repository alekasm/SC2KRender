#pragma once
#include <Windows.h>
#include <string>
#include "Types.h"
#define TILES_DIMENSION 128

#define FORM 0x4d524f46
#define ISCD 0x48444353 //Referred to as SCDH


struct MapTile
{
  uint16_t altm;
  int height; //derived from altm
  int water_height; //derived from altm
  XTERType xter;
  XBLDType xbld;
  XZONType xzon;
  XUNDType xund;
  XBITType xbit;
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
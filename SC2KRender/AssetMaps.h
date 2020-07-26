#pragma once
#include <map>
#include <string>
#include "Types.h"
namespace
{
  std::map<XBLDType, std::wstring> xbld_map =
  {
    {XBLD_LOWER_CLASS_1, L"lowerclass1"},
    {XBLD_LOWER_CLASS_2, L"lowerclass2"},
    {XBLD_LOWER_CLASS_3, L"lowerclass3"},
    {XBLD_ROAD_1, L"road1"},
    {XBLD_ROAD_2, L"road1"},
    {XBLD_ROAD_11, L"road3"},
    {XBLD_ROAD_12, L"road3"},
    {XBLD_ROAD_13, L"road3"},
    {XBLD_ROAD_14, L"road3"},
    {XBLD_ROAD_15, L"road3"},
    {XBLD_ROAD_7, L"roadcorner"},
    {XBLD_ROAD_8, L"roadcorner"},
    {XBLD_ROAD_9, L"roadcorner"},
    {XBLD_ROAD_10, L"roadcorner"},
    {XBLD_CROSSOVER_ROAD_POWER_1, L"road3"},
    {XBLD_CROSSOVER_ROAD_POWER_2, L"road3"},
    {XBLD_CROSSOVER_ROAD_RAIL_1, L"cross_road_rail"},
    {XBLD_CROSSOVER_ROAD_RAIL_2, L"cross_road_rail"},
    {XBLD_CROSSOVER_RAIL_POWER_1, L"rail1"},
    {XBLD_CROSSOVER_RAIL_POWER_2, L"rail1"},
    {XBLD_RAIL_1, L"rail1"},
    {XBLD_RAIL_2, L"rail1"},
    {XBLD_RAIL_11, L"railcross"},
    {XBLD_RAIL_12, L"railcross"},
    {XBLD_RAIL_13, L"railcross"},
    {XBLD_RAIL_14, L"railcross"},
    {XBLD_RAIL_15, L"railcross"},

  }; 
}
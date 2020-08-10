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
    {XBLD_MIDDLE_CLASS_1, L"middleclass1"},
    {XBLD_MIDDLE_CLASS_2, L"middleclass2"},
    {XBLD_MIDDLE_CLASS_3, L"middleclass3"},
    {XBLD_MIDDLE_CLASS_4, L"middleclass4"},
    {XBLD_MIDDLE_CLASS_5, L"middleclass5"},
    {XBLD_UPPER_CLASS_1, L"upperclass1"},
    {XBLD_UPPER_CLASS_2, L"upperclass2"},
    {XBLD_UPPER_CLASS_3, L"upperclass3"},
    {XBLD_UPPER_CLASS_4, L"upperclass4"},
    {XBLD_GAS_STATION_1, L"gasstation1"},
    {XBLD_GAS_STATION_2, L"gasstation1"},
    {XBLD_CONVENIENCE_STORE, L"convenience"},
    {XBLD_BED_AND_BREAKFAST_INN, L"bednbreak"},
    {XBLD_SMALL_OFFICE_1, L"smalloffice1"},
    {XBLD_SMALL_OFFICE_2, L"smalloffice2"},
    {XBLD_WAREHOUSE, L"warehouse"},
    {XBLD_TOY_STORE, L"toystore"},

    {XBLD_ROAD_1, L"road"},
    {XBLD_ROAD_2, L"road"},
    {XBLD_ROAD_3, L"road_long"},
    {XBLD_ROAD_4, L"road_long"},
    {XBLD_ROAD_5, L"road_long"},
    {XBLD_ROAD_6, L"road_long"},
    {XBLD_ROAD_7, L"road_corner"},
    {XBLD_ROAD_8, L"road_corner"},
    {XBLD_ROAD_9, L"road_corner"},
    {XBLD_ROAD_10, L"road_corner"},
    {XBLD_ROAD_11, L"road_blank"},
    {XBLD_ROAD_12, L"road_blank"},
    {XBLD_ROAD_13, L"road_blank"},
    {XBLD_ROAD_14, L"road_blank"},
    {XBLD_ROAD_15, L"road_blank"},

    {XBLD_RAIL_1, L"rail"},
    {XBLD_RAIL_2, L"rail"},
    {XBLD_RAIL_3, L"rail_long"},
    {XBLD_RAIL_4, L"rail_long"},
    {XBLD_RAIL_5, L"rail_long"},
    {XBLD_RAIL_6, L"rail_long"},
    {XBLD_RAIL_7, L"rail_corner"},
    {XBLD_RAIL_8, L"rail_corner"},
    {XBLD_RAIL_9, L"rail_corner"},
    {XBLD_RAIL_10, L"rail_corner"},
    {XBLD_RAIL_11, L"rail_cross"},
    {XBLD_RAIL_12, L"rail_cross"},
    {XBLD_RAIL_13, L"rail_cross"},
    {XBLD_RAIL_14, L"rail_cross"},
    {XBLD_RAIL_15, L"rail_cross"},
    {XBLD_RAIL_SLOPE_1, L"rail"},
    {XBLD_RAIL_SLOPE_2, L"rail"},
    {XBLD_RAIL_SLOPE_3, L"rail"},
    {XBLD_RAIL_SLOPE_4, L"rail"},

    {XBLD_CROSSOVER_ROAD_POWER_1, L"road_blank"},
    {XBLD_CROSSOVER_ROAD_POWER_2, L"road_blank"},
    {XBLD_CROSSOVER_ROAD_RAIL_1, L"cross_road_rail"},
    {XBLD_CROSSOVER_ROAD_RAIL_2, L"cross_road_rail"},
    {XBLD_CROSSOVER_RAIL_POWER_1, L"rail"},
    {XBLD_CROSSOVER_RAIL_POWER_2, L"rail"},



  }; 
}
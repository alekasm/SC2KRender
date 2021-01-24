#pragma once

//Define all enum values because we will be loading and casting from a param file
enum TileRenderMode { ALWAYS = 0, NEVER = 1, MODEL_VISIBLE = 2, MODEL_HIDDEN = 3 };

enum XZONType;
enum XUNDType;
enum XBITType;

enum XTERType
{
  XTER_FLAT, // 0x00
  XTER_SLOPE_N, // 0x01
  XTER_SLOPE_E, // 0x02
  XTER_SLOPE_S, // 0x03
  XTER_SLOPE_W, // 0x04
  XTER_SLOPE_NE, // 0x05
  XTER_SLOPE_SE, // 0x06
  XTER_SLOPE_SW, // 0x07
  XTER_SLOPE_NW, // 0x08
  XTER_CORNER_NE, // 0x09
  XTER_CORNER_SE, // 0xA
  XTER_CORNER_SW, // 0xB
  XTER_CORNER_NW, // 0xC
  XTER_HIGHGROUND, // 0xD
  XTER_UNUSED_0E, // 0xE
  XTER_UNUSED_0F, // 0xF
  XTER_UNDERWATER_FLAT, // 0x10
  XTER_UNDERWATER_SLOPE_N, // 0x11
  XTER_UNDERWATER_SLOPE_E,// 0x12
  XTER_UNDERWATER_SLOPE_S, // 0x13
  XTER_UNDERWATER_SLOPE_W, // 0x14
  XTER_UNDERWATER_SLOPE_NE, // 0x15
  XTER_UNDERWATER_SLOPE_SE, // 0x16
  XTER_UNDERWATER_SLOPE_SW, // 0x17
  XTER_UNDERWATER_SLOPE_NW, // 0x18
  XTER_UNDERWATER_CORNER_NE,// 0x19
  XTER_UNDERWATER_CORNER_SE, // 0x1A
  XTER_UNDERWATER_CORNER_SW,// 0x1B
  XTER_UNDERWATER_CORNER_NW,// 0x1C
  XTER_UNDERWATER_HIGHGROUND,// 0x1D
  XTER_UNUSED_1E,// 0x1E
  XTER_UNUSED_1F,// 0x1F
  XTER_WATER_SUBMERGED_FLAT,// 0x20
  XTER_WATER_SUBMERGED_SLOPE_N,// 0x21
  XTER_WATER_SUBMERGED_SLOPE_E, // 0x22
  XTER_WATER_SUBMERGED_SLOPE_S,// 0x23
  XTER_WATER_SUBMERGED_SLOPE_W,// 0x24
  XTER_WATER_SUBMERGED_SLOPE_NE,// 0x25
  XTER_WATER_SUBMERGED_SLOPE_SE,// 0x26
  XTER_WATER_SUBMERGED_SLOPE_SW,// 0x27
  XTER_WATER_SUBMERGED_SLOPE_NW,// 0x28
  XTER_WATER_SUBMERGED_CORNER_NE,// 0x29
  XTER_WATER_SUBMERGED_CORNER_SE, // 0x2A
  XTER_WATER_SUBMERGED_CORNER_SW, // 0x2B
  XTER_WATER_SUBMERGED_CORNER_NW,// 0x2C
  XTER_WATER_SUBMERGED_HIGHGROUND, // 0x2D
  XTER_UNUSED_2E, // 0x2E
  XTER_UNUSED_2F, // 0x2F
  XTER_SURFACE_WATER_FLAT, // 0x30
  XTER_SURFACE_WATER_SLOPE_N, // 0x31
  XTER_SURFACE_WATER_SLOPE_E, // 0x32
  XTER_SURFACE_WATER_SLOPE_S, // 0x33
  XTER_SURFACE_WATER_SLOPE_W, // 0x34
  XTER_SURFACE_WATER_SLOPE_NE, // 0x35
  XTER_SURFACE_WATER_SLOPE_SE, // 0x36
  XTER_SURFACE_WATER_SLOPE_SW, // 0x37
  XTER_SURFACE_WATER_SLOPE_NW, // 0x38
  XTER_SURFACE_WATER_CORNER_NE, // 0x39
  XTER_SURFACE_WATER_CORNER_SE,// 0x3A
  XTER_SURFACE_WATER_CORNER_SW, // 0x3B
  XTER_SURFACE_WATER_CORNER_NW, // 0x3C
  XTER_SURFACE_WATER_HIGHGROUND, // 0x3D
  XTER_WATERFALL, // 0x3E
  XTER_UNUSED_3F,// 0x3F
  XTER_SURFACE_WATER_CANAL_WE,// 0x40
  XTER_SURFACE_WATER_CANAL_NS, // 0x41
  XTER_SURFACE_WATER_BAY_OPEN_S, // 0x42
  XTER_SURFACE_WATER_BAY_OPEN_W, // 0x43
  XTER_SURFACE_WATER_BAY_OPEN_N, // 0x44
  XTER_SURFACE_WATER_BAY_OPEN_E // 0x45
};

#define XBLD_IS_TREE(v) (v >= XBLD_TREES_1 && v <= XBLD_TREES_7)
#define XBLD_IS_BUILDING(v) (v >= XBLD_LOWER_CLASS_1)
#define XBLD_IS_BRIDGE(v) (v >= XBLD_BRIDGE_SUSPENSION_PIECE_1 && v <= XBLD_BRIDGE_RAIL_PIECE_2)
#define XBLD_IS_TUNNEL(v) (v >= XBLD_TUNNEL_1 && v <= XBLD_TUNNEL_4)
#define XBLD_IS_HIGHWAY_CROSSOVER(v) (v >= XBLD_HIGHWAY_CROSSOVER_1 && v <= XBLD_HIGHWAY_CROSSOVER_6)
#define XBLD_IS_HIGHWAY_ONRAMP(v) (v >= XBLD_HIGHWAY_ONRAMP_1 && v <= XBLD_HIGHWAY_ONRAMP_4)
#define XBLD_IS_HYDROELECTRIC(v) (v >= XBLD_HYDROELECTRIC_1 && v <= XBLD_HYDROELECTRIC_2)
#define XBLD_IS_HYDROELECTRIC(v) (v >= XBLD_HYDROELECTRIC_1 && v <= XBLD_HYDROELECTRIC_2)
#define XBLD_IS_NON_BUILDING(v) (v >= XBLD_CLEAR && v <= 0x6F)
#define XBLD_IS_BUILDING(v) (v >= XBLD_LOWER_CLASS_1)
#define XBLD_IS_TREES(v) (v >= XBLD_TREES_1 && v <= XBLD_TREES_7)
#define XBLD_IS_HIGHWAY_SLOPE(v) (v >= XBLD_HIGHWAY_SLOPE_1 && v <= XBLD_HIGHWAY_SLOPE_4)
#define XBLD_IS_HIGHWAY_CORNER(v) (v >= XBLD_HIGHWAY_CORNER_1 && v <= XBLD_HIGHWAY_CORNER_4)
#define XBLD_IS_HIGHWAY_BRIDGE(v) (v >= XBLD_HIGHWAY_BRIDGE_1 && v <= XBLD_HIGHWAY_BRIDGE_2)
#define XBLD_IS_HIGHWAY(v) (v == 0x69 || XBLD_IS_HIGHWAY_SLOPE(v) || (v >= XBLD_HIGHWAY_1 &&  v <= XBLD_HIGHWAY_CROSSOVER_6) || XBLD_IS_HIGHWAY_CORNER(v) || XBLD_IS_HIGHWAY_BRIDGE(v))
#define XBLD_IS_RAIL_BRIDGE(v)(v == 0x5A || v == 0x5B)
#define XBLD_IS_POWER_LINE(v) (v >= XBLD_POWER_LINE_1 && v <= XBLD_POWER_LINE_15)
#define XBLD_IS_ROAD(v) (v >= XBLD_ROAD_1 && v <= XBLD_ROAD_15)


enum XBLDType
{
  XBLD_CLEAR, // 0x00
  XBLD_RUBBLE_1, //0x01
  XBLD_RUBBLE_2, //0x02
  XBLD_RUBBLE_3, //0x03
  XBLD_RUBBLE_4, //0x04
  XBLD_RADIOACTIVE, //0x05
  XBLD_TREES_1, //0x06
  XBLD_TREES_2, //0x07
  XBLD_TREES_3, //0x08
  XBLD_TREES_4, //0x09
  XBLD_TREES_5, //0x0A
  XBLD_TREES_6, //0x0B
  XBLD_TREES_7, //0x0C
  XBLD_SMALL_PARK, //0x0D

  XBLD_POWER_LINE_1, //0x0E
  XBLD_POWER_LINE_2, //0x0F
  XBLD_POWER_LINE_3, //0x10
  XBLD_POWER_LINE_4, //0x11
  XBLD_POWER_LINE_5, //0x12
  XBLD_POWER_LINE_6, //0x13
  XBLD_POWER_LINE_7, //0x14
  XBLD_POWER_LINE_8, //0x15
  XBLD_POWER_LINE_9, //0x16
  XBLD_POWER_LINE_10, //0x17
  XBLD_POWER_LINE_11, //0x18
  XBLD_POWER_LINE_12, //0x19
  XBLD_POWER_LINE_13, //0x1A
  XBLD_POWER_LINE_14, //0x1B
  XBLD_POWER_LINE_15, //0x1C

  XBLD_ROAD_1, //0x1D
  XBLD_ROAD_2, //0x1E
  XBLD_ROAD_3, //0x1F
  XBLD_ROAD_4, //0x20
  XBLD_ROAD_5, //0x21
  XBLD_ROAD_6, //0x22
  XBLD_ROAD_7, //0x23
  XBLD_ROAD_8, //0x24
  XBLD_ROAD_9, //0x25
  XBLD_ROAD_10, //0x26
  XBLD_ROAD_11, //0x27
  XBLD_ROAD_12, //0x28
  XBLD_ROAD_13, //0x29
  XBLD_ROAD_14, //0x2A
  XBLD_ROAD_15, //0x2B

  XBLD_RAIL_1, //0x2C
  XBLD_RAIL_2, //0x2D
  XBLD_RAIL_3, //0x2E
  XBLD_RAIL_4, //0x2F
  XBLD_RAIL_5, //0x30
  XBLD_RAIL_6, //0x31
  XBLD_RAIL_7, //0x32
  XBLD_RAIL_8, //0x33
  XBLD_RAIL_9, //0x34
  XBLD_RAIL_10, //0x35
  XBLD_RAIL_11, //0x36
  XBLD_RAIL_12, //0x37
  XBLD_RAIL_13, //0x38
  XBLD_RAIL_14, //0x39
  XBLD_RAIL_15, //0x3A
  XBLD_RAIL_SLOPE_1, //0x3B
  XBLD_RAIL_SLOPE_2, //0x3C
  XBLD_RAIL_SLOPE_3, //0x3D
  XBLD_RAIL_SLOPE_4, //0x3E
  XBLD_TUNNEL_1, //0x3F
  XBLD_TUNNEL_2, //0x40
  XBLD_TUNNEL_3, //0x41
  XBLD_TUNNEL_4, //0x42

  //SKIP
  XBLD_CROSSOVER_ROAD_POWER_1 = 0x43,
  XBLD_CROSSOVER_ROAD_POWER_2,
  XBLD_CROSSOVER_ROAD_RAIL_1,
  XBLD_CROSSOVER_ROAD_RAIL_2,
  XBLD_CROSSOVER_RAIL_POWER_1,
  XBLD_CROSSOVER_RAIL_POWER_2,

  XBLD_HIGHWAY_1, //0x49
  XBLD_HIGHWAY_2, //0x4A
  XBLD_HIGHWAY_CROSSOVER_1, //0x4B
  XBLD_HIGHWAY_CROSSOVER_2, //0x4C
  XBLD_HIGHWAY_CROSSOVER_3, //0x4D
  XBLD_HIGHWAY_CROSSOVER_4, //0x4E
  XBLD_HIGHWAY_CROSSOVER_5, //0x4F
  XBLD_HIGHWAY_CROSSOVER_6, //0x50
  
  XBLD_BRIDGE_SUSPENSION_PIECE_1 = 0x51,
  XBLD_BRIDGE_SUSPENSION_PIECE_2,
  XBLD_BRIDGE_SUSPENSION_PIECE_3,
  XBLD_BRIDGE_SUSPENSION_PIECE_4,
  XBLD_BRIDGE_SUSPENSION_PIECE_5,
  XBLD_BRIDGE_COMMON_PIECE_1, //0x56
  XBLD_BRIDGE_COMMON_PIECE_2, //0x57
  XBLD_BRIDGE_COMMON_PIECE_3, //0x58
  XBLD_BRIDGE_COMMON_PIECE_4, //0x59
  XBLD_BRIDGE_RAIL_PIECE_1, //0x5A
  XBLD_BRIDGE_RAIL_PIECE_2, //0x5B
  XBLD_ELEVATED_POWER_LINE, //0x5C
  XBLD_HIGHWAY_ONRAMP_1, //0x5D
  XBLD_HIGHWAY_ONRAMP_2, //0x5E
  XBLD_HIGHWAY_ONRAMP_3, //0x5F
  XBLD_HIGHWAY_ONRAMP_4, //0x60
  XBLD_HIGHWAY_SLOPE_1, //0x61
  XBLD_HIGHWAY_SLOPE_2, //0x62
  XBLD_HIGHWAY_SLOPE_3, //0x63
  XBLD_HIGHWAY_SLOPE_4, //0x64
  XBLD_HIGHWAY_CORNER_1, //0x65
  XBLD_HIGHWAY_CORNER_2, //0x66
  XBLD_HIGHWAY_CORNER_3, //0x67
  XBLD_HIGHWAY_CORNER_4, //0x68
  XBLD_HIGHWAY_BRIDGE_1 = 0x6A,
  XBLD_HIGHWAY_BRIDGE_2 = 0x6B,
  XBLD_SUB_RAIL_1, //0x6C
  XBLD_SUB_RAIL_2, //0x6D
  XBLD_SUB_RAIL_3, //0x6E
  XBLD_SUB_RAIL_4, //0x6F

  //SKIP
  XBLD_LOWER_CLASS_1 = 0x70,
  XBLD_LOWER_CLASS_2,
  XBLD_LOWER_CLASS_3,
  XBLD_MIDDLE_CLASS_1,
  XBLD_MIDDLE_CLASS_2,
  XBLD_MIDDLE_CLASS_3,
  XBLD_MIDDLE_CLASS_4,
  XBLD_MIDDLE_CLASS_5,
  XBLD_UPPER_CLASS_1,
  XBLD_UPPER_CLASS_2,
  XBLD_UPPER_CLASS_3,
  XBLD_UPPER_CLASS_4,
  XBLD_GAS_STATION_1,
  XBLD_BED_AND_BREAKFAST_INN,
  XBLD_CONVENIENCE_STORE,
  XBLD_GAS_STATION_2,
  XBLD_SMALL_OFFICE_1,
  XBLD_SMALL_OFFICE_2,
  XBLD_WAREHOUSE,
  XBLD_TOY_STORE,
  XBLD_WAREHOUSE_1,
  XBLD_CHEMICAL_STORAGE,
  XBLD_WAREHOUSE_2,
  XBLD_INDUSTRIAL_SUBSTATION,

  // Skip
  XBLD_HYDROELECTRIC_1 = 0xC6,
  XBLD_HYDROELECTRIC_2,
  XBLD_ZOO = 0xDA,
  XBLD_RUNWAY = 0xDD,
  XBLD_PIER = 0xDF,

  //Skip
  XBLD_MARINA = 0xF8,
  XBLD_BRAUN_LLAMA_DOME = 0xFF
};

enum SceneryObject
{
  TREE_TRUNKS_1 = 306,
  TREE_TRUNKS_2 = 307,
  TREE_TRUNKS_3 = 308,
  TREE_TRUNKS_4 = 309,
  TREE_TRUNKS_5 = 310,
  TREE_TRUNKS_6 = 311,
  TREE_TRUNKS_7 = 312,
  PILLAR = 1000,
  BRIDGE_SUSPENSION_PILLAR = 1001,
  BRIDGE_RAISED_PILLAR = 1002,
  BRIDGE_RAISED_ARC = 1003,
  BRIDGE_REINFORCED = 1004,
  BRIDGE_SUSPENSION_1 = 1005,
  BRIDGE_SUSPENSION_2 = 1006,
  BRIDGE_SUSPENSION_3 = 1007
};
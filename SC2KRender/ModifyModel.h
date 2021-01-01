#pragma once
#include "Model3D.h"

struct ModifyModel
{
  static void RotateModel(int32_t model_id, Model3D* model);
  static void AddSecondaryModel(const MapSceneTile*,
    const Model3D*,
    const XBLDType,
    std::vector<Model3D*>*);
  static void TransformHighwayOnRamp(const MapTile* map_tile, Model3D* model);
  static void TransformHighwayCorners(MapSceneTile** tiles,
    std::vector<Model3D*>* v_model3d,
    const std::vector<ModelTileVector>& clusters);
  static void TransformHighwayBridge(MapSceneTile** tiles,
    std::vector<Model3D*>* v_model3d,
    const std::vector<ModelTileVector>& clusters);
  static void FillTunnels(MapSceneTile** tiles, std::vector<Model3D*>* v_model3d);
};
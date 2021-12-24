#pragma once
#include "DirectXTK.h"
#include <map>
#include <filesystem>
#include <codecvt>
#include <locale>
#include <iostream>

enum TileRenderMode;
struct AssetLoader
{
  static void FindFiles(std::wstring directory, std::vector<std::wstring>& vector);
  static void LoadModels(
    Microsoft::WRL::ComPtr<ID3D11Device1> device,
    const std::unique_ptr<DirectX::EffectFactory>& effect,
    std::wstring directory);
  static void LoadCustomAssets(std::wstring file);
  static void LoadSprites(Microsoft::WRL::ComPtr<ID3D11Device1> device, std::wstring directory);
  static void LoadXBLDVisibilityParameters(std::wstring file);
  static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>* mresources;
  static std::map<std::wstring, CD3D11_TEXTURE2D_DESC>* mdescriptions;
  static std::map<std::wstring, std::shared_ptr<DirectX::Model>>* mmodels;
  static std::map<int32_t, std::wstring> xbld_map;
  static std::map<int32_t, TileRenderMode> xbld_visible_map;
};

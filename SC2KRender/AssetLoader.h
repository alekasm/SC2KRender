#pragma once
#include "DirectXTK.h"
#include <map>
#include <filesystem>
#include <codecvt>
#include <locale>
#include <iostream>
struct AssetLoader
{
  static bool FindFiles(std::wstring directory, std::vector<std::wstring>& vector)
  {     
    std::cout << "Current Path: " << std::filesystem::current_path() << std::endl;
    for (const std::filesystem::directory_entry& entry :
      std::filesystem::recursive_directory_iterator(directory))
    {
      if (entry.is_regular_file())
      {
        vector.push_back(entry.path().wstring());
      }
    }
  }

  static void LoadModels(
    Microsoft::WRL::ComPtr<ID3D11Device1> device,
    const std::unique_ptr<DirectX::IEffectFactory>& effect,
    std::wstring directory)
  {
    mmodels = new std::map<std::wstring, std::shared_ptr<DirectX::Model>>();
    if(!std::filesystem::is_directory(directory))
      return;
    std::vector<std::wstring> files;    
    FindFiles(directory, files);
    
    for (const std::wstring& wfilename : files)
    {
      std::wstring key(wfilename);
      key.erase(0, key.find_last_of('\\') + 1);
      key.erase(key.find('.'), key.size() - 1);
      std::unique_ptr<DirectX::Model> model = DirectX::Model::CreateFromCMO(device.Get(), wfilename.c_str(), *effect);
      mmodels->operator[](key) = std::move(model);
    }
  }

  static void LoadSprites(Microsoft::WRL::ComPtr<ID3D11Device1> device, std::wstring directory)
  {
    mresources = new std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
    mdescriptions = new std::map<std::wstring, CD3D11_TEXTURE2D_DESC>();
    if (!std::filesystem::is_directory(directory))
      return;
    std::vector<std::wstring> files;
    FindFiles(directory, files);

    for (const std::wstring& wfilename : files)
    {
      Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
      DirectX::CreateWICTextureFromFile(device.Get(), wfilename.c_str(), nullptr,
        texture.ReleaseAndGetAddressOf());

      Microsoft::WRL::ComPtr<ID3D11Resource> resource;
      DirectX::CreateWICTextureFromFile(device.Get(), wfilename.c_str(),
        resource.GetAddressOf(),
        texture.ReleaseAndGetAddressOf());

      Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
      resource.As(&texture2d);
      CD3D11_TEXTURE2D_DESC desc;
      texture2d->GetDesc(&desc);
      
      std::wstring key(wfilename);
      key.erase(0, key.find_last_of('\\') + 1);
      key.erase(key.find('.'), key.size() - 1);
      mresources->operator[](key) = texture;
      mdescriptions->operator[](key) = desc;
    }
  }
  static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>* mresources;
  static std::map<std::wstring, CD3D11_TEXTURE2D_DESC>* mdescriptions;
  static std::map<std::wstring, std::shared_ptr<DirectX::Model>>* mmodels;
};
std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>* AssetLoader::mresources;
std::map<std::wstring, CD3D11_TEXTURE2D_DESC>* AssetLoader::mdescriptions;
std::map<std::wstring, std::shared_ptr<DirectX::Model>>* AssetLoader::mmodels;
#include "AssetLoader.h"
#include <map>
#include <string>
#include <fstream>
#include "Types.h"

std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>* AssetLoader::mresources;
std::map<std::wstring, CD3D11_TEXTURE2D_DESC>* AssetLoader::mdescriptions;
std::map<std::wstring, std::shared_ptr<DirectX::Model>>* AssetLoader::mmodels;
std::map<int32_t, std::wstring> AssetLoader::xbld_map;

void AssetLoader::LoadSprites(Microsoft::WRL::ComPtr<ID3D11Device1> device, std::wstring directory)
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

void AssetLoader::FindFiles(std::wstring directory, std::vector<std::wstring>& vector)
{
  for (const std::filesystem::directory_entry& entry :
    std::filesystem::recursive_directory_iterator(directory))
  {
    if (entry.is_regular_file())
    {
      vector.push_back(entry.path().wstring());
    }
  }
}

void AssetLoader::LoadModels(
  Microsoft::WRL::ComPtr<ID3D11Device1> device,
  const std::unique_ptr<DirectX::IEffectFactory>& effect,
  std::wstring directory)
{
  mmodels = new std::map<std::wstring, std::shared_ptr<DirectX::Model>>();
  if (!std::filesystem::is_directory(directory))
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

void AssetLoader::LoadCustomAssets(std::wstring file)
{
  std::wifstream file_stream(file);
  if (!file_stream.good())
    return;
  std::wstring line;
  while (std::getline(file_stream, line))
  {
    auto it = line.find(',');
    if (it == std::string::npos || line.empty())
      continue;
    std::wstring enum_value = line.substr(0, it);
    std::wstring file_value = line.substr(it + 1, line.size());
    int32_t xbld_value = _wtoi(enum_value.c_str());
    xbld_map[xbld_value] = file_value;
  }
}
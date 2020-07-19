#pragma once
#include "DirectXTK.h"
struct AssetLoader
{
  static void LoadSprites(Microsoft::WRL::ComPtr<ID3D11Device1> device, std::string directory)
  {
    sprites = new std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
    m_texture = new std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
    DirectX::CreateWICTextureFromFile(device.Get(), L"assets/sprites/tree.png", nullptr,
      texture.ReleaseAndGetAddressOf());

    m_texture->push_back(texture);

    
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    DirectX::CreateWICTextureFromFile(device.Get(), L"assets/sprites/tree.png",
      resource.GetAddressOf(),
      texture.GetAddressOf());

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    resource.As(&texture2d);
    CD3D11_TEXTURE2D_DESC desc;
    texture2d->GetDesc(&desc);

    sprites->push_back(texture);
  }
  static std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>* sprites;
  static std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>* m_texture;
};
std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>* AssetLoader::sprites;
std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>* AssetLoader::m_texture;
//std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>>* AssetLoader::sprites;
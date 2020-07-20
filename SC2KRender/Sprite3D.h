#pragma once
#include "DirectXTK.h"
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
enum Orientation { X, Z };
struct Sprite3D
{
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource;
  DirectX::VertexPositionTexture v1, v2, v3, v4;
  Sprite3D(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource, Vector3 origin, Orientation orientation)
  {
    this->resource = resource;
    Vector3 a, b, c, d;

    if (orientation == X)
    {
      a = Vector3(origin.x, origin.y, origin.z);
      b = Vector3(origin.x + 1.f, origin.y, origin.z);
      c = Vector3(origin.x + 1.f, origin.y + HEIGHT_INCREMENT, origin.z);
      d = Vector3(origin.x, origin.y + HEIGHT_INCREMENT, origin.z);
    }
    else if (orientation == Z)
    {
      a = Vector3(origin.x, origin.y, origin.z);
      b = Vector3(origin.x, origin.y, origin.z + 1.f);
      c = Vector3(origin.x, origin.y + HEIGHT_INCREMENT, origin.z + 1.f);
      d = Vector3(origin.x, origin.y + HEIGHT_INCREMENT, origin.z);
    }
    v1 = DirectX::VertexPositionTexture(a, Vector2(0.f, 1.f));
    v2 = DirectX::VertexPositionTexture(b, Vector2(1.f, 1.f));
    v3 = DirectX::VertexPositionTexture(c, Vector2(1.f, 0.f));
    v4 = DirectX::VertexPositionTexture(d, Vector2(0.f, 0.f));
  }
  Sprite3D() = delete;

};
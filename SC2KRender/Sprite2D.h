#pragma once
#include "DirectXTK.h"
#include "Scene.h"
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
struct Sprite2D
{
  Vector3 position3d;
  Vector2 position2d;
  Vector2 origin2d;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource;
  Sprite2D(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource, CD3D11_TEXTURE2D_DESC desc, const Vector3& origin)
  {
    this->resource = resource;
    position3d = Vector3(origin);
    origin2d = Vector2(static_cast<float>(desc.Width) / 2, static_cast<float>(desc.Height));
  }
  void Update(Scene* scene)
  {
    Vector3 transform = Vector3(position3d * scene->GetScale());
    transform = Vector3::Transform(transform, scene->GetViewMatrix());
    transform = Vector3::Transform(transform, scene->GetProjectionMatrix());
    Vector2 dim = scene->GetWindowDimensions();
    transform.x = dim.x * (transform.x + 1.f) / 2.f;
    transform.y = dim.y * (1.f - (transform.y + 1.f) / 2.f);
    position2d = Vector2(transform.x, transform.y);
  }
  Sprite2D() = delete;
};
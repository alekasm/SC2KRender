#pragma once
#include "DirectXTK.h"
#include "Scene.h"
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;
struct Model3D
{
  std::shared_ptr<DirectX::Model> model;
  DirectX::XMMATRIX m_world, m_world_identity;
  Vector3 origin;

  Model3D(std::shared_ptr<DirectX::Model> model, Vector3 origin)
  {
    this->model = model;
    this->origin = origin;
    m_world_identity = Matrix::Identity;
  }

  void Update(Scene* scene)
  {
    float s = scene->GetScale();
    m_world = m_world_identity;
    m_world *= DirectX::XMMatrixTranslation(origin.x * s, origin.y * s, origin.z * s);
    m_world = XMMatrixMultiply(scene->GetWorldMatrix(), m_world);  
  }
};
#pragma once
#include "DirectXTK.h"
#include "Scene.h"
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;
struct Model3D
{
  std::shared_ptr<DirectX::Model> model;
  Matrix m_world, m_world_identity;
  Vector3 origin, origin_scaled;

  Model3D(std::shared_ptr<DirectX::Model> model, Vector3 origin)
  {
    this->model = model;
    this->origin = origin;
    m_world_identity = Matrix::Identity; 
  }

  void Update(Scene* scene)
  { 
    float scale = scene->GetScale();
    m_world = Matrix::Identity;
    m_world *= m_world_identity;  
    m_world *= DirectX::XMMatrixTranslation(origin.x, origin.y, origin.z);    
    m_world.m[3][0] *= scale;
    m_world.m[3][1] *= scale;
    m_world.m[3][2] *= scale;
    origin_scaled = origin * scale;
    m_world = XMMatrixMultiply(scene->GetWorldMatrix(), m_world); 
  }

};
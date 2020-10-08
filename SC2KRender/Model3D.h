#pragma once
#include "DirectXTK.h"
#include "Scene.h"
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;
struct Model3D
{
  std::shared_ptr<DirectX::Model> model;
  Matrix m_world, m_world_identity;
  Vector3 origin, offset;

  Model3D(std::shared_ptr<DirectX::Model> model, Vector3 origin)
  {
    this->model = model;
    this->origin = origin;
    m_world_identity = Matrix::Identity; 
    offset = Vector3::Zero;
  }

   void Update(Scene* scene)
  { 
    float s = scene->GetScale();
    m_world = Matrix::Identity;
    m_world *= m_world_identity;  
    m_world *= DirectX::XMMatrixTranslation(origin.x, origin.y, origin.z);    
    m_world.m[3][0] *= s;
    m_world.m[3][1] *= s;
    m_world.m[3][2] *= s;
    m_world = XMMatrixMultiply(scene->GetWorldMatrix(), m_world); 
  }
};
#pragma once
#include "DirectXTK.h"

//It will probably get inlined anyways, but just to be sure...
static inline void CreateFrustumPlanes(
  const DirectX::SimpleMath::Matrix& view_proj,
  DirectX::SimpleMath::Plane* frustum_planes)
{
  // left
  frustum_planes[0].x = view_proj.m[0][3] + view_proj.m[0][0];
  frustum_planes[0].y = view_proj.m[1][3] + view_proj.m[1][0];
  frustum_planes[0].z = view_proj.m[2][3] + view_proj.m[2][0];
  frustum_planes[0].w = view_proj.m[3][3] + view_proj.m[3][0];

  // right
  frustum_planes[1].x = view_proj.m[0][3] - view_proj.m[0][0];
  frustum_planes[1].y = view_proj.m[1][3] - view_proj.m[1][0];
  frustum_planes[1].z = view_proj.m[2][3] - view_proj.m[2][0];
  frustum_planes[1].w = view_proj.m[3][3] - view_proj.m[3][0];

  // bottom
  frustum_planes[2].x = view_proj.m[0][3] + view_proj.m[0][1];
  frustum_planes[2].y = view_proj.m[1][3] + view_proj.m[1][1];
  frustum_planes[2].z = view_proj.m[2][3] + view_proj.m[2][1];
  frustum_planes[2].w = view_proj.m[3][3] + view_proj.m[3][1];

  // top
  frustum_planes[3].x = view_proj.m[0][3] - view_proj.m[0][1];
  frustum_planes[3].y = view_proj.m[1][3] - view_proj.m[1][1];
  frustum_planes[3].z = view_proj.m[2][3] - view_proj.m[2][1];
  frustum_planes[3].w = view_proj.m[3][3] - view_proj.m[3][1];

  // near
  frustum_planes[4].x = view_proj.m[0][2];
  frustum_planes[4].y = view_proj.m[1][2];
  frustum_planes[4].z = view_proj.m[2][2];
  frustum_planes[4].w = view_proj.m[3][2];

  // far
  frustum_planes[5].x = view_proj.m[0][3] - view_proj.m[0][2];
  frustum_planes[5].y = view_proj.m[1][3] - view_proj.m[1][2];
  frustum_planes[5].z = view_proj.m[2][3] - view_proj.m[2][2];
  frustum_planes[5].w = view_proj.m[3][3] - view_proj.m[3][2];
  for (unsigned int i = 0; i < 6; ++i)
  {
    frustum_planes[i].Normalize();
  }
}

#pragma once
#include <vector>
#include "DirectXColors.h"
namespace DirectX::Colors
{
  XMGLOBALCONST XMVECTORF32 SC2K_SEA_BLUE = { {{0.f, 0.f, 0.5450980f, 0.5f}} };

  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_BRIGHT = { {{0.6941176f, 0.6235294f, 0.3725490f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_NORMAL = { {{0.6078431f, 0.5294117f, 0.2784313f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_DARK_1 = { {{0.5294117f, 0.4196078f, 0.2f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_DARK_2 = { {{0.4509803f, 0.3254901f, 0.1372549f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_DARK_3 = { {{0.5529411f, 0.4f, 0.1725490f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_DIRT_DARKEST = { {{0.3725490f, 0.2313725f, 0.0745098f, 1.0f}} };

  XMGLOBALCONST XMVECTORF32 SC2K_SEA_BRIGHT = { {{0.0156863f, 0.0156863f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_SEA_NORMAL = { {{0.f, 0.f, 0.5450980f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_SEA_DARK_1 = { {{0.f, 0.f, 0.4666666f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_SEA_DARK_2 = { {{0.f, 0.f, 0.31764705f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_SEA_DARK_3 = { {{0.f, 0.f, 0.2352941f, 1.0f}} };
  XMGLOBALCONST XMVECTORF32 SC2K_SEA_DARKEST = { {{0.f, 0.f, 0.1568627f, 1.0f}} };


}

namespace
{
  enum SC2K_COLOR{ BRIGHT, NORMAL, DARK_1, DARK_2, DARK_3, DARKEST };
  std::vector<std::vector<DirectX::XMVECTORF32>> color_palette =
  {
    {DirectX::Colors::SC2K_DIRT_BRIGHT, DirectX::Colors::SC2K_DIRT_NORMAL, DirectX::Colors::SC2K_DIRT_DARK_1,
    DirectX::Colors::SC2K_DIRT_DARK_2, DirectX::Colors::SC2K_DIRT_DARK_3, DirectX::Colors::SC2K_DIRT_DARKEST},

    {DirectX::Colors::SC2K_SEA_BRIGHT, DirectX::Colors::SC2K_SEA_NORMAL, DirectX::Colors::SC2K_SEA_DARK_1,
    DirectX::Colors::SC2K_SEA_DARK_2, DirectX::Colors::SC2K_SEA_DARK_3, DirectX::Colors::SC2K_SEA_DARKEST}
  };

  std::vector<DirectX::XMVECTORF32> fill_colors =
  {
    DirectX::Colors::SC2K_DIRT_DARKEST, DirectX::Colors::DarkBlue,
    DirectX::Colors::DarkBlue, DirectX::Colors::Blue, DirectX::Colors::CadetBlue
  };
}
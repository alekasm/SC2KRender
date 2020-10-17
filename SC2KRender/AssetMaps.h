#pragma once
#include <map>
#include <string>
#include <fstream>
#include "Types.h"
namespace
{
  //ids 0 - 255 (0xFF) are reserved for XBLD
  //the remaining ids are free to use
  std::map<int32_t, std::wstring> xbld_map;
}

static void LoadCustomAssets(std::wstring file)
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
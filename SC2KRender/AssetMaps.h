#pragma once
#include <map>
#include <string>
#include "Types.h"
namespace
{
  std::map<XBLDType, std::wstring> xbld_map =
  {
    {XBLD_LOWER_CLASS_1, L"lowerclass1"},
    {XBLD_LOWER_CLASS_2, L"lowerclass2"},
    {XBLD_LOWER_CLASS_3, L"lowerclass3"}
  };
}
#include <fstream>
#include <map>
#include "MapData.h"
#include "MapLoader.h"
#include <conio.h>
#include <algorithm>
#include <array>
#include <type_traits>
#include <vector>

//http://djm.cc/simcity-2000-info.txt

typedef BOOL (*ExtractDataFunc)(FILE*, uint32_t, Map&);

uint16_t swap_uint16(uint16_t val)
{
  return (val << 8) | (val >> 8);
}

BOOL UnimplementedFunction(FILE* file, uint32_t segment_size, Map& map)
{
  fseek(file, segment_size, SEEK_CUR);
  return TRUE;
}

BOOL ExtractCNAMFunction(FILE* file, uint32_t segment_size, Map& map)
{
  //Assume in this case segment_size always = 32
  char cnam_data[32];
  fread(&cnam_data, sizeof(cnam_data), 1, file);
  cnam_data[31] = '\0'; //enforce null termination
  std::string city_name = std::string(cnam_data);
  //Remove unit separator
  city_name.erase(remove(city_name.begin(), city_name.end(), 0x1F), city_name.end()); 
  map.city_name = city_name;  
  return TRUE;
}

template <typename Type>
Type BigEndianByteToType(std::vector<BYTE> bytes)
{
  return (Type)((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3] << 0));
}

BOOL ExtractMISCFunction(FILE* file, uint32_t segment_size, Map& map)
{
  printf("Reading MISC(0x%x), Size: %d\n", ftell(file), segment_size);

  unsigned int start_pos = ftell(file);
  std::vector<BYTE> values;
  while (ftell(file) - start_pos < segment_size)
  {
    BYTE size_data;
    fread(&size_data, sizeof(BYTE), 1, file);
    bool repeat_compression = size_data > 128;
    if (repeat_compression)
    {
      int repeat_amount = static_cast<int>(size_data) - 127;
      BYTE tile_type_data;
      fread(&tile_type_data, sizeof(BYTE), 1, file);
      for (int row_repeat = 0; row_repeat < repeat_amount; ++row_repeat)
      {
        values.push_back(tile_type_data);
      }
    }
    else
    {
      for (int row_index = 0; row_index < size_data; ++row_index)
      {
        BYTE tile_type_data;
        fread(&tile_type_data, sizeof(BYTE), 1, file);
        values.push_back(tile_type_data);
      }
    }
  }

  std::vector<BYTE> year_founded(values.begin() + 0xC, values.begin() + 0xC + 0x4);
  std::vector<BYTE> rotation(values.begin() + 0x8, values.begin() + 0x8 + 0x4);
  std::vector<BYTE> money(values.begin() + 0x14, values.begin() + 0x14 + 0x4);

  map.founding_year = BigEndianByteToType<uint32_t>(year_founded);
  map.rotation = BigEndianByteToType<uint32_t>(rotation);
  map.money_supply = BigEndianByteToType<int32_t>(money);
  return TRUE;
}

BOOL ExtractALTMFunction(FILE* file, uint32_t segment_size, Map& map)
{
  printf("Reading ALTM(0x%x), Size: %d\n", ftell(file), segment_size);
  for (unsigned int x = 0; x < 128; ++x)
  {
    for (unsigned int y = 0; y < 128; ++y)
    {
      uint16_t altitude_map;
      fread(&altitude_map, sizeof(altitude_map), 1, file);   
      altitude_map = swap_uint16(altitude_map); //endian conversion
      // 0 - 4 = height, 5 - 9 = water height
      int height = (altitude_map & 0b0000000000011111);   
      int water_height = ((altitude_map >> 5) & 0b00000011111);
      map.tiles[x + 128 * y].height = height;
      map.tiles[x + 128 * y].altm = altitude_map;
      map.tiles[x + 128 * y].water_height = water_height;
    }
  }
  return TRUE;
}

template <typename Type>
void Assign(MapTile& tile, BYTE data)
{
  if (std::is_same<Type, XTERType>::value)
  {
    tile.xter = (XTERType)data;
  }
  else if (std::is_same<Type, XBLDType>::value)
  {
    tile.xbld = (XBLDType)data;
  }
  else if (std::is_same<Type, XZONType>::value)
  {
    tile.xzon = (XZONType)data;
  }
  else if (std::is_same<Type, XUNDType>::value)
  {
    tile.xund = (XUNDType)data;
  }
  else if (std::is_same<Type, XBITType>::value)
  {
    tile.xbit = (XBITType)data;
  }
}

template <typename Type>
BOOL DecompressData(FILE* file, uint32_t segment_size, Map& map)
{
  //printf("Reading XTER(0x%x), Size: %d\n", ftell(file), segment_size);
  /*
  This data segment uses a compression scheme. The first byte is a "data_size" byte which
  says how many bytes to read. If the size > 128, then it's "compressed" otherwise it's
  uncompressed.

  Compressed:
  Subtract 127 from the size_data byte and these are the amount of compressed bytes. The
  second byte is the tile type. The next "size_data" tiles are of this type.

  Uncompressed:
  Read the next bytes one-by-one "size_data" times, each byte represents a unique tile's
  value.
  */
  unsigned int start_pos = ftell(file);
  unsigned int tile_number = 0;
  while (ftell(file) - start_pos < segment_size)
  {
    BYTE size_data;
    fread(&size_data, sizeof(BYTE), 1, file);
    bool repeat_compression = size_data > 127;
    if (repeat_compression)
    {
      int repeat_amount = static_cast<int>(size_data) - 127;
      BYTE tile_type_data;
      fread(&tile_type_data, sizeof(BYTE), 1, file);
      for (int row_repeat = 0; row_repeat < repeat_amount; ++row_repeat)
      {
        unsigned int x = tile_number % 128;
        unsigned int y = tile_number / 128;
        Assign<Type>(map.tiles[y + 128 * x], tile_type_data);
        ++tile_number;
      }
    }
    else
    {
      for (int row_index = 0; row_index < size_data; ++row_index)
      {
        BYTE tile_type_data;
        fread(&tile_type_data, sizeof(BYTE), 1, file);
        unsigned int x = tile_number % 128;
        unsigned int y = tile_number / 128;
        Assign<Type>(map.tiles[y + 128 * x], tile_type_data);
        ++tile_number;
      }
    }
  }
  if (tile_number != 128 * 128)
  {
    printf("Error decompressing data, processed %d/%d tiles!\n", tile_number, 128 * 128);
    return FALSE;
  }
  return TRUE;
}

BOOL ExtractXTERFunction(FILE* file, uint32_t segment_size, Map& map)
{
  printf("Reading XTER(0x%x), Size: %d\n", ftell(file), segment_size);
  return DecompressData<XTERType>(file, segment_size, map);
}

BOOL ExtractXBLDFunction(FILE* file, uint32_t segment_size, Map& map)
{
  printf("Reading XBLD(0x%x), Size: %d\n", ftell(file), segment_size);
  return DecompressData<XBLDType>(file, segment_size, map);
}

BOOL ExtractXZONFunction(FILE* file, uint32_t segment_size, Map& map)
{
  printf("Reading XZON(0x%x), Size: %d\n", ftell(file), segment_size);
  return DecompressData<XZONType>(file, segment_size, map);
}

BOOL ExtractXUNDFunction(FILE* file, uint32_t segment_size, Map& map)
{
  printf("Reading XUND(0x%x), Size: %d\n", ftell(file), segment_size);
  return DecompressData<XUNDType>(file, segment_size, map);
}

BOOL ExtractXBITFunction(FILE* file, uint32_t segment_size, Map& map)
{
  printf("Reading XBIT(0x%x), Size: %d\n", ftell(file), segment_size);
  return DecompressData<XBITType>(file, segment_size, map);
}

#define NAME_VALUE(s) (strtol(s))
namespace
{
  std::map<std::string, ExtractDataFunc> segment_types =
  {
    {"CNAM", ExtractCNAMFunction},
    {"ALTM", ExtractALTMFunction},
    {"XTER", ExtractXTERFunction},
    {"XBLD", ExtractXBLDFunction},
    {"XZON", ExtractXZONFunction},
    {"XUND", ExtractXUNDFunction},
    {"XBIT", ExtractXBITFunction},
    {"MISC", ExtractMISCFunction}
  };
}

bool MapLoader::LoadMap(std::string filename, Map& out)
{
  FILE* file;
  int result = fopen_s(&file, filename.c_str(), "rb");
  if (file == NULL)
  {
    printf("Failed to load file: %s, Result: %d\n", filename.c_str(), result);
    return false;
  }

  MapHeader header;
  fread(&header, sizeof(header), 1, file);
  header.totalbytes = _byteswap_ulong(header.totalbytes); 
  bool is_sc2kfile = header.iff_file == FORM && header.file_type == ISCD;  
  if (!is_sc2kfile)
  {
    printf("The file is not a valid SC2K Map: %s\n", filename.c_str());
    return false;
  }

  Map map;
  map.header = header;
  map.tiles = new MapTile[TILES_DIMENSION * TILES_DIMENSION];
  long total_filesize = header.totalbytes + 0x8; //The header doesn't include the first 8 bytes
  while (ftell(file) < total_filesize)
  {
    MapSegmentHeader segment_header;
    fread(&segment_header, sizeof(segment_header), 1, file);
    segment_header.totalbytes = _byteswap_ulong(segment_header.totalbytes);
    unsigned int next_seek = segment_header.totalbytes;

    std::map<std::string, ExtractDataFunc>::iterator it;
    std::string segtype(segment_header.type + 0, segment_header.type + 4);
    it = segment_types.find(segtype);
    if (it == segment_types.end())
    {
      printf("Skipping %s(0x%x), Size: %d\n", segtype.c_str(), ftell(file), segment_header.totalbytes);
      fseek(file, segment_header.totalbytes, SEEK_CUR);
      continue;
    }
    if (!it->second(file, segment_header.totalbytes, map))
    {
      return false;
    }
  }

  printf("City Name: %s\n", map.city_name.c_str());
  //printf("Founding Year: %d\n", map.founding_year);
  //printf("Days Elapsed: %d\n", map.days_elapsed);
  //printf("Money Supply: %d\n", map.money_supply);

  fclose(file);
  out = map;
  return true;
}
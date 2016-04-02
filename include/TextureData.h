#ifndef TEXTUREDATA_H__
#define TEXTUREDATA_H__

#include <ngl/Util.h>
#include <string>

enum textureType {TEXTURE2D, TEXTURECUBE, BUFFER};

struct TextureData
{
  GLuint id;
  textureType type;
  std::string textureSource;
};

#endif//TEXTUREDATA_H__

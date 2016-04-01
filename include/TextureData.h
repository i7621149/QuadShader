#ifndef TEXTUREDATA_H__
#define TEXTUREDATA_H__

#include <ngl/Util.h>
#include <string>

struct TextureData
{
  GLuint id;
  bool isBuffer;
  std::string textureFile;
};

#endif//TEXTUREDATA_H__

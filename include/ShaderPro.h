#ifndef SHADERPRO_H__
#define SHADERPRO_H__

#include <string>
#include <vector>
#include <ngl/Util.h>

#include "TextureData.h"

class ShaderPro
{
public :
  ShaderPro();
  ~ShaderPro();

  void compile();

  void loadFragSource();
  void loadVertSource();
  void loadSource(const std::string &_fileName);

  GLuint m_progID;

  GLuint m_out;

  std::string m_sourceFile;
  std::string m_fragFile;
  std::string m_vertFile;

  std::vector<TextureData> m_textures;
};

#endif

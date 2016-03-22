#ifndef SHADERSET_H__
#define SHADERSET_H__

#include <string>
#include <vector>
#include <ngl/Util.h>

struct ShaderPro
{
  ShaderPro();
  ~ShaderPro();

  std::string *m_shaderName;
  GLuint m_id;
  std::vector<GLuint> m_textures;
  std::vector<std::string> m_textureFiles;
};

#endif

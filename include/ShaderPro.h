#ifndef SHADERPRO_H__
#define SHADERPRO_H__

#include <string>
#include <vector>
#include <ngl/Util.h>

//struct textureData
//{
//  bool isBuffer;
//  std::string file;
//};

class ShaderPro
{
public :
  ShaderPro();
  ~ShaderPro();

  void loadFragSource(const std::string &_fragSource);
  void loadSource(const std::string &_fileName);
  void compile();

  std::string m_shaderName;
  std::string m_fragName;
  std::string m_vertName;
  std::string m_fragSource;
  std::string m_vertSource;
  GLuint m_id;
  GLuint m_buffer;
  std::vector<GLuint> m_textures;
  //std::vector<textureData> m_textureData;
};

#endif

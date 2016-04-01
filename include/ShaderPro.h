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

  void loadVertSource();

  void loadFragSource();

  std::string loadSource(const std::string &_fileName);

  std::string getFragBase();

  void loadTextures();

  void loadImage(int _textureUnit, TextureData _texture);

  void printShaderData();

  enum class DebugMode {COMPILE, LINK}; //useful for setting stuff, being readable

  void printInfoLog(GLuint _id, DebugMode _mode);

  std::string m_name;

  GLuint m_progID;

  GLuint m_fragID;

  GLuint m_vertID;

  GLuint m_outID;

  std::string m_fragFile;

  std::string m_vertFile;

  std::vector<TextureData> m_textures;


};

#endif

#ifndef SHADERLIBPRO_H__
#define SHADERLIBPRO_H__

#include <ngl/ShaderLib.h>
#include <ngl/Singleton.h>
#include <string>

class ShaderLibPro : public ngl::Singleton<ShaderLibPro>
{
  friend class ngl::Singleton<ShaderLibPro>;
 // friend class ngl::ShaderLib;
public :
  void newShaderProgram(const std::string &_progName, const std::string &_fragFile, const std::string &_vertFile = "shaders/QuadVertex.glsl");

  int useShaderProgram(const std::string &_progName);

  void useTexture(std::string _textureFile, int _textureUnit);

private :
  ShaderLibPro();
  ~ShaderLibPro();
  std::string loadShaderSource(const std::string &_fileName);

  void loadTexture(std::string _textureFile, int _channelNum);

  void createFrameBuffer(int _bufferNum, int _textureUnit);

  // using a pointer attribute to avoid creating it locally at the start of every function
  ngl::ShaderLib *m_shader;

  std::vector<GLuint> m_textures;

  std::vector<std::string> m_textureFiles;

  std::vector<GLuint> m_frameBuffers;

  std::string m_currentShader;
};

#endif

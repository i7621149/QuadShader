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

  int useTexture(int _textureUnit, const std::string &_textureFile = "");

  void createFrameBuffer(int _bufferNum, int _textureUnit, int _width, int _height);

private :
  ShaderLibPro();

  ~ShaderLibPro();

  std::string loadShaderSource(const std::string &_fileName);

  void loadTextureFile(int _channelNum, const std::string &_textureFile = "", int _width = 0, int _height = 0);

  void createBufferTexture(int _textureUnit);

  // using a pointer attribute for convenience, to avoid creating it locally at the start of every function
  ngl::ShaderLib *m_shader;

  std::vector<GLuint> m_textures;

  std::vector<std::string> m_textureFiles;

  std::vector<GLuint> m_frameBuffers;

  std::vector<GLuint> m_depthStencilBuffers;

  std::string m_currentShader;
};

#endif

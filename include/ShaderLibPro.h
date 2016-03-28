#ifndef SHADERLIBPRO_H__
#define SHADERLIBPRO_H__

#include <ngl/ShaderLib.h>
#include <ngl/Singleton.h>
#include <string>

typedef struct ShaderData
{
  std::string name;
  std::vector<GLuint> textures;
} ShaderData;

class ShaderLibPro : public ngl::Singleton<ShaderLibPro>
{
  friend class ngl::Singleton<ShaderLibPro>;
 // friend class ngl::ShaderLib;
public :
  void newShaderProgram(const std::string &_progName, const std::string &_fragFile, const std::string &_vertFile = "shaders/QuadVertex.glsl");

  int useShaderProgram(const std::string &_progName);

  int useTexture(int _textureUnit, const std::string &_textureFile = "");

  void createFrameBuffer(int _bufferNum, int _textureUnit);

private :
  ShaderLibPro();
  ~ShaderLibPro();
  std::string loadShaderSource(const std::string &_fileName);

  void loadTextureFile(int _channelNum, const std::string &_textureFile = "");

  void createBufferTexture(int _textureUnit);

  // using a pointer attribute to avoid creating it locally at the start of every function
  ngl::ShaderLib *m_shader;

  std::vector<ShaderData> m_shaderData;

  std::vector<GLuint> m_textures;

  std::vector<std::string> m_textureFiles;

  std::vector<GLuint> m_frameBuffers;

  std::vector<GLuint> m_depthStencilBuffers;

  std::string m_currentShader;
};

#endif

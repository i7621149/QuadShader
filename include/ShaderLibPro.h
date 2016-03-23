#ifndef SHADERLIBPRO_H__
#define SHADERLIBPRO_H__

#include <ngl/ShaderLib.h>
#include <ngl/Singleton.h>
#include <string>
#include "ShaderPro.h"

#ifndef NGLSCENE_H__
#define NGLSCENE_H__

class NGLScene
{
public :
  void drawScene(void);
};

#endif

//struct ShaderLibPro{

//};

class ShaderLibPro : public ngl::Singleton<ShaderLibPro>
{
  friend class ngl::Singleton<ShaderLibPro>;
 // friend class ngl::ShaderLib;
public :
  void newShaderProgram(const std::string &_progName, const std::string &_fragFile, const std::string &_vertFile = "shaders/QuadVertex.glsl");

  int useShaderProgram(const std::string &_progName);

  int useTexture(int _textureUnit, const std::string &_textureFile = "");

  void createFrameBuffer(int _bufferNum, int _textureUnit);

  void draw(int _shaderNum, NGLScene *scene);

  void setShaderUniforms();

  // uniforms for shader
  ngl::Vec3 m_resolution;
  float m_globalTime;
  float m_timeDelta;
  int m_frame;
  ngl::Vec4 m_mouse;
  ngl::Vec4 m_date;

private :
  ShaderLibPro();

  ~ShaderLibPro();

  std::string loadShaderSource(const std::string &_fileName);

  void loadTextureFile(int _channelNum, const std::string &_textureFile);

  void createBufferTexture(int _textureUnit);

  // using a pointer attribute for convenience, to avoid creating it locally at the start of every function
  ngl::ShaderLib *m_shader;

  std::vector<GLuint> m_textures;

  std::vector<std::string> m_textureFiles;

  std::vector<GLuint> m_frameBuffers;

  std::vector<GLuint> m_depthStencilBuffers;

  std::vector<ShaderPro *> m_shaderPros;

  std::string m_currentShader;
};

#endif

#ifndef SHADERLIBPRO_H__
#define SHADERLIBPRO_H__

#ifndef NGLSCENE_H__
#define NGLSCENE_H__

// define NGLScene drawScene function, as this is called in ShaderLibPro draw
class NGLScene
{
public :
  void drawScene(void);
};

#endif


#include <ngl/ShaderLib.h>
#include <ngl/Singleton.h>
#include <string>
#include "ShaderPro.h"


class ShaderLibPro : public ngl::Singleton<ShaderLibPro>
{
  friend class ngl::Singleton<ShaderLibPro>;
 // friend class ngl::ShaderLib;
public :
  void newShaderProgram(const std::string &_progName, const std::string &_fragFile, const std::string &_vertFile = "shaders/QuadVertex.glsl");

  int useShaderProgram(const std::string &_progName);

  int useTexture(int _textureUnit, const std::string &_textureFile = "");

  void createFrameBuffer(int _bufferNum, int _textureUnit);

  void draw(NGLScene *scene);

  void setShaderUniforms();

  // uniforms
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

  void loadTextureFile(int _channelNum, const std::string &_textureFile = "");

  void createBufferTexture(int _textureUnit);

  std::vector<ShaderPro> *m_shaders;

  std::vector<GLuint> m_frameBuffers;

  std::vector<GLuint> m_depthStencilBuffers;

  ShaderPro *m_currentShader;
};

#endif

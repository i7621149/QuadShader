#ifndef SHADERLIBPRO_H__
#define SHADERLIBPRO_H__

#include "ngl/Singleton.h"
#include <string>

#include "ShaderPro.h"

#ifndef NGLSCENE_H__
#define NGLSCENE_H__

// define NGLScene drawScene function, as this is called in ShaderLibPro draw
class NGLScene
{
public :
  void drawScene(GLuint _progID);
};

#endif//NGLSCENE_H__


class ShaderLibPro : public ngl::Singleton<ShaderLibPro>
{
  friend class ngl::Singleton<ShaderLibPro>;
public :

  void setShaderInfo(const std::string &_sourceFile);

  void draw(NGLScene *_scene);

  void loadShaders();

  ShaderPro *getShader(const std::string &_shaderName);

private :
  ShaderLibPro();
  ~ShaderLibPro();

  std::vector<ShaderPro*> m_shaders;
};

#endif//SHADERLIBPRO_H__

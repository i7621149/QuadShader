#ifndef SHADERSET_H__
#define SHADERSET_H__

#include <string>

#include "ShaderPro.h"
#include "Entity.h"

#ifndef NGLSCENE_H__
#define NGLSCENE_H__

// define NGLScene drawScene function, as this is called in ShaderLibPro draw
class NGLScene
{
public :
  void drawScene(GLuint _progID);
};

#endif//NGLSCENE_H__

class ShaderSet
{
public :
  ShaderSet(const std::string &_sourceFile);
  ~ShaderSet();

  void setShaderInfo(const std::string &_sourceFile);

  void draw(Entity *_entity, ngl::Mat4 _camVP);

  void loadShaders();

  ShaderPro *getShader(const std::string &_shaderName);

private :
  std::vector<ShaderPro*> m_shaders;
};

#endif//SHADERLIBPRO_H__

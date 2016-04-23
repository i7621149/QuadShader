#ifndef SHADERSET_H__
#define SHADERSET_H__

#include <string>

#include "ShaderPro.h"
#include "Entity.h"
#include "ngl/Camera.h"

class ShaderSet
{
public :
  ShaderSet(const std::string &_sourceFile);
  ~ShaderSet();

  void setShaderInfo(const std::string &_sourceFile);

  void draw(Entity *_entity, ngl::Camera *_cam);

  void loadShaders();

  ShaderPro *getShader(const std::string &_shaderName);

private :
  std::vector<ShaderPro*> m_shaders;
};

#endif//SHADERLIBPRO_H__

#ifndef SHADERLIBPRO_H__
#define SHADERLIBPRO_H__

#include <ngl/Singleton.h>
#include <string>

#include "ShaderPro.h"

#ifndef NGLSCENE_H__
#define NGLSCENE_H__

// define NGLScene drawScene function, as this is called in ShaderLibPro draw
class NGLScene
{
  void drawScene();
};

#endif//NGLSCENE_H__


class ShaderLibPro : public ngl::Singleton<ShaderLibPro>
{
  friend class ngl::Singleton<ShaderLibPro>;
public :

  void setShaderInfo(const std::string &_sourceFile);

  void loadShaders();

  void draw(NGLScene *_scene);

private :
  ShaderLibPro();
  ~ShaderLibPro();


  std::vector<ShaderPro*> m_shaders;
};

#endif//SHADERLIBPRO_H__

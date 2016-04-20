#ifndef SHADERLIBPRO_H__
#define SHADERLIBPRO_H__

#include "ngl/Singleton.h"
#include <string>
#include <memory>

#include "ShaderSet.h"

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
  void addShader(const std::string &_sourceFile);

  void draw(NGLScene *_scene);

  void useShader(int _index);

  int getShaderSetAmount() {return m_shaderSets.size();}

private :
  ShaderLibPro();
  ~ShaderLibPro();

  std::vector<std::unique_ptr<ShaderSet>> m_shaderSets;
  int m_currentShaderindex;
};

#endif//SHADERLIBPRO_H__

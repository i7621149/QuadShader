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
  void newShaderProgram(std::string _progName, std::string _fragFile, std::string _vertFile = "shaders/QuadVertex.glsl");
  void useShaderProgram(std::string _progName);
  void loadTexture(std::string _progName, std::string _textureFile, GLuint *textures, int _channelNum);
private :
  ShaderLibPro();
  ~ShaderLibPro();
  // using a pointer attribute to avoid creating it locally at the start of every function
  ngl::ShaderLib *m_shader;
  // unordered map? eg to check whether textures are initialised for it?
};

#endif

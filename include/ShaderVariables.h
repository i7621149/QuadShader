#ifndef SHADERVARIABLES_H__
#define SHADERVARIABLES_H__

#include <ngl/Util.h>
#include <ngl/Singleton.h>

struct ShaderVariables : public ngl::Singleton<ShaderVariables>
{
  friend class ngl::Singleton<ShaderVariables>;
public :
  void reset(bool _hard=false);
  void loadToShader(GLuint _progID);
  void printVariables();

  ngl::Vec3 resolution;
  float globalTime;
  float timeDelta;
  int frame;
  ngl::Vec4 mouse;
  ngl::Vec4 date;

private :
  ShaderVariables();
  ~ShaderVariables();
};

#endif

#ifndef SHADERVARIABLES_H__
#define SHADERVARIABLES_H__

#include <ngl/Util.h>
#include <ngl/Singleton.h>

struct ShaderVariables : public ngl::Singleton<ShaderVariables>
{
public :
  ShaderVariables();
  ~ShaderVariables();
  void reset();

  ngl::Vec3 resolution;
  float globalTime;
  float timeDelta;
  int frame;
  ngl::Vec4 mouse;
  ngl::Vec4 date;
};

#endif

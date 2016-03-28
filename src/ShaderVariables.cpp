#include "ShaderVariables.h"

ShaderVariables::ShaderVariables() :
  resolution(0,0,0),
  globalTime(0),
  timeDelta(0),
  frame(0),
  mouse(0,0,0,0),
  date(0,0,0,0)
{

}

ShaderVariables::~ShaderVariables()
{

}

void ShaderVariables::reset()
{
  resolution = ngl::Vec3
}

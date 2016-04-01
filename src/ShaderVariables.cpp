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

void ShaderVariables::reset(bool _hard)
{
  globalTime = 0;
  timeDelta = 0;
  frame = 0;
  if(_hard){
    resolution = ngl::Vec3(0, 0, 0);
    mouse = ngl::Vec4(0,0,0,0);
    date = ngl::Vec4(0,0,0,0);
  }
}

void ShaderVariables::loadToShader(GLuint _progID)
{
  glUniform3f(glGetUniformLocation(_progID, "iResolution"),resolution[0], resolution[1], resolution[2]);
  glUniform1f(glGetUniformLocation(_progID, "iGlobalTime"), globalTime);
  glUniform1f(glGetUniformLocation(_progID, "iTimeDelta"), timeDelta);
  glUniform1i(glGetUniformLocation(_progID, "iFrame"), frame);
  glUniform4f(glGetUniformLocation(_progID, "iMouse"), mouse[0], mouse[1], mouse[2], mouse[3]);
  glUniform4f(glGetUniformLocation(_progID, "iDate"), date[0], date[1], date[2], date[3]);
}

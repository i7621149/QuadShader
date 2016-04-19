#ifndef BOX_H__
#define BOX_H__

#include "Entity.h"

#include "ngl/Vec3.h"
#include "ngl/Transformation.h"

class Box : public Entity
{
public :
  Box();
  Box(ngl::Vec3 _pos);
  ~Box() = default;

  void update();
  void draw(GLuint _progID, ngl::Mat4 _VPMatrix);

private :
  ngl::Vec3 m_rotSpeed = ngl::Vec3(0,0,0);
};

#endif//BOX_H__

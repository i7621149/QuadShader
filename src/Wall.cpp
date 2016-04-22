#include "Wall.h"
#include "ngl/VAOPrimitives.h"

Wall::Wall(ngl::Vec3 _pos, ngl::Vec3 _size) :
  Entity(5, _pos)
{
  m_scale = _size;
}

void Wall::draw()
{
  ngl::VAOPrimitives::instance()->draw("cube");
}

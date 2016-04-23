#include "Floor.h"
#include "ngl/VAOPrimitives.h"

Floor::Floor(ngl::Vec3 _pos, ngl::Vec3 _size) :
  Entity(5, _pos)
{
  m_scale = _size;
}

void Floor::draw()
{
  m_mesh->draw();
}

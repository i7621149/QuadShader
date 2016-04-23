#include "Wall.h"
#include "ngl/VAOPrimitives.h"

Wall::Wall(ngl::Vec3 _pos, ngl::Vec3 _size) :
  Entity(4, _pos)
{
  m_scale = _size;
}

void Wall::draw()
{
  m_mesh->draw();
}

void Wall::loadMesh(ngl::Obj *_mesh)
{
  m_mesh = _mesh;
}

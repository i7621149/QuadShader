#include "Block.h"
#include "ngl/VAOPrimitives.h"

Block::Block(int _id, ngl::Vec3 _pos, ngl::Vec3 _size) :
  Entity(_id, _pos)
{
  m_scale = _size;
}

void Block::draw()
{
  m_mesh->draw();
}

void Block::loadMesh(ngl::Obj *_mesh)
{
  m_mesh = _mesh;
}

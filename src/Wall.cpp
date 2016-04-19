#include "Wall.h"
#include "ngl/VAOPrimitives.h"
#include "ngl/Transformation.h"

Wall::Wall(ngl::Vec3 _pos, ngl::Vec3 _size) :
  Entity(_pos)
{
  m_scale = _size;
}

Wall::~Wall()
{

}

void Wall::update()
{

}


void Wall::draw()
{
  ngl::VAOPrimitives::instance()->draw("cube");
}

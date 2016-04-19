#include "Entity.h"

Entity::Entity(ngl::Vec3 _pos) :
  m_pos(_pos),
  m_alive(true)
{

}

ngl::Transformation Entity::getTransform()
{
  ngl::Transformation t;
  t.setPosition(m_pos);
  t.setRotation(m_rot);
  t.setScale(m_scale);
  t.getMatrix();
  return t;
}

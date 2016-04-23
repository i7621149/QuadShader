#include "Entity.h"
#include "ShaderLibPro.h"
#include "ShaderVariables.h"
#include "ngl/Obj.h"

Entity::Entity(int _id, ngl::Vec3 _pos) :
  m_matID(_id),
  m_pos(_pos),
  m_currentIndex(0),
  m_alive(true)
{

}

ngl::Mat4 Entity::getTransformMatrix()
{
  ngl::Transformation t;
  t.setPosition(m_pos);
  t.setRotation(m_rot);
  t.setScale(m_scale);
  return t.getMatrix();
}

void Entity::resetIndexRange(int _min, int _max)
{
  m_minIndex = _min;
  m_maxIndex = _max;
}

void Entity::setCurrentIndex(int _index)
{
  m_currentIndex = _index;
  if(m_currentIndex + m_minIndex > m_maxIndex)
  {
    std::cerr << "assigning shaderIndex too large!" << std::endl;
  }
}

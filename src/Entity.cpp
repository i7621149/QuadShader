#include "Entity.h"
#include "ShaderLibPro.h"
#include "ShaderVariables.h"
#include "ngl/Random.h"
#include "ngl/Obj.h"

Entity::Entity(int _id, ngl::Vec3 _pos) :
  m_matID(_id),
  m_pos(_pos),
  m_shaderIndex(0),
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

void Entity::randomiseShaderIndex()
{
  if(m_currentMaxIndex < m_maxIndex)
  {
    m_currentMaxIndex++;
  }

  int randomIndex = (int)ngl::Random::instance()->randomPositiveNumber(m_currentMaxIndex);
  std::cout << randomIndex << "/" << m_currentMaxIndex << std::endl;

  setShaderIndex(randomIndex);
}

void Entity::resetIndexRange(int _min, int _max)
{
  m_minIndex = _min;
  m_maxIndex = _max;
  m_currentMaxIndex = m_minIndex;
}

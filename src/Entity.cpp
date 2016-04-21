#include "Entity.h"
#include "ShaderLibPro.h"
#include "ShaderVariables.h"

Entity::Entity(ngl::Vec3 _pos) :
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

#include "Entity.h"

#include "ShaderVariables.h"

Entity::Entity(ngl::Vec3 _pos) :
  m_pos(_pos),
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

void Entity::loadDataToShader(GLuint _progID, ngl::Mat4 _VPMatrix)
{
  ngl::Mat4 MVP;

  MVP = getTransformMatrix()* _VPMatrix;

  ShaderVariables::instance()->MVP = MVP;
  ShaderVariables::instance()->loadToShader(_progID);
}

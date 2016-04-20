#ifndef ENTITY_H__
#define ENTITY_H__

#include "ngl/Vec3.h"
#include "ngl/Transformation.h"

class Entity
{
public :
  Entity(ngl::Vec3 _pos);
  ~Entity() = default;

  ngl::Mat4 getTransformMatrix();
  ngl::Vec3 getPos() {return m_pos;}
  ngl::Vec3 getVel() {return m_vel;}
  ngl::Vec3 getRot() {return m_rot;}
  ngl::Vec3 getScale() {return m_scale;}
  int getShaderIndex() {return m_shaderIndex;}
  void setShaderIndex(int _index) {m_shaderIndex = _index;}

  bool isAlive() {return m_alive;}

  virtual void update() = 0;
  virtual void draw(GLuint _progID, ngl::Mat4 VPMatrix) = 0;
  void loadDataToShader(GLuint _progID, ngl::Mat4 _VPMatrix);

protected :
  ngl::Vec3 m_pos;
  ngl::Vec3 m_vel = ngl::Vec3(0,0,0);
  ngl::Vec3 m_rot = ngl::Vec3(0,0,0);
  ngl::Vec3 m_scale = ngl::Vec3(1,1,1);

  int m_shaderIndex;

  bool m_alive;
};

#endif//ENTITY_H__

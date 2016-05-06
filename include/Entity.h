#ifndef ENTITY_H__
#define ENTITY_H__

#include "ngl/Vec3.h"
#include "ngl/Transformation.h"

class Entity
{
public :
  Entity(int _id, ngl::Vec3 _pos);
  ~Entity() = default;

  ngl::Mat4 getTransformMatrix();
  ngl::Vec3 getPos() {return m_pos;}
  ngl::Vec3 getVel() {return m_vel;}
  ngl::Vec3 getRot() {return m_rot;}
  ngl::Vec3 getScale() {return m_scale;}

  void setPos(ngl::Vec3 _pos) {m_pos = _pos;}
  void setVel(ngl::Vec3 _vel) {m_vel = _vel;}
  void setRot(ngl::Vec3 _rot) {m_rot = _rot;}
  void setScale(ngl::Vec3 _scale) {m_scale = _scale;}

  int getShaderLibIndex() {return m_currentIndex + m_minIndex;}
  int getCurrentIndex(){ return m_currentIndex;}
  void setCurrentIndex(int _index);
  void resetIndexRange(int _min, int _max);
  int getMinIndex(){return m_minIndex;}
  int getMaxIndex(){return m_maxIndex;}

  int getMatID() {return m_matID;}

  bool isAlive() {return m_alive;}

  virtual void update() = 0;
  virtual void draw() = 0;

protected :
  int m_matID;
  ngl::Vec3 m_pos;
  ngl::Vec3 m_vel = ngl::Vec3(0,0,0);
  ngl::Vec3 m_rot = ngl::Vec3(0,0,0);
  ngl::Vec3 m_scale = ngl::Vec3(1,1,1);

  int m_currentIndex;

  int m_minIndex;
  int m_maxIndex;

  bool m_alive;
};

#endif//ENTITY_H__

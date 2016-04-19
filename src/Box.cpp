#include "Box.h"
#include <QTime>
#include <iostream>
#include <cmath>
#include "ngl/Util.h"
#include "ngl/VAOPrimitives.h"
#include "ngl/Random.h"

Box::Box() :
  Entity(ngl::Vec3(0,0,0))
{
  m_rotSpeed = ngl::Random::instance()->getRandomNormalizedVec3();
  m_rotSpeed *= 3.0;
}

Box::Box(ngl::Vec3 _pos) :
  Entity(_pos)
{
  // set random rotation
  m_rotSpeed = ngl::Random::instance()->getRandomNormalizedVec3();
  m_rotSpeed *= 3.0;
}

void Box::update()
{
  int time = QTime::currentTime().msecsSinceStartOfDay();

  float yPos = sin(ngl::radians((time%2000)/2000.0*360)) / 5.0 + 0.5;

  m_pos.m_y = yPos;

  m_rot += m_rotSpeed;
}

void Box::draw(GLuint _progID, ngl::Mat4 _VPMatrix)
{
  loadDataToShader(_progID, _VPMatrix);
  ngl::VAOPrimitives::instance()->draw("cube");
}

#include "Pill.h"
#include <QTime>
#include <iostream>
#include <cmath>
#include "ngl/Util.h"
#include "ngl/VAOPrimitives.h"
#include "ngl/Random.h"

Pill::Pill() :
  Entity(3, ngl::Vec3(0,0,0))
{
  m_rotSpeed = ngl::Random::instance()->getRandomNormalizedVec3();
  m_rotSpeed *= 3.0;
}

Pill::Pill(ngl::Vec3 _pos) :
  Entity(3, _pos)
{
  // set random rotation upon creation
  m_rotSpeed = ngl::Random::instance()->getRandomNormalizedVec3() * 3.0;
}

void Pill::update(Player *_player1, Player *_player2)
{
  // making the pill move up and down in a sin wave
  int time = QTime::currentTime().msecsSinceStartOfDay();
  float yPos = (sin(ngl::radians(((time%2000)/2000.0 + m_offset)*360))) * 0.2 + 0.7;
  m_pos.m_y = yPos;

  // rotate pill
  m_rot += m_rotSpeed;

  // check for player collisions
  ngl::Vec3 collisionPos = m_pos;
  collisionPos.m_y = 0;
  // check for valid players before picking pill up
  if(_player1 && (_player1->getPos() - collisionPos).lengthSquared() < _player1->getPickUpRad())
  {
    _player1->pickUpPill();
    m_alive = false;
  }
  else if(_player2 && (_player2->getPos() - collisionPos).lengthSquared() < _player2->getPickUpRad())
  {
    _player2->pickUpPill();
    m_alive = false;
  }


}

void Pill::draw()
{
  m_mesh->draw();
}

void Pill::reset(ngl::Vec3 _pos)
{
  m_pos = _pos;

  // call update just to set its floating position
  // without this is holds for a frame and then jumps which looks gross
  update(nullptr, nullptr);
  m_alive = true;
}

void Pill::loadMesh(ngl::Obj *_mesh)
{
  m_mesh = _mesh;
}


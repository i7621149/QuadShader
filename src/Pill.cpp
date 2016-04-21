#include "Pill.h"
#include <QTime>
#include <iostream>
#include <cmath>
#include "ngl/Util.h"
#include "ngl/VAOPrimitives.h"
#include "ngl/Random.h"

Pill::Pill() :
  Entity(ngl::Vec3(0,0,0))
{
  m_rotSpeed = ngl::Random::instance()->getRandomNormalizedVec3();
  m_rotSpeed *= 3.0;
}

Pill::Pill(ngl::Vec3 _pos) :
  Entity(_pos)
{
  // set random rotation
  m_rotSpeed = ngl::Random::instance()->getRandomNormalizedVec3();
  m_rotSpeed *= 3.0;
}

void Pill::update(Player *_player1, Player *_player2)
{
  int time = QTime::currentTime().msecsSinceStartOfDay();

  float yPos = sin(ngl::radians((time%2000)/2000.0*360)) / 5.0 + 0.5;

  m_pos.m_y = yPos;

  m_rot += m_rotSpeed;

  if(_player1 && _player2)
  {
    if((_player1->getPos() - m_pos).lengthSquared() < 1)
    {
      _player1->pickUpBox();
      m_alive = false;
    }
    else if((_player2->getPos() - m_pos).lengthSquared() < 1)
    {
      _player2->pickUpBox();
      m_alive = false;
    }
  }

}

void Pill::draw()
{
  ngl::VAOPrimitives::instance()->draw("cube");
}

void Pill::reset(ngl::Vec3 _pos)
{
  m_pos = _pos;
  update(nullptr, nullptr);
  m_alive = true;
}

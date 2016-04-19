#include "Player.h"
#include <cmath>
#include <iostream>
#include "ngl/NGLStream.h"
#include "ngl/Util.h"
#include "ngl/VAOPrimitives.h"

Player::Player() :
  Entity(ngl::Vec3(0,0,0))
{

}

Player::~Player()
{

}

void Player::update(ngl::Vec3 _dir)
{
  if(_dir.length()>0)
  {
    _dir.normalize();
  }

  m_vel = _dir;

  if(m_vel.length() > m_maxSpeed)
  {
    m_vel.normalize();
    m_vel = m_vel * m_maxSpeed;
  }

  m_pos += m_vel;
}

void Player::draw()
{
  ngl::VAOPrimitives::instance()->draw("teapot");
}

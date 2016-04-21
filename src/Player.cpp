#include "Player.h"
#include <cmath>
#include <iostream>
#include "ngl/NGLStream.h"
#include "ngl/Util.h"
#include "ngl/VAOPrimitives.h"
#include "ngl/Random.h"

#include "ShaderLibPro.h"

Player::Player(ngl::Vec3 _pos, float _areaSize) :
  Entity(_pos),
  m_areaSize(_areaSize)
{
}

Player::~Player()
{

}

void Player::update(ngl::Vec3 _dir, bool _attack, Player *_otherPlayer)
{
  // update function for user controlled player
  // if player is on ground then calculate movement
  if(m_grounded)
  {
    if(m_stunnedTime <= 0)
    {
      if(_dir.lengthSquared() > 0)
      {
        _dir.normalize();
      }

      m_vel = _dir;

      if(m_vel.lengthSquared() > m_maxSpeed*m_maxSpeed)
      {
        m_vel.normalize();
        m_vel *= m_maxSpeed;
      }

      if(m_vel.lengthSquared() > 0)
      {
        m_vel.m_y = 0.2f;
      }
      m_grounded = false;
    }
    else
    {
      // only recover from stun while on the ground
      // stun means player cannot move in x or z
      m_vel.m_x = 0;
      m_vel.m_z = 0;
      m_stunnedTime --;
    }
  }
  else
  {
    // if player is not on the ground, add falling acceleration to vel
    m_vel.m_y -= 0.09f;
  }

  if(m_stunnedTime <= 0 || m_grounded)
  {
    m_rot.m_x = 0;
    m_rot.m_z = 0;
  }
  else
  {
    // random rotation in air if not grounded
    m_rot.m_x = ngl::Random::instance()->randomNumber(360);
    m_rot.m_z = ngl::Random::instance()->randomNumber(360);
  }

  // test to see if player goes out of bounds, if so, bounce back
  ngl::Vec3 newPos = m_pos + m_vel;
  if(newPos.m_x < -m_areaSize || newPos.m_x > m_areaSize)
  {
    m_vel.m_x *= -1.5f;
    m_vel.m_y = 0.7f;
    m_stunnedTime = m_stunnedCooldown / 2.0;
    m_grounded = false;
  }
  if(newPos.m_z < -m_areaSize || newPos.m_z > m_areaSize)
  {
    m_vel.m_z *= -1.5f;
    m_vel.m_y = 0.7f;
    m_stunnedTime = m_stunnedCooldown / 2.0;
    m_grounded = false;
  }

  // set attack if attacking is true
  if(_attack && m_attackTime <= 0)
  {
    m_attacking = true;
    m_prevYPos = m_rot.m_y;

    ngl::Vec3 playerDistVec = m_pos - _otherPlayer->getPos();
    // check distance to other player, for hit/miss
    if(playerDistVec.lengthSquared() < m_attackRad*m_attackRad)
    {
      _otherPlayer->hit();
    }
    m_attackTime = m_attackCooldown;
  }
  else
  {
    m_attackTime--;
  }

  // calculate rotations for attacking
  if(m_attacking)
  {
    m_spin += 30;
    m_rot.m_y = m_prevYPos + m_spin;
    if(m_spin >= 360)
    {
      m_spin = 0;
      m_attacking = false;
    }
  }
  else
  {
    // rotation depends on input direction rather than vel
    if(_dir.lengthSquared() > 0)
    {
      m_rot.m_y = ngl::degrees(atan2(_dir.m_x, _dir.m_z))-90;
    }
  }

  m_pos += m_vel;
  // if player hits ground
  if(m_pos.m_y <= 0)
  {
    m_pos.m_y = 0;
    m_grounded = true;
  }
}

void Player::aiUpdate(Player *_otherPlayer)
{

}

void Player::draw()
{
  if(m_attacking)
  {
    ngl::VAOPrimitives::instance()->draw("cube");
  }
  else
  {
    m_mesh->draw();
  }
}

void Player::loadMesh(const std::string &_modelName)
{
  m_mesh.reset(new ngl::Obj(_modelName));
  m_mesh->createVAO();
}


void Player::hit()
{
  m_vel = ngl::Vec3(0,1,0);
  m_pos.m_y = 0.1f;
  m_grounded = false;
  m_stunnedTime = m_stunnedCooldown;
}

void Player::pickUpPill()
{
  m_score++;
}

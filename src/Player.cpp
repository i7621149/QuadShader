#include "Player.h"
#include <cmath>
#include <iostream>
#include "ngl/NGLStream.h"
#include "ngl/Util.h"
#include "ngl/VAOPrimitives.h"
#include "ngl/Random.h"

#include "ShaderLibPro.h"

Player::Player(int _playerNum, ngl::Vec3 _pos, float _areaSize) :
  Entity(_playerNum, _pos),
  m_areaSize(_areaSize)
{
  resetControls();
}

Player::~Player()
{

}

void Player::update(Player *_otherPlayer)
{
  // update function for user controlled player

  ngl::Vec3 dir;
  if(m_control.left) dir.m_x -= 1;
  if(m_control.right) dir.m_x += 1;
  if(m_control.up) dir.m_z -= 1;
  if(m_control.down) dir.m_z += 1;

  // if player attack or stun has not cooled down, cancel queued attack
  // this means that hitting attack while the player is stunned or attacking won't result in a delayed attack
  if(m_attackTime > 0 || m_stunnedTime > 0)
  {
    m_control.attack = false;
  }

  // if player is on ground then calculate movement
  if(m_grounded)
  {
    if(m_stunnedTime <= 0)
    {
      if(dir.lengthSquared() > 0)
      {
        dir.normalize();
      }

      m_vel = dir;

      if(m_vel.lengthSquared() > m_maxSpeed*m_maxSpeed)
      {
        m_vel.normalize();
        m_vel *= m_maxSpeed;
      }

      if(m_vel.lengthSquared() > 0)
      {
        m_vel.m_y = 0.15f;
      }
      m_grounded = false;
    }
    else
    {
      // only recover from stun while on the ground
      // stun means player cannot move in x or z
      m_vel.m_x = 0;
      m_vel.m_z = 0;
      m_stunnedTime--;
    }
  }
  else
  {
    // if player is not on the ground, add falling acceleration to vel
    m_vel.m_y -= 0.09f;
  }

  // if the player is on the ground, flip them the right way up
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
  // very limited form of checking, but enough to work for the square arena
  // a proper collision system would need to be added if the arena shape could change
  // but this works for the current game
  ngl::Vec3 newPos = m_pos + m_vel;
  // x walls
  if(newPos.m_x < -m_areaSize || newPos.m_x > m_areaSize)
  {
    m_vel.m_x *= -1.5f;
    m_vel.m_y = 0.7f;
    m_stunnedTime = m_stunnedCooldown / 2.0;
    m_grounded = false;
  }
  // z walls
  if(newPos.m_z < -m_areaSize || newPos.m_z > m_areaSize)
  {
    m_vel.m_z *= -1.5f;
    m_vel.m_y = 0.7f;
    m_stunnedTime = m_stunnedCooldown / 2.0;
    m_grounded = false;
  }

  // set attack if attacking is true
  if(m_control.attack && m_attackTime <= 0 && m_stunnedTime <= 0)
  {
    m_control.attack = false;
    m_attacking = true;
    m_prevYPos = m_rot.m_y;

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
      m_scale = ngl::Vec3(1,1,1);
    }
    else
    {
      // check distance to other player, for hit/miss
      ngl::Vec3 playerDistVec = m_pos - _otherPlayer->getPos();
      if(playerDistVec.lengthSquared() < m_attackRad*m_attackRad)
      {
        _otherPlayer->hit();
      }
      // scale the player to show farther reach while attacking
      m_scale = ngl::Vec3(m_attackRad, 1.0, m_attackRad);
    }

  }
  else
  {
    // rotation depends on input direction rather than vel
    if(dir.lengthSquared() > 0)
    {
      m_rot.m_y = ngl::degrees(atan2(dir.m_x, dir.m_z))-90;
    }
  }

  // control is snappier with this, in which momentum is not conserved in the air if the player is not stunned
  if(m_stunnedTime <= 0 && !(dir.length() > 0))
  {
    m_vel.m_x = 0;
    m_vel.m_z = 0;
  }

  m_pos += m_vel;
  // if player hits ground
  if(m_pos.m_y <= 0)
  {
    m_pos.m_y = 0;
    m_grounded = true;
  }
}

void Player::draw()
{
  // draw either normal mesh or attack mesh
  if(m_attacking)
  {
    m_attackMesh->draw();
  }
  else
  {
    m_mesh->draw();
  }
}

void Player::loadMeshes(ngl::Obj *_mesh, ngl::Obj *_attackMesh)
{
  // two meshes, one for regular, one on attack
  m_mesh = _mesh;
  m_attackMesh = _attackMesh;
}


void Player::hit()
{
  // fly into the air, and stun the player
  m_vel = ngl::Vec3(0,1,0);
  m_pos.m_y = 0.1f;
  m_grounded = false;
  m_stunnedTime = m_stunnedCooldown;
}

void Player::pickUpPill()
{
  // simply increase score when player picks up pill
  m_score++;
}

void Player::reset(ngl::Vec3 _startPos)
{
  // set position, reset velocity
  m_pos = _startPos;
  m_vel = ngl::Vec3(0,0,0);

  // rotate to face center
  ngl::Vec3 aim = ngl::Vec3(0,0,0) - m_pos;
  m_rot = ngl::Vec3(0, ngl::degrees(atan2(aim.m_x, aim.m_z))-90, 0);

  //reset score and controls
  m_score = 0;
  resetControls();
}

float Player::getPickUpRad()
{
  // if the player is attacking their pickup radius is larger
  if(m_attacking)
  {
    // pretty generous here to reward players who are using advanced skill, "attack to pick up pills"
    return m_attackRad + 2.5;
  }
  else
  {
    // relatively generous radius here as well
    return 2.0;
  }
}

void Player::resetControls()
{
  // set all of control structure to false, useful in case player is holding direction before game start
  m_control.left = false;
  m_control.right = false;
  m_control.up = false;
  m_control.down = false;
  m_control.attack = false;
}


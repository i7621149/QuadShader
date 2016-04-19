#ifndef PLAYER_H__
#define PLAYER_H__

#include "Entity.h"

#include "ngl/Vec3.h"
#include "ngl/Transformation.h"

class Player : public Entity
{
public :
  Player(ngl::Vec3 _pos, float _areaSize);
  ~Player();

  void update() {std::cerr << "player needs direction to update with" << std::endl;}
  void update(ngl::Vec3 _dir, bool _attack, Player *_otherPlayer);
  void draw(GLuint _progID, ngl::Mat4 _VPMatrix);

  void attack(Player *_otherPlayer);
  void hit();

private :
  float m_prevYPos = 0;
  float m_spin = 0;
  float m_maxSpeed = 0.3;
  float m_attackRad = 5;
  float m_stunnedTime = 0;
  float m_stunnedStep = 0.1;
  bool m_attacking = false;
  bool m_grounded = true;
  float m_areaSize;
};

#endif//PLAYER_H__

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

  void hit();
  void pickUpBox();
  int getScore() {return m_score;}

private :
  float m_prevYPos = 0.0f;
  float m_spin = 0.0f;
  float m_maxSpeed = 0.3f;
  float m_attackRad = 5.0f;
  float m_stunnedTime = 0.0;
  float m_stunnedStep = 0.1f;
  bool m_attacking = false;
  bool m_grounded = true;
  int m_score = 0;

  float m_areaSize;
};

#endif//PLAYER_H__

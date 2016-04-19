#ifndef PLAYER_H__
#define PLAYER_H__

#include "Entity.h"

#include "ngl/Vec3.h"
#include "ngl/Transformation.h"

class Player : public Entity
{
public :
  Player();
  ~Player();

  void update() {std::cerr << "player needs direction to update with" << std::endl;}
  void update(ngl::Vec3 _dir);
  void draw();

private :
  float m_prevYRot = 0;
  float m_maxSpeed = 0.3;
};

#endif//PLAYER_H__

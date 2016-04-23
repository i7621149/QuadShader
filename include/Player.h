#ifndef PLAYER_H__
#define PLAYER_H__

#include "Entity.h"

#include "ngl/Vec3.h"
#include "ngl/Transformation.h"
#include <memory>
#include <vector>
#include "ngl/Obj.h"

class Player : public Entity
{
public :
  Player(int _playerNum, ngl::Vec3 _pos, float _areaSize);
  ~Player();

  void update() {std::cerr << "player needs direction to update with" << std::endl;}
  void update(ngl::Vec3 _dir, bool _attack, Player *_otherPlayer);
  void draw();

  void hit();
  void pickUpPill();
  void setScore(int _score) {m_score = _score;}
  int getScore() {return m_score;}
  void loadMeshes(ngl::Obj *_mesh, ngl::Obj *_attackMesh);

  void setPos(ngl::Vec3 _pos) {m_pos = _pos;}

  float getPickUpRad();

private :
  ngl::Obj *m_mesh;
  ngl::Obj *m_attackMesh;
  float m_prevYPos = 0.0f;
  float m_spin = 0.0f;
  float m_maxSpeed = 0.3f;
  float m_attackRad = 2.8f;
  float m_stunnedTime = 0.0f;
  float m_stunnedCooldown = 10.0f;
  bool m_attacking = false;
  bool m_grounded = true;
  int m_score = 0;
  float m_attackTime = 0.0f;
  float m_attackCooldown = 30.0f;
  float m_areaSize;
};

#endif//PLAYER_H__

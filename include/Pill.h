#ifndef PILL_H__
#define PILL_H__

#include "Entity.h"

#include "ngl/Vec3.h"
#include "ngl/Transformation.h"
#include "Player.h"

class Pill : public Entity
{
public :
  Pill();
  Pill(ngl::Vec3 _pos);
  ~Pill() = default;

  void update() {std::cerr << "pill needs player positions to update" << std::endl;}
  void update(Player *_player1, Player *_player2);
  void draw();
  void reset(ngl::Vec3 _pos);

private :
  ngl::Vec3 m_rotSpeed = ngl::Vec3(0,0,0);
};

#endif//PILL_H__

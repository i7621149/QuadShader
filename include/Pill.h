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

  void setOffset(float _offset) {m_offset = _offset;}

  void loadMesh(ngl::Obj *_mesh);

private :
  ngl::Vec3 m_rotSpeed = ngl::Vec3(0,0,0);
  float m_offset;
  // using an obj since ngl capsule does not have UVs
  ngl::Obj *m_mesh;
};

#endif//PILL_H__

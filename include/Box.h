#ifndef BOX_H__
#define BOX_H__

#include "Entity.h"

#include "ngl/Vec3.h"
#include "ngl/Transformation.h"
#include "Player.h"

class Box : public Entity
{
public :
  Box();
  Box(ngl::Vec3 _pos);
  ~Box() = default;

  void update() {std::cerr << "box needs player positions to update" << std::endl;}
  void update(Player *_player1, Player *_player2);
  void draw(GLuint _progID, ngl::Mat4 _VPMatrix);
  void reset(ngl::Vec3 _pos);

private :
  ngl::Vec3 m_rotSpeed = ngl::Vec3(0,0,0);
};

#endif//BOX_H__

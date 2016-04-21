#ifndef FLOOR_H__
#define FLOOR_H__

#include "Entity.h"


class Floor : public Entity
{
public :
  Floor(ngl::Vec3 _pos, ngl::Vec3 _size);
  ~Floor() = default;

  void update() {}
  void draw();

  void createQuad();
};

#endif//FLOOR_H__

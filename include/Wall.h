#ifndef WALL_H__
#define WALL_H__

#include "Entity.h"

#include "ngl/Vec3.h"
#include "ngl/Transformation.h"

class Wall : public Entity
{
public :
  Wall(ngl::Vec3 _pos, ngl::Vec3 _size);
  ~Wall();

  void update();
  void draw();
};

#endif//WALL_H__

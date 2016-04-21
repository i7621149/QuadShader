#ifndef BACKGROUND_H__
#define BACKGROUND_H__

#include "Entity.h"

class Background : public Entity
{
public :
  Background();
  ~Background() = default;

  void update() {;}
  void draw();

  void createQuad();

private :
  GLuint m_vaoID;
};

#endif//BACKGROUND_H__

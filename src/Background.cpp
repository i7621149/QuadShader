#include "Background.h"

#include "ngl/Mat4.h"
#include "ShaderVariables.h"

Background::Background() :
  Entity(ngl::Vec3::zero())
{

}

void Background::draw()
{
  // create new identity matrix
  ngl::Mat4 identityMat4;
  ngl::Mat3 identityMat3;

  ShaderVariables::instance()->MVP = identityMat4;

  // bind and draw our quad
  glBindVertexArray(m_vaoID);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Background::createQuad()
{
  // a simple quad object which fills the screen, from Jon's code
  float* vert = new float[18];
  const static float s=1.0;
  vert[0] =-s; vert[1] =  s; vert[2] =-1.0;
  vert[3] = s; vert[4] =  s; vert[5] =-1.0;
  vert[6] = -s; vert[7] = -s; vert[8]= -1.0;

  vert[9] =-s; vert[10]= -s; vert[11]=-1.0;
  vert[12] =s; vert[13]= -s; vert[14]=-1.0;
  vert[15] =s; vert[16]= s; vert[17]=-1.0;
  // allocate a VertexArray
  glGenVertexArrays(1, &m_vaoID);
  // now bind a vertex array object for our verts
  glBindVertexArray(m_vaoID);
  // now we are going to bind this to our vbo
  GLuint vboID;
  glGenBuffers(1, &vboID);
  // now bind this to the VBO buffer
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  // allocate the buffer datra
  glBufferData(GL_ARRAY_BUFFER, 18*sizeof(GLfloat), vert, GL_STATIC_DRAW);
  // now fix this to the attribute buffer 0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  // enable and bind this attribute (will be inPosition in the shader)
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // clean up
  delete [] vert;
}

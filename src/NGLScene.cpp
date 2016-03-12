
#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <iostream>
#include <ngl/NGLStream.h>

#include <cstdlib>

NGLScene::NGLScene() :
  m_mode(0),
  m_time(0),
  m_mouseDown(false),
  m_texture(nullptr),
  m_globalTime(QTime::currentTime())
{
  setTitle("Shader Tests");
  //m_image = new char[WIDTH*HEIGHT*3*sizeof(char)];
  m_globalTime.start();
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  delete m_texture;
}

void NGLScene::resizeGL(QResizeEvent *_event)
{
  m_width=_event->size().width()*devicePixelRatio();
  m_height=_event->size().height()*devicePixelRatio();

  ngl::ShaderLib::instance()->setRegisteredUniform("iResolution", ngl::Vec3(m_width, m_height, 1.0));
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
  ngl::ShaderLib::instance()->setRegisteredUniform("iResolution", ngl::Vec3(m_width, m_height, 1.0));
}


void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  createQuad();

  //create sphere on initialize
  ngl::VAOPrimitives::instance()->createSphere("mySphere", 1.0, 100);

  //instance of shader manager
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  shader->createShaderProgram("quad");

  //attatch two shader stages to the shader
  shader->attachShader("quadVertex", ngl::ShaderType::VERTEX);
  shader->attachShader("quadFragment", ngl::ShaderType::FRAGMENT);

  //load the shaders text source
  shader->loadShaderSource("quadVertex", "shaders/QuadVertex.glsl");
  shader->loadShaderSource("quadFragment", "shaders/QuadFragment.glsl");

  //compile source code
  shader->compileShader("quadVertex");
  shader->compileShader("quadFragment");

  //attach to created program
  shader->attachShaderToProgram("quad", "quadVertex");
  shader->attachShaderToProgram("quad", "quadFragment");

  //link it up
  shader->linkProgramObject("quad");
  //tell shader to use it
  shader->use("quad");

  startTimer(16);

}

void NGLScene::loadTexture(const std::string &file)
{
  QImage image;
  bool loaded=image.load("textures/quentin.jpg");
  if(loaded == true)
  {
    int width=image.width();
    int height=image.height();

    unsigned char *data = new unsigned char[ width*height*3];
    unsigned int index=0;
    QRgb colour;
    for( int y=0; y<height; ++y)
    {
      for( int x=0; x<width; ++x)
      {
        colour=image.pixel(x,y);

        data[index++]=qRed(colour);
        data[index++]=qGreen(colour);
        data[index++]=qBlue(colour);
      }
    }
  }
  m_texture = new QOpenGLTexture(image);

}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0,0,m_width,m_height);
  ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
  glBindVertexArray(m_vaoID);		// select first bind the array
  glDrawArrays(GL_TRIANGLES, 0, 6);	// draw object
}

void NGLScene::createQuad()
{
  // a simple quad object
  float* vert = new float[18];	// vertex array
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

  delete [] vert;
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{

}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  if(_event->button() == Qt::LeftButton){
    m_mouseDown = true;
    //std::cout << "click!" << std::endl;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  if(_event->button() == Qt::LeftButton){
    m_mouseDown = false;
    //std::cout << "unclick!" << std::endl;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow

  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  int newMode = m_mode;
  switch (_event->key())
  {
  // escape key to quite
    case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
    case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
    case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
    case Qt::Key_0 : newMode = 0; break;
    case Qt::Key_1 : newMode = 1; break;
    case Qt::Key_2 : newMode = 2; break;
    case Qt::Key_3 : newMode = 3; break;
    default : break;
  }
  // finally update the GLWindow and re-draw
  if(newMode != m_mode){
    m_mode = newMode;
    shader->setRegisteredUniform("mode", m_mode);
  }
  update();
}

void NGLScene::timerEvent(QTimerEvent *)
{
  m_time++;
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->setRegisteredUniform("time", m_time);

  std::cout << m_globalTime.elapsed() << ", "<< m_time << std::endl;

  float globalSeconds = m_globalTime.elapsed()/1000.0;
  shader->setRegisteredUniform("iGlobalTime", globalSeconds);
  ngl::Vec4 mouseData;
  if(m_mouseDown){
    QPoint p = this->mapFromGlobal(QCursor::pos());
    //std::cout << p.x() << ", " << p.y() << std::endl;
    mouseData[0] = p.x();
    mouseData[1] = m_height-p.y();
    mouseData[2] = 1.0; //mouse is pressed
  }
  else{
    mouseData[2] = 0.0; //mouse is not pressed
  }
  ngl::ShaderLib::instance()->setRegisteredUniform("iMouse", mouseData);

  update();
}


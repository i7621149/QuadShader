#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include "ShaderLibPro.h"
#include <ngl/VAOPrimitives.h>
#include <iostream>
#include <ngl/NGLStream.h>
#include <cstdlib>

NGLScene::NGLScene() :
  m_frame(0),
  m_mouseDown(false),
  m_time(QTime::currentTime()),
  m_lastFrameTime(0)
{
  setTitle("Felix's Shader");
}

NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL(QResizeEvent *_event)
{
  m_width=_event->size().width()*devicePixelRatio();
  m_height=_event->size().height()*devicePixelRatio();
  // set resolution in shader
  // uses a Vec3 to be compatible with Shadertoy
  ngl::ShaderLib::instance()->setRegisteredUniform("iResolution", ngl::Vec3(m_width, m_height, 1.0));
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
  // set resolution in shader
  // uses a Vec3 to be compatible with Shadertoy
  ngl::ShaderLib::instance()->setRegisteredUniform("iResolution", ngl::Vec3(m_width, m_height, 1.0));
}

void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  // this allows for automatic generation and compilation of shaders, plus loading texture easily
  ShaderLibPro *shader = ShaderLibPro::instance();
  // using shaderLibPro to generate simple vert/frag shaders
  shader->newShaderProgram("snail", "shaders/SnailFragment.glsl");
  shader->newShaderProgram("text", "shaders/TextInfoFragment.glsl");


  // generate texture unit ids
  glGenTextures(4, m_textures);
  // load textures to the 4 active texture units
  shader->loadTexture("snail", "/home/i7621149/CA1/images/tex12.png", m_textures, 0);
  shader->loadTexture("snail", "/home/i7621149/CA1/images/tex19.png", m_textures, 1);
  shader->loadTexture("snail", "/home/i7621149/CA1/images/tex09.png", m_textures, 2);
  shader->loadTexture("snail", "/home/i7621149/CA1/images/tex16.png", m_textures, 3);

  // should already be used after creating new shader and loading textures but this makes sure
  setCurrentShader("snail");

  // define the quad
  createQuad();

  // set up timer loop
  startTimer(16);
}

void NGLScene::paintGL()
{
  // increase frame number variable for shader
  m_frame++;
  ngl::ShaderLib::instance()->setRegisteredUniform("iFrame", m_frame);

  // clear the screen and depth buffer
  glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // not sure if glViewport is necessary?
  glViewport(0,0,m_width,m_height);
  // bind and draw our quad
  glBindVertexArray(m_vaoID);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // calculate time taken to render the frame (time since last frame was rendered)
  float renderTime = (m_time.elapsed() - m_lastFrameTime) / 1000.0;
  ngl::ShaderLib::instance()->setRegisteredUniform("iTimeDelta", renderTime);
  m_lastFrameTime = m_time.elapsed();
}

void NGLScene::createQuad()
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

void NGLScene::setCurrentShader(const std::string &_progName)
{
  // use shader
  if(m_currentShader != _progName){
    ShaderLibPro::instance()->useShaderProgram(_progName);
    m_currentShader = _progName;
  }
  // set resolution of shader
  resizeGL(m_width, m_height);
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
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  if(_event->button() == Qt::LeftButton){
    m_mouseDown = false;
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

  //ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  //int newMode = m_mode;
  switch (_event->key())
  {
  // escape key to quite
    case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
    case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
    case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;

    case Qt::Key_0 : setCurrentShader("snail"); break;
    case Qt::Key_1 : setCurrentShader("text"); break;

    default : break;
  }
  /*
  // finally update the GLWindow and re-draw
  if(newMode != m_mode){
    m_mode = newMode;
    shader->setRegisteredUniform("mode", m_mode);
  }
  */
  //update();
}

void NGLScene::timerEvent(QTimerEvent *)
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  //getting seconds by dividing milliseconds by 1000
  float globalSeconds = m_time.elapsed()/1000.0;
  shader->setRegisteredUniform("iGlobalTime", globalSeconds);

  QDate date = QDate::currentDate();
  float dateYear = date.year();
  float dateMonth = date.month();
  float dateDay = date.day();
  float dateSeconds = (m_time.msecsSinceStartOfDay() + m_time.elapsed()) / 1000.0;
  shader->setRegisteredUniform("iDate", dateYear, dateMonth, dateDay, dateSeconds);

  ngl::Vec4 mouseData;
  if(m_mouseDown){
    QPoint p = this->mapFromGlobal(QCursor::pos());
    //std::cout << p.x() << ", " << p.y() << std::endl;
    mouseData[0] = p.x();
    mouseData[1] = m_height-p.y();
    mouseData[2] = 1.0; // mouse is pressed
  }
  else{
    mouseData[2] = 0.0; // mouse is not pressed
  }
  ngl::ShaderLib::instance()->setRegisteredUniform("iMouse", mouseData);

  update();
}

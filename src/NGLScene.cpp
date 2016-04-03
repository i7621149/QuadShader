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
#include "ShaderVariables.h"

NGLScene::NGLScene() :
  m_fullScreen(false),
  m_mouseDown(false),
  m_time(QTime::currentTime()),
  m_lastFrameTime(0),
  m_mouseData(0,0,0,0),
  m_mode(drawMode::TEAPOT),
  m_position(2,1,2),
  m_focus(0,0.5,0),
  m_up(0,1,0),
  m_cam(m_position, m_focus, m_up),
  m_view(ngl::lookAt(m_position, m_focus, m_up)),
  m_oldYRotation(0)
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
  //ngl::ShaderLib::instance()->setRegisteredUniform("iResolution", ngl::Vec3(m_width, m_height, 1.0));
  ShaderVariables::instance()->resolution = ngl::Vec3(m_width, m_height, 1.0);

  m_project = ngl::perspective(45.0f, float(m_width)/m_height, 0.2f, 20.0f);
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
  // set resolution in shader
  // uses a Vec3 to be compatible with Shadertoy, even though resolution is (x,y)
  //ngl::ShaderLib::instance()->setRegisteredUniform("iResolution", ngl::Vec3(m_width, m_height, 1.0));
  ShaderVariables::instance()->resolution = ngl::Vec3(m_width, m_height, 1.0);

  m_project = ngl::perspective(45.0f, float(m_width)/m_height, 0.2f, 20.0f);
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

  // enable nice cube maps, no seams
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  //ShaderLibPro::instance()->setShaderInfo("shaders/test.glll");
  //ShaderLibPro::instance()->setShaderInfo("shaders/buffertest/buffertest.glll");
  ShaderLibPro::instance()->setShaderInfo("shaders/geotest/geotest.glll");

  // define the quad
  createQuad();

  ngl::VAOPrimitives::instance()->createSphere("sphere", 1.0, 100);

  // set up timer loop
  startTimer(16);
}

void NGLScene::paintGL()
{
  // increase frame number variable for shader
  ShaderVariables::instance()->frame++;

  ShaderLibPro::instance()->draw(this);

  // calculate time taken to render the frame (time since last frame was rendered)
  float renderTime = (m_time.elapsed() - m_lastFrameTime) / 1000.0;
  //ngl::ShaderLib::instance()->setRegisteredUniform("iTimeDelta", renderTime);
  ShaderVariables::instance()->timeDelta = renderTime;

  m_lastFrameTime = m_time.elapsed();
}

void NGLScene::drawScene()
{
  // clear the screen and depth buffer
  glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // not sure if glViewport is necessary?
  glViewport(0, 0, m_width, m_height);

  if(m_mode == drawMode::QUAD){
    drawQuad();
  }
  else{
    drawGeo();
  }

}

void NGLScene::drawGeo()
{
  std::cout << "drawing geo" << std::endl;
  loadGeoDataToShaderVariables();
  switch(m_mode){
    case drawMode::TEAPOT :
      ngl::VAOPrimitives::instance()->draw("teapot");
      break;
    case drawMode::SPHERE :
      ngl::VAOPrimitives::instance()->draw("sphere");
      break;
  }
}

void NGLScene::drawQuad()
{
  // create new identity matrix
  ngl::Mat4 identityMat4;
  ngl::Mat3 identityMat3;
  // should be identity automatically
  //MVP.identity();

  ShaderVariables::instance()->MVP = identityMat4;

  // bind and draw our quad
  glBindVertexArray(m_vaoID);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void NGLScene::loadGeoDataToShaderVariables()
{
  /*
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_mouseGlobalTX;
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MV",MV);
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
  shader->setShaderParamFromMat4("M",M);
  */

  //ngl::Mat4 MV = M * m_cam.getViewMatrix();
  //ngl::Mat4 MVP = M * m_cam.getVPMatrix();
  //ngl::Mat3 normalMatrix = MV;
  //normalMatrix.inverse();

  ngl::Mat4 MVP;

  MVP = m_transform.getMatrix() * m_view * m_project;

  std::cout << MVP << std::endl;


  ShaderVariables::instance()->MVP = MVP;
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




//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  //if(_event->button() == Qt::LeftButton){
    m_mouseData[2] = _event->x();
    m_mouseData[3] = m_height-_event->y();
    m_mouseDown = true;
    m_oldYRotation = m_transform.getRotation()[1];
  //}
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  //if(_event->button() == Qt::LeftButton){
    m_mouseData[2] *= -1;
    m_mouseData[3] *= -1;
    m_mouseDown = false;
  //}
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

    // render wireframe or shaded, mostly for bugfixing
    case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
    case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;

    // toggle fullscreen because who has time for two buttons for this
    case Qt::Key_F : toggleFullScreen(); break;

    case Qt::Key_0 : m_mode = drawMode::QUAD; break;
    case Qt::Key_1 : m_mode = drawMode::TEAPOT; break;
    case Qt::Key_2 : m_mode = drawMode::SPHERE; break;
    //case Qt::Key_3 : setCurrentShader("dolphin"); break;
    //case Qt::Key_4 : setCurrentShader("new"); break;

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
  //ngl::ShaderLib *shaderLib=ngl::ShaderLib::instance();

  //getting seconds by dividing milliseconds by 1000
  float globalSeconds = m_time.elapsed()/1000.0;
  //shaderLib->setRegisteredUniform("iGlobalTime", globalSeconds);
  ShaderVariables::instance()->globalTime = globalSeconds;

  QDate date = QDate::currentDate();
  float dateYear = date.year();
  float dateMonth = date.month();
  float dateDay = date.day();
  float dateSeconds = (m_time.msecsSinceStartOfDay() + m_time.elapsed()) / 1000.0;
  //shaderLib->setRegisteredUniform("iDate", dateYear, dateMonth, dateDay, dateSeconds);
  ShaderVariables::instance()->date = ngl::Vec4(dateYear, dateMonth, dateDay, dateSeconds);

  if(m_mouseDown){
    QPoint p = this->mapFromGlobal(QCursor::pos());
    m_mouseData[0] = p.x();
    m_mouseData[1] = m_height-p.y();

    float rotation;

    rotation = m_mouseData[0] - m_mouseData[2];

    m_transform.setRotation(0, m_oldYRotation+rotation, 0);
  }
  //shaderLib->setRegisteredUniform("iMouse", m_mouseData);
  ShaderVariables::instance()->mouse = m_mouseData;

  update();
}

void NGLScene::toggleFullScreen()
{
  // complex fullscreen shaders can be expensive and slow, so be careful making window fullscreen (or just large)
  m_fullScreen ^= true;
  m_fullScreen ? showFullScreen() : showNormal();
}

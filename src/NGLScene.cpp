#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include "ngl/NGLInit.h"
#include "ShaderLibPro.h"
#include "ngl/VAOPrimitives.h"
#include <iostream>
#include "ngl/NGLStream.h"
#include <cstdlib>
#include "ShaderVariables.h"
#include <cmath>
#include "ngl/Random.h"

NGLScene::NGLScene() :
  m_fullScreen(false),
  m_mouseDown(false),
  m_time(QTime::currentTime()),
  m_lastFrameTime(0),
  m_mouseData(0,0,0,0),
  m_camPos(0, 15, 35),
  m_camLookHeight(10),
  m_cam(m_camPos , ngl::Vec3(0, m_camLookHeight, 0), ngl::Vec3::up()),
  m_areaSize(13),
  m_wallWidth(1),
  m_wallHeight(2),
  m_multiplayer(true),
  m_player1(ngl::Vec3(-m_areaSize/2, 0, 0), m_areaSize-m_wallWidth),
  m_player2(ngl::Vec3(m_areaSize/2, 0, 0), m_areaSize-m_wallWidth),
  m_player1Attack(false),
  m_player2Attack(false),
  m_floorDepth(100),
  m_floor(ngl::Vec3(0,-m_floorDepth/2,0), ngl::Vec3((m_areaSize+m_wallWidth)*2, m_floorDepth, (m_areaSize+m_wallWidth)*2)),
  m_matchTime(60),
  m_mode(MAIN)
{
  setTitle("Shader Splash");
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

  m_cam.setShape(45.0f,(float)width()/height(),0.05f,350.0f);
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
  // set resolution in shader
  // uses a Vec3 to be compatible with Shadertoy, even though resolution is (x,y)
  //ngl::ShaderLib::instance()->setRegisteredUniform("iResolution", ngl::Vec3(m_width, m_height, 1.0));
  ShaderVariables::instance()->resolution = ngl::Vec3(m_width, m_height, 1.0);

  m_cam.setShape(45.0f,(float)width()/height(),0.05f,350.0f);
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

  ShaderLibPro *shader = ShaderLibPro::instance();


  m_text.reset(new ngl::Text(QFont("Arial",31)));

  m_background.createQuad();


  ngl::VAOPrimitives::instance()->createCapsule("pill", 0.3, 0.6, 8);
  for(int i=0; i<3; i++)
  {
    ngl::Vec3 pos = ngl::Random::instance()->getRandomPoint(m_areaSize-m_wallWidth-1, 0, m_areaSize-m_wallWidth-1);
    m_pills.push_back(Pill(pos));
    m_pills[i].setOffset(ngl::Random::instance()->randomPositiveNumber());
    m_pills[i].setShaderIndex(1);
  }

  m_walls.push_back(Wall(ngl::Vec3(m_areaSize,0,m_wallWidth/2.0), ngl::Vec3(1,m_wallHeight,m_areaSize*2)));
  m_walls.push_back(Wall(ngl::Vec3(-m_areaSize,0,-m_wallWidth/2.0), ngl::Vec3(1,m_wallHeight,m_areaSize*2)));
  m_walls.push_back(Wall(ngl::Vec3(-m_wallWidth/2.0,0,m_areaSize), ngl::Vec3(m_areaSize*2,m_wallHeight,m_wallWidth)));
  m_walls.push_back(Wall(ngl::Vec3(m_wallWidth/2.0,0,-m_areaSize), ngl::Vec3(m_areaSize*2,m_wallHeight,m_wallWidth)));

  m_player1Ctrl = ngl::Vec3::zero();
  m_player2Ctrl = ngl::Vec3::zero();

  int minIndex = 0;
  int maxIndex = 0;
  shader->addShader("shaders/Background/background1.txt");
  shader->addShader("shaders/Background/background2.txt");
  shader->addShader("shaders/Background/background3.txt");
  shader->addShader("shaders/Background/background4.txt");
  shader->addShader("shaders/Background/background5.txt");
  maxIndex = shader->getShaderSetAmount() - 1;
  m_background.resetIndexRange(minIndex, maxIndex);
  m_background.setShaderIndex(minIndex);

  minIndex = maxIndex+1;
  shader->addShader("shaders/Player/hamster1.txt");
  maxIndex = shader->getShaderSetAmount() - 1;
  m_player1.resetIndexRange(minIndex, maxIndex);
  m_player2.resetIndexRange(minIndex, maxIndex);
  m_player1.setShaderIndex(minIndex);
  m_player2.setShaderIndex(minIndex);


  for(Wall &wall : m_walls)
  {
    wall.setShaderIndex(2);
  }
  m_floor.setShaderIndex(0);

  // pretty unneccessary to have 2 at the moment but could have two player models
  m_player1.loadMeshes("models/hamster.obj", "models/attack.obj");
  m_player2.loadMeshes("models/hamster.obj", "models/attack.obj");

  // set up timer loop
  startTimer(16);
}

void NGLScene::paintGL()
{
  // increase frame number variable for shader
  ShaderVariables::instance()->frame++;

  if(m_mode == MAIN)
  {
    drawScene();
  }

  m_text->setScreenSize(width()*devicePixelRatio(),height()*devicePixelRatio());
  m_text->setColour(1,1,0);
  QString text=QString("%1 : %2").arg(m_player1.getScore()).arg(m_player2.getScore());
  m_text->renderText(10,20,text);
  text=QString("%1").arg(m_matchTime-(m_time.elapsed()/1000));
  m_text->renderText(m_width-100,20,text);

  // calculate time taken to render the frame (time since last frame was rendered)
  float renderTime = (m_time.elapsed() - m_lastFrameTime) / 1000.0;
  //ngl::ShaderLib::instance()->setRegisteredUniform("iTimeDelta", renderTime);
  ShaderVariables::instance()->timeDelta = renderTime;

  m_lastFrameTime = m_time.elapsed();
}

// drawing scene takes programID
void NGLScene::drawScene()
{
  ShaderLibPro *shader = ShaderLibPro::instance();
  // clear the screen and depth buffer
  glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, m_width, m_height);

  // draw background quad with identity matrix
  shader->draw(&m_background, ngl::Mat4());
  // clear buffer so everything draws over background
  glClear(GL_DEPTH_BUFFER_BIT);

  ngl::Mat4 camVP = m_cam.getVPMatrix();
  for(Wall &wall : m_walls)
  {
    shader->draw(&wall, camVP);
  }
  shader->draw(&m_floor, camVP);

  for(Pill &pill: m_pills)
  {
    shader->draw(&pill, camVP);
  }
  shader->draw(&m_player1, camVP);
  shader->draw(&m_player2, camVP);
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  //if(_event->button() == Qt::LeftButton)
  //{
    m_mouseData[2] = _event->x()*devicePixelRatio();
    m_mouseData[3] = m_height-_event->y()*devicePixelRatio();
    m_mouseDown = true;
  //}
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  //if(_event->button() == Qt::LeftButton)
  //{
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

  if(!_event->isAutoRepeat())
  {
    switch(m_mode)
    {
      case TITLE :
      {

      }
      break;
      case MAIN :
      {
        switch (_event->key())
        {
          // escape key to quite
          case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;

          // render wireframe or shaded, mostly for bugfixing
          //case Qt::Key_Z : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
          //case Qt::Key_X : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;

          // toggle fullscreen because who has time for two buttons for this
          case Qt::Key_F : toggleFullScreen(); break;

          // player 1 controls
          case Qt::Key_A : m_player1Ctrl[0] -= 1; break;
          case Qt::Key_D: m_player1Ctrl[0] += 1; break;
          case Qt::Key_W : m_player1Ctrl[2] -= 1; break;
          case Qt::Key_S : m_player1Ctrl[2] += 1; break;
          case Qt::Key_Space : m_player1Attack = true; break;
          default : break;
        }
        if(m_multiplayer)
        {
          switch(_event->key())
          {
            // player 2 controls
            case Qt::Key_Left : m_player2Ctrl[0] -= 1; break;
            case Qt::Key_Right: m_player2Ctrl[0] += 1; break;
            case Qt::Key_Up : m_player2Ctrl[2] -= 1; break;
            case Qt::Key_Down : m_player2Ctrl[2] += 1; break;
            case Qt::Key_Shift : m_player2Attack = true; break;
            default : break;
          }
        }
      }
      break;
      case FINISHED :
      {

      }
      break;
      default :
      break;
    }
  }
}

void NGLScene::keyReleaseEvent(QKeyEvent *_event)
{
  if(m_mode == MAIN)
  {
    if(!_event->isAutoRepeat())
    {

      switch (_event->key())
      {
        // player 1 controls
        case Qt::Key_A : m_player1Ctrl[0] += 1; break;
        case Qt::Key_D: m_player1Ctrl[0] -= 1; break;
        case Qt::Key_W : m_player1Ctrl[2] += 1; break;
        case Qt::Key_S : m_player1Ctrl[2] -= 1; break;

        default : break;
      }

      if(m_multiplayer)
      {
        switch(_event->key())
        {
          // player 2 controls
          case Qt::Key_Left : m_player2Ctrl[0] += 1; break;
          case Qt::Key_Right: m_player2Ctrl[0] -= 1; break;
          case Qt::Key_Up : m_player2Ctrl[2] += 1; break;
          case Qt::Key_Down : m_player2Ctrl[2] -= 1; break;
          default : break;
        }
      }
    }
  }
}

void NGLScene::timerEvent(QTimerEvent *_event)
{

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

  if(m_mouseDown)
  {
    QPoint p = this->mapFromGlobal(QCursor::pos());
    m_mouseData[0] = p.x()*devicePixelRatio();
    m_mouseData[1] = m_height-(p.y()*devicePixelRatio());
  }

  ShaderVariables::instance()->mouse = m_mouseData;

  m_player1.update(m_player1Ctrl, m_player1Attack, &m_player2);

  if(m_multiplayer)
  {
    m_player2.update(m_player2Ctrl, m_player2Attack, &m_player1);
  }
  else
  {
    //m_player2.aiUpdate(&m_player1);
  }

  for(Pill &pill: m_pills)
  {
    if(pill.isAlive())
    {
      pill.update(&m_player1, &m_player2);
    }
    else
    {
      // pill is hit
      remixShaders();
      pill.reset(ngl::Random::instance()->getRandomPoint(m_areaSize-m_wallWidth-1, 0, m_areaSize-m_wallWidth-1));
      pill.setOffset(ngl::Random::instance()->randomPositiveNumber());
    }
  }

  updateCamera();

  m_player1Attack = false;
  m_player2Attack = false;

  update();
}

void NGLScene::toggleFullScreen()
{
  // complex fullscreen shaders can be expensive and slow, so be careful making window fullscreen (or just large)
  m_fullScreen ^= true;
  m_fullScreen ? showFullScreen() : showNormal();
}

void NGLScene::updateCamera()
{
  ngl::Vec3 look = (m_player1.getPos() + m_player2.getPos()) / 2.0;

  // camera follows players, but actually points inbetween players and origin (hence dividing), and is then offset to look down a bit
  look /= 3.0;
  look.m_z +=5;

  // look at ground, so bouncing player doesn't affect camera
  look.m_y = m_camLookHeight;

  float playerDist = (m_player1.getPos() - m_player2.getPos()).length();

  ngl::Vec3 eye = pow((playerDist/m_areaSize + 1.0), 0.3) * m_camPos;

  m_cam.set(eye, look, ngl::Vec3::up());
}

void NGLScene::remixShaders()
{
  m_background.randomiseShaderIndex();

//  int maxIndex = ShaderLibPro::instance()->getShaderSetAmount();

//  ngl::Random *rng = ngl::Random::instance();


//  int wallIndex = (int)rng->randomPositiveNumber(maxIndex);
//  for(Wall &wall : m_walls)
//  {
//    wall.setShaderIndex(wallIndex);
//  }
//  m_floor.setShaderIndex((int)rng->randomPositiveNumber(maxIndex));

//  int pillIndex = (int)rng->randomPositiveNumber(maxIndex);
//  for(Pill &pill: m_pills)
//  {
//    pill.setShaderIndex(pillIndex);
//  }
//  m_player1.setShaderIndex((int)rng->randomPositiveNumber(maxIndex));
//  m_player2.setShaderIndex((int)rng->randomPositiveNumber(maxIndex));
}

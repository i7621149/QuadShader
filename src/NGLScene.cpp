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
  m_camBounce(0),
  m_cam(m_camPos, ngl::Vec3(0, 7.5, 0), ngl::Vec3::up()),
  m_areaSize(13),
  m_wallWidth(1),
  m_wallHeight(2),
  m_multiplayer(false),
  m_remixStep(5),
  m_player1(1, ngl::Vec3(-m_areaSize/2, 0, 0), m_areaSize-m_wallWidth),
  m_player2(2, ngl::Vec3(m_areaSize/2, 0, 0), m_areaSize-m_wallWidth),
  m_player1Attack(false),
  m_player2Attack(false),
  m_floorDepth(1),
  m_floor(5, ngl::Vec3(0,-m_floorDepth/2,0), ngl::Vec3((m_areaSize)*2, m_floorDepth, (m_areaSize)*2)),
  m_matchTime(60),
  m_mode(TITLE),
  m_textHeight(32),
  m_camMode(CONTROLFLIP),
  m_normalShaderIndexes(),
  m_sinShaderIndexes()
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

  // set random seed
  ngl::Random::instance()->setSeed(m_time.msecsSinceStartOfDay());
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  ShaderLibPro *shader = ShaderLibPro::instance();


  m_text.reset(new ngl::Text(QFont("Ariel", 32, 100)));
  m_instructText.reset(new ngl::Text(QFont("Ariel", 18)));

  m_background.createQuad();

  m_walls.push_back(Block(4, ngl::Vec3(m_areaSize, -m_floorDepth/2.0, m_wallWidth/2.0), ngl::Vec3(1, m_wallHeight+m_floorDepth, m_areaSize*2)));
  m_walls.push_back(Block(4, ngl::Vec3(-m_areaSize, -m_floorDepth/2.0, -m_wallWidth/2.0), ngl::Vec3(1, m_wallHeight+m_floorDepth, m_areaSize*2)));
  m_walls.push_back(Block(4, ngl::Vec3(-m_wallWidth/2.0, -m_floorDepth/2.0, m_areaSize), ngl::Vec3(m_areaSize*2, m_wallHeight+m_floorDepth, m_wallWidth)));
  m_walls.push_back(Block(4, ngl::Vec3(m_wallWidth/2.0, -m_floorDepth/2.0, -m_areaSize), ngl::Vec3(m_areaSize*2, m_wallHeight+m_floorDepth, m_wallWidth)));

  m_player1Ctrl = ngl::Vec3::zero();
  m_player2Ctrl = ngl::Vec3::zero();

  // only load each mesh once for efficiency
  m_playerMesh.reset(new ngl::Obj("models/hamster.obj"));
  m_playerAttackMesh.reset(new ngl::Obj("models/attack.obj"));
  m_pillMesh.reset(new ngl::Obj("models/pill.obj"));
  m_playerMesh->createVAO();
  m_playerAttackMesh->createVAO();
  m_pillMesh->createVAO();

  m_boxMesh.reset(new ngl::Obj("models/box.obj"));
  m_boxMesh->createVAO();

  m_player1.loadMeshes(m_playerMesh.get(), m_playerAttackMesh.get());
  m_player2.loadMeshes(m_playerMesh.get(), m_playerAttackMesh.get());

  for(int i=0; i<3; i++)
  {
    ngl::Vec3 pos = ngl::Random::instance()->getRandomPoint(m_areaSize-m_wallWidth-1, 0, m_areaSize-m_wallWidth-1);
    m_pills.push_back(Pill(pos));
    m_pills[i].setOffset(ngl::Random::instance()->randomPositiveNumber());
    m_pills[i].loadMesh(m_pillMesh.get());
  }

  for(Block &wall : m_walls)
  {
    wall.loadMesh(m_boxMesh.get());
  }
  m_floor.loadMesh(m_boxMesh.get());

  int minIndex = 0;
  int maxIndex = 0;
  shader->addShader("shaders/Background/background01.txt");
  shader->addShader("shaders/Background/background02.txt");
  shader->addShader("shaders/Background/background03.txt");
  shader->addShader("shaders/Background/background04.txt");
  shader->addShader("shaders/Background/background05.txt");
  shader->addShader("shaders/Background/background06.txt");
  shader->addShader("shaders/Background/background07.txt");
  shader->addShader("shaders/Background/background08.txt");
  shader->addShader("shaders/Background/background09.txt");
  shader->addShader("shaders/Background/background10.txt");
  shader->addShader("shaders/Background/background11.txt");
  shader->addShader("shaders/Background/background12.txt");
  shader->addShader("shaders/Background/background13.txt");
  shader->addShader("shaders/Background/background14.txt");
  shader->addShader("shaders/Background/background15.txt");
  shader->addShader("shaders/Background/background16.txt");
  shader->addShader("shaders/Background/background17.txt");
  shader->addShader("shaders/Background/background18.txt");
  shader->addShader("shaders/Background/background19.txt");
  shader->addShader("shaders/Background/background20.txt");
  maxIndex = shader->getShaderSetAmount() - 1;
  m_background.resetIndexRange(minIndex, maxIndex);
  m_background.setCurrentIndex(0);

  minIndex = maxIndex + 1;
  shader->addShader("shaders/Player/hamster01.txt");
  shader->addShader("shaders/Player/hamster02.txt");
  shader->addShader("shaders/Player/hamster03.txt");
  shader->addShader("shaders/Player/hamster04.txt");
  shader->addShader("shaders/Player/hamster05.txt");
  shader->addShader("shaders/Player/hamster06.txt");
  shader->addShader("shaders/Player/hamster07.txt");
  shader->addShader("shaders/Player/hamster08.txt");
  shader->addShader("shaders/Player/hamster09.txt");
  shader->addShader("shaders/Player/hamster10.txt");
  maxIndex = shader->getShaderSetAmount() - 1;
  m_player1.resetIndexRange(minIndex, maxIndex);
  m_player2.resetIndexRange(minIndex, maxIndex);
  m_player1.setCurrentIndex(0);
  m_player2.setCurrentIndex(0);

  minIndex = maxIndex + 1;
  shader->addShader("shaders/Pill/pill01.txt");
  shader->addShader("shaders/Pill/pill02.txt");
  shader->addShader("shaders/Pill/pill03.txt");
  shader->addShader("shaders/Pill/pill04.txt");
  shader->addShader("shaders/Pill/pill05.txt");
  shader->addShader("shaders/Pill/pill06.txt");
  shader->addShader("shaders/Pill/pill07.txt");
  shader->addShader("shaders/Pill/pill08.txt");
  shader->addShader("shaders/Pill/pill09.txt");
  shader->addShader("shaders/Pill/pill10.txt");
  maxIndex = shader->getShaderSetAmount() - 1;
  for(Pill &pill : m_pills)
  {
    pill.resetIndexRange(minIndex, maxIndex);
    pill.setCurrentIndex(0);
  }

  minIndex = maxIndex +1;
  shader->addShader("shaders/Block/block01.txt");
  shader->addShader("shaders/Block/block02.txt");
  shader->addShader("shaders/Block/block03.txt");
  shader->addShader("shaders/Block/block04.txt");
  shader->addShader("shaders/Block/block05.txt");
  shader->addShader("shaders/Block/block06.txt");
  shader->addShader("shaders/Block/block07.txt");
  shader->addShader("shaders/Block/block08.txt");
  shader->addShader("shaders/Block/block09.txt");
  shader->addShader("shaders/Block/block10.txt");
  maxIndex = shader->getShaderSetAmount() - 1;
  for(Block &wall : m_walls)
  {
    wall.resetIndexRange(minIndex, maxIndex);
    wall.setCurrentIndex(0);
  }
  m_floor.resetIndexRange(minIndex, maxIndex);
  m_floor.setCurrentIndex(0);

  m_normalShaderIndexes = {0, 2, 4, 6, 8, 9};
  m_sinShaderIndexes = {1, 3, 5, 7};

  m_backgroundShaderNum = 20;
  m_geoShaderNum = m_normalShaderIndexes.size() + m_sinShaderIndexes.size();

  // set up timer loop
  startTimer(16);
}

void NGLScene::paintGL()
{
  ShaderLibPro *shader = ShaderLibPro::instance();
  // increase frame number variable for shader
  ShaderVariables::instance()->frame++;
  // clear the screen and depth buffer
  glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, m_width, m_height);


  if(m_mode == MAIN)
  {
    drawScene();
  }

  // draw background quad
  shader->draw(&m_background, nullptr);

  renderText(ngl::Vec2(2, 2), ngl::Colour(0.0, 0.0, 0.0));
  renderText(ngl::Vec2(0, 0), ngl::Colour(1.0, 1.0, 0.0));

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

  for(Block &wall : m_walls)
  {
    shader->draw(&wall, &m_cam);
  }
  shader->draw(&m_floor, &m_cam);

  for(Pill &pill: m_pills)
  {
    shader->draw(&pill, &m_cam);
  }
  shader->draw(&m_player1, &m_cam);

  if(m_multiplayer)
  {
    shader->draw(&m_player2, &m_cam);
  }
}

void NGLScene::renderText(ngl::Vec2 _startPos, ngl::Colour _col)
{
  QString text;

  ngl::Vec2 textPos = _startPos + ngl::Vec2(30, 30);

  m_text->setScreenSize(width()*devicePixelRatio(), height()*devicePixelRatio());
  m_text->setColour(_col);

  switch(m_mode)
  {
    case MAIN :
      text=QString("SCORE");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_x += m_width-180;
      text=QString("TIME");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_x -= m_width-180;
      textPos.m_y += m_textHeight + 10;
      if(m_multiplayer)
      {
        text=QString("%1 : %2").arg(m_player1.getScore()).arg(m_player2.getScore());
      }
      else
      {
        text=QString("%1!").arg(m_player1.getScore());
      }
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_x += m_width-180;
      text=QString("%1%2").arg(m_matchTime-(m_time.elapsed()/1000)).arg(m_matchTime-(m_time.elapsed()/1000) > 9 ? "" : "!");
      m_text->renderText(textPos.m_x, textPos.m_y, text);
    break;
    case TITLE :
      text=QString("SHADER SMASH");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30;
      text=QString("Someone left some pills in the hamster cage!");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 5;
      text=QString("Play as a hamster and eat as many as you can!");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 5;
      text=QString("You've got 60 seconds to grab those pills!");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 5;
      text=QString("Player1:");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_x += 20;
      textPos.m_y += m_textHeight + 5;
      text=QString("WASD: move");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 5;
      text=QString("Space: attack");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_x -= 20;
      textPos.m_y += m_textHeight + 5;
      text=QString("Player2:");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_x += 20;
      textPos.m_y += m_textHeight + 5;
      text=QString("Arrow Keys: move");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 5;
      text=QString("Shift: attack");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 10;
      text=QString("PLAY?");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30;
      text=QString("SPACE: SINGLE PLAYER");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30;
      text=QString("SHIFT: TWO PLAYER");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 20;
      text=QString("Attacking will also let you grab farther away pills!");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
    break;
    case FINISHED :
      textPos.m_x = 60;
      textPos.m_y += 20;
      text=QString("FINAL SCORE");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30;
      if(m_multiplayer)
      {
        text=QString("%1 : %2").arg(m_player1.getScore()).arg(m_player2.getScore());
      }
      else
      {
        text=QString("%1!").arg(m_player1.getScore());
      }
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 100;
      text=QString("PLAY AGAIN?");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30;
      text=QString("SPACE: SINGLE PLAYER");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30;
      text=QString("SHIFT: TWO PLAYER");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 100;
      text=QString("Remember:");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 10;
      text=QString("Attacking will also let you grab farther away pills!");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);

    break;
    default : break;
  }

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
      case MAIN :
      {
        switch (_event->key())
        {
          // escape key to quite
          case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;

          // render wireframe or shaded, mostly for bugfixing
          // wireframe makes things fast and look broken
          case Qt::Key_Z : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
          case Qt::Key_X : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;

          // toggle fullscreen because who has time for two buttons for this
          // can make things very slow
          case Qt::Key_F11 : toggleFullScreen(); break;

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
      case TITLE :
      case FINISHED :
        switch (_event->key())
        {
          // escape key to quite
          case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;

          // render wireframe or shaded, mostly for bugfixing
          // wireframe makes things fast and look broken
          case Qt::Key_F9 : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
          case Qt::Key_F10 : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;

          // toggle fullscreen because who has time for two buttons for this
          // can make things very slow
          case Qt::Key_F11 : toggleFullScreen(); break;
          case Qt::Key_Space :
            m_multiplayer = false;
            m_player1Ctrl = ngl::Vec3::zero();
            m_player2Ctrl = ngl::Vec3::zero();
            startGame();
          break;
          case Qt::Key_Shift :
            m_multiplayer = true;
            m_player1Ctrl = ngl::Vec3::zero();
            m_player2Ctrl = ngl::Vec3::zero();
            startGame();
          default : break;
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
      if(m_multiplayer)
      {
        pill.update(&m_player1, &m_player2);
      }
      else
      {
        pill.update(&m_player1, nullptr);
      }
    }
    else
    {
      // pill is hit
      int totalScore = m_player1.getScore()+m_player2.getScore();
      if(totalScore % m_remixStep == 0)
      {
        remixShaders();
      }

      pill.reset(ngl::Random::instance()->getRandomPoint(m_areaSize-m_wallWidth-1, 0, m_areaSize-m_wallWidth-1));
      pill.setOffset(ngl::Random::instance()->randomPositiveNumber());
    }
  }

  // only update camera after collecting inital pills
  if(m_player1.getScore()+m_player2.getScore() >= m_remixStep)
  {
    updateCamera();
  }

  m_player1Attack = false;
  m_player2Attack = false;

  m_camBounce /= 1.09;

  if(m_mode == MAIN && m_time.elapsed()/1000 >= m_matchTime)
  {
    m_mode = FINISHED;
  }

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
  float playerDist = (m_player1.getPos() - m_player2.getPos()).length();
  ngl::Vec3 eye = pow((playerDist/m_areaSize + 1.0), 0.2) * m_camPos;
  eye += ngl::Random::instance()->getRandomNormalizedVec3() * m_camBounce*1.5;

  ngl::Vec3 look = (m_player1.getPos() + m_player2.getPos()) / 2.0;
  // camera follows players, but actually points inbetween players and origin (hence dividing), and is then offset to look down a bit
  look /= 3.0;
  look.m_z +=5;
  // look towards ground
  look.m_y = (eye.m_y) / 2;

  ngl::Vec3 wiggle;
  switch(m_camMode)
  {
    case NORMAL :
      m_cam.set(eye, look, ngl::Vec3::up());
    break;
    case FLIPPED :
      // actually sets camera to other side
      m_cam.set(eye.reflect(ngl::Vec3(0,0,-1)), look, -ngl::Vec3::up());
    break;
    case RANDOM :
      wiggle = ngl::Random::instance()->getRandomNormalizedVec3() * 0.03;
      m_cam.set(eye, look + wiggle, ngl::Vec3::up());
    break;
    case CONTROLFLIP :
      m_cam.set(eye.reflect(ngl::Vec3(0,0,-1)), look, ngl::Vec3::up());
    break;
    case ROLL :
      m_cam.set(eye, look, ngl::Vec3::up());
      m_cam.roll(30*sin(m_time.elapsed()/1000.0));
    break;
    default : break;
  }
}

void NGLScene::remixShaders()
{
  ngl::Random *rng = ngl::Random::instance();
  // screen shake
  m_camBounce = 1.0;
  // generate random shader index for background, not 0
  int backgroundShaderIndex = (int)rng->randomPositiveNumber(m_backgroundShaderNum-1) + 1;
  m_background.setCurrentIndex(backgroundShaderIndex);

  int normOrSin = (int)rng->randomPositiveNumber(2);
  int player1ShaderIndex;
  int player2ShaderIndex;
  int pillShaderIndex;
  int blockShaderIndex;

  if(normOrSin == 0)
  {
    // normal shader
    player1ShaderIndex = m_normalShaderIndexes[(int)rng->randomPositiveNumber(m_normalShaderIndexes.size())];
    player2ShaderIndex = m_normalShaderIndexes[(int)rng->randomPositiveNumber(m_normalShaderIndexes.size())];
    pillShaderIndex = m_normalShaderIndexes[(int)rng->randomPositiveNumber(m_normalShaderIndexes.size())];
    blockShaderIndex = m_normalShaderIndexes[(int)rng->randomPositiveNumber(m_normalShaderIndexes.size())];
  }
  else
  {
    // sin shader
    player1ShaderIndex = m_sinShaderIndexes[(int)rng->randomPositiveNumber(m_sinShaderIndexes.size())];
    player2ShaderIndex = m_sinShaderIndexes[(int)rng->randomPositiveNumber(m_sinShaderIndexes.size())];
    pillShaderIndex = m_sinShaderIndexes[(int)rng->randomPositiveNumber(m_sinShaderIndexes.size())];
    blockShaderIndex = m_sinShaderIndexes[(int)rng->randomPositiveNumber(m_sinShaderIndexes.size())];
  }

  m_player1.setCurrentIndex(player1ShaderIndex);
  m_player2.setCurrentIndex(player2ShaderIndex);

  for(Pill &pill : m_pills)
  {
    pill.setCurrentIndex(pillShaderIndex);
  }

  for(Block &wall : m_walls)
  {
    wall.setCurrentIndex(blockShaderIndex);
  }
  m_floor.setCurrentIndex(blockShaderIndex);

  int camMode = (int)ngl::Random::instance()->randomPositiveNumber(100);
  if(camMode < 40)
  {
    m_camMode = NORMAL;
  }
  else if(camMode < 60)
  {
    m_camMode = FLIPPED;
  }
  else if(camMode < 70)
  {
    m_camMode = RANDOM;
  }
  else if(camMode < 80)
  {
    m_camMode = CONTROLFLIP;
  }
  else
  {
    m_camMode = ROLL;
  }
}

void NGLScene::startGame()
{
  // cooldown on end screen so that it isn't just skipped immediately if the player is spamming attack at the end
  if(m_mode == TITLE || m_time.elapsed()/1000 > m_matchTime + 2)
  {
    m_time.restart();

    m_player1.setPos(ngl::Vec3(-m_areaSize/2, 0, 0));
    m_player2.setPos(ngl::Vec3(m_areaSize/2, 0, 0));

    m_player1.setScore(0);
    m_player2.setScore(0);

    m_player1Ctrl = ngl::Vec3::zero();
    m_player2Ctrl = ngl::Vec3::zero();
    m_mode = MAIN;

    m_background.setCurrentIndex(0);
    m_player1.setCurrentIndex(0);
    m_player2.setCurrentIndex(0);

    for(Pill &pill : m_pills)
    {
      pill.setCurrentIndex(0);
    }

    for(Block &wall : m_walls)
    {
      wall.setCurrentIndex(0);
    }
    m_floor.setCurrentIndex(0);

    m_camMode = NORMAL;
  }
}

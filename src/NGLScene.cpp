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
  m_player1(1, ngl::Vec3(0.0,13.7,27.0), m_areaSize-m_wallWidth),
  m_player2(2, ngl::Vec3(0.0,12.5,27.0), m_areaSize-m_wallWidth),
  m_floorDepth(1),
  m_floor(5, ngl::Vec3(0,-m_floorDepth/2,0), ngl::Vec3((m_areaSize)*2, m_floorDepth, (m_areaSize)*2)),
  m_matchTime(60),
  m_mode(TITLE),
  m_textHeight(32*devicePixelRatio()),
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
  ShaderVariables::instance()->resolution = ngl::Vec3(m_width, m_height, 1.0);

  m_cam.setShape(45.0f,(float)width()/height(),0.05f,350.0f);
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
  // set resolution in shader
  // uses a Vec3 to be compatible with Shadertoy
  ShaderVariables::instance()->resolution = ngl::Vec3(m_width, m_height, 1.0);

  m_cam.setShape(45.0f,(float)width()/height(),0.05f,350.0f);
}

void NGLScene::initializeGL()
{
  // init ngl
  ngl::NGLInit::instance();

  // set random seed
  ngl::Random::instance()->setSeed(m_time.msecsSinceStartOfDay());
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  ShaderLibPro *shader = ShaderLibPro::instance();

  m_text.reset(new ngl::Text(QFont("Ariel", m_textHeight, 100)));
  m_instructText.reset(new ngl::Text(QFont("Ariel", m_textHeight/2)));

  // create background quad
  m_background.createQuad();

  // "place" walls
  m_walls.push_back(Block(4, ngl::Vec3(m_areaSize, -m_floorDepth/2.0, m_wallWidth/2.0), ngl::Vec3(1, m_wallHeight+m_floorDepth, m_areaSize*2)));
  m_walls.push_back(Block(4, ngl::Vec3(-m_areaSize, -m_floorDepth/2.0, -m_wallWidth/2.0), ngl::Vec3(1, m_wallHeight+m_floorDepth, m_areaSize*2)));
  m_walls.push_back(Block(4, ngl::Vec3(-m_wallWidth/2.0, -m_floorDepth/2.0, m_areaSize), ngl::Vec3(m_areaSize*2, m_wallHeight+m_floorDepth, m_wallWidth)));
  m_walls.push_back(Block(4, ngl::Vec3(m_wallWidth/2.0, -m_floorDepth/2.0, -m_areaSize), ngl::Vec3(m_areaSize*2, m_wallHeight+m_floorDepth, m_wallWidth)));

  // only load each mesh once for efficiency
  m_playerMesh.reset(new ngl::Obj("models/hamster.obj"));
  m_playerAttackMesh.reset(new ngl::Obj("models/attack.obj"));
  m_pillMesh.reset(new ngl::Obj("models/pill.obj"));
  m_playerMesh->createVAO();
  m_playerAttackMesh->createVAO();
  m_pillMesh->createVAO();

  // load box mesh
  // used over ngl's cube since i needed it to deform with a sin wave, so wanted something with subdivisions
  m_boxMesh.reset(new ngl::Obj("models/box.obj"));
  m_boxMesh->createVAO();

  // give meshes to players
  m_player1.loadMeshes(m_playerMesh.get(), m_playerAttackMesh.get());
  m_player2.loadMeshes(m_playerMesh.get(), m_playerAttackMesh.get());

  for(int i=0; i<3; i++)
  {
    ngl::Vec3 pos = ngl::Random::instance()->getRandomPoint(m_areaSize-m_wallWidth-1, 0, m_areaSize-m_wallWidth-1);
    m_pills.push_back(Pill(pos));
    m_pills[i].setOffset(ngl::Random::instance()->randomPositiveNumber());
    m_pills[i].loadMesh(m_pillMesh.get());
  }

  // give meshes to walls
  for(Block &wall : m_walls)
  {
    wall.loadMesh(m_boxMesh.get());
  }
  m_floor.loadMesh(m_boxMesh.get());

  int minIndex = 0;
  int maxIndex = 0;


  // background shaders
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
  m_backgroundShaderNum = shader->getShaderSetAmount();


  // player shaders
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


  // pill shaders
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


  // block shaders
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

  // geometry shaders should match up so that if the floor is a sin wave, the geometry sits on top rather than clipping through
  // would be better to calculate this somehow but this way works well enough and i have the freedom to add more of either
  m_normalShaderIndexes = {0, 2, 4, 6, 8, 9};
  m_sinShaderIndexes = {1, 3, 5, 7};

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
  else if(m_mode == TITLE)
  {
    // draw players on title screen
    m_player1.setRot(ngl::Vec3(0, m_time.elapsed()/10, 0));
    m_player2.setRot(ngl::Vec3(0, m_time.elapsed()/10, 0));
    shader->draw(&m_player1, &m_cam);
    shader->draw(&m_player2, &m_cam);
  }

  // draw background quad, after geo for efficiency
  // takes a null pointer for camera, which means it will be drawn without transforms
  shader->draw(&m_background, nullptr);

  // render text twice, once for shadow in black and once for yellow
  // this shows up on pretty much everything (like complex shaders), even if it's a bit yucky
  renderText(ngl::Vec2(2, 2), ngl::Colour(0.0, 0.0, 0.0));
  renderText(ngl::Vec2(0, 0), ngl::Colour(1.0, 1.0, 0.0));

  // calculate time taken to render the frame (time since last frame was rendered)
  float renderTime = (m_time.elapsed() - m_lastFrameTime) / 1000.0;
  ShaderVariables::instance()->timeDelta = renderTime;

  m_lastFrameTime = m_time.elapsed();
}

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

  // use device pixel ratio a lot to decide position
  float dpr = devicePixelRatio();

  // works by creating a textPos and incrementing that
  // this means that adding text is relatively simple, even in the middle of the block of text
  // results in slightly uglier code, but it's clear and can be ignored for the rest of the program
  ngl::Vec2 textPos = _startPos + ngl::Vec2(30*dpr, 30*dpr);

  m_text->setScreenSize(width()*devicePixelRatio(), height()*devicePixelRatio());
  m_text->setColour(_col);

  switch(m_mode)
  {
    case MAIN :
      text=QString("SCORE");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_x += m_width-180*dpr;
      text=QString("TIME");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_x -= m_width-180*dpr;
      textPos.m_y += m_textHeight + 10*dpr;
      if(m_multiplayer)
      {
        text=QString("%1 : %2").arg(m_player1.getScore()).arg(m_player2.getScore());
      }
      else
      {
        text=QString("%1!").arg(m_player1.getScore());
      }
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_x += m_width-180*dpr;
      text=QString("%1%2").arg(m_matchTime-(m_time.elapsed()/1000)).arg(m_matchTime-(m_time.elapsed()/1000) > 9 ? "" : "!");
      m_text->renderText(textPos.m_x, textPos.m_y, text);
    break;
    case TITLE :
      text=QString("SHADER SMASH");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30*dpr;
      text=QString("Someone left some pills in the hamster cage!");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 5*dpr;
      text=QString("Play as a hamster and eat as many as you can!");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 5*dpr;
      text=QString("You've got 60 seconds to grab those pills!");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 5*dpr;
      text=QString("Player1:");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_x += 20*dpr;
      textPos.m_y += m_textHeight + 5*dpr;
      text=QString("WASD: move");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 5*dpr;
      text=QString("Space: attack");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_x -= 20*dpr;
      textPos.m_y += m_textHeight + 5*dpr;
      text=QString("Player2:");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_x += 20*dpr;
      textPos.m_y += m_textHeight + 5*dpr;
      text=QString("Arrow Keys: move");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 5*dpr;
      text=QString("Shift: attack");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 10*dpr;
      text=QString("PLAY?");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30*dpr;
      text=QString("SPACE: SINGLE PLAYER");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30*dpr;
      text=QString("SHIFT: TWO PLAYER");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 20*dpr;
      text=QString("Attacking will also let you grab farther away pills!");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
    break;
    case FINISHED :
      textPos.m_x = 60*dpr;
      textPos.m_y += 20*dpr;
      text=QString("WHAT A TRIP!");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30*dpr;
      text=QString("FINAL SCORE");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30*dpr;
      if(m_multiplayer)
      {
        text=QString("%1 : %2").arg(m_player1.getScore()).arg(m_player2.getScore());
      }
      else
      {
        text=QString("%1!").arg(m_player1.getScore());
      }
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 100*dpr;
      text=QString("PLAY AGAIN?");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30*dpr;
      text=QString("SPACE: SINGLE PLAYER");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 30*dpr;
      text=QString("SHIFT: TWO PLAYER");
      m_text->renderText(textPos.m_x, textPos.m_y, text);

      textPos.m_y += m_textHeight + 100*dpr;
      text=QString("Remember:");
      m_instructText->renderText(textPos.m_x, textPos.m_y, text);
      textPos.m_y += m_textHeight + 10*dpr;
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
  // mouse reading reverse engineered from shadertoy
  m_mouseData[2] = _event->x()*devicePixelRatio();
  m_mouseData[3] = m_height-_event->y()*devicePixelRatio();
  m_mouseDown = true;
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // mouse reading reverse engineered from shadertoy
  m_mouseData[2] *= -1;
  m_mouseData[3] *= -1;
  m_mouseDown = false;
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
          case Qt::Key_A : m_player1.m_control.left=true; break;
          case Qt::Key_D: m_player1.m_control.right=true; break;
          case Qt::Key_W : m_player1.m_control.up=true; break;
          case Qt::Key_S : m_player1.m_control.down=true; break;
          case Qt::Key_Space : m_player1.m_control.attack=true; break;

          default : break;
        }
        if(m_multiplayer)
        {
          switch(_event->key())
          {
            // player 2 controls
            case Qt::Key_Left : m_player2.m_control.left=true; break;
            case Qt::Key_Right: m_player2.m_control.right=true; break;
            case Qt::Key_Up : m_player2.m_control.up=true; break;
            case Qt::Key_Down :m_player2.m_control.down=true; break;
            case Qt::Key_Shift : m_player2.m_control.attack=true; break;

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
          // wireframe makes things play faster and look broken
          case Qt::Key_F9 : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
          case Qt::Key_F10 : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;

          // toggle fullscreen because who has time for two buttons for this
          // can make things very slow especially with complex quad shaders in background. use with caution
          case Qt::Key_F11 : toggleFullScreen(); break;

          // start single or multiplayer game
          case Qt::Key_Space :
            m_multiplayer = false;
            startGame();
          break;
          case Qt::Key_Shift :
            m_multiplayer = true;
            startGame();
          default : break;
        }
      break;

      default : break;
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
        case Qt::Key_A :m_player1.m_control.left=false; break;
        case Qt::Key_D: m_player1.m_control.right=false; break;
        case Qt::Key_W : m_player1.m_control.up=false; break;
        case Qt::Key_S : m_player1.m_control.down=false; break;

        default : break;
      }

      if(m_multiplayer)
      {
        switch(_event->key())
        {
          // player 2 controls
          case Qt::Key_Left : m_player2.m_control.left=false; break;
          case Qt::Key_Right: m_player2.m_control.right=false; break;
          case Qt::Key_Up : m_player2.m_control.up=false; break;
          case Qt::Key_Down : m_player2.m_control.down=false; break;

          default : break;
        }
      }
    }
  }
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
  // set shader variables:
  // time, date and mouse

  //getting seconds by dividing milliseconds by 1000
  float globalSeconds = m_time.elapsed()/1000.0;
  ShaderVariables::instance()->globalTime = globalSeconds;

  QDate date = QDate::currentDate();
  float dateYear = date.year();
  float dateMonth = date.month();
  float dateDay = date.day();
  float dateSeconds = (m_time.msecsSinceStartOfDay() + m_time.elapsed()) / 1000.0;
  ShaderVariables::instance()->date = ngl::Vec4(dateYear, dateMonth, dateDay, dateSeconds);

  // update mouse position, again used from shadertoy
  // might be better to only do on mouse move
  if(m_mouseDown)
  {
    QPoint p = this->mapFromGlobal(QCursor::pos());
    m_mouseData[0] = p.x()*devicePixelRatio();
    m_mouseData[1] = m_height-(p.y()*devicePixelRatio());
  }
  ShaderVariables::instance()->mouse = m_mouseData;


  // if mode is main, update players
  if(m_mode == MAIN)
  {
    m_player1.update(&m_player2);

    if(m_multiplayer)
    {
      m_player2.update(&m_player1);
    }

    // update pills and check for hits
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

    // this is a quick and easy way to reduce screen shake over time
    m_camBounce /= 1.09;
  }

  // finish game when time is up
  if(m_mode == MAIN && m_time.elapsed()/1000 >= m_matchTime)
  {
    m_mode = FINISHED;
  }

  // draw
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
  // calculate where camera should look
  // it's relatively dynamic and follows the two players
  float playerDist = (m_player1.getPos() - m_player2.getPos()).length();
  ngl::Vec3 eye = pow((playerDist/m_areaSize + 1.0), 0.2) * m_camPos;

  // add screen shake, eg on pill pickup
  eye += ngl::Random::instance()->getRandomNormalizedVec3() * m_camBounce*1.5;

  // camera follows players, but actually points inbetween players and origin (hence dividing)
  // this keeps it from being too confusing/dynamic
  ngl::Vec3 look;

  if(m_player1.getScore()+m_player2.getScore() >= m_remixStep)
  {
    if(m_multiplayer)
    {
      // for multiplayer it follows average position of players
      look = (m_player1.getPos() + m_player2.getPos()) / 2.0;
    }
    else
    {
      look = m_player1.getPos();
    }
  }
  // camera follows player but this reduces effect
  look /= 3.0;

  // look towards ground
  look.m_y = eye.m_y / 2;

  // wiggle is used for random wiggle in RANDOM mode
  ngl::Vec3 wiggle;
  // use camMode to add variety to shaders
  // only one is active at a time to avoid reverse controls and flipped camera at the same time for example
  // adds some variety to visuals aside from shaders, as well as changing gameplay slightly
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
      wiggle = ngl::Random::instance()->getRandomNormalizedVec3() * 0.06;
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

  // decide whether to choose a normal or sin shaders
  int normOrSin = (int)rng->randomPositiveNumber(2);
  int player1ShaderIndex;
  int player2ShaderIndex;
  int pillShaderIndex;
  int blockShaderIndex;

  // then generate corresponding random numbers
  if(normOrSin == 0)
  {
    // normal shaders
    player1ShaderIndex = m_normalShaderIndexes[(int)rng->randomPositiveNumber(m_normalShaderIndexes.size())];
    player2ShaderIndex = m_normalShaderIndexes[(int)rng->randomPositiveNumber(m_normalShaderIndexes.size())];
    pillShaderIndex = m_normalShaderIndexes[(int)rng->randomPositiveNumber(m_normalShaderIndexes.size())];
    blockShaderIndex = m_normalShaderIndexes[(int)rng->randomPositiveNumber(m_normalShaderIndexes.size())];
  }
  else
  {
    // sin shaders
    player1ShaderIndex = m_sinShaderIndexes[(int)rng->randomPositiveNumber(m_sinShaderIndexes.size())];
    player2ShaderIndex = m_sinShaderIndexes[(int)rng->randomPositiveNumber(m_sinShaderIndexes.size())];
    pillShaderIndex = m_sinShaderIndexes[(int)rng->randomPositiveNumber(m_sinShaderIndexes.size())];
    blockShaderIndex = m_sinShaderIndexes[(int)rng->randomPositiveNumber(m_sinShaderIndexes.size())];
  }

  // then set shaders generated

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

  // randomly set the camera mode as well

  int camMode = (int)ngl::Random::instance()->randomPositiveNumber(100);
  if(camMode < 30)
  {
    m_camMode = NORMAL;
  }
  else if(camMode < 50)
  {
    m_camMode = FLIPPED;
  }
  else if(camMode < 65)
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
  // if on title || cooldown on end screen so that it isn't just skipped immediately if the player is spamming attack at the end
  if(m_mode == TITLE || m_time.elapsed()/1000 > m_matchTime + 1)
  {
    // reset everything proper

    m_time = QTime::currentTime();

    m_player1.reset(ngl::Vec3(-m_areaSize/2, 0, 0));
    m_player2.reset(ngl::Vec3(m_areaSize/2, 0, 0));

    m_mode = MAIN;

    m_background.setCurrentIndex(0);
    m_player1.setCurrentIndex(0);
    m_player2.setCurrentIndex(0);

    for(Pill &pill : m_pills)
    {
      pill.reset(ngl::Random::instance()->getRandomPoint(m_areaSize-m_wallWidth-1, 0, m_areaSize-m_wallWidth-1));
      pill.setOffset(ngl::Random::instance()->randomPositiveNumber());
      pill.setCurrentIndex(0);
    }

    for(Block &wall : m_walls)
    {
      wall.setCurrentIndex(0);
    }
    m_floor.setCurrentIndex(0);

    m_camMode = NORMAL;

    updateCamera();
  }
}

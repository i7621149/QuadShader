#ifndef NGLSCENE_H__
#define NGLSCENE_H__
#include "ngl/Camera.h"
#include "ngl/Colour.h"
#include "ngl/Light.h"
#include "ngl/Transformation.h"
#include "ngl/Text.h"
#include <QOpenGLWindow>
#include <QOpenGLTexture>
#include <QTime>
#include <vector>
#include <memory>
#include "ngl/Transformation.h"
#include "Player.h"
#include "Pill.h"
#include "Block.h"
#include "Background.h"
#include <QTime>
#include <array>

//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------
class NGLScene : public QOpenGLWindow
{
public:
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief ctor for our NGL drawing class
  /// @param [in] parent the parent window to the class
  //----------------------------------------------------------------------------------------------------------------------
  NGLScene();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief dtor must close down ngl and release OpenGL resources
  //----------------------------------------------------------------------------------------------------------------------
  ~NGLScene();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the initialize class is called once when the window is created and we have a valid GL context
  /// use this to setup any default GL stuff
  //----------------------------------------------------------------------------------------------------------------------
  void initializeGL();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this is called everytime we want to draw the scene
  //----------------------------------------------------------------------------------------------------------------------
  void paintGL();

  void drawScene();

  void renderText(ngl::Vec2 _startPos, ngl::Colour _col);

private:
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this is called everytime we resize the window
  //----------------------------------------------------------------------------------------------------------------------
  // Qt 5.5.1 must have this implemented and uses it
  void resizeGL(QResizeEvent *_event);
  // Qt 5.x uses this instead! http://doc.qt.io/qt-5/qopenglwindow.html#resizeGL
  void resizeGL(int _w, int _h);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Qt Event called when a key is pressed
  /// @param [in] _event the Qt event to query for size etc
  //----------------------------------------------------------------------------------------------------------------------
  void keyPressEvent(QKeyEvent *_event);

  void keyReleaseEvent(QKeyEvent *_event);
  //----------------------------------------------------------------------------------------------------------------------
  void toggleFullScreen();

  /// @brief this method is called every time a mouse is moved
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mouseMoveEvent (QMouseEvent * _event );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is pressed
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mousePressEvent ( QMouseEvent *_event);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is released
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mouseReleaseEvent ( QMouseEvent *_event );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse wheel is moved
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void wheelEvent( QWheelEvent *_event);

  void timerEvent(QTimerEvent *_event);

  void updateCamera();

  void remixShaders();

  void createQuad();

  void startGame();

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief window width
  //----------------------------------------------------------------------------------------------------------------------
  int m_width;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief window height
  //----------------------------------------------------------------------------------------------------------------------
  int m_height;

  GLuint m_vaoID;

  bool m_fullScreen;

  bool m_mouseDown;

  QTime m_time;

  int m_lastFrameTime;

  ngl::Vec4 m_mouseData;

  ngl::Vec3 m_camPos;

  float m_camBounce;

  ngl::Camera m_cam;

  ngl::Transformation m_transform;

  float m_areaSize;
  float m_wallWidth;
  float m_wallHeight;

  bool m_multiplayer;

  int m_remixStep;

  Player m_player1;
  Player m_player2;

  ngl::Vec3 m_player1Ctrl;
  ngl::Vec3 m_player2Ctrl;

  bool m_player1Attack;
  bool m_player2Attack;

  float m_floorDepth;
  Block m_floor;

  std::vector<Pill> m_pills;

  std::vector<Block> m_walls;

  Background m_background;

  std::unique_ptr<ngl::Text> m_text;
  std::unique_ptr<ngl::Text> m_instructText;

  int m_matchTime;

  enum GameMode {MAIN, TITLE, FINISHED};

  GameMode m_mode;

  int m_textHeight;

  std::unique_ptr<ngl::Obj> m_playerMesh;
  std::unique_ptr<ngl::Obj> m_playerAttackMesh;
  std::unique_ptr<ngl::Obj> m_pillMesh;
  std::unique_ptr<ngl::Obj> m_boxMesh;

  int m_geoShaderNum;

  bool m_tripping;
};

#endif

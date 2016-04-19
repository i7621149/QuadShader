/********************************************************************************************************
OpenGL setup, using QtGui and NGL, modified from http://qt-project.org/doc/qt-5.0/qtgui/openglwindow.html
*********************************************************************************************************/
#include <QtGui/QGuiApplication>
#include <QApplication>
#include <iostream>
#include "NGLScene.h"

int main(int argc, char **argv)
{
  // make an application
  QApplication app(argc, argv);


  // need to set format so create container to set info required
  QSurfaceFormat format;

  // setting samples for better quality image
  format.setSamples(4);
  // set gl version
  #if defined( __APPLE__)
    format.setMajorVersion(4);
    format.setMinorVersion(1);
  #else
    // latest version
    format.setMajorVersion(4);
    format.setMinorVersion(3);
  #endif
  // set to core only so that deprecated functions are avoided easily
  format.setProfile(QSurfaceFormat::CoreProfile);

  format.setDepthBufferSize(24);
  // creating context
  NGLScene window;
  // set format specified
  window.setFormat(format);

  // print version to check success
  std::cout << "Profile is " << format.majorVersion() << " " << format.minorVersion() << "\n";

  // resize and show the window
  window.resize(512, 288);
  window.show();

  // run the application
  return app.exec();
}

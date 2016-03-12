/****************************************************************************
basic OpenGL demo modified from http://qt-project.org/doc/qt-5.0/qtgui/openglwindow.html
****************************************************************************/
#include <QtGui/QGuiApplication>
#include <iostream>
#include "NGLScene.h"

int main(int argc, char **argv)
{
  //our application
  QGuiApplication app(argc, argv);
  //need to set format so create container to set info required
  QSurfaceFormat format;

  //setting samples for better quality image
  //set to 1 for testing
  format.setSamples(1);
  #if defined( __APPLE__)
    // at present mac osx Mountain Lion only supports GL3.2
    // the new mavericks will have GL 4.x so can change
    format.setMajorVersion(4);
    format.setMinorVersion(1);
  #else
    // with luck we have the latest GL version so set to this
    format.setMajorVersion(4);
    format.setMinorVersion(3);
  #endif
  //set to core only so that deprecated functions are avoided easily
  format.setProfile(QSurfaceFormat::CoreProfile);

  format.setDepthBufferSize(24);
  //creating context
  NGLScene window;
  //set format specified
  window.setFormat(format);


  //print version to check success
  std::cout<<"Profile is "<<format.majorVersion()<<" "<<format.minorVersion()<<"\n";

  //resize and show the window
  window.resize(500, 281);
  window.show();

  //run the application
  return app.exec();
}




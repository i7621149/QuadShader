#include "ShaderLibPro.h"

#include <QImage>
#include <sstream>
#include <fstream>

ShaderLibPro::ShaderLibPro() :
  m_shader(ngl::ShaderLib::instance())
{

}

ShaderLibPro::~ShaderLibPro()
{

}

void ShaderLibPro::newShaderProgram(const std::string &_progName, const std::string &_fragFile, const std::string &_vertFile)
{
  std::string fragShader = _progName + "Frag";
  std::string vertShader = _progName + "Vert";
  std::cout << fragShader << ", " << vertShader << std::endl;

  m_shader->createShaderProgram(_progName);

  m_shader->attachShader(vertShader, ngl::ShaderType::VERTEX);
  m_shader->attachShader(fragShader, ngl::ShaderType::FRAGMENT);

  // create shader from vert source and base, which includes version and layout in
  std::string vertShaderSource = loadShaderSource("shaders/BaseVertex.glsl") + loadShaderSource(_vertFile) + "\0";
  m_shader->loadShaderSourceFromString(vertShader, vertShaderSource);

  // create shader from frag source and base, which includes version and uniforms
  std::string fragShaderSource = loadShaderSource("shaders/BaseFragment.glsl") + loadShaderSource(_fragFile) + "\0";
  m_shader->loadShaderSourceFromString(fragShader, fragShaderSource);

  // compile source code
  m_shader->compileShader(vertShader);
  m_shader->compileShader(fragShader);

  // attach to created program
  m_shader->attachShaderToProgram(_progName, vertShader);
  m_shader->attachShaderToProgram(_progName, fragShader);

  // link it up
  m_shader->linkProgramObject(_progName);
  useShaderProgram(_progName);
}

void ShaderLibPro::useShaderProgram(const std::string &_progName)
{
  m_shader->use(_progName);
}

void ShaderLibPro::loadTexture(std::string _progName, std::string _textureFile, GLuint *textures, int _channelNum)
{
  // make sure program is active otherwise this will break
  useShaderProgram(_progName);
  // based on jon's image loading
  GLuint progID = m_shader->getProgramID(_progName);

  QImage image;
  // load given texture files
  bool loaded=image.load(_textureFile.c_str());
  if(loaded == true)
  {
    int width=image.width();
    int height=image.height();

    unsigned char *data = new unsigned char[ width*height*3];
    unsigned int index=0;
    for( int y=0; y<height; ++y)
    {
      for( int x=0; x<width; ++x)
      {
        // getting RGB from the image
        QRgb colour=image.pixel(x,y);
        data[index++]=qRed(colour);
        data[index++]=qGreen(colour);
        data[index++]=qBlue(colour);
      }
    }

    // setting up mipmap parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // set wrapping parameters for textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set active texture unit
    glActiveTexture(GL_TEXTURE0 + _channelNum);
    // bind the texture to the GLuint
    glBindTexture(GL_TEXTURE_2D, textures[_channelNum]);
    // load texture
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    // creating mipmaps, needs to be done after texture is loaded
    glGenerateMipmap(GL_TEXTURE_2D);

    // calculate texture channel name
    // this allows for easier adding of textures, as they can all be called iChannelX where X is their number
    std::ostringstream convertStream;
    convertStream << _channelNum;
    std::string channelName = "iChannel" + convertStream.str();

    // get texture unit location and set uniform up
    GLuint texLocation = glGetUniformLocation(progID, channelName.c_str());
    glUniform1i(texLocation, _channelNum);

    // print info to confirm texture loaded
    std::cout << "Loaded texture to " << channelName.c_str() << std::endl;
    // clean up
    delete[] data;
  }
  else{
    std::cerr << _textureFile << " was not found" << std::endl;
    exit(EXIT_FAILURE);
  }
}

// just loading text from files
std::string ShaderLibPro::loadShaderSource(const std::string &_fileName)
{

  std::ifstream shaderSource(_fileName.c_str());
  if(!shaderSource.is_open()){
    std::cerr << _fileName << " was not found" << std::endl;
    exit(EXIT_FAILURE);
  }

  // this is loading the file
  std::string source = std::string(std::istreambuf_iterator<char>(shaderSource),
                                   std::istreambuf_iterator<char>()
                                   );
  shaderSource.close();
  // testing that we've got the text from the file
  //std::cout << source << std::endl;

  return source;
}

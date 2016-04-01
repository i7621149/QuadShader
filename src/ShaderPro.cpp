#include "ShaderPro.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <boost/algorithm/string/replace.hpp>
#include <QImage>

ShaderPro::ShaderPro() :
  m_progID(0),
  m_outID(0),
  m_fragFile("\0"),
  m_vertFile("\0"),
  m_textures(0)
{

}

ShaderPro::~ShaderPro()
{

}

void ShaderPro::compile()
{
  loadVertSource();
  loadFragSource();
  loadTextures();
  glLinkProgram(m_progID);
  glUseProgram(m_progID);
}

void ShaderPro::loadVertSource()
{
  std::cout << "loading vert source" << std::endl;

  std::string vertSource = loadSource("shaders/BaseVertex.glsl") + loadSource(m_vertFile) + "\0";

  // testing that we've got the text from the file
  std::cout << vertSource << std::endl;

  const char *cVertSource = vertSource.c_str();

  glShaderSource(m_vertID, 1, &cVertSource, NULL);
  glCompileShader(m_vertID);
  printInfoLog(m_vertID, DebugMode::COMPILE);
  glAttachShader(m_progID, m_vertID);
}

void ShaderPro::loadFragSource()
{
  std::cout << "loading frag source" << std::endl;

  std::string fragBase = getFragBase();
  std::string fragSource = fragBase + loadSource(m_fragFile) + "\0";

  boost::replace_all(fragSource, "texture2D", "texture");
  boost::replace_all(fragSource, "textureCube", "texture");

  // testing that we've got the text from the file
  std::cout << fragSource << std::endl;

  const char *cFragSource = fragSource.c_str();

  glShaderSource(m_fragID, 1, &cFragSource, NULL);
  glCompileShader(m_fragID);
  printInfoLog(m_fragID, DebugMode::COMPILE);
  glAttachShader(m_progID, m_fragID);
}

std::string ShaderPro::loadSource(const std::string &_fileName)
{
  std::ifstream shaderSource(_fileName);
  if(!shaderSource.is_open()){
    std::cerr << "file not found: " << _fileName << std::endl;
    exit(EXIT_FAILURE);
  }

  // put contents into string
  std::string source = std::string(std::istreambuf_iterator<char>(shaderSource),
                                   std::istreambuf_iterator<char>()
                                  );
  shaderSource.close();
  return source;
}

std::string ShaderPro::getFragBase()
{
  std::string fragBase = loadSource("shaders/BaseFragment.glsl");

  int textureNum = 0;
  for(TextureData texture: m_textures){

    std::string textureString = std::to_string(textureNum);

    if(texture.type == TEXTURE2D){
      fragBase += "uniform sampler2D iChannel" + textureString + ";\n";
    }

    textureNum++;
  }
  return fragBase;
}

void ShaderPro::loadTextures()
{
  int textureUnit = 0;
  for(TextureData texture : m_textures){
    // currently just doing it for TEXTURE2D
    if(texture.type == TEXTURE2D){
      loadImage(textureUnit, texture);
    }
    textureUnit++;
  }
}

void ShaderPro::loadImage(int _textureUnit, TextureData _texture)
{
  QImage image;
  // load given texture files

  std::cout << "\n\n\n\n\nloading " << _texture.textureFile << "\n\n\n\n\n\n" << std::endl;

  bool loaded=image.load(_texture.textureFile.c_str());
  if(loaded == true)
  {
    int width=image.width();
    int height=image.height();

    unsigned char *data = new unsigned char[ width*height*3];
    unsigned int index=0;
    // reversed height so it loads as is used in shader
    for( int y=height-1; y>=0; --y)
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

    // set active texture unit
    glActiveTexture(GL_TEXTURE0 + _textureUnit);


    // is this the right place??
    glBindTexture(GL_TEXTURE_2D, _texture.id);

    // load texture
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);

    // setting up mipmap parameters
    // not sure whehter the first should be GL_LINEAR or GL_LINEAR_MIPMAP_LINEAR?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // set wrapping parameters for textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // creating mipmaps, needs to be done after texture is loaded
    glGenerateMipmap(GL_TEXTURE_2D);

    // calculate texture channel name
    // this allows for easier adding of textures, as they can all be called iChannelX where X is their number
    std::ostringstream convertStream;
    convertStream << _textureUnit;
    std::string channelName = "iChannel" + convertStream.str();

    // get texture unit location and set uniform up
    GLuint texLocation = glGetUniformLocation(m_progID, channelName.c_str());
    glUniform1i(texLocation, _textureUnit);

    // print info to confirm texture loaded
    std::cout << "Loaded texture to " << channelName.c_str() << std::endl;
    // clean up
    delete[] data;
  }
  else{
    std::cerr << _texture.textureFile << " was not found" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void ShaderPro::printShaderData()
{
  std::cout << "SHADER INFO" << std::endl;
  std::cout << "m_name: " << m_name << std::endl;
  std::cout << "m_progID: " << m_progID << std::endl;
  std::cout << "m_fragID: " << m_name << std::endl;
  std::cout << "m_vertID: " << m_progID << std::endl;
  std::cout << "m_outID: " << m_name << std::endl;
  std::cout << "m_fragFile: " << m_fragFile << std::endl;
  std::cout << "m_vertFile: " << m_vertFile << std::endl;

  std::cout << "Textures:" << std::endl;
  for(TextureData texture : m_textures){
    std::cout << "Texture: " << texture.id << ", " << texture.type << ", " << texture.textureFile << std::endl;
  }
}

// this allows us to print off info about whether the shader compiled correctly
// because a broken shader is just going to render default stuff so need a way to bug check
void ShaderPro::printInfoLog(GLuint _id, DebugMode _mode)
{
  GLint infoLogLength = 0;
  GLint charsWritten = 0;
  glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &infoLogLength); //get length of infolog from opengl or something
  if(infoLogLength > 0){
    std::unique_ptr<char []>infoLog(new char [infoLogLength]);
    glGetShaderInfoLog(_id, infoLogLength, &charsWritten, infoLog.get());
    std::cerr << "LOG:\n" << infoLog.get() << std::endl;

    GLenum mode;
    if(_mode == DebugMode::COMPILE){
      mode = GL_COMPILE_STATUS;
    }
    else if(_mode == DebugMode::LINK){
      mode = GL_LINK_STATUS;
    }
    else{
      mode = 0;
      std::cerr << "MODE FORMATTING ERROR!" << std::endl;
    }

    glGetShaderiv(_id, mode, &infoLogLength);
    if(infoLogLength == GL_FALSE){
      std::cerr << "shader compilation or linking error" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

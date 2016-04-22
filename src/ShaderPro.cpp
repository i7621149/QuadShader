#include "ShaderPro.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <boost/algorithm/string/replace.hpp>
#include <QImage>

ShaderPro::ShaderPro() :
  m_progID(0),
  m_outBufferID(0),
  m_outTextureID(0),
  m_outDepthStencilID(0),
  m_fragFile("\0"),
  m_vertFile("\0"),
  m_textures(0)
{

}

ShaderPro::~ShaderPro()
{
  if(m_name != "MAIN")
  {
    glDeleteFramebuffers(1, &m_outBufferID);
  }
}

void ShaderPro::compile()
{
  loadVertSource();
  loadFragSource();

  if(m_name != "MAIN")
  {
    setUpFramebuffer();
  }

  glLinkProgram(m_progID);
  glUseProgram(m_progID);
  loadTextures();
}

void ShaderPro::loadVertSource()
{
  std::cout << "loading vert source" << std::endl;

  std::string vertSource = loadSource("shaders/BaseVertex.glsl") + loadSource(m_vertFile) + "\0";

  // print out final vertSource
  //std::cout << vertSource << std::endl;

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

  // print out final fragSource
  //std::cout << fragSource << std::endl;

  const char *cFragSource = fragSource.c_str();

  glShaderSource(m_fragID, 1, &cFragSource, NULL);
  glCompileShader(m_fragID);
  printInfoLog(m_fragID, DebugMode::COMPILE);
  glAttachShader(m_progID, m_fragID);
}

// simple loading text from file
std::string ShaderPro::loadSource(const std::string &_fileName)
{
  std::ifstream shaderSource(_fileName);
  if(!shaderSource.is_open())
  {
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

// add required information to fragment shader, such as correct uniforms
std::string ShaderPro::getFragBase()
{
  std::string fragBase = loadSource("shaders/BaseFragment.glsl");

  int textureNum = 0;
  for(TextureData texture: m_textures)
  {

    std::string textureString = std::to_string(textureNum);

    if(texture.type == TEXTURE2D)
    {
      fragBase += "uniform sampler2D iChannel" + textureString + ";\n";
    }
    else if(texture.type == BUFFER)
    {
      fragBase += "uniform sampler2D iChannel" + textureString + ";\n";
    }

    textureNum++;
  }
  return fragBase;
}

void ShaderPro::loadTextures()
{
  int textureUnit = 0;
  for(TextureData texture : m_textures)
  {
    // load texturefile
    if(texture.type == TEXTURE2D)
    {
      std::cout << std::endl << "loading 2D texture" << std::endl;
      loadImage(textureUnit, texture, GL_TEXTURE_2D);
    }
    textureUnit++;
  }
}

void ShaderPro::texturesToShader()
{
  int textureUnit = 0;
  for(TextureData texture : m_textures){

    glActiveTexture(GL_TEXTURE0 + textureUnit);

    if(texture.type == TEXTURE2D)
    {
      // is this the right place??
      glBindTexture(GL_TEXTURE_2D, texture.id);
    }
    else if(texture.type == BUFFER)
    {
      glBindTexture(GL_TEXTURE_2D, texture.id);
    }

    // calculate texture channel name
    // this allows for easier adding of textures, as they can all be called iChannelX where X is their number
    std::ostringstream convertStream;
    convertStream << textureUnit;
    std::string channelName = "iChannel" + convertStream.str();

    // get texture unit location and set uniform up
    GLuint texLocation = glGetUniformLocation(m_progID, channelName.c_str());

    glUniform1i(texLocation, textureUnit);

    textureUnit++;
  }
}

void ShaderPro::loadImage(int _textureUnit, TextureData _texture, GLenum _type)
{
  std::cout << "texture ID: " << _texture.id <<std::endl;
  std::cout << "texture unit: " << _textureUnit << std::endl;

  QImage image;
  // load given texture files
  bool loaded=image.load(_texture.textureSource.c_str());
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
    glBindTexture(_type, _texture.id);

    // load texture
    glTexImage2D(_type,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);

    // setting up mipmap parameters
    // not sure whether the first should be GL_LINEAR or GL_LINEAR_MIPMAP_LINEAR?
    glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // set wrapping parameters for textures
    glTexParameteri(_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // creating mipmaps, needs to be done after texture is loaded
    glGenerateMipmap(_type);

    // calculate texture channel name
    // this allows for easier adding of textures, as they can all be called iChannelX where X is their number
    std::ostringstream convertStream;
    convertStream << _textureUnit;
    std::string channelName = "iChannel" + convertStream.str();

    // get texture unit location and set uniform up
    GLuint texLocation = glGetUniformLocation(m_progID, channelName.c_str());

    glUniform1i(texLocation, _textureUnit);

    if(texLocation == -1)
    {
      // warning, as it might not exist if it is never used in shader
      std::cerr << "WARNING: cannot get uniform location of " << channelName << "\nPerhaps it is not being used in the shader?" << std::endl;
    }
    else
    {
      // print info to confirm texture loaded
      std::cout << "Loaded texture to " << channelName.c_str() << std::endl;
    }
    // clean up
    delete[] data;
  }
  else
  {
    std::cerr << _texture.textureSource << " was not found" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void ShaderPro::printShaderData()
{
  std::cout << std::endl;
  std::cout << "SHADER INFO" << std::endl;
  std::cout << "m_name: " << m_name << std::endl;
  std::cout << "m_progID: " << m_progID << std::endl;
  std::cout << "m_fragID: " << m_fragID << std::endl;
  std::cout << "m_vertID: " << m_vertID << std::endl;
  std::cout << "m_outBufferID: " << m_outBufferID << std::endl;
  std::cout << "m_outTextureID: " << m_outTextureID << std::endl;
  std::cout << "m_outDepthStencilID: " << m_outDepthStencilID << std::endl;
  std::cout << "m_fragFile: " << m_fragFile << std::endl;
  std::cout << "m_vertFile: " << m_vertFile << std::endl;

  std::cout << "Textures:" << std::endl;
  for(TextureData texture : m_textures)
  {
    std::cout << "Texture: " << texture.id << ", " << texture.type << ", " << texture.textureSource << std::endl;
  }
  std::cout << std::endl;
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
    if(_mode == DebugMode::COMPILE)
    {
      mode = GL_COMPILE_STATUS;
    }
    else if(_mode == DebugMode::LINK)
    {
      mode = GL_LINK_STATUS;
    }
    else
    {
      mode = 0;
      std::cerr << "MODE FORMATTING ERROR!" << std::endl;
    }

    glGetShaderiv(_id, mode, &infoLogLength);
    if(infoLogLength == GL_FALSE)
    {
      std::cerr << "shader compilation or linking error" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

void ShaderPro::setUpFramebuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_outBufferID);

  glBindTexture(GL_TEXTURE_2D, m_outTextureID);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 288, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  // set wrapping parameters for textures
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // dont think this works here?
  //glGenerateMipmap(GL_TEXTURE_2D);

  // color attachment0?
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_outTextureID, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, m_outDepthStencilID);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 512, 288);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_outDepthStencilID);
}

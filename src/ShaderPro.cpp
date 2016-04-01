#include "ShaderPro.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <boost/algorithm/string/replace.hpp>
#include "ShaderVariables.h"

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
  glLinkProgram(m_progID);
  glUseProgram(m_progID);
  ShaderVariables::instance()->printVariables();
  ShaderVariables::instance()->loadToShader(m_progID);
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

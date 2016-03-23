#include "ShaderPro.h"

ShaderPro::ShaderPro() :
  m_shaderName(""),
  m_fragName(""),
  m_vertName(""),
  m_fragSource(""),
  m_vertSource(""),
  m_id(0),
  m_buffer(0),
  m_textures(0),
  m_textureData({false, ""})
{

}

ShaderPro::~ShaderPro()
{

}

void ShaderPro::loadFragSource(const std::string &_fragSource)
{
 m_fragSource = loadSource("shaders/BaseFragment") + loadSource(_fragSource) + "\0";
}

void ShaderPro::loadSource(const std::string &_fileName)
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

void ShaderPro::compile()
{

}

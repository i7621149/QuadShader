#include "ShaderLibPro.h"

#include <iostream>
#include <QImage>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>
#include "ShaderVariables.h"
#include "NGLScene.h"

ShaderLibPro::ShaderLibPro() :
  m_shaderSets(0),
  m_currentShaderindex(0)
{

}

ShaderLibPro::~ShaderLibPro()
{
  //std::cout << "shutting down ShaderLibPro" << std::endl;


  //glDeleteTextures(m_textures.size(), &(m_textures[0]));

  // this is a thing???
  //glDeleteFramebuffers(m_frameBuffers.size(), &(m_frameBuffers[0]));
  // ????????????????? what goes in there ????????????
  //glDeleteRenderbuffers(m_depthStencilBuffers.size(), &(m_depthStencilBuffers[0]));
}

void ShaderLibPro::useShader(int _index)
{
  if(_index >= m_shaderSets.size())
  {
    std::cerr << "accessing invalid shader!" << std::endl;
    exit(EXIT_FAILURE);
  }
  else
  {
    m_currentShaderindex = _index;
  }
}

void ShaderLibPro::addShader(const std::string &_sourceFile)
{
  m_shaderSets.push_back(std::unique_ptr<ShaderSet>(new ShaderSet(_sourceFile)));
}

void ShaderLibPro::draw(NGLScene *_scene)
{
  m_shaderSets[m_currentShaderindex]->draw(_scene);
}

//ShaderPro *ShaderLibPro::getShader(const std::string &_shaderName)
//{
//  for(ShaderPro *shader : m_shaders)
//  {
//    if(shader->m_name == _shaderName)
//    {
//      std::cout << "shader " << _shaderName << " found!" << std::endl;
//      return shader;
//    }
//  }
//  std::cerr << "ERROR: Shader " << _shaderName << " not found!" << std::endl;
//  return nullptr;
//}

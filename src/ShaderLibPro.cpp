#include "ShaderLibPro.h"

#include <QImage>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>

//#include <boost/iostreams/filtering_stream.hpp>

ShaderLibPro::ShaderLibPro() :
  m_shaders(0)
{

}

ShaderLibPro::~ShaderLibPro()
{
  //std::cout << "shutting down ShaderLibPro" << std::endl;


  //glDeleteTextures(m_textures.size(), &(m_textures[0]));

  //this is a thing???
  //glDeleteFramebuffers(m_frameBuffers.size(), &(m_frameBuffers[0]));
  //????????????????? what goes in there ????????????
  //glDeleteRenderbuffers(m_depthStencilBuffers.size(), &(m_depthStencilBuffers[0]));
}

void ShaderLibPro::setShaderInfo(const std::string &_sourceFile)
{
  std::cout << "loading shaders" << std::endl;
  std::ifstream file(_sourceFile);

  if(!file.is_open()){
    std::cerr << "file not found" << _sourceFile << std::endl;
    exit(EXIT_FAILURE);
  }

  bool mainIsSet = false;
  std::string line;
  // go through first to set up each shader
  while(std::getline(file, line)){
    std::istringstream iss(line);
    std::vector<std::string> splitString((std::istream_iterator<std::string>(iss)), (std::istream_iterator<std::string>()));
    if(splitString[0] == "START"){
      std::cout << "new Shader " << std::endl;

      ShaderPro *shader = new ShaderPro;
      m_shaders.push_back(shader);

      shader->m_progID = glCreateProgram();
      shader->m_name = splitString[1];

      if(splitString[1] != "MAIN"){
        std::cout << "not main, do frame buffer stuff" << std::endl;





        // frame buffer stuff!





      }
      else{
        mainIsSet = true;
        std::cout << "MAIN!" << std::endl;
      }
    }
  }


  if(!mainIsSet){
    std::cerr << "WARNING: no main shader set!" << std::endl;
  }



  int lineNum = 1;
  int shaderNum = 0;
  ShaderPro *currentShader = nullptr;

  // reset file
  file.clear();
  file.seekg(0, file.beg);
  while(std::getline(file, line)){
    std::istringstream iss(line);
    std::vector<std::string> splitString((std::istream_iterator<std::string>(iss)), (std::istream_iterator<std::string>()));

    if(splitString[0] == "START"){
      if(shaderNum < m_shaders.size()){
        currentShader = m_shaders[shaderNum];
      }
      else{
        std::cerr << "ERROR: trying to work on shader that doesn't fit, line " << lineNum << std::endl;
      }
    }
    // if we have a valid currentShader:
    else if(currentShader){
      if(splitString[0] == "VERTEX"){
          std::cout << "vertex!" << std::endl;

          currentShader->m_vertID = glCreateShader(GL_VERTEX_SHADER);
          currentShader->m_vertFile = splitString[1];
      }
      else if(splitString[0] == "FRAGMENT"){
          std::cout << "fragment!" << std::endl;

          currentShader->m_fragID = glCreateShader(GL_FRAGMENT_SHADER);
          currentShader->m_fragFile = splitString[1];

      }
      else if(splitString[0] == "TEXTURE"){
          std::cout << "adding texture: ";

          TextureData texture;
          GLuint texID;
          if(splitString[1] == "2D"){
            std::cout << "2D!" << std::endl;

            glGenTextures(1, &texID);
            texture.id = texID;

            texture.type = TEXTURE2D;
            texture.textureFile = splitString[1];
          }
          else if(splitString[1] == "CUBE"){
            std::cout << "cube!" << std::endl;

            glGenTextures(1, &texID);
            texture.id = texID;

            texture.type = TEXTURECUBE;
            texture.textureFile = splitString[1];
          }
          else if(splitString[1] == "BUFFER"){
            std::cout << "buffer!" << std::endl;
            texture.type = BUFFER;
            texture.textureFile = "\0";
          }
          else{
            std::cerr << "ERROR: unrecognised texture type, line " << lineNum << std::endl;
          }

          currentShader->m_textures.push_back(texture);
      }

      else if(splitString[0] == "END"){
          std::cout << "end of Shader" << std::endl;

          currentShader = nullptr;
          shaderNum++;
      }
      else{
        std::cerr << "ERROR: glll line " << lineNum << std::endl;
      }
    }
    else{
      std::cerr << "ERROR: no current shader, line " << lineNum << std::endl;
    }
    lineNum++;
  }

  loadShaders();
}

void ShaderLibPro::draw(NGLScene *_scene)
{

}

void ShaderLibPro::loadShaders()
{
  for(ShaderPro *shader : m_shaders){
    shader->compile();
  }
}








/*
void ShaderLibPro::newShaderProgram(const std::string &_progName, const std::string &_fragFile, const std::string &_vertFile)
{
  ShaderData shaderData = {_progName, std::vector<GLuint>(0) };
  m_shaderData.push_back(shaderData);
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
  //replace deprecated functions that shadertoy uses
  boost::replace_all(fragShaderSource, "texture2D", "texture");
 // boost::replace_all(fragShaderSource, "textureCube", "texture");
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

<<<<<<< HEAD
void ShaderLibPro::draw(NGLScene *scene)
{
  setShaderUniforms();
  scene->drawScene();
}

void ShaderLibPro::setShaderUniforms()
{
  m_shader->setRegisteredUniform("iResolution", m_resolution);
  m_shader->setRegisteredUniform("iGlobalTime", m_globalTime);
  m_shader->setRegisteredUniform("iTimeDelta", m_timeDelta);
  m_shader->setRegisteredUniform("iFrame", m_frame);
  m_shader->setRegisteredUniform("iMouse", m_mouse);
  m_shader->setRegisteredUniform("iDate", m_date);
}

=======
>>>>>>> 9472b6eb9ea9e8deb078c9b58c5cf422d60ee571
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

int ShaderLibPro::useShaderProgram(const std::string &_progName)
{
  // if it is not the current shader, use shader
  if(m_currentShader != _progName){
    m_shader->use(_progName);
    m_currentShader = _progName;

    // load/reload current textures to shader
    for(int i=0; i<m_textures.size(); i++){
      std::cout << "accessing texture " << i <<std::endl;
      loadTextureFile(i, m_textureFiles[i]);
    }

    // return 1 for using different program
    return 1;
  }
  else{
    // return 0 if shader has not changed
    return 0;
  }
}

int ShaderLibPro::useTexture(int _textureUnit, const std::string &_textureFile)
{
  // if the texture unit id isn't generated yet, do so
  int numOfTextures = m_textures.size();
  if(_textureUnit >= numOfTextures){
      if(_textureUnit > numOfTextures){
        std::cout << "texture unit specified is higher than necessary" << std::endl;
        std::cout << "generating texture " << numOfTextures << ", rather than texture " << _textureUnit << std::endl;

        // set textureUnit to be one more than current last
        _textureUnit = numOfTextures;
      }
      // add texture file and id to vectors
      m_textureFiles.push_back("");
      m_textures.push_back(0);
      std::cout << "accessing textures" <<std::endl;

      glGenTextures( 1, &(m_textures[numOfTextures]) );

      // moved this to here, not entirely sure if it's the correct place?
      glBindTexture(GL_TEXTURE_2D, m_textures[_textureUnit]);
  }

  // set file string in vector
  m_textureFiles[_textureUnit] = _textureFile;


  // if a string is given, load file
  if(_textureFile != ""){
    loadTextureFile(_textureUnit, _textureFile);
  }

  return _textureUnit;
}

void ShaderLibPro::loadTextureFile(int _textureUnit, const std::string &_textureFile)
{
  // based on jon's image loading
  GLuint progID = m_shader->getProgramID(m_currentShader);

  QImage image;
  // load given texture files
  bool loaded=image.load(_textureFile.c_str());
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

    // bind the texture to the GLuint - put somewhere else now, not sure if right?
    //glBindTexture(GL_TEXTURE_2D, m_textures[_channelNum]);
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
    GLuint texLocation = glGetUniformLocation(progID, channelName.c_str());
    glUniform1i(texLocation, _textureUnit);

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

void ShaderLibPro::createFrameBuffer(int _bufferNum, int _textureUnit)
{
  // if the buffer id isn't generated yet, do so
  int numOfBuffers = m_frameBuffers.size();
  if(_bufferNum >= numOfBuffers){
      if(_bufferNum > numOfBuffers){
        std::cout << "buffer num specified is higher than necessary" << std::endl;
        std::cout << "generating buffer " << numOfBuffers << ", rather than texture " << _bufferNum << std::endl;

        // set bufferNum to be one more than current last
        _bufferNum = numOfBuffers;
      }

      // add frame buffer id to vector
      m_frameBuffers.push_back(0);
      glGenFramebuffers( 1, &(m_frameBuffers[numOfBuffers]) );
      m_depthStencilBuffers.push_back(0);
      glGenRenderbuffers( 1, &(m_depthStencilBuffers[numOfBuffers]) );
  }

  createBufferTexture(_textureUnit);

  glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffers[_bufferNum]);

  // not sure about the depth number, and the resolution is hard coded again
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, 512, 288);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffers[_bufferNum]);
  glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffers[0]);
}

void ShaderLibPro::createBufferTexture(int _textureUnit)
{
  // useTexture returns the actual textureUnit being used, so need to set _textureUnit in case
  _textureUnit = useTexture(_textureUnit);

  // not in useTexture function, so set here? also not sure if necessary?
  glActiveTexture(GL_TEXTURE0 + _textureUnit);

  // width and height should NOT be hard coded
  // if this varies from regular width/height, a glViewport call is probably necessary
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 288, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  // setting up mipmap parameters
  // might only want these to be linear?
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  // set wrapping parameters for textures
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // not sure whether this works here?
  glGenerateMipmap(GL_TEXTURE_2D);

  //woah boy wtf???????????????
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textures[_textureUnit], 0);
}
*/

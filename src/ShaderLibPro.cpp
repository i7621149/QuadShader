#include "ShaderLibPro.h"

#include <QImage>
#include <sstream>
#include <fstream>

ShaderLibPro::ShaderLibPro() :
  m_shader(ngl::ShaderLib::instance()),
  m_textures(0),
  m_frameBuffers(0),
  m_currentShader("snail")
{

}

ShaderLibPro::~ShaderLibPro()
{
  // when does this get called?


  glDeleteTextures(m_textures.size(), &(m_textures[0]));
  glDeleteFramebuffers(m_frameBuffers.size(), &(m_frameBuffers[0]));
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

    // load current textures to shader
    for(int i=0; i<m_textures.size(); i++){
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

void ShaderLibPro::useTexture(int _textureUnit, const std::string &_textureFile)
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
      glGenTextures( 1, &(m_textures[numOfTextures]) );
  }

  // set file string in vector
  m_textureFiles[_textureUnit] = _textureFile;

  // if a string is given, load file
  if(_textureFile != ""){
    loadTextureFile(_textureUnit, _textureFile);
  }
}

void ShaderLibPro::loadTextureFile(int _channelNum, const std::string &_textureFile)
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

    // setting up mipmap parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // set wrapping parameters for textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set active texture unit
    glActiveTexture(GL_TEXTURE0 + _channelNum);
    // bind the texture to the GLuint
    glBindTexture(GL_TEXTURE_2D, m_textures[_channelNum]);
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
  }

  // set file string in vector
  //m_textureFiles[_textureUnit] = _textureFile;
  //loadTexture(m_currentShader, _textureFile, &(m_textures[0]), _textureUnit);
}

void ShaderLibPro::createBufferTexture(int _textureUnit)
{
 // glGenTextures();
}

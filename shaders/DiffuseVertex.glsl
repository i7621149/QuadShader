#version 400 core

layout (location = 0) in vec3  inPosition;
uniform int time;

out float iGlobalTime;

void main()
{
  gl_Position = vec4(inPosition, 1.0);

  iGlobalTime = time * 0.05;

}


